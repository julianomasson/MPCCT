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
class vtkActor;

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
    void addCamera();
    void updateCamera();
    void removeSelectedCamera();
    void about();
    // Go to the selected camera
    void cameraDoubleClick(QListWidgetItem* item);

private:
    void createActions();
    void createDockWindows();
    void createStatusBar();
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
    // The mesh/point cloud actor
    vtkSmartPointer<vtkActor> actor;
};