#pragma once

#include <vtkSmartPointer.h>

class QListWidgetItem;
class vtkCamera;

class Camera
{
public:
	Camera(vtkSmartPointer<vtkCamera> camera, QListWidgetItem* item,
		int vtkRenderWindowSizeWidth, int vtkRenderWindowSizeHeight);
	Camera();
	~Camera();

	void setCamera(vtkSmartPointer<vtkCamera> camera);
	void setListItem(QListWidgetItem* item);
	void setRenderWindowSize(int width, int height);

	vtkSmartPointer<vtkCamera> getCamera() { return camera; };
	QListWidgetItem* getListItem() { return item; };
	int* getRenderWindowSize() { return vtkRenderWindowSize; };

private:
	vtkSmartPointer<vtkCamera> camera;
	QListWidgetItem* item;
	// The size we should use in the offscreen render window
	// to preserve the aspect ratio used in the camera definition
	int vtkRenderWindowSize[2];
};