#pragma once
#include <vector>

#include <vtkSmartPointer.h>

#include <qstring.h>

class vtkCamera;
class QListWidgetItem;
class Camera;

class Project
{
public:
    Project();
	Project(QString plyFilename);
	~Project();

	bool load(const QString& filename);
	bool save(QString filename = "");
    // Check if the project is loaded from the disk or if it is new
    bool isOnDisk() { return !path.isEmpty(); };

    void addCamera(vtkSmartPointer<vtkCamera> camera, QListWidgetItem* item,
        int vtkRenderWindowSizeWidth, int vtkRenderWindowSizeHeight);
    void updateCamera(QListWidgetItem* selectedItem, vtkSmartPointer<vtkCamera> camera,
        int vtkRenderWindowSizeWidth, int vtkRenderWindowSizeHeight);
    bool removeCamera(QListWidgetItem* selectedItem);
    Camera* getCamera(int cameraIndex);
    Camera* getCamera(QListWidgetItem* selectedItem);
  
    QString getPlyFilename() { return plyFilename; };
    QString getPath() { return path; };
    size_t getNumberOfCameras() { return cameras.size(); };

private:
    // ply filename used to locate the ply during the capture process
    QString plyFilename;
    // The path to the .json file used to save the project
    QString path;
    // A vector to hold our cameras
    std::vector<Camera*> cameras;
    int findCameraIdByListItem(QListWidgetItem* selectedItem);
};