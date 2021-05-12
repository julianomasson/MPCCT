#pragma once

#include <vtkSmartPointer.h>

class QListWidgetItem;
class vtkCamera;

class Camera
{
public:
	Camera(vtkSmartPointer<vtkCamera> camera, QListWidgetItem* item);
	Camera();
	~Camera();

	void SetCamera(vtkSmartPointer<vtkCamera> camera);
	void SetListItem(QListWidgetItem* item);

	vtkSmartPointer<vtkCamera> GetCamera() { return camera; };
	QListWidgetItem* GetListItem() { return item; };

private:
	vtkSmartPointer<vtkCamera> camera;
	QListWidgetItem* item;
};