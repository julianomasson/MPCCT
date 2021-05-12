#include "Camera.h"

#include <vtkCamera.h>

#include <qlistwidget.h>


Camera::Camera(vtkSmartPointer<vtkCamera> camera, QListWidgetItem* item)
{
	SetCamera(camera);
	SetListItem(item);
}

Camera::Camera()
{
	camera = nullptr;
	item = nullptr;
}

Camera::~Camera()
{
}

void Camera::SetCamera(vtkSmartPointer<vtkCamera> camera)
{
	if (!camera)
	{
		return;
	}
	this->camera = vtkSmartPointer<vtkCamera>::New();
	this->camera->DeepCopy(camera);
}

void Camera::SetListItem(QListWidgetItem* item)
{
	if (!item)
	{
		return;
	}
	this->item = item;
}
