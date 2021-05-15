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

#include "CaptureScreenshotsWidget.h"

#include "Project.h"
#include "Camera.h"

MainWindow::MainWindow() : project(nullptr)
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
    setEnabledProjectActions(false);
}

void MainWindow::newProject()
{
    if (project != nullptr)
    {
        auto reply = QMessageBox::question(this, tr("MPCCT"),
            tr("Do you want to remove the current project?"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
        {
            return;
        }
        else
        {
            // TODO: remove the mesh/point cloud
            // Clear the current project
            camerasList->clear();
            lastPlyFilename = "";
            delete project;
        }
    }
    project = new Project();
    setEnabledProjectActions(true);
}

void MainWindow::open()
{
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open cameras file"), "", tr("json File (*.json)"));
    if (fileName.isEmpty())
    {
        return;
    }
    if (project != nullptr)
    {
        auto reply = QMessageBox::question(this, tr("MPCCT"),
            tr("Do you want to remove the current project?"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
        {
            return;
        }
        else
        {
            // Clear the current project
            camerasList->clear();
            delete project;
        }
    }
    project = new Project();
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    auto error = !project->load(fileName);
    QGuiApplication::restoreOverrideCursor();
    if (error)
    {
        QMessageBox::warning(this, tr("MPCCT"),
            tr("Cannot open file %1.")
            .arg(QDir::toNativeSeparators(fileName)));
        delete project;
    }
    else
    {
        // Populate the camera list
        const auto numberOfCameras = project->getNumberOfCameras();
        for (size_t i = 0; i < numberOfCameras; i++)
        {
            camerasList->addItem(project->getCamera(i)->getListItem());
        }
        statusBar()->showMessage(tr("Opened '%1'").arg(fileName), 2000);
        setEnabledProjectActions(true);
    }
}

void MainWindow::save()
{
    if (lastPlyFilename.isEmpty())
    {
        QMessageBox::warning(this, tr("MPCCT"),
            tr("You need to load a mesh/point cloud before saving."));
        return;
    }
    if (project->isOnDisk())
    {
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        project->save();
        QGuiApplication::restoreOverrideCursor();
        statusBar()->showMessage(tr("Saved '%1'").arg(project->getPath()), 2000);
    }
    else
    {
        saveAs();
    }
}

void MainWindow::saveAs()
{
    if (lastPlyFilename.isEmpty())
    {
        QMessageBox::warning(this, tr("MPCCT"),
            tr("You need to load a mesh/point cloud before saving."));
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Choose a file name"), "", tr("json File (*.json)"));
    if (fileName.isEmpty())
    {
        return;
    }
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    project->save(fileName);
    QGuiApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
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
    QFileInfo fileInfo(fileName);
    lastPlyFilename = fileInfo.fileName();
}

void MainWindow::addCamera()
{
    QListWidgetItem* item = new QListWidgetItem("Camera");
    camerasList->addItem(item);
    project->addCamera(renderer->GetActiveCamera(), item,
        vtkWidget->size().width(), vtkWidget->size().height());
}

void MainWindow::updateCamera()
{
    const auto currentItem = camerasList->currentItem();
    if (!currentItem)
    {
        return;
    }
    project->updateCamera(currentItem, renderer->GetActiveCamera(),
        vtkWidget->size().width(), vtkWidget->size().height());
}

void MainWindow::removeSelectedCamera()
{
    const auto currentItem = camerasList->currentItem();
    if (!currentItem)
    {
        return;
    }
    if (project->removeCamera(currentItem))
    {
        auto item = camerasList->takeItem(camerasList->currentRow());
        delete item;
        camerasList->update();
    }
}

void MainWindow::captureScreenshots()
{
    // Find the base filename
    auto plyFileName = QFileDialog::getOpenFileName(this,
        tr("Open mesh/point cloud"), "", tr("3D Files (*.ply)"));
    if (plyFileName.isEmpty())
    {
        return;
    }

    QFileInfo fileInfo(plyFileName);
    QFileInfo fileInfo2(fileInfo.absolutePath() + ".abc");
    // Get the base directory
    QDir directory(fileInfo2.absolutePath());
    if (!directory.exists(fileInfo.fileName()))
    {
        if (!directory.mkdir(fileInfo.fileName()))
        {
            return;
        }
    }
    // Create the cameras folders
    for (size_t i = 0; i < project->getNumberOfCameras(); i++)
    {
        std::string folderName = fileInfo.fileName().toStdString() + "\\camera_" + std::to_string(i);
        if (!directory.exists(folderName.c_str()))
        {
            if (!directory.mkdir(folderName.c_str()))
            {
                return;
            }
        }
    }


    // Loop trought each file
    QStringList plyFiles = directory.entryList(QStringList() << fileInfo.fileName(), QDir::Files);
    for(const auto& fileName : plyFiles)
    {
        // Load each filename
    }


    //std::string filenameToUse;
    //wxArrayString plyFiles;
    //wxDir::GetAllFiles(mainDir, &plyFiles, filenameToUse, wxDIR_FILES | wxDIR_DIRS);

    //// Create the ply folder
    //std::string plyFolder = mainDir + "\\" + filenameToUse;
    //if (!wxDirExists(plyFolder))
    //{
    //    wxMkdir(plyFolder);
    //}
    //// Create the cameras folders
    //for (size_t i = 0; i < cameras.size(); i++)
    //{
    //    auto folderName = plyFolder + "\\camera_" + std::to_string(i);
    //    if (!wxDirExists(folderName))
    //    {
    //        wxMkdir(folderName);
    //    }
    //}


    //// Create the offscreen renderer
    //vtkNew<vtkRenderer> rendererOffScreen;
    //vtkNew<vtkRenderWindow> renderWindowOffScreen;
    //renderWindowOffScreen->SetOffScreenRendering(1);
    //renderWindowOffScreen->SetSize(vtkOffscreenWindowSize.width(),
    //    vtkOffscreenWindowSize.height());
    //renderWindowOffScreen->AddRenderer(rendererOffScreen);



    //// Loop through the ply files
    //for (const auto& plyFile : plyFiles)
    //{
    //    auto actor = ReadPLY(plyFile.ToStdString());
    //    rendererOffScreen->AddActor(actor);
    //    // Loop through the cameras and save screenshots
    //    for (size_t i = 0; i < cameras.size(); i++)
    //    {
    //        rendererOffScreen->SetActiveCamera(cameras[i]);
    //        rendererOffScreen->ResetCameraClippingRange();
    //        renderWindowOffScreen->Render();

    //        TakeScreenshot(renderWindowOffScreen,
    //            plyFolder + "\\camera_" + std::to_string(i) + "\\" + wxFileName(plyFile).GetDirs().Last().ToStdString() + ".png");
    //    }
    //    rendererOffScreen->RemoveActor(actor);
    //}
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About MPCCT"),
        tr("The <b>MPCCT - Mesh/Point Cloud Comparison Tool</b> allows you "
            "to capture screenshots from different camera views "
            "from different meshes/poitn clouds."));
}

void MainWindow::cameraDoubleClick(QListWidgetItem* item)
{
    if (!item)
    {
        return;
    }
    auto camera = project->getCamera(item);
    if (camera)
    {
        auto vtkCamera = camera->getCamera();
        renderer->GetActiveCamera()->SetPosition(vtkCamera->GetPosition());
        renderer->GetActiveCamera()->SetFocalPoint(vtkCamera->GetFocalPoint());
        renderer->GetActiveCamera()->SetViewUp(vtkCamera->GetViewUp());
        renderer->GetActiveCamera()->SetViewAngle(vtkCamera->GetViewAngle());
        renderer->ResetCameraClippingRange();
        renderer->GetRenderWindow()->Render();
    }
}

void MainWindow::createActions()
{
    // File
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar* fileToolBar = addToolBar(tr("File"));

    QAction* newProjectAct = new QAction(QIcon(":/images/newProject.png"), tr("&New project"), this);
    newProjectAct->setShortcuts(QKeySequence::New);
    newProjectAct->setStatusTip(tr("Open a project"));
    connect(newProjectAct, &QAction::triggered, this, &MainWindow::newProject);
    fileMenu->addAction(newProjectAct);
    fileToolBar->addAction(newProjectAct);

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
    actions.emplace_back(saveAct);

    QAction* saveAsAct = new QAction(QIcon(":/images/saveAs.png"), tr("&Save as..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the current project as"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);
    actions.emplace_back(saveAsAct);

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
    actions.emplace_back(addCameraAct);

    QAction* updateCameraAct = new QAction(QIcon(":/images/updateCamera.png"), tr("&Update camera"), this);
    updateCameraAct->setStatusTip(tr("Update the selected camera parameters"));
    connect(updateCameraAct, &QAction::triggered, this, &MainWindow::updateCamera);
    editMenu->addAction(updateCameraAct);
    editToolBar->addAction(updateCameraAct);
    actions.emplace_back(updateCameraAct);

    QAction* removeSelectedCameraAct = new QAction(QIcon(":/images/removeCamera.png"), tr("&Remove selected camera"), this);
    removeSelectedCameraAct->setStatusTip(tr("Remove selected camera from list"));
    connect(removeSelectedCameraAct, &QAction::triggered, this, &MainWindow::removeSelectedCamera);
    editMenu->addAction(removeSelectedCameraAct);
    editToolBar->addAction(removeSelectedCameraAct);
    actions.emplace_back(removeSelectedCameraAct);

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

    QDockWidget* dock2 = new QDockWidget(tr("Capture tool"), this);
    dock2->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock2->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    captureTool = new CaptureScreenshotsWidget(dock2);
    dock2->setWidget(captureTool);
    addDockWidget(Qt::RightDockWidgetArea, dock2);
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

void MainWindow::setEnabledProjectActions(bool enable)
{
    for (const auto& action : actions)
    {
        action->setEnabled(enable);
    }
}
