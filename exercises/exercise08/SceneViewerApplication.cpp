#include "SceneViewerApplication.h"

#include <ituGL/asset/TextureCubemapLoader.h>
#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/geometry/VertexFormat.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>
#include <ituGL/asset/ShaderLoader.h>
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
#include <ituGL/scene/Transform.h>

#include <fstream>
#include <sstream>

#include <cmath>
#include <iostream>
#include <numbers> 
using namespace std;
using namespace glm;
SceneViewerApplication::SceneViewerApplication()
    : Application(1024, 1024, "Planet Scene Exam")
    , m_renderer(GetDevice())
{
}
struct Vertex
{
    vec3 position;
    vec2 texCoord;
    vec3 color;
    vec3 normal;
};
void SceneViewerApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());
    
    InitializeCamera();
    //InitializeLights();
    InitializeMaterial();
    InitializeModels();
    InitializeRenderer();
}

void SceneViewerApplication::Update()
{
    Application::Update();

    // Update camera controller
    m_cameraController.Update(GetMainWindow(), GetDeltaTime());

    m_scene.GetSceneNode("Earth")->GetTransform()->SetRotation(m_scene.GetSceneNode("Earth")->GetTransform()->GetRotation()+vec3(0,0.001f,0));

    // Add the scene nodes to the renderer
    RendererSceneVisitor rendererSceneVisitor(m_renderer);
    m_scene.AcceptVisitor(rendererSceneVisitor);
}

void SceneViewerApplication::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Render the scene
    m_renderer.Render();

    // Render the debug user interface
    RenderGUI();
}

void SceneViewerApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void SceneViewerApplication::InitializeCamera()
{
    // Create the main camera
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->SetViewMatrix(glm::vec3(-1, 1, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera->SetPerspectiveProjectionMatrix(1.0f, 1.0f, 0.1f, 100.0f);

    // Create a scene node for the camera
    std::shared_ptr<SceneCamera> sceneCamera = std::make_shared<SceneCamera>("camera", camera);

    // Add the camera node to the scene
    m_scene.AddSceneNode(sceneCamera);

    // Set the camera scene node to be controlled by the camera controller
    m_cameraController.SetCamera(sceneCamera);
}

void SceneViewerApplication::InitializeLights()
{
    // Create a directional light and add it to the scene
    //std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
    //directionalLight->SetDirection(glm::vec3(-0.3f, -1.0f, -0.3f)); // It will be normalized inside the function
    //directionalLight->SetIntensity(3.0f);
    //m_scene.AddSceneNode(std::make_shared<SceneLight>("directional light", directionalLight));

    // Create a point light and add it to the scene
    //std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
    //pointLight->SetPosition(glm::vec3(0, 0, 0));
    //pointLight->SetDistanceAttenuation(glm::vec2(5.0f, 10.0f));
    //m_scene.AddSceneNode(std::make_shared<SceneLight>("point light", pointLight));
}

void SceneViewerApplication::InitializeMaterial()
{
    //// Load and build shader
    //std::vector<const char*> vertexShaderPaths;
    //vertexShaderPaths.push_back("shaders/version330.glsl");
    //vertexShaderPaths.push_back("shaders/default.vert");
    //Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

    //std::vector<const char*> fragmentShaderPaths;
    //fragmentShaderPaths.push_back("shaders/version330.glsl");
    //fragmentShaderPaths.push_back("shaders/utils.glsl");
    //fragmentShaderPaths.push_back("shaders/lambert-ggx.glsl");
    //fragmentShaderPaths.push_back("shaders/lighting.glsl");
    //fragmentShaderPaths.push_back("shaders/default_pbr.frag");
    //Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

    //std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
    //shaderProgramPtr->Build(vertexShader, fragmentShader);

    //// Get transform related uniform locations
    //ShaderProgram::Location cameraPositionLocation = shaderProgramPtr->GetUniformLocation("CameraPosition");
    //ShaderProgram::Location worldMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldMatrix");
    //ShaderProgram::Location viewProjMatrixLocation = shaderProgramPtr->GetUniformLocation("ViewProjMatrix");

    //// Register shader with renderer
    //m_renderer.RegisterShaderProgram(shaderProgramPtr,
    //    [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
    //    {
    //        if (cameraChanged)
    //        {
    //            shaderProgram.SetUniform(cameraPositionLocation, camera.ExtractTranslation());
    //            shaderProgram.SetUniform(viewProjMatrixLocation, camera.GetViewProjectionMatrix());
    //        }
    //        shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);
    //    },
    //    m_renderer.GetDefaultUpdateLightsFunction(*shaderProgramPtr)
    //);

    //// Filter out uniforms that are not material properties
    //ShaderUniformCollection::NameSet filteredUniforms;
    //filteredUniforms.insert("CameraPosition");
    //filteredUniforms.insert("WorldMatrix");
    //filteredUniforms.insert("ViewProjMatrix");
    //filteredUniforms.insert("LightIndirect");
    //filteredUniforms.insert("LightColor");
    //filteredUniforms.insert("LightPosition");
    //filteredUniforms.insert("LightDirection");
    //filteredUniforms.insert("LightAttenuation");

    //// Create reference material
    //assert(shaderProgramPtr);
    //m_defaultMaterial = std::make_shared<Material>(shaderProgramPtr, filteredUniforms);

    
    //Shader vertexShader(Shader::VertexShader);
    //
    //LoadAndCompileShader(vertexShader, "shaders/planet/default.vert");

    //// Load and compile fragment shader
    //Shader fragmentShader(Shader::FragmentShader);
    //LoadAndCompileShader(fragmentShader, "shaders/planet/default.frag");

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
                //shaderProgram.SetUniform(cameraPositionLocation, camera.ExtractTranslation());
                shaderProgram.SetUniform(planetViewProjLocation, camera.GetViewProjectionMatrix());
            }

            shaderProgram.SetUniform(planetWorldMatrixLocation, worldMatrix);
            shaderProgram.SetUniform(planetModeLocation, 4u);
        }, 
        m_renderer.GetDefaultUpdateLightsFunction(*planetShaderProgramPtr));

    assert(planetShaderProgramPtr);
    m_planetMaterial = std::make_shared<Material>(planetShaderProgramPtr);

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

void SceneViewerApplication::InitializeModels()
{
    m_skyboxTexture = TextureCubemapLoader::LoadTextureShared("models/skybox/spaceSkybox.png", TextureObject::FormatRGB, TextureObject::InternalFormatSRGB8);

    m_skyboxTexture->Bind();
    float maxLod;
    m_skyboxTexture->GetParameter(TextureObject::ParameterFloat::MaxLod, maxLod);
    TextureCubemapObject::Unbind();

    //m_defaultMaterial->SetUniformValue("AmbientColor", glm::vec3(0.25f));

    //m_defaultMaterial->SetUniformValue("EnvironmentTexture", m_skyboxTexture);
    //m_defaultMaterial->SetUniformValue("EnvironmentMaxLod", maxLod);
    //m_defaultMaterial->SetUniformValue("Color", glm::vec3(1.0f));

    //// Configure loader
    //ModelLoader loader(m_defaultMaterial);

    //// Create a new material copy for each submaterial
    //loader.SetCreateMaterials(true);

    //// Flip vertically textures loaded by the model loader
    //loader.GetTexture2DLoader().SetFlipVertical(true);

    //// Link vertex properties to attributes
    //loader.SetMaterialAttribute(VertexAttribute::Semantic::Position, "VertexPosition");
    //loader.SetMaterialAttribute(VertexAttribute::Semantic::Normal, "VertexNormal");
    //loader.SetMaterialAttribute(VertexAttribute::Semantic::Tangent, "VertexTangent");
    //loader.SetMaterialAttribute(VertexAttribute::Semantic::Bitangent, "VertexBitangent");
    //loader.SetMaterialAttribute(VertexAttribute::Semantic::TexCoord0, "VertexTexCoord");

    //// Link material properties to uniforms
    //loader.SetMaterialProperty(ModelLoader::MaterialProperty::DiffuseColor, "Color");
    //loader.SetMaterialProperty(ModelLoader::MaterialProperty::DiffuseTexture, "ColorTexture");
    //loader.SetMaterialProperty(ModelLoader::MaterialProperty::NormalTexture, "NormalTexture");
    //loader.SetMaterialProperty(ModelLoader::MaterialProperty::SpecularTexture, "SpecularTexture");

    // Load models
    //std::shared_ptr<Model> chestModel = loader.LoadShared("models/treasure_chest/treasure_chest.obj");
    //m_scene.AddSceneNode(std::make_shared<SceneModel>("treasure chest", chestModel));

    //std::shared_ptr<Model> cameraModel = loader.LoadShared("models/camera/camera.obj");
    //m_scene.AddSceneNode(std::make_shared<SceneModel>("camera model", cameraModel));

    //std::shared_ptr<Model> teaSetModel = loader.LoadShared("models/tea_set/tea_set.obj");
    //m_scene.AddSceneNode(std::make_shared<SceneModel>("tea set", teaSetModel));

    //std::shared_ptr<Model> clockModel = loader.LoadShared("models/alarm_clock/alarm_clock.obj");
    //m_scene.AddSceneNode(std::make_shared<SceneModel>("alarm clock", clockModel));
    // Create containers for the vertex data

    

    std::shared_ptr<Model> planetModel = std::make_shared<Model>(make_shared<Mesh>());


    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3, VertexAttribute::Semantic::Position);
    vertexFormat.AddVertexAttribute<float>(2, VertexAttribute::Semantic::TexCoord0);
    vertexFormat.AddVertexAttribute<float>(3, VertexAttribute::Semantic::Color0);
    vertexFormat.AddVertexAttribute<float>(3, VertexAttribute::Semantic::Normal);
    std::vector<Vertex> vertices;

    // Create container for the element data
    std::vector<unsigned int> indices;
    unsigned int m_grid = 512u;
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

    vec3 offset = vec3(3.5f, 0.0f, 0.0f);


    // Iterate over each VERTEX
    for (unsigned int u = 0; u < 6; u++)
    {
        for (unsigned int j = 0; j < rowCount; ++j)
        {
            for (unsigned int i = 0; i < columnCount; ++i)
            {
                Vertex& vertex = vertices.emplace_back();
                // -0.5f is the offset
                float x = i * scale - 0.5f;
                float y = j * scale - 0.5f;
                float noise;
                float z = 0.0f;
                switch (u)
                {
                case 5:
                    //Back
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    //the *0.5f is the scale
                    vertex.position = glm::normalize((vec3(z - 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 4:
                    //Front
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z + 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 3:
                    //Left
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z - 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 2:
                    //Bottom
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    
                    vertex.position = glm::normalize((vec3(x, y, (z * -1) - 0.5f) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 1:
                    //Right
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    
                    vertex.position = glm::normalize((vec3(x, z + 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * 0.5f;
                    noise = clamp(stb_perlin_fbm_noise3(vertex.position.x * 2.0f, vertex.position.y * 2.0f, vertex.position.z * 2.0f, lacunarity, gain, octaves), minVal, maxVal);
                    vertex.color = GetColorFromHeight(noise);
                    vertex.normal = vertex.position;
                    vertex.position += vertex.position * 2.0f * noise * maxHeight - offset;
                    break;
                case 0:
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
                    unsigned int top_left = top_right - 1;
                    unsigned int bottom_right = top_right - columnCount;
                    unsigned int bottom_left = bottom_right - 1;

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
                vec3 temp = cross(deltaX, deltaY);
                if (sign(dot(vertex.normal, temp)) > 0)
                {
                    vertex.normal = cross(deltaY, deltaX);
                }
                else {
                    vertex.normal = temp;
                }

                

            }
        }
    }
    planetModel->AddMaterial(m_planetMaterial);
    planetModel->GetMesh().AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, vertices, indices,
        vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());
    


    //Sun
    std::vector<Vertex> sunVertices;

    // Create container for the element data
    std::vector<unsigned int> sunIndices;
    // Iterate over each VERTEX

    float sunScale = 1.0f;

    for (unsigned int u = 0; u < 6; u++)
    {
        for (unsigned int j = 0; j < rowCount; ++j)
        {
            for (unsigned int i = 0; i < columnCount; ++i)
            {
                Vertex& vertex = sunVertices.emplace_back();
                vertex.color = vec3(0.969, 0.839, 0.408);
                // -0.5f is the offset
                float x = i * scale - 0.5f;
                float y = j * scale - 0.5f;
                float z = 0.0f;
                switch (u)
                {
                case 5:
                    //Back
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z - 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 4:
                    //Front
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(z + 0.5f, x, y) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 3:
                    //Left
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z - 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 2:
                    //Bottom
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, y, (z * -1) - 0.5f) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 1:
                    //Right
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, z + 0.5f, y) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                case 0:
                    //Top
                    vertex.texCoord = vec2(static_cast<float>(i), static_cast<float>(j));
                    vertex.position = glm::normalize((vec3(x, y, z + 0.5f) * 2.0f) / scale - vec3(1.0f)) * sunScale;
                    vertex.normal = normalize(vertex.position);
                    break;
                }

                if (i > 0 && j > 0)
                {
                    unsigned int offset = rowCount * columnCount * u;
                    unsigned int top_right = (j * columnCount + i) + offset; // Current vertex
                    unsigned int top_left = top_right - 1;
                    unsigned int bottom_right = top_right - columnCount;
                    unsigned int bottom_left = bottom_right - 1;
                    
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
    sunModel->AddMaterial(m_planetMaterial);
    sunModel->GetMesh().AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, sunVertices, sunIndices,
        vertexFormat.LayoutBegin(static_cast<int>(sunVertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());
    //    std::shared_ptr<Material> planetMaterial = std::make_shared<Material>();
   
    m_scene.AddSceneNode(std::make_shared<SceneModel>("Sun", sunModel));

    /*std::vector<const char*> vertexShaderPaths;
    vertexShaderPaths.push_back("shaders/version330.glsl");
    vertexShaderPaths.push_back("shaders/planet/default.vert");
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

    std::vector<const char*> fragmentShaderPaths;
    fragmentShaderPaths.push_back("shaders/version330.glsl");
    fragmentShaderPaths.push_back("shaders/planet/default.frag");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

    std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
    shaderProgramPtr->Build(vertexShader, fragmentShader);
    m_renderer.RegisterShaderProgram(shaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            
        },
        m_renderer.GetDefaultUpdateLightsFunction(*shaderProgramPtr)
    );*/
    

    m_scene.AddSceneNode(std::make_shared<SceneModel>("Earth", planetModel));
}

void SceneViewerApplication::InitializeRenderer()
{
    m_renderer.AddRenderPass(std::make_unique<ForwardRenderPass>());
    m_renderer.AddRenderPass(std::make_unique<SkyboxRenderPass>(m_skyboxTexture));
}

void SceneViewerApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    // Draw GUI for scene nodes, using the visitor pattern
    ImGuiSceneVisitor imGuiVisitor(m_imGui, "Scene");
    m_scene.AcceptVisitor(imGuiVisitor);

    // Draw GUI for camera controller
    m_cameraController.DrawGUI(m_imGui);

    m_imGui.EndFrame();
}
