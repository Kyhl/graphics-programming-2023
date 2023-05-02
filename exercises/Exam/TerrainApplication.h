#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include <ituGL/application/Application.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/geometry/Mesh.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/shader/Material.h>
#include <glm/mat4x4.hpp>
#include <vector>
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
    void BuildShaders();
    void UpdateOutputMode();
    void LoadAndCompileShader(Shader& shader, const char* path);
private:
    unsigned int m_grid;
    ShaderProgram m_shaderProgram;
    ShaderProgram::Location m_Mode;
    ShaderProgram::Location m_Matrix;
    Camera m_camera;

    VertexBufferObject m_vbo;
    VertexArrayObject m_vao;
    ElementBufferObject m_ebo;
};
