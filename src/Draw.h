#pragma once
#include <vtkSmartPointer.h>

class vtkActor;
class vtkRenderer;

class Draw
{
public:
	Draw();
	~Draw();

	static vtkSmartPointer<vtkActor> createActorFromPLY(const std::string& filename, vtkSmartPointer<vtkRenderer> renderer);

};