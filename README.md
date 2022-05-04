# 3D Model Viewer

This application allows the user to load .obj 3D model files and view them on their screen. The users can change the camera position along the x,y
 and z coordinates to get different viewing angles. The center of mass of the model is situated at the origin and the camera is always pointed towards 
 the origin. The user can also input a function of x and y to get a Non-Linear projection. 
 
 Linear Algebra and perspective projection were used to get the viewport coordinates of the model. First the global coordinates of the model are 
 converted into the camera coordinate system using a transformation matrix. Then the concept of similar triangles are applied to get the cooridnates 
 on the image plane. These coordinates are then converted to viewport coordinates.
 
 This application has been developed with C++, using the gtkmm-3.0 library for the GUI, tinyexpr library to parse the function in x and y, and the 
 cairomm library to draw on the canvas.
 
 ![linear view](https://raw.githubusercontent.com/ayugupt/3D_ComputerGraphics/master/images/linear.png)
 
 Linear view of a cube
 
 ![non_linear_view](https://raw.githubusercontent.com/ayugupt/3D_ComputerGraphics/master/images/non_linear.png)
 
 Non linear view of a cube
 
 ![cylinder](https://raw.githubusercontent.com/ayugupt/3D_ComputerGraphics/master/images/cylinder.png)
 
 Linear view of a cylinder
