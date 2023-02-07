#pragma once

#include <ituGL/application/Application.h>
#include "ituGL/geometry/VertexBufferObject.h"
#include "ituGL/geometry/VertexArrayObject.h"
#include "ituGL/geometry/ElementBufferObject.h"
#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include <iostream>
#include <array>
#include <valarray>
// (todo) 01.1: Include the libraries you need


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

private:
    unsigned int m_gridX, m_gridY;
    unsigned int m_shaderProgram;

    // (todo) 01.1: Declare an VBO, VAO
    VertexBufferObject vbo;
    VertexArrayObject vao;

    // (todo) 01.5: Declare an EBO
    ElementBufferObject ebo;
};
