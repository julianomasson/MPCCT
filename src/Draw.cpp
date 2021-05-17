#include "Draw.h"

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPLYReader.h>
#include <vtkRenderer.h>

vtkSmartPointer<vtkActor> Draw::createActorFromPLY(const std::string& filename,
    vtkSmartPointer<vtkRenderer> renderer)
{
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(filename.c_str());
    reader->Update();
    auto polyData = reader->GetOutput();
    if (!polyData)
    {
        return nullptr;
    }
    vtkNew<vtkPolyDataMapper> mapper;
    if (polyData->GetPolys()->GetNumberOfCells() == 0)//it is a point cloud
    {
        vtkNew<vtkCellArray> vertices;
        vertices->InsertNextCell(polyData->GetPoints()->GetNumberOfPoints());
        auto numberOfPoints = polyData->GetPoints()->GetNumberOfPoints();
        for (vtkIdType i = 0; i < numberOfPoints; i++)
        {
            vertices->InsertCellPoint(i);
        }
        polyData->SetVerts(vertices);
        mapper->ScalarVisibilityOn();
        mapper->SetScalarModeToUsePointData();
    }
    mapper->SetInputData(polyData);
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    renderer->AddActor(actor);
    return actor;
}