# Software Renderer


1. Programming language:  **C/C++**
2. Target Platform: **Windows**

Since i always wanted to know more about computer graphics and after reading [Eric Arneback's article](https://erkaman.github.io/posts/beginner_computer_graphics.html) and checking [Dmitry Sokolov's project](https://github.com/ssloy/tinyrenderer), i decided to create my own software rasterizer. I will try to minimize external dependencies to learn the concepts needed from first principles. That said, i am not going to use any graphics API (such as Opengl) or any library for Opengl development (such as glew or glfw). Along the way, i will try to upload here news, pictures describing the current state of the rasterizer. Something like a development diary.

The initial goals/steps of this project are the following:

1) Create a Windows platform layer (create a window, draw on it, handle user input etc.)
2) Learn basic computer graphics concepts in theory and practice
3) Maximize its performance (i.e. frames per second)
4) Get familiar with git

## Update 1



<p align="center">
    <img src="https://user-images.githubusercontent.com/33458365/86264347-0da69e80-bbcb-11ea-854d-43e7f0e50154.png" >
    <em>Figure 1. Draw lines, show FPS and Megacycles per frame</em>
</p>
