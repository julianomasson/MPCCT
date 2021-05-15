#include "Project.h"

#include <vtkSmartPointer.h>
#include <vtkCamera.h>

#include <qlistwidget.h>

#include "json.hpp"
#include "Camera.h"

Project::Project() : path("")
{
}

Project::~Project()
{
    cameras.clear();
}

bool Project::load(const QString& filename)
{
    nlohmann::json jsonFile;
    try
    {
        std::ifstream parametersFile(filename.toStdString());
        if (!parametersFile.is_open())
        {
            return false;
        }
        jsonFile = nlohmann::json::parse(parametersFile);
    }
    catch (const std::exception&)
    {
        return false;
    }
    // Load cameras
    for (auto& [key, value] : jsonFile.items())
    {
        if (value["type"] == "Camera")
        {
            vtkNew<vtkCamera> vtkCam;
            vtkCam->SetPosition(value["position"][0],
                value["position"][1],
                value["position"][2]);
            vtkCam->SetFocalPoint(value["focalPoint"][0],
                value["focalPoint"][1],
                value["focalPoint"][2]);
            vtkCam->SetViewUp(value["viewUp"][0],
                value["viewUp"][1],
                value["viewUp"][2]);
            vtkCam->SetViewAngle(value["viewAngle"]);
            QListWidgetItem* listItem = new QListWidgetItem("Camera");
            cameras.emplace_back(
                new Camera(vtkCam, listItem, value["width"], value["height"])
            );
        }
    }
    path = filename;
    return true;
}

bool Project::save(QString filename)
{
    if (filename.isEmpty() && path.isEmpty())
    {
        return false;
    }
    if (filename.isEmpty())
    {
        filename = path;
    }
    nlohmann::json jsonFile;
    // Add the cameras
    int idx = 0;
    for (const auto& camera : cameras)
    {
        const auto vtkCam = camera->getCamera();
        std::stringstream s;
        s << "Camera" << idx;
        jsonFile[s.str()] = {
            { "type", "Camera"},
            { "width", camera->getRenderWindowSize()[0]},
            { "height", camera->getRenderWindowSize()[1]},
            { "position",  {vtkCam->GetPosition()[0],
                            vtkCam->GetPosition()[1],
                            vtkCam->GetPosition()[2]}
            },
            { "focalPoint",{vtkCam->GetFocalPoint()[0],
                            vtkCam->GetFocalPoint()[1],
                            vtkCam->GetFocalPoint()[2]}
            },
            { "viewUp",    {vtkCam->GetViewUp()[0],
                            vtkCam->GetViewUp()[1],
                            vtkCam->GetViewUp()[2]}
            },
            { "viewAngle", vtkCam->GetViewAngle()}
        };
        idx++;
    }
    std::ofstream output;
    output.open(filename.toStdString());
    if (!output.is_open())
    {
        return false;
    }
    output << std::setw(4) << jsonFile;
    output.close();
    path = filename;
    return true;
}

void Project::addCamera(vtkSmartPointer<vtkCamera> camera, QListWidgetItem* item,
    int vtkRenderWindowSizeWidth, int vtkRenderWindowSizeHeight)
{
    cameras.emplace_back(new Camera(camera, item,
        vtkRenderWindowSizeWidth, vtkRenderWindowSizeHeight));
}

void Project::updateCamera(QListWidgetItem* selectedItem, vtkSmartPointer<vtkCamera> camera, int vtkRenderWindowSizeWidth, int vtkRenderWindowSizeHeight)
{
    auto id = findCameraIdByListItem(selectedItem);
    if (id != -1)
    {
        cameras[id]->setCamera(camera);
        cameras[id]->setRenderWindowSize(
            vtkRenderWindowSizeWidth, vtkRenderWindowSizeHeight);
    }
}

bool Project::removeCamera(QListWidgetItem* selectedItem)
{
    auto id = findCameraIdByListItem(selectedItem);
    if (id != -1)
    {
        cameras.erase(cameras.begin() + id);
        return true;
    }
    return false;
}

Camera* Project::getCamera(int cameraIndex)
{
    if (cameraIndex != -1 && cameraIndex < cameras.size())
    {
        return cameras[cameraIndex];
    }
    return nullptr;
}

Camera* Project::getCamera(QListWidgetItem* selectedItem)
{
    auto id = findCameraIdByListItem(selectedItem);
    return getCamera(id);
}

int Project::findCameraIdByListItem(QListWidgetItem* selectedItem)
{
    const auto numberOfCameras = cameras.size();
    for (size_t i = 0; i < numberOfCameras; i++)
    {
        if (selectedItem == cameras[i]->getListItem())
        {
            return i;
        }
    }
    return -1;
}
