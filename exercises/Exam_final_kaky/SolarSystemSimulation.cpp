#include "SolarSystemSimulation.h"

#include <ituGL/asset/TextureCubemapLoader.h>
#include <ituGL/geometry/VertexFormat.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>
#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/shader/Material.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/scene/SceneModel.h>

#include <ituGL/renderer/SkyboxRenderPass.h>
#include <ituGL/renderer/ForwardRenderPass.h>
#include <ituGL/scene/RendererSceneVisitor.h>
#include <ituGL/scene/Transform.h>
#include <ituGL/scene/ImGuiSceneVisitor.h>
#include <imgui.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#include <stb_image.h>

using namespace std;
using namespace glm;
SolarSystemSimulation::SolarSystemSimulation()
    : Application(1024, 1024, "Planet Scene Exam")
    , m_renderer(GetDevice())
    , m_enablePlanetRotation(true)
    , m_planetRotationSpeed(1.0f)
{
}
struct Vertex
{
    vec3 position;
    vec2 texCoord;
    vec3 color;
    vec3 normal;
};

void SolarSystemSimulation::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());
    InitializeCamera();
    InitializeMaterial();
    InitializeModels();
    InitializeRenderer();
}

void SolarSystemSimulation::Update()
{
    Application::Update();
    UpdateOutputMode();
    // Update camera controller
    m_cameraController.Update(GetMainWindow(), GetDeltaTime());

    if (m_enablePlanetRotation)
    {
        m_scene.GetSceneNode("Earth")->GetTransform()->SetRotation(m_scene.GetSceneNode("Earth")->GetTransform()->GetRotation() + vec3(0.0f, m_planetRotationSpeed * 0.001f, 0.0f));

        m_scene.GetSceneNode("Mars")->GetTransform()->SetRotation(m_scene.GetSceneNode("Mars")->GetTransform()->GetRotation() + vec3(0.0f, m_planetRotationSpeed * -0.002f, 0.0f));
    }

    // Add the scene nodes to the renderer
    RendererSceneVisitor rendererSceneVisitor(m_renderer);
    m_scene.AcceptVisitor(rendererSceneVisitor);
}

void SolarSystemSimulation::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Render the scene
    m_renderer.Render();

    // Render the debug user interface
    RenderGUI();
}

void SolarSystemSimulation::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void SolarSystemSimulation::InitializeCamera()
{
    // Create the main camera
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->SetViewMatrix(glm::vec3(-5, 5, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera->SetPerspectiveProjectionMatrix(1.0f, 1.0f, 0.1f, 100.0f);

    // Create a scene node for the camera
    std::shared_ptr<SceneCamera> sceneCamera = std::make_shared<SceneCamera>("camera", camera);

    // Add the camera node to the scene
    m_scene.AddSceneNode(sceneCamera);

    // Set the camera scene node to be controlled by the camera controller
    m_cameraController.SetCamera(sceneCamera);
}


void SolarSystemSimulation::InitializeMaterial()
{

    Shader planetVertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/planet/default.vert");
    Shader planetFragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/planet/default.frag");

    std::shared_ptr<ShaderProgram> planetShaderProgramPtr = std::make_shared<ShaderProgram>();
    planetShaderProgramPtr->Build(planetVertexShader, planetFragmentShader);

    ShaderProgram::Location planetModeLocation = planetShaderProgramPtr->GetUniformLocation("Mode");
    ShaderProgram::Location planetWorldMatrixLocation = planetShaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location planetViewProjLocation = planetShaderProgramPtr->GetUniformLocation("ViewProjMatrix");
    m_renderer.RegisterShaderProgram(planetShaderProgramPtr, [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            if (cameraChanged)
            {
                shaderProgram.SetUniform(planetViewProjLocation, camera.GetViewProjectionMatrix());
            }

            shaderProgram.SetUniform(planetWorldMatrixLocation, worldMatrix);
        },
        m_renderer.GetDefaultUpdateLightsFunction(*planetShaderProgramPtr));

    assert(planetShaderProgramPtr);
    m_planetMaterial = std::make_shared<Material>(planetShaderProgramPtr);
    m_planetMaterial->SetUniformValue("Mode", 4u);

    Shader sunVertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/sun/sun.vert");
    Shader sunFragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/sun/sun.frag");

    std::shared_ptr<ShaderProgram> sunShaderProgramPtr = std::make_shared<ShaderProgram>();
    sunShaderProgramPtr->Build(sunVertexShader, sunFragmentShader);

    ShaderProgram::Location sunWorldMatrixLocation = sunShaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location sunViewProjLocation = sunShaderProgramPtr->GetUniformLocation("ViewProjMatrix");

    m_renderer.RegisterShaderProgram(sunShaderProgramPtr, [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            if (cameraChanged)
            {
                shaderProgram.SetUniform(sunViewProjLocation, camera.GetViewProjectionMatrix());
            }

            shaderProgram.SetUniform(sunWorldMatrixLocation, worldMatrix);

        },
        m_renderer.GetDefaultUpdateLightsFunction(*sunShaderProgramPtr));
    // Sun material
    m_sunMaterial = std::make_shared<Material>(sunShaderProgramPtr);
    m_sunMaterial->SetUniformValue("Mode", 4u);
    m_sunMaterial->SetUniformValue("Color", glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
    m_sunMaterial->SetUniformValue("ColorTexture", LoadTexture("textures/sun.png"));
    m_sunMaterial->SetUniformValue("ColorTextureScale", glm::vec2(0.0625f));
    m_sunMaterial->SetBlendEquation(Material::BlendEquation::Add);
    m_sunMaterial->SetBlendParams(Material::BlendParam::SourceAlpha, Material::BlendParam::OneMinusSourceAlpha);

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

void SolarSystemSimulation::InitializeModels()
{
    //Initialize the skybox.
    m_skyboxTexture = TextureCubemapLoader::LoadTextureShared("models/skybox/spaceSkybox.png", TextureObject::FormatRGB, TextureObject::InternalFormatSRGB8);

    m_skyboxTexture->Bind();
    float maxLod;
    m_skyboxTexture->GetParameter(TextureObject::ParameterFloat::MaxLod, maxLod);
    TextureCubemapObject::Unbind();

    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3, VertexAttribute::Semantic::Position);
    vertexFormat.AddVertexAttribute<float>(2, VertexAttribute::Semantic::TexCoord0);
    vertexFormat.AddVertexAttribute<float>(3, VertexAttribute::Semantic::Color0);
    vertexFormat.AddVertexAttribute<float>(3, VertexAttribute::Semantic::Normal);
    
    //Size of the grid for the individual planes
    unsigned int gridSize = 512u;

    // Grid scale to convert the entire grid to size 1x1
    float scale = (1.0f / gridSize);

    // Number of columns and rows
    unsigned int columnCount = gridSize + 1u;
    unsigned int rowCount = gridSize + 1u;

    //Parameter for the noise.
    float maxHeight = 0.05f;
    float lacunarity = 1.9f;
    float gain = 0.55f;
    int octaves = 8;
    float minVal = -0.2f;
    float maxVal = 1.0f;

    std::shared_ptr<Model> planetModel = std::make_shared<Model>(make_shared<Mesh>());

    vec3 offset = vec3(6.5f, 0.0f, 0.0f);
    std::vector<Vertex> vertices;

    // Create container for the element data
    std::vector<unsigned int> indices;

    //Earth
    // Iterate over each VERTEX
    for (unsigned int u = 0u; u < 6u; u++)
    {
        for (unsigned int j = 0u; j < rowCount; ++j)
        {
            for (unsigned int i = 0u; i < columnCount; ++i)
            {
                Vertex& vertex = vertices.emplace_back();
                // -0.5f is the offset
                float x = i * scale - 0.5f;
                float y = j * scale - 0.5f;
                float noise;
                float z = 0.0f;
                switch (u)
                {
                case 5u:
                    //Back
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z - 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 4u:
                    //Front
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z + 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 3u:
                    //Left
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z - 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 2u:
                    //Bottom
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, y, (z * -1) - 0.5f) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 1u:
                    //Right
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z + 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 0u:
                    //Top
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, y, z + 0.5f) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                }

                if (i > 0 && j > 0)
                {
                    unsigned int offset = rowCount * columnCount * u;
                    unsigned int top_right = (j * columnCount + i) + offset; // Current vertex
                    unsigned int top_left = top_right - 1u;
                    unsigned int bottom_right = top_right - columnCount;
                    unsigned int bottom_left = bottom_right - 1u;

                    vec3 facing = cross((vertices[bottom_left].position - vertices[bottom_right].position), (vertices[top_left].position - vertices[bottom_right].position));

                    if (sign(dot(vertex.normal, facing)) > 0)
                    {
                        //Triangle 1
                        indices.push_back(bottom_right);
                        indices.push_back(bottom_left);
                        indices.push_back(top_left);

                        //Triangle 2
                        indices.push_back(top_left);
                        indices.push_back(top_right);
                        indices.push_back(bottom_right);
                    }
                    else
                    {
                        //Triangle 1
                        indices.push_back(bottom_right);
                        indices.push_back(top_left);
                        indices.push_back(bottom_left);

                        //Triangle 2
                        indices.push_back(top_left);
                        indices.push_back(bottom_right);
                        indices.push_back(top_right);
                    }
                }
            }
        }
    }
    for (unsigned int u = 0u; u < 6u; u++)
    {
        for (unsigned int j = 0u; j < rowCount; ++j)
        {
            for (unsigned int i = 0u; i < columnCount; ++i)
            {
                // Get the vertex at (i, j)
                unsigned int offset = (rowCount * columnCount) * u;
                int index = j * columnCount + i + offset;
                Vertex& vertex = vertices[index];

                // Compute the delta in X
                unsigned int prevX = i > 0u ? index - 1u : index;
                unsigned int nextX = i < gridSize ? index + 1u : index;

                // Compute the delta in Y
                int prevY = j > 0 ? index - columnCount : index;
                int nextY = j < gridSize ? index + columnCount : index;

                vec3 deltaX = normalize(vertices[nextX].position - vertices[prevX].position);
                vec3 deltaY = normalize(vertices[nextY].position - vertices[prevY].position);
                vec3 crossProduct = cross(deltaX, deltaY);
                if (sign(dot(vertex.normal, crossProduct)) > 0)
                {
                    vertex.normal = cross(deltaY, deltaX);
                }
                else {
                    vertex.normal = crossProduct;
                }
            }
        }
    }
    planetModel->AddMaterial(m_planetMaterial);
    planetModel->GetMesh().AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, vertices, indices,
        vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());

    std::shared_ptr<Model> marsModel = std::make_shared<Model>(make_shared<Mesh>());

    vec3 marsOffset = vec3(3.5f, 0.0f, 0.0f);
    std::vector<Vertex> marsVertices;

    // Create container for the element data
    std::vector<unsigned int> marsIndices;

    float marsScale = 0.2f;
    minVal = -2.0f;
    maxVal = 2.0f;


    //Mars
    // Iterate over each VERTEX
    for (unsigned int u = 0u; u < 6u; u++)
    {
        for (unsigned int j = 0u; j < rowCount; ++j)
        {
            for (unsigned int i = 0u; i < columnCount; ++i)
            {
                Vertex& vertex = marsVertices.emplace_back();
                vertex.color = vec3(0.69, 0.184, 0.016);

                // -0.5f is the offset
                float x = i * scale - 0.5f;
                float y = j * scale - 0.5f;
                float noise;
                float z = 0.0f;
                switch (u)
                {
                case 5u:
                    //Back
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z - 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * marsScale;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - marsOffset;
                    break;
                case 4u:
                    //Front
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z + 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * marsScale;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - marsOffset;
                    break;
                case 3u:
                    //Left
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z - 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * marsScale;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - marsOffset;
                    break;
                case 2u:
                    //Bottom
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, y, (z * -1) - 0.5f) * 2.0f) / scale - vec3(1.0f)) * marsScale;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - marsOffset;
                    break;
                case 1u:
                    //Right
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z + 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * marsScale;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - marsOffset;
                    break;
                case 0u:
                    //Top
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, y, z + 0.5f) * 2.0f) / scale - vec3(1.0f)) * marsScale;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - marsOffset;
                    break;
                }

                if (i > 0u && j > 0u)
                {
                    unsigned int offset = rowCount * columnCount * u;
                    unsigned int top_right = (j * columnCount + i) + offset; // Current vertex
                    unsigned int top_left = top_right - 1u;
                    unsigned int bottom_right = top_right - columnCount;
                    unsigned int bottom_left = bottom_right - 1u;

                    vec3 facing = cross((marsVertices[bottom_left].position - marsVertices[bottom_right].position), (marsVertices[top_left].position - marsVertices[bottom_right].position));

                    if (sign(dot(vertex.normal, facing)) > 0)
                    {
                        //Triangle 1
                        marsIndices.push_back(bottom_right);
                        marsIndices.push_back(bottom_left);
                        marsIndices.push_back(top_left);

                        //Triangle 2
                        marsIndices.push_back(top_left);
                        marsIndices.push_back(top_right);
                        marsIndices.push_back(bottom_right);
                    }
                    else
                    {
                        //Triangle 1
                        marsIndices.push_back(bottom_right);
                        marsIndices.push_back(top_left);
                        marsIndices.push_back(bottom_left);

                        //Triangle 2
                        marsIndices.push_back(top_left);
                        marsIndices.push_back(bottom_right);
                        marsIndices.push_back(top_right);
                    }
                }
            }
        }
    }
    for (unsigned int u = 0u; u < 6u; u++)
    {
        for (unsigned int j = 0u; j < rowCount; ++j)
        {
            for (unsigned int i = 0u; i < columnCount; ++i)
            {
                // Get the vertex at (i, j)
                unsigned int offset = (rowCount * columnCount) * u;
                int index = j * columnCount + i + offset;
                Vertex& vertex = marsVertices[index];

                // Compute the delta in X
                unsigned int prevX = i > 0u ? index - 1u : index;
                unsigned int nextX = i < gridSize ? index + 1u : index;

                // Compute the delta in Y
                int prevY = j > 0 ? index - columnCount : index;
                int nextY = j < gridSize ? index + columnCount : index;

                vec3 deltaX = normalize(marsVertices[nextX].position - marsVertices[prevX].position);
                vec3 deltaY = normalize(marsVertices[nextY].position - marsVertices[prevY].position);
                vec3 crossProduct = cross(deltaX, deltaY);
                if (sign(dot(vertex.normal, crossProduct)) > 0)
                {
                    vertex.normal = cross(deltaY, deltaX);
                }
                else {
                    vertex.normal = crossProduct;
                }
            }
        }
    }
    marsModel->AddMaterial(m_planetMaterial);
    marsModel->GetMesh().AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, marsVertices, marsIndices,
        vertexFormat.LayoutBegin(static_cast<int>(marsVertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());
    struct SunVertex
    {
        SunVertex() = default;
        SunVertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2 texCoord)
            : position(position), normal(normal), texCoord(texCoord) {}
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };
    VertexFormat sunVertexFormat;
    sunVertexFormat.AddVertexAttribute<float>(3, VertexAttribute::Semantic::Position);
    sunVertexFormat.AddVertexAttribute<float>(3);
    sunVertexFormat.AddVertexAttribute<float>(2);
    //Sun
    std::vector<SunVertex> sunVertices;

    // Create container for the element data
    std::vector<unsigned int> sunIndices;
    // Iterate over each VERTEX

    float sunScale = 1.0f;
    rowCount -= gridSize * 0.75f;
    columnCount -= gridSize * 0.75f;
    scale = (1.0f / (gridSize*0.25f));
    for (unsigned int u = 0u; u < 6u; u++)
    {
        for (unsigned int j = 0u; j < rowCount; ++j)
        {
            for (unsigned int i = 0u; i < columnCount; ++i)
            {
                SunVertex& vertex = sunVertices.emplace_back();
                float x = i * scale - 0.5f;
                float y = j * scale - 0.5f;
                float z = 0.0f;
                switch (u)
                {
                case 5u:
                    //Back
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z - 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 4u:
                    //Front
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z + 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 3u:
                    //Left
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z - 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 2u:
                    //Bottom
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, y, (z * -1) - 0.5f) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 1u:
                    //Right
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z + 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 0u:
                    //Top
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, y, z + 0.5f) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                }

                if (i > 0u && j > 0u)
                {
                    unsigned int offset = rowCount * columnCount * u;
                    unsigned int top_right = (j * columnCount + i) + offset; // Current vertex
                    unsigned int top_left = top_right - 1u;
                    unsigned int bottom_right = top_right - columnCount;
                    unsigned int bottom_left = bottom_right - 1u;

                    vec3 facing = cross((sunVertices[bottom_left].position - sunVertices[bottom_right].position), (sunVertices[top_left].position - sunVertices[bottom_right].position));

                    if (sign(dot(vertex.position, facing)) < 0)
                    {
                        //Triangle 1
                        sunIndices.push_back(bottom_right);
                        sunIndices.push_back(top_right);
                        sunIndices.push_back(top_left);

                        //Triangle 2
                        sunIndices.push_back(top_left);
                        sunIndices.push_back(bottom_left);
                        sunIndices.push_back(bottom_right);
                    }
                    else
                    {
                        //Triangle 1
                        sunIndices.push_back(bottom_right);
                        sunIndices.push_back(bottom_left);
                        sunIndices.push_back(top_left);

                        //Triangle 2
                        sunIndices.push_back(top_left);
                        sunIndices.push_back(top_right);
                        sunIndices.push_back(bottom_right);
                    }
                }
            }
        }
    }

    std::shared_ptr<Model> sunModel = std::make_shared<Model>(make_shared<Mesh>());
    sunModel->AddMaterial(m_sunMaterial);
    sunModel->GetMesh().AddSubmesh<SunVertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, sunVertices, sunIndices,
        sunVertexFormat.LayoutBegin(static_cast<int>(sunVertices.size()), true /* interleaved */), sunVertexFormat.LayoutEnd());

    m_scene.AddSceneNode(std::make_shared<SceneModel>("Sun", sunModel));

    m_scene.AddSceneNode(std::make_shared<SceneModel>("Earth", planetModel));

    m_scene.AddSceneNode(std::make_shared<SceneModel>("Mars", marsModel));
}

