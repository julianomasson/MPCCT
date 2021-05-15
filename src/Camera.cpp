#include "Camera.h"

#include <vtkCamera.h>

#include <qlistwidget.h>

Camera::Camera(vtkSmartPointer<vtkCamera> camera, QListWidgetItem* item,
	int vtkRenderWindowSizeWidth, int vtkRenderWindowSizeHeight)
{
	setCamera(camera);
	setListItem(item);
	setRenderWindowSize(vtkRenderWindowSizeWidth, vtkRenderWindowSizeHeight);
}

Camera::Camera()
{
	camera = nullptr;
	item = nullptr;
	setRenderWindowSize(0, 0);
}

Camera::~Camera()
{
}

void Camera::setCamera(vtkSmartPointer<vtkCamera> camera)
{
	if (!camera)
	{
		return;
	}
	this->camera = vtkSmartPointer<vtkCamera>::New();
	this->camera->DeepCopy(camera);
}

void Camera::setListItem(QListWidgetItem* item)
{
	if (!item)
	{
		return;
	}
	this->item = item;
}

void Camera::setRenderWindowSize(int width, int height)
{
	this->vtkRenderWindowSize[0] = width;
	this->vtkRenderWindowSize[1] = height;
}
