# MPCCT - Mesh/Point Cloud Comparison Tool

The objective of this project is to capture screenshots of multiple meshes/point clouds of the same scene reconstructed with different algorithms. This allow you to compare them with the same point of view without the need to open all at once. 

To use the software you need to have the base dir with the following arrangement (all the meshes/point clouds should be in ply format):  

- Base dir
    - Algorithm_1
         - mesh0.ply
         - mesh1.ply
    - Algorithm_2
         - mesh0.ply
         - mesh1.ply
    - Algorithm_3
         - mesh0.ply
         - mesh1.ply

         
### How to use? ###

1. Create a project using the "New project.." button;  
     - Select the desired mesh/point cloud to load;  
2. Move the camera to the desired point of view and press the "Add camera" button;  
3. Repeat the process until you added all the cameras you want;  
4. You can double click the cameras in the camera list to check the point of view, and if it's wrong you can update or delete the camera;
5. Save the project;
6. Load the project with the capture tool;
7. Select the base dir;
8. Press the "Start capturing..." button.

### How do I get set up? ###

Use Visual Studio 2019 or newer and CMake >= 3.14.

Libraries  
1 - Qt >= 5.15.2.  
2 - VTK >= 9.0.1 - You need to build it with Qt.  

### TODO ###

- [x] Basic version
- [ ] Add a program icon
- [ ] Add a progress bar in the capturing process
- [ ] Draw the cameras
- [ ] Create an installer