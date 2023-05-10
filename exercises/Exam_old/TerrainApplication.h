#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/scene/Scene.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/CameraController.h>
#include <ituGL/utils/DearImGui.h>

class TerrainApplication : public Application
{
public:
    TerrainApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializePlanet(glm::vec3 offset, float scaleSize);
    void BuildShaders();
    void UpdateOutputMode();
    void LoadAndCompileShader(Shader& shader, const char* path);
    void RenderGUI();
private:
    unsigned int m_grid;
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Camera controller
    CameraController m_cameraController;

    // Global scene
    Scene m_scene;

    
    ShaderProgram m_shaderProgram;
    ShaderProgram::Location m_Mode;
    ShaderProgram::Location m_Matrix;
    //Camera m_camera;
    
    VertexBufferObject m_vbo;
    VertexArrayObject m_vao;
    ElementBufferObject m_ebo;
};
