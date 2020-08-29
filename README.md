# Software Renderer


1. Programming language:  **C/C++**
2. Target Platform: **Windows**
3. Status: **Ongoing**

Since i always wanted to know more about computer graphics and after reading [Eric Arneback's article](https://erkaman.github.io/posts/beginner_computer_graphics.html) and checking [Dmitry Sokolov's project](https://github.com/ssloy/tinyrenderer), i decided to create my own software rasterizer. I will try to minimize external dependencies to learn the concepts needed from first principles. That said, i am not going to use any graphics API (such as Opengl) or any library for Opengl development (such as glew or glfw). Along the way, i will try to upload here news, pictures describing the current state of the rasterizer as well as the references that i used to achieve certain functionalities. Something like a development diary.

The initial goals/steps of this project are the following:

1) Create a Windows platform layer (create a window, draw on it, handle user input etc.)
2) Learn basic computer graphics concepts in theory and practice
3) Maximize its performance (i.e. frames per second)
4) Get familiar with git and some C++ features

General Notes:
1) FPS shown below are from the Debug-x86 version (not a release build)

## Update 1
<p align="center">
    <img src="https://user-images.githubusercontent.com/33458365/86264347-0da69e80-bbcb-11ea-854d-43e7f0e50154.png" >
    <em>Figure 1. Draw lines, show FPS and Megacycles per frame</em>
</p>

## Update 2
<p align="center">
    <img src="https://user-images.githubusercontent.com/33458365/91637633-85017e80-ea12-11ea-89d4-ef1d20db5115.png" >
    <em>Figure 2. Render an OBJ file</em>
</p>

## Update 3
<p align="center">
    <img src="https://user-images.githubusercontent.com/33458365/91637661-b7ab7700-ea12-11ea-9a2b-19e5936cb73a.png" >
    <em>Figure 3. Scanline rasterization</em>
</p>

## References

### Computer Graphics Courses
1) [TinyRenderer](https://github.com/ssloy/tinyrenderer)
1) [Introduction to Computer Graphics MIT](https://www.youtube.com/watch?v=t7g2oaNs-c8&list=PLQ3UicqQtfNuKZjdA3fY1_X9gXn13JLlW)

### Windows Platform Layer
1) [msdn](https://docs.microsoft.com/en-us/windows/win32/desktop-programming)
2) [handmadehero](https://hero.handmade.network/episode/code)

### Math Library
1) foundations of game engine development volume 1 ([amazon.com](https://www.amazon.com/Foundations-Game-Engine-Development-Mathematics/dp/0985811749))
2) Mathematics for 3D Game Programming and Computer Graphics, Third Edition ([amazon.com](https://www.amazon.com/Mathematics-Programming-Computer-Graphics-Third/dp/1435458869/ref=pd_lpo_14_img_2/142-8654584-2538444?_encoding=UTF8&pd_rd_i=1435458869&pd_rd_r=eac40526-d2f1-410a-b352-692446c66803&pd_rd_w=STsqN&pd_rd_wg=pSwos&pf_rd_p=7b36d496-f366-4631-94d3-61b87b52511b&pf_rd_r=ACB5T0CG0TSWBCAMT7TY&psc=1&refRID=ACB5T0CG0TSWBCAMT7TY))
3) [On Vector Math Libraries, Nathan Reed](http://www.reedbeta.com/blog/on-vector-math-libraries/)
