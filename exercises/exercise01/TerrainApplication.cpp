#include "TerrainApplication.h"
#include "ituGL/geometry/VertexAttribute.h"

#define STB_PERLIN_IMPLEMENTATION
#include <cmath>
#include <iostream>
#include <vector>
#include <stb_perlin.h>


// Helper structures. Declared here only for this exercise
struct Vector2
{
    Vector2() : Vector2(0.f, 0.f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x, y;
};

struct Vector3
{
    Vector3() : Vector3(0.f,0.f,0.f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;

    Vector3 Normalize() const
    {
        float length = std::sqrt(1 + x * x + y * y);
        return Vector3(x / length, y / length, z / length);
    }
};

// (todo) 01.8: Declare an struct with the vertex format
struct Vertex {
    Vector3 pos;
    Vector2 textureCoordinate;
    Vector3 color;
    Vector3 normal;
};
Vector3 GetColor(float z) {
    if (z < -0.50) {
        return Vector3(0, 0, 1);
    }
    else if (z < -0.4) {
        return Vector3(0.76,0.70,0.50);
    }
    else if (z < -0.25) {
        return Vector3(0, 1, 0);
    }
    else if(z < 0.45)
    {
        return Vector3(0.5,0.5,0.5);
    }
    else {
        return Vector3(1, 1, 1);
    }
}

TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(64), m_gridY(64), m_shaderProgram(0)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();
    glEnable(GL_DEPTH_TEST);
    // Build shaders and store in m_shaderProgram
    BuildShaders();

    // (todo) 01.1: Create containers for the vertex position
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    //ebo.AllocateData<unsigned int>(std::span(indices));
    float lac = 5.0f;
    float gain = 1.2f;
    int oct = 2;


    // (todo) 01.1: Fill in vertex data
    for(float i = 0;i<m_gridY;i++)
    {
        for(float u=0;u<m_gridX;u++)
        {
            auto height1 = stb_perlin_fbm_noise3(u/m_gridX -0.5f,i/m_gridY -0.5f,0,lac, gain, oct);
            auto height2 =stb_perlin_fbm_noise3((u+1.0f)/m_gridX -0.5f,(i)/m_gridY-0.5f,0,lac, gain, oct);
            auto height3 = stb_perlin_fbm_noise3((u)/m_gridX -0.5f,(i+1)/m_gridY -0.5f,0,lac, gain, oct);
            auto height4 = stb_perlin_fbm_noise3((u+1.0f)/m_gridX -0.5f,(i+1)/m_gridY-0.5f,0,lac, gain, oct);

            Vertex v1;
            v1.pos = Vector3(u/m_gridX -0.5,i/m_gridY -0.5,    height1*0.1);
            v1.textureCoordinate = Vector2(0,0);
            v1.color = GetColor(height1);
            v1.normal = Vector3(1,1,1);

            Vertex v2;
            v2.pos = Vector3((u+1)/m_gridX -0.5,(i)/m_gridY-0.5 , height2*0.1);
            v2.textureCoordinate = Vector2(1,0);
            v2.color = GetColor(height2);
            v2.normal = Vector3(1,1,1);

            Vertex v3;
            v3.pos = Vector3((u)/m_gridX -0.5,(i+1)/m_gridY -0.5, height3*0.1);
            v3.textureCoordinate = Vector2(0,1);
            v3.color = GetColor(height3);
            v3.normal = Vector3(1,1,1);

            Vertex v4;
            v4.pos = Vector3((u+1)/m_gridX -0.5,(i+1)/m_gridY-0.5,height4*0.1);
            v4.textureCoordinate = Vector2(1,1);
            v4.color = GetColor(height4);
            v4.normal = Vector3(1,1,1);

            vertices.emplace_back(v1);
            vertices.emplace_back(v2);
            vertices.emplace_back(v3);
            vertices.emplace_back(v4);

            //Indice declarations
            indices.emplace_back(i*m_gridY*4+u*4);
            indices.emplace_back(i*m_gridY*4+u*4+3);
            indices.emplace_back(i*m_gridY*4+u*4+1);
            indices.emplace_back(i*m_gridY*4+u*4);
            indices.emplace_back(i*m_gridY*4+u*4+2);
            indices.emplace_back(i*m_gridY*4+u*4+3);

        }
    }
    float spaceBetweenVerticesH = 1.0 / float(m_gridX);
    float spaceBetweenVerticesV = 1.0 / float(m_gridY);
    for (int i = 0; i < m_gridY; i++) {
        for (int u = 0; u < m_gridX; u++) {
            for (int j = 0; j < 4; j++) {
                Vector3 left;
                Vector3 right;
                Vector3 up;
                Vector3 down;

                int currentRow;
                int currentColumn;

                if (j == 0) {
                    currentRow = i;
                    currentColumn = u;
                }
                else if (j == 1) {
                    currentRow = i;
                    currentColumn = u + 1;
                }
                else if (j == 2) {
                    currentRow = i + 1;
                    currentColumn = u + 1;
                }
                else {
                    currentRow = i + 1;
                    currentColumn = u;
                }

                float x = vertices[i * m_gridX * 4 + u * 4 + j].pos.x;
                float y = vertices[i * m_gridX * 4 + u * 4 + j].pos.y;

                if (currentColumn - 1 < 0) {
                    left.x = x;
                    left.z = stb_perlin_fbm_noise3(x, y, 0, lac, gain, oct) * 0.1;

                }
                else {
                    left.x = x - spaceBetweenVerticesH;
                    left.z = stb_perlin_fbm_noise3(x - spaceBetweenVerticesH, y, 0, lac, gain, oct) * 0.1;
                }

                if (currentColumn + 1 >= m_gridX) {
                    right.x = x;
                    right.z = stb_perlin_fbm_noise3(x, y, 0, lac, gain, oct) * 0.1;
                }
                else {
                    right.x = x + spaceBetweenVerticesH;
                    right.z = stb_perlin_fbm_noise3(x + spaceBetweenVerticesH, y, 0, lac, gain, oct) * 0.1;
                }

                if (currentRow - 1 < 0) {
                    up.y = y;
                    up.z = stb_perlin_fbm_noise3(x, y, 0, lac, gain, oct) * 0.1;
                }
                else {
                    up.y = y - spaceBetweenVerticesV;
                    up.z = stb_perlin_fbm_noise3(x, y - spaceBetweenVerticesV, 0, lac, gain, oct) * 0.1;
                }

                if (currentRow + 1 >= m_gridY) {
                    down.y = y;
                    down.z = stb_perlin_fbm_noise3(x, y, 0, lac, gain, oct) * 0.1;
                }
                else {
                    down.y = y + spaceBetweenVerticesV;
                    down.z = stb_perlin_fbm_noise3(x, y + spaceBetweenVerticesV, 0, lac, gain, oct) * 0.1;
                }

                float deltaX = float(right.z - left.z) / (right.x - left.x);
                float deltaY = float(up.z - down.z) / (up.y - down.y);
                Vector3 normal(deltaX, deltaY, 1);
                vertices[i * m_gridX * 4 + u * 4 + j].normal = normal.Normalize();
            }
        }
    }
    // (todo) 01.1: Initialize VAO, and VBO
    vao.Bind();