void SolarSystemSimulation::InitializeRenderer()
{
    m_renderer.AddRenderPass(std::make_unique<ForwardRenderPass>());
    m_renderer.AddRenderPass(std::make_unique<SkyboxRenderPass>(m_skyboxTexture));
}

void SolarSystemSimulation::RenderGUI()
{
    m_imGui.BeginFrame();

    ImGui::Checkbox("Enable Planet Rotation", &m_enablePlanetRotation);
    
    ImGui::DragFloat("Planet Rotation Speed", &m_planetRotationSpeed, 0.05f);

    // Draw GUI for scene nodes, using the visitor pattern
    ImGuiSceneVisitor imGuiVisitor(m_imGui, "Scene");
    m_scene.AcceptVisitor(imGuiVisitor);

    m_imGui.EndFrame();
}
std::shared_ptr<Texture2DObject> SolarSystemSimulation::LoadTexture(const char* path)
{
    std::shared_ptr<Texture2DObject> texture = std::make_shared<Texture2DObject>();

    int width = 0;
    int height = 0;
    int components = 0;

    // Load the texture data here
    unsigned char* data = stbi_load(path, &width, &height, &components, 4);

    texture->Bind();
    texture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA, std::span<const unsigned char>(data, width * height * 4));

    // Generate mipmaps
    texture->GenerateMipmap();

    // Release texture data
    stbi_image_free(data);

    return texture;
}
void SolarSystemSimulation::UpdateOutputMode()
{
    for (int i = 1; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            m_planetMaterial->SetUniformValue("Mode", (unsigned int)i);
            m_sunMaterial->SetUniformValue("Mode", (unsigned int)i);
            break;
        }
    }

}