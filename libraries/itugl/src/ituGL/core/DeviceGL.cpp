#include <ituGL/core/DeviceGL.h>

#include <ituGL/application/Window.h>
#include <GLFW/glfw3.h>
#include <cassert>

DeviceGL* DeviceGL::m_instance = nullptr;

DeviceGL::DeviceGL() : m_contextLoaded(false)
{
    glfwInit();
}

DeviceGL::~DeviceGL()
{
    glfwTerminate();
}

// Set the window that OpenGL will use for rendering
void DeviceGL::SetCurrentWindow(Window& window)
{
    GLFWwindow* glfwWindow = window.GetInternalWindow();
    glfwMakeContextCurrent(glfwWindow);
    glfwSetFramebufferSizeCallback(glfwWindow, FrameBufferResized);
    m_contextLoaded = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
}

// Set the dimensions of the viewport
void DeviceGL::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
}

// Poll the events in the window event queue
void DeviceGL::PollEvents()
{
    glfwPollEvents();
}

// Clear the framebuffer with the specified color
void DeviceGL::Clear(float r, float g, float b, float a)
{
}

// Callback called when the framebuffer changes size
void DeviceGL::FrameBufferResized(GLFWwindow* window, GLsizei width, GLsizei height)
{
}
