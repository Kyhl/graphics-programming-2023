#include <ituGL/application/Window.h>

// Create the internal GLFW window. We provide some hints about it to OpenGL
Window::Window(int width, int height, const char* title) : m_window(nullptr)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
}

// If we have an internal GLFW window, destroy it
Window::~Window()
{
}

// Get the current dimensions (width and height) of the window
void Window::GetDimensions(int& width, int& height) const
{
}

// Tell the window that it should close
void Window::Close()
{
    glfwSetWindowShouldClose(m_window,true);
}

// Get if the window should be closed this frame
bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

// Swaps the front and back buffers of the window
void Window::SwapBuffers()

{
    glfwSwapBuffers(m_window);
}
