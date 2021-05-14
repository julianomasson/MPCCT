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

class Camera;
class CaptureScreenshotsWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
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
    void cameraDoubleClick(const QListWidgetItem* item);

private:
    void createActions();
    void createDockWindows();
    void createStatusBar();
    void takeScreenshot(vtkSmartPointer<vtkRenderWindow> renderWindow,
        const std::string& filename, const bool setTransparentBackgroud = false);
    void saveCameras(const std::string& filename);

    QVTKOpenGLNativeWidget* vtkWidget;
    vtkSmartPointer<vtkRenderer> renderer;
    QListWidget* camerasList;

    CaptureScreenshotsWidget* captureTool;

    // The path to the .json file used to save the cameras
    QString projectPath;
    // The size we should use in the offscreen render window
    // to preserve the aspect ratio used in the cameras definition
    QSize vtkOffscreenWindowSize;
    // A vector to hold our cameras
    std::vector<Camera*> cameras;
};