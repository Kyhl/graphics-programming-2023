#include "TerrainApplication.h"

#include <ituGL/asset/TextureCubemapLoader.h>
#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>

#include <ituGL/lighting/DirectionalLight.h>
#include <ituGL/lighting/PointLight.h>
#include <ituGL/scene/SceneLight.h>

#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/shader/Material.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/scene/SceneModel.h>

#include <ituGL/renderer/SkyboxRenderPass.h>
#include <ituGL/renderer/ForwardRenderPass.h>
#include <ituGL/scene/RendererSceneVisitor.h>

#include <ituGL/scene/ImGuiSceneVisitor.h>
#include <imgui.h>
#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>


#include <fstream>
#include <sstream>

#include <cmath>
#include <iostream>
#include <numbers>  // for PI constant
using namespace glm;
using namespace std;




struct Vertex
{
    vec3 position;
    vec2 texCoord;
    vec3 color;
    vec3 normal;
};

// Forward declare helper function
vec3 GetColorFromHeight(float height);


TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Exam kaky")
    , m_grid(256)
    , m_Mode(0)
    , m_Matrix(0)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();
    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    // Build shaders and store in m_shaderProgram
    BuildShaders();

    //InitializePlanet(vec3(0.0f), 1.0f);

    InitializePlanet(vec3(-0.75f,-0.0f,0.0f), 0.1f);


}

