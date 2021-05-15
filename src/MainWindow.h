#pragma once
#include <vector>

#include <QMainWindow>

#include <vtkSmartPointer.h>

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QListWidgetItem;
QT_END_NAMESPACE

class QVTKOpenGLNativeWidget;
class vtkRenderer;
class vtkRenderWindow;

class Project;
class CaptureScreenshotsWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void newProject();
    void open();
    void save();
    void saveAs();
    // Load mesh/point cloud
    void load();
    void addCamera();
    void updateCamera();
    void removeSelectedCamera();
    void captureScreenshots();
    void about();
    // Go to the selected camera
    void cameraDoubleClick(QListWidgetItem* item);

private:
    void createActions();
    void createDockWindows();
    void createStatusBar();
    void takeScreenshot(vtkSmartPointer<vtkRenderWindow> renderWindow,
        const std::string& filename, const bool setTransparentBackgroud = false);
    void setEnabledProjectActions(bool enable);

    QVTKOpenGLNativeWidget* vtkWidget;
    vtkSmartPointer<vtkRenderer> renderer;
    QListWidget* camerasList;

    CaptureScreenshotsWidget* captureTool;

    // A vector of actions we should disable
    // if we do not have a project
    std::vector<QAction*> actions;

    // Project that will hold the cameras
    Project* project;
};