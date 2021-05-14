#include "MainWindow.h"
#include <QtWidgets>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <vtkPLYReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>

#include "json.hpp"
#include "Camera.h"

MainWindow::MainWindow()
{
    // Set up the QVTK window
    vtkWidget = new QVTKOpenGLNativeWidget(this);
    vtkWidget->setMinimumSize(200, 200);
    renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkWidget->setRenderWindow(renderWindow.Get());
    renderWindow->AddRenderer(renderer);
    vtkWidget->update();
    renderWindow->Render();

    setCentralWidget(vtkWidget);

    createActions();
    createStatusBar();
    createDockWindows();

    setWindowTitle(tr("MPCCT"));
}

void MainWindow::open()
{
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open cameras file"), "", tr("json File (*.json)"));
    if (fileName.isEmpty())
    {
        return;
    }
    // Clear the current project
    projectPath = "";
    cameras.clear();
    camerasList->clear();
    //
    nlohmann::json jsonFile;
    try
    {
        std::ifstream parametersFile(fileName.toStdString());
        if (!parametersFile.is_open())
        {
            QMessageBox::warning(this, tr("MPCCT"),
                tr("Cannot open file %1.")
                .arg(QDir::toNativeSeparators(fileName)));
            return;
        }
        jsonFile = nlohmann::json::parse(parametersFile);
    }
    catch (const std::exception&)
    {
        QMessageBox::warning(this, tr("MPCCT"),
            tr("Cannot open file %1.")
            .arg(QDir::toNativeSeparators(fileName)));
        return;
    }
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    // Load cameras
    for (auto& [key, value] : jsonFile.items()) 
    {
        if (key == "width")
        {
            vtkOffscreenWindowSize.setWidth(value);
        }
        else if (key == "height")
        {
            vtkOffscreenWindowSize.setHeight(value);
        }
        else if (value["type"] == "Camera")
        {
            vtkNew<vtkCamera> vtkCam;
            vtkCam->SetPosition(value["position"][0],
                value["position"][1],
                value["position"][2]);
            vtkCam->SetFocalPoint(value["focalPoint"][0],
                value["focalPoint"][1],
                value["focalPoint"][2]);
            vtkCam->SetViewUp(value["viewUp"][0],
                value["viewUp"][1],
                value["viewUp"][2]);
            vtkCam->SetViewAngle(value["viewAngle"]);
            QListWidgetItem* item = new QListWidgetItem("Camera");
            camerasList->addItem(item);
            cameras.emplace_back(new Camera(vtkCam, item));
        }
    }
    //
    QGuiApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Opened '%1'").arg(fileName), 2000);
    projectPath = fileName;
}

void MainWindow::save()
{
    if (projectPath.isEmpty())
    {
        saveAs();
    }
    else
    {
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        saveCameras(projectPath.toStdString());
        QGuiApplication::restoreOverrideCursor();
        statusBar()->showMessage(tr("Saved '%1'").arg(projectPath), 2000);
    }
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Choose a file name"), "", tr("json File (*.json)"));
    if (fileName.isEmpty())
    {
        return;
    }
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    saveCameras(fileName.toStdString());
    QGuiApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
    projectPath = fileName;
}

void MainWindow::load()
{
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open mesh/point cloud"), "", tr("3D Files (*.ply)"));
    if (fileName.isEmpty())
    {
        return;
    }
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName.toStdString().c_str());
    reader->Update();
    
    auto polyData = reader->GetOutput();
    if (!polyData)
    {
        return;
    }

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->GetRenderWindow()->Render();
}

void MainWindow::addCamera()
{
    QListWidgetItem* item = new QListWidgetItem("Camera");
    camerasList->addItem(item);
    cameras.emplace_back(new Camera(renderer->GetActiveCamera(), item));
}

void MainWindow::updateCamera()
{
    const auto currentItem = camerasList->currentItem();
    if (!currentItem)
    {
        return;
    }
    const auto numberOfCameras = cameras.size();
    for (size_t i = 0; i < numberOfCameras; i++)
    {
        if (currentItem == cameras[i]->GetListItem())
        {
            cameras[i]->SetCamera(renderer->GetActiveCamera());
            break;
        }
    }
}

void MainWindow::removeSelectedCamera()
{
    const auto currentItem = camerasList->currentItem();
    if (!currentItem)
    {
        return;
    }
    const auto numberOfCameras = cameras.size();
    for (size_t i = 0; i < numberOfCameras; i++)
    {
        if (currentItem == cameras[i]->GetListItem())
        {
            cameras.erase(cameras.begin() + i);
            auto item = camerasList->takeItem(camerasList->currentRow());
            delete item;
            camerasList->update();
            break;
        }
    }
}

void MainWindow::captureScreenshots()
{
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About MPCCT"),
        tr("The <b>MPCCT - Mesh/Point Cloud Comparison Tool</b> allows you "
            "to capture screenshots from different camera views "
            "from different meshes/poitn clouds."));
}

void MainWindow::cameraDoubleClick(const QListWidgetItem* item)
{
    if (!item)
    {
        return;
    }
    const auto numberOfCameras = cameras.size();
    for (size_t i = 0; i < numberOfCameras; i++)
    {
        if (item == cameras[i]->GetListItem())
        {
            renderer->SetActiveCamera(cameras[i]->GetCamera());
            renderer->ResetCameraClippingRange();
            renderer->GetRenderWindow()->Render();
            break;
        }
    }
}

void MainWindow::createActions()
{
    // File
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar* fileToolBar = addToolBar(tr("File"));

    QAction* openAct = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a project"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    QAction* saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the current project"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    QAction* saveAsAct = new QAction(QIcon(":/images/saveAs.png"), tr("&Save as..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the current project as"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    QAction* loadAct = new QAction(QIcon(":/images/load.png"), tr("&Load mesh/point cloud..."), this);
    loadAct->setStatusTip(tr("Load a mesh/point cloud"));
    connect(loadAct, &QAction::triggered, this, &MainWindow::load);
    fileMenu->addAction(loadAct);
    fileToolBar->addAction(loadAct);

    QAction* quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));
    fileMenu->addAction(quitAct);

    // Edit

    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar* editToolBar = addToolBar(tr("Edit"));

    QAction* addCameraAct = new QAction(QIcon(":/images/addCamera.png"), tr("&Add camera"), this);
    addCameraAct->setStatusTip(tr("Add the current camera to the list"));
    connect(addCameraAct, &QAction::triggered, this, &MainWindow::addCamera);
    editMenu->addAction(addCameraAct);
    editToolBar->addAction(addCameraAct);

    QAction* updateCameraAct = new QAction(QIcon(":/images/updateCamera.png"), tr("&Update camera"), this);
    updateCameraAct->setStatusTip(tr("Update the selected camera parameters"));
    connect(updateCameraAct, &QAction::triggered, this, &MainWindow::updateCamera);
    editMenu->addAction(updateCameraAct);
    editToolBar->addAction(updateCameraAct);

    QAction* removeSelectedCameraAct = new QAction(QIcon(":/images/removeCamera.png"), tr("&Remove selected camera"), this);
    removeSelectedCameraAct->setStatusTip(tr("Remove selected camera from list"));
    connect(removeSelectedCameraAct, &QAction::triggered, this, &MainWindow::removeSelectedCamera);
    editMenu->addAction(removeSelectedCameraAct);
    editToolBar->addAction(removeSelectedCameraAct);

    editMenu->addSeparator();
    editToolBar->addSeparator();

    QAction* startCapturingAct = new QAction(QIcon(":/images/play.png"), tr("&Start capturing"), this);
    startCapturingAct->setStatusTip(tr("Start capturing screenshots"));
    connect(startCapturingAct, &QAction::triggered, this, &MainWindow::captureScreenshots);
    editMenu->addAction(startCapturingAct);
    editToolBar->addAction(startCapturingAct);

    menuBar()->addSeparator();

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction* aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("About information"));
}

void MainWindow::createDockWindows()
{
    QDockWidget* dock = new QDockWidget(tr("Cameras"), this);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    camerasList = new QListWidget(dock);
    dock->setWidget(camerasList);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(camerasList, &QListWidget::itemDoubleClicked,
        this, &MainWindow::cameraDoubleClick);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::takeScreenshot(vtkSmartPointer<vtkRenderWindow> renderWindow,
    const std::string& filename, const bool setTransparentBackgroud)
{
    vtkNew<vtkWindowToImageFilter> filter;
    filter->SetInput(renderWindow);
    if (setTransparentBackgroud)
    {
        filter->SetInputBufferTypeToRGBA();
    }
    filter->Update();

    vtkNew<vtkPNGWriter> pngWriter;
    pngWriter->SetFileName(filename.c_str());
    pngWriter->SetInputConnection(filter->GetOutputPort());
    pngWriter->Write();
}

void MainWindow::saveCameras(const std::string& filename)
{
    nlohmann::json jsonFile;
    // Get vtk window size
    jsonFile["width"] = vtkWidget->size().width();
    jsonFile["height"] = vtkWidget->size().height();
    // Add the cameras
    int idx = 0;
    for (const auto& camera : cameras)
    {
        const auto vtkCam = camera->GetCamera();
        std::stringstream s;
        s << "Camera" << idx;
        jsonFile[s.str()] = {
            { "type", "Camera"},
            { "position",  {vtkCam->GetPosition()[0],
                            vtkCam->GetPosition()[1],
                            vtkCam->GetPosition()[2]}
            },
            { "focalPoint",{vtkCam->GetFocalPoint()[0],
                            vtkCam->GetFocalPoint()[1],
                            vtkCam->GetFocalPoint()[2]}
            },
            { "viewUp",    {vtkCam->GetViewUp()[0],
                            vtkCam->GetViewUp()[1],
                            vtkCam->GetViewUp()[2]}
            },
            { "viewAngle", vtkCam->GetViewAngle()}
        };
        idx++;
    }
    std::ofstream output(filename);
    output << std::setw(4) << jsonFile;
    output.close();
}