void TerrainApplication::Update()
{
    Application::Update();
    /*const Window& window = GetMainWindow();

    glm::vec2 mousePosition = window.GetMousePosition(true);
    m_camera.SetViewMatrix(glm::vec3(0.0f, 10.0f, 10.0f), glm::vec3(mousePosition, 0.0f));

    int width, height;
    window.GetDimensions(width, height);
    float aspectRatio = static_cast<float>(width) / height;
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, -10.1f, 200.0f);*/
    // Update camera controller
    m_cameraController.Update(GetMainWindow(), GetDeltaTime());

    // Add the scene nodes to the renderer
   /* RendererSceneVisitor rendererSceneVisitor(m_renderer);
    m_scene.AcceptVisitor(rendererSceneVisitor);*/
    UpdateOutputMode();
}
void TerrainApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Set shader to be used
    m_shaderProgram.Use();

    // Bind the grid VAO
    m_vao.Bind();

    // Draw the grid (m_gridX * m_gridY quads, 6 vertices per quad)
    glDrawElements(GL_TRIANGLES, ((m_grid * m_grid) * 6) * 6 * 2, GL_UNSIGNED_INT, nullptr);

    RenderGUI();
    // No need to unbind every time
    //VertexArrayObject::Unbind();
}
void TerrainApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void TerrainApplication::InitializeCamera()
{
    const Window& window = GetMainWindow();
    // Create the main camera
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->SetViewMatrix(glm::vec3(0, 10.0f, 10.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    int width, height;
    window.GetDimensions(width, height);
    float aspectRatio = static_cast<float>(width) / height;
    camera->SetPerspectiveProjectionMatrix(1.0f, aspectRatio, -10.1f, 200.0f);

    // Create a scene node for the camera
    std::shared_ptr<SceneCamera> sceneCamera = std::make_shared<SceneCamera>("camera", camera);

    // Add the camera node to the scene
    m_scene.AddSceneNode(sceneCamera);

    // Set the camera scene node to be controlled by the camera controller
    m_cameraController.SetCamera(sceneCamera);
}
void TerrainApplication::InitializePlanet(vec3 offset, float scaleSize)
{
    // Create containers for the vertex data
    std::vector<Vertex> vertices;

    // Create container for the element data
    std::vector<unsigned int> indices;

    // Grid scale to convert the entire grid to size 1x1
    float scale = (1.0f / m_grid);

    // Number of columns and rows
    unsigned int columnCount = m_grid + 1;
    unsigned int rowCount = m_grid + 1;
    float maxHeight = 0.05f;
    float lacunarity = 1.9f;
    float gain = 0.55f;
    float octaves = 8;
    float minVal = -0.2f;
    float maxVal = 1.0f;
    // Iterate over each VERTEX
    for (unsigned int u = 0; u < 6; u++)
    {
        for (unsigned int j = 0; j < rowCount; ++j)
        {
            for (unsigned int i = 0; i < columnCount; ++i)
            {
                Vertex& vertex = vertices.emplace_back();
                // -0.5f is the offset
                float x = i * scale  - 0.5f;
                float y = j * scale  - 0.5f;
                float noise;
                float z = 0.0f;
                switch (u)
                {
                case 5:
                    //Back
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(1.0f, 0.0f, 0.0f);
                    //the *0.5f is the scale
                    vertex.position = glm::normalize((vec3(z - 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 4:
                    //Front
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(1.0f, 0.0f, 0.0f);
                    vertex.position = glm::normalize((vec3(z + 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 3:
                    //Left
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(0.0f, 1.0f, 0.0f);
                    vertex.position = glm::normalize((vec3(x, z - 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 2:
                    //Bottom
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(0.0f, 0.0f, 1.0f);
                    vertex.position = glm::normalize((vec3(x, y, (z * -1) - 0.5f) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 1:
                    //Right
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(0.0f, 1.0f, 0.0f);
                    vertex.position = glm::normalize((vec3(x, z + 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 0:
                    //Top
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(0.0f, 0.0f, 1.0f);
                    vertex.position = glm::normalize((vec3(x, y, z + 0.5f) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight -offset;
                    break;
                }

                if (i > 0 && j > 0)
                {
                    unsigned int offset = rowCount * columnCount * u;
                    unsigned int top_right = (j * columnCount + i) + offset; // Current vertex
                    unsigned int top_left = top_right - 1;
                    unsigned int bottom_right = top_right - columnCount;
                    unsigned int bottom_left = bottom_right - 1;

                    //Triangle 1
                    indices.push_back(bottom_left);
                    indices.push_back(bottom_right);
                    indices.push_back(top_left);

                    //Triangle 2
                    indices.push_back(bottom_right);
                    indices.push_back(top_left);
                    indices.push_back(top_right);
                }
            }
        }
    }
    for (unsigned int u = 0; u < 6; u++)
    {
        for (unsigned int j = 0; j < rowCount; ++j)
        {
            for (unsigned int i = 0; i < columnCount; ++i)
            {
                // Get the vertex at (i, j)
                unsigned int offset = (rowCount * columnCount) * u;
                int index = j * columnCount + i + offset;
                Vertex& vertex = vertices[index];

                // Compute the delta in X
                unsigned int prevX = i > 0 ? index - 1 : index;
                unsigned int nextX = i < m_grid ? index + 1 : index;

                // Compute the delta in Y
                int prevY = j > 0 ? index - columnCount : index;
                int nextY = j < m_grid ? index + columnCount : index;

                vec3 deltaX = normalize(vertices[nextX].position - vertices[prevX].position);
                vec3 deltaY = normalize(vertices[nextY].position - vertices[prevY].position);

                vertex.normal = cross(deltaX, deltaY);
                if (sign(dot(vertex.position, vertex.normal)) < 0)
                {
                    vertex.normal = cross(deltaY, deltaX);
                }
            }
        }
    }

    //Second planet
    for (unsigned int u = 0; u < 6; u++)
    {
        for (unsigned int j = 0; j < rowCount; ++j)
        {
            for (unsigned int i = 0; i < columnCount; ++i)
            {
                Vertex& vertex = vertices.emplace_back();
                float x = i * scale - 0.5f;
                float y = j * scale - 0.5f;
                float noise;
                float z = 0.0f;
                switch (u)
                {
                case 5:
                    //Back
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(1.0f, 0.0f, 0.0f);
                    vertex.position = glm::normalize((vec3(z - 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * scaleSize;
                    noise = 0.0f;//clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = vec3(0.969, 0.839, 0.408);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight + offset;
                    break;
                case 4:
                    //Front
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(1.0f, 0.0f, 0.0f);
                    vertex.position = glm::normalize((vec3(z + 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * scaleSize;
                    noise = 0.0f;//clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = vec3(0.969, 0.839, 0.408);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight + offset;
                    break;
                case 3:
                    //Left
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(0.0f, 1.0f, 0.0f);
                    vertex.position = glm::normalize((vec3(x, z - 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * scaleSize;
                    noise = 0.0f;//clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = vec3(0.969, 0.839, 0.408);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight + offset;
                    break;
                case 2:
                    //Bottom
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(0.0f, 0.0f, 1.0f);
                    vertex.position = glm::normalize((vec3(x, y, (z * -1) - 0.5f) * 2.0f) / scale - vec3(1.0f)) * scaleSize;
                    noise = 0.0f;//clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = vec3(0.969, 0.839, 0.408);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight + offset;
                    break;
                case 1:
                    //Right
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(0.0f, 1.0f, 0.0f);
                    vertex.position = glm::normalize((vec3(x, z + 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * scaleSize;
                    noise = 0.0f;//clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = vec3(0.969, 0.839, 0.408);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight + offset;
                    break;
                case 0:
                    //Top
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.normal = vec3(0.0f, 0.0f, 1.0f);
                    vertex.position = glm::normalize((vec3(x, y, z + 0.5f) * 2.0f) / scale - vec3(1.0f)) * scaleSize;
                    noise = 0.0f;//clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = vec3(0.969, 0.839, 0.408);
                    vertex.position += vertex.position * 2.0f * noise * maxHeight + offset;
                    break;
                }

                if (i > 0 && j > 0)
                {
                    unsigned int offset = rowCount * columnCount * (u + 6);
                    unsigned int top_right = (j * columnCount + i) + offset; // Current vertex
                    unsigned int top_left = top_right - 1;
                    unsigned int bottom_right = top_right - columnCount;
                    unsigned int bottom_left = bottom_right - 1;

                    //Triangle 1
                    indices.push_back(bottom_left);
                    indices.push_back(bottom_right);
                    indices.push_back(top_left);

                    //Triangle 2
                    indices.push_back(bottom_right);
                    indices.push_back(top_left);
                    indices.push_back(top_right);
                }
            }
        }
    }
    for (unsigned int u = 0; u < 6; u++)
    {
        for (unsigned int j = 0; j < rowCount; ++j)
        {
            for (unsigned int i = 0; i < columnCount; ++i)
            {
                // Get the vertex at (i, j)
                unsigned int offset = (rowCount * columnCount) * (u + 6);
                int index = j * columnCount + i + offset;
                Vertex& vertex = vertices[index];

                // Compute the delta in X
                unsigned int prevX = i > 0 ? index - 1 : index;
                unsigned int nextX = i < m_grid ? index + 1 : index;

                // Compute the delta in Y
                int prevY = j > 0 ? index - columnCount : index;
                int nextY = j < m_grid ? index + columnCount : index;

                vec3 deltaX = normalize(vertices[nextX].position - vertices[prevX].position);
                vec3 deltaY = normalize(vertices[nextY].position - vertices[prevY].position);

                vertex.normal = cross(deltaX, deltaY);
                if (sign(dot(vertex.position, vertex.normal)) <= 0)
                {
                    vertex.normal = cross(deltaY, deltaX);
                }
            }
        }
    }

    





    // Declare attributes
    VertexAttribute positionAttribute(Data::Type::Float, 3);
    VertexAttribute texCoordAttribute(Data::Type::Float, 2);
    VertexAttribute colorAttribute(Data::Type::Float, 3);
    VertexAttribute normalAttribute(Data::Type::Float, 3);

    // Compute offsets inside the VERTEX STRUCT
    size_t positionOffset = 0u;
    size_t texCoordOffset = positionOffset + positionAttribute.GetSize();
    size_t colorOffset = texCoordOffset + texCoordAttribute.GetSize();
    size_t normalOffset = colorOffset + colorAttribute.GetSize();

    // Allocate uninitialized data for the total size in the VBO
    m_vbo.Bind();
    m_vbo.AllocateData(std::span(vertices));

    // The stride is not automatic now. Each attribute element is "sizeof(Vertex)" bytes apart from next
    GLsizei stride = sizeof(Vertex);

    // Set the pointer to the data in the VAO (notice that this offsets are for a single element)
    m_vao.Bind();
    m_vao.SetAttribute(0, positionAttribute, static_cast<GLint>(positionOffset), stride);
    m_vao.SetAttribute(1, texCoordAttribute, static_cast<GLint>(texCoordOffset), stride);
    m_vao.SetAttribute(2, colorAttribute, static_cast<GLint>(colorOffset), stride);
    m_vao.SetAttribute(3, normalAttribute, static_cast<GLint>(normalOffset), stride);

    // With VAO bound, bind EBO to register it (and allocate element buffer at the same time)
    m_ebo.Bind();
    m_ebo.AllocateData(std::span(indices));

    // Unbind VAO, and VBO
    VertexBufferObject::Unbind();
    VertexArrayObject::Unbind();

    // Unbind EBO (when VAO is no longer bound)
    ElementBufferObject::Unbind();

    // Enable wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
}
vec3 GetColorFromHeight(float height)
{
    if (height > 0.47f)
    {
        return vec3(1.0f, 1.0f, 1.0f); // Snow
    }
    else if (height > 0.15f)
    {
        return vec3(0.3f, 0.3f, 0.35f); // Rock
    }
    else if (height > -0.08f)
    {
        return vec3(0.1f, 0.4f, 0.15f); // Grass
    }
    else if (height > -0.12f)
    {
        return vec3(0.6f, 0.5f, 0.4f); // Sand
    }
    else
    {
        return vec3(0.1f, 0.1f, 0.3f); // Water
    }
}

void TerrainApplication::BuildShaders()
{
    //const char* vertexShaderSource = "#version 330 core\n"
    //    "layout (location = 0) in vec3 aPos;\n"
    //    "layout (location = 1) in vec2 aTexCoord;\n"
    //    "layout (location = 2) in vec3 aColor;\n"
    //    "layout (location = 3) in vec3 aNormal;\n"
    //    "uniform mat4 Matrix = mat4(1);\n"
    //    "out vec2 texCoord;\n"
    //    "out vec3 color;\n"
    //    "out vec3 normal;\n"
    //    "void main()\n"
    //    "{\n"
    //    "   texCoord = aTexCoord;\n"
    //    "   color = aColor;\n"
    //    "   normal = aNormal;\n"
    //    "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    //    "}\0";
    //const char* fragmentShaderSource = "#version 330 core\n"
    //    "uniform uint Mode = 0u;\n"
    //    "in vec2 texCoord;\n"
    //    "in vec3 color;\n"
    //    "in vec3 normal;\n"
    //    "out vec4 FragColor;\n"
    //    "void main()\n"
    //    "{\n"
    //    "   switch (Mode)\n"
    //    "   {\n"
    //    "   default:\n"
    //    "   case 0u:\n"
    //    "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    //    "       break;\n"
    //    "   case 1u:\n"
    //    "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
    //    "       break;\n"
    //    "   case 2u:\n"
    //    "       FragColor = vec4(color, 1.0f);\n"
    //    "       break;\n"
    //    "   case 3u:\n"
    //    "       FragColor = vec4(normalize(normal), 1.0f);\n"
    //    "       break;\n"
    //    "   case 4u:\n"
    //    "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
    //    "       break;\n"
    //    "   }\n"
    //    "}\n\0";

    //// vertex shader
    //unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    //glCompileShader(vertexShader);
    //// check for shader compile errors
    //int success;
    //char infoLog[512];
    //glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    //if (!success)
    //{
    //    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    //    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    //}
    //// fragment shader
    //unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    //glCompileShader(fragmentShader);
    //// check for shader compile errors
    //glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    //if (!success)
    //{
    //    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    //    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    //}
    //// link shaders
    //unsigned int shaderProgram = glCreateProgram();
    //glAttachShader(shaderProgram, vertexShader);
    //glAttachShader(shaderProgram, fragmentShader);
    //glLinkProgram(shaderProgram);
    //// check for linking errors
    //glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    //if (!success) {
    //    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    //    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    //}

    // Load and compile vertex shader
    Shader vertexShader(Shader::VertexShader);
    LoadAndCompileShader(vertexShader, "shaders/default.vert");

    // Load and compile fragment shader
    Shader fragmentShader(Shader::FragmentShader);
    LoadAndCompileShader(fragmentShader, "shaders/default.frag");

    // Attach shaders and link
    if (!m_shaderProgram.Build(vertexShader, fragmentShader))
    {
        std::cout << "Error linking shaders" << std::endl;
    }
    m_Mode = m_shaderProgram.GetUniformLocation("Mode");

    m_Matrix = m_shaderProgram.GetUniformLocation("Matrix");
    //glDeleteShader(vertexShader);
    //glDeleteShader(fragmentShader);
    /*m_shaderProgram = shaderProgram;*/
}
void TerrainApplication::LoadAndCompileShader(Shader& shader, const char* path)
{
    // Open the file for reading
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "Can't find file: " << path << std::endl;
        std::cout << "Is your working directory properly set?" << std::endl;
        return;
    }

    // Dump the contents into a string
    std::stringstream stringStream;
    stringStream << file.rdbuf() << '\0';

    // Set the source code from the string
    shader.SetSource(stringStream.str().c_str());

    // Try to compile
    if (!shader.Compile())
    {
        // Get errors in case of failure
        std::array<char, 256> errors;
        shader.GetCompilationErrors(errors);
        std::cout << "Error compiling shader: " << path << std::endl;
        std::cout << errors.data() << std::endl;
    }
}

void TerrainApplication::UpdateOutputMode()
{
    for (unsigned int i = 0u; i <= 4u; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            /*int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);*/
            m_shaderProgram.SetUniform(m_Mode, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        //int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        //m_shaderProgram.SetUniform(m_Matrix, projMatrix);
        //glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }
}
void TerrainApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    // Draw GUI for scene nodes, using the visitor pattern
    ImGuiSceneVisitor imGuiVisitor(m_imGui, "Scene");
    m_scene.AcceptVisitor(imGuiVisitor);

    // Draw GUI for camera controller
    m_cameraController.DrawGUI(m_imGui);

    m_imGui.EndFrame();
}