    vbo.Bind();
    vbo.AllocateData(std::span(vertices));

    VertexAttribute position(Data::Type::Float, 3);
    VertexAttribute textures(Data::Type::Float, 2);
    VertexAttribute colors(Data::Type::Float,3);
    VertexAttribute normals(Data::Type::Float,3);

    vao.SetAttribute(0, position, 0, sizeof(Vertex));
    vao.SetAttribute(1, textures, sizeof(Vector3), sizeof(Vertex));
    vao.SetAttribute(2, colors, sizeof(Vector3) + sizeof(Vector2), sizeof(Vertex));
    vao.SetAttribute(3,normals,sizeof(Vector3)+sizeof(Vector2)+sizeof(Vector3),sizeof(Vertex));

    // (todo) 01.5: Initialize EBO
    ebo.Bind();
    ebo.AllocateData<unsigned int>(std::span(indices));
    // (todo) 01.1: Unbind VAO, and VBO
    vao.Unbind();
    vbo.Unbind();
    // (todo) 01.5: Unbind EBO
    ebo.Unbind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void TerrainApplication::Update()
{
    Application::Update();

    UpdateOutputMode();
}

void TerrainApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    vao.Bind();
    // Set shader to be used
    glUseProgram(m_shaderProgram);

    // (todo) 01.1: Draw the grid

    //glDrawArrays(GL_TRIANGLES, 0, vertexSize+1000);
    glDrawElements(GL_TRIANGLES, 2*3*m_gridX * m_gridY, GL_UNSIGNED_INT, 0);


}

void TerrainApplication::Cleanup()
{
    Application::Cleanup();
}

void TerrainApplication::BuildShaders()
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in vec3 aColor;\n"
        "layout (location = 3) in vec3 aNormal;\n"
        "uniform mat4 Matrix = mat4(1);\n"
        "out vec2 texCoord;\n"
        "out vec3 color;\n"
        "out vec3 normal;\n"
        "void main()\n"
        "{\n"
        "   texCoord = aTexCoord;\n"
        "   color = aColor;\n"
        "   normal = aNormal;\n"
        "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "uniform uint Mode = 0u;\n"
        "in vec2 texCoord;\n"
        "in vec3 color;\n"
        "in vec3 normal;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   switch (Mode)\n"
        "   {\n"
        "   default:\n"
        "   case 0u:\n"
        "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "       break;\n"
        "   case 1u:\n"
        "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
        "       break;\n"
        "   case 2u:\n"
        "       FragColor = vec4(color, 1.0f);\n"
        "       break;\n"
        "   case 3u:\n"
        "       FragColor = vec4(normalize(normal), 1.0f);\n"
        "       break;\n"
        "   case 4u:\n"
        "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
        "       break;\n"
        "   }\n"
        "}\n\0";

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
    m_shaderProgram = shaderProgram;
}

void TerrainApplication::UpdateOutputMode()
{
    for (int i = 0; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }
}
