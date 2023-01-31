#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <iostream>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/ElementBufferObject.h>
#define _USE_MATH_DEFINES
#include <cmath>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(Window& window);
unsigned int buildShaderProgram();
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    DeviceGL DGL;
    Window window(SCR_WIDTH,SCR_HEIGHT,"LearnOpenGL");
    if (!window.IsValid())
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    DGL.SetCurrentWindow(window);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!DGL.IsReady())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    unsigned int shaderProgram = buildShaderProgram();
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            //Triangle #1
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.5f,  0.5f, 0.0f,  // top

            //Triangle #2
        -0.5f, 0.5f, 0.0f, // right



    };
    unsigned int triangles[] = {
            0,1,2,
            2,0,3
    };

    VertexBufferObject VBO;
    VertexArrayObject VAO;
    ElementBufferObject EBO;


    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    VAO.Bind();
    VBO.Bind();
    EBO.Bind();

    int vertCount = sizeof(vertices)/sizeof(float);
    int triCount = sizeof(triangles)/sizeof(uint32_t);

    EBO.AllocateData(std::as_bytes(std::span(triangles,triCount)));
    VBO.AllocateData(std::as_bytes(std::span(vertices,vertCount)));

    VertexAttribute position(Data::Type::Float,3);
    VAO.SetAttribute(0,position,0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    VBO.Unbind();
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    VAO.Unbind();
    EBO.Unbind();

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float time = 0.5;
    float angle = time * (M_PI/180);
    // render loop
    // -----------


    while (!window.ShouldClose())
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 1.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        VAO.Bind();
        VBO.Bind();
        EBO.Bind();
        time += 0.1;
        vertices[0] = sqrt(2)/2*std::sin((45+time) * (M_PI/180));
        vertices[1] =  sqrt(2)/2*std::cos((45+time) * (M_PI/180));
        vertices[3] =  sqrt(2)/2*std::sin((135+time) * (M_PI/180));
        vertices[4] =  sqrt(2)/2*std::cos((135+time) * (M_PI/180));
        vertices[6] =  sqrt(2)/2*std::sin((225+time) * (M_PI/180));
        vertices[7] =  sqrt(2)/2*std::cos((225+time) * (M_PI/180));
        vertices[9] =  sqrt(2)/2*std::sin((315+time) * (M_PI/180));
        vertices[10] = sqrt(2)/2*std::cos((315+time) * (M_PI/180));

        VBO.UpdateData((std::span(vertices,vertCount)),0);
        // draw our first triangle
        glUseProgram(shaderProgram);
         // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES,triCount,GL_UNSIGNED_INT,0);
        // glBindVertexArray(0); // no need to unbind it every time 

        angle = time * (M_PI/180);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        window.SwapBuffers();
        DGL.PollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(Window& window)
{
    if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        window.Close();
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
unsigned int buildShaderProgram()
{
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}