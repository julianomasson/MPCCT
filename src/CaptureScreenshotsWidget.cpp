#include "CaptureScreenshotsWidget.h"

#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include <qpushbutton.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qdiriterator.h>

#include "Project.h"
#include "Camera.h"
#include "Draw.h"

CaptureScreenshotsWidget::CaptureScreenshotsWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Capture Screenshots"));

    QPushButton* browseButtonProjects = new QPushButton(tr("&Browse..."), this);
    connect(browseButtonProjects, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::browseProjectFiles);
    lineProjects = new QLineEdit(this);
    lineProjects->setReadOnly(true);

    QPushButton* browseButtonDir = new QPushButton(tr("&Browse..."), this);
    connect(browseButtonDir, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::browseMainDir);
    lineMainDir = new QLineEdit(this);
    lineMainDir->setReadOnly(true);

    QPushButton* clearButton = new QPushButton(tr("&Clear"), this);
    connect(clearButton, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::clearParameters);

    QPushButton* startButton = new QPushButton(tr("&Start capturing..."), this);
    connect(startButton, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::startCapturing);
    
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Select the projects files "
        "that will be used in the capturing process.")), 0, 0, 1, 3);
    mainLayout->addWidget(new QLabel(tr("Projects:")), 1, 0);
    mainLayout->addWidget(lineProjects, 1, 1);
    mainLayout->addWidget(browseButtonProjects, 1, 2);

    mainLayout->addWidget(new QLabel(tr("Select the base directory that has "
        "all the meshes to compare.")), 2, 0, 1, 3);
    mainLayout->addWidget(new QLabel(tr("Base dir:")), 3, 0);
    mainLayout->addWidget(lineMainDir, 3, 1);
    mainLayout->addWidget(browseButtonDir, 3, 2);

    mainLayout->addWidget(clearButton, 4, 1, Qt::AlignRight | Qt::AlignBottom);
    mainLayout->addWidget(startButton, 4, 2, Qt::AlignRight | Qt::AlignBottom);
    this->setLayout(mainLayout);
}

void CaptureScreenshotsWidget::browseProjectFiles()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Cameras files (*.json)"));
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec())
    {
        projectFiles = dialog.selectedFiles();
        std::stringstream s;
        for (const auto& camFile : projectFiles)
        {
            s << camFile.toStdString() << ";";
        }
        lineProjects->setText(s.str().c_str());
    }
}

void CaptureScreenshotsWidget::browseMainDir()
{
    mainDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    lineMainDir->setText(mainDir);
}

void CaptureScreenshotsWidget::startCapturing()
{
    if (projectFiles.size() == 0 || mainDir.isEmpty())
    {
        QMessageBox::warning(this, tr("MPCCT"),
            tr("You need to select the cameras and the base dir."));
        return;
    }
    // Loop trought projects
    for (const auto& projectFile : projectFiles)
    {
        Project project;
        if (!project.load(projectFile))
        {
            QMessageBox::warning(this, tr("MPCCT"),
                tr("Cannot open file %1.")
                .arg(QDir::toNativeSeparators(projectFile)));
            continue;
        }
        // Create the project directory
        auto plyFilename = project.getPlyFilename();
        QDir mainDirectory(mainDir);
        if (!mainDirectory.exists(plyFilename))
        {
            if (!mainDirectory.mkdir(plyFilename))
            {
                QMessageBox::critical(this, tr("MPCCT"),
                    tr("Error creating the folder %1.")
                    .arg(QDir::toNativeSeparators(
                        mainDir + "\\" + plyFilename)));
                return;
            }
        }
        // Create the cameras folders
        std::string projectDir = mainDir.toStdString() + "\\" + plyFilename.toStdString();
        QDir projectDirectory(projectDir.c_str());
        for (size_t i = 0; i < project.getNumberOfCameras(); i++)
        {
            std::string folderName = "camera_" + std::to_string(i);
            if (!projectDirectory.exists(folderName.c_str()))
            {
                if (!projectDirectory.mkdir(folderName.c_str()))
                {
                    QMessageBox::critical(this, tr("MPCCT"),
                        tr("Error creating the folder %1.")
                        .arg(QDir::toNativeSeparators(
                            mainDir + "\\" + plyFilename + "\\" + folderName.c_str())));
                    return;
                }
            }
        }
        // Create the offscreen renderer
        vtkNew<vtkRenderer> rendererOffScreen;
        vtkNew<vtkRenderWindow> renderWindowOffScreen;
        renderWindowOffScreen->SetOffScreenRendering(1);
        renderWindowOffScreen->AddRenderer(rendererOffScreen);
        // Find ply files
        QDirIterator plyFileIterator(mainDir, QStringList() << plyFilename, QDir::Files, QDirIterator::Subdirectories);
        // Loop through the ply files
        while (plyFileIterator.hasNext())
        {
            auto plyFile = plyFileIterator.next();
            auto actor = Draw::createActorFromPLY(plyFile.toStdString(), rendererOffScreen);
            if (!actor)
            {
                continue;
            }
            // Loop through the cameras and save screenshots
            const auto numberOfCameras = project.getNumberOfCameras();
            // Get the dir name
            QFileInfo fileInfo(plyFile);
            const auto dirName = fileInfo.absoluteDir().dirName().toStdString();
            for (size_t i = 0; i < numberOfCameras; i++)
            {
                const auto camera = project.getCamera(i);
                if (!camera)
                {
                    continue;
                }
                renderWindowOffScreen->SetSize(camera->getRenderWindowSize());
                rendererOffScreen->SetActiveCamera(camera->getvtkCamera());
                rendererOffScreen->ResetCameraClippingRange();
                renderWindowOffScreen->Render();
                
                takeScreenshot(renderWindowOffScreen,
                    projectDir + "\\camera_" + std::to_string(i) +
                    "\\" + dirName + ".png");
            }
            rendererOffScreen->RemoveActor(actor);
        }
    }
    QMessageBox::information(this, tr("MPCCT"),
        tr("Finished the capturing process."));
}

void CaptureScreenshotsWidget::clearParameters()
{
    projectFiles.clear();
    lineProjects->setText("");
    mainDir = "";
    lineMainDir->setText("");
}

void CaptureScreenshotsWidget::takeScreenshot(vtkSmartPointer<vtkRenderWindow> renderWindow, const std::string& filename, const bool setTransparentBackgroud)
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
