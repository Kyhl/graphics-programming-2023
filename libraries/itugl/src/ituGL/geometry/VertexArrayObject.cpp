#include <ituGL/geometry/VertexArrayObject.h>

#include <ituGL/geometry/VertexAttribute.h>

// Create the object initially null, get object handle and generate 1 vertex array
VertexArrayObject::VertexArrayObject() : Object(NullHandle)
{
    Handle& handle = GetHandle();
    glGenVertexArrays(1, &handle);
}

// Get object handle and delete 1 vertex array
VertexArrayObject::~VertexArrayObject()
{
    Handle& handle = GetHandle();
    glDeleteVertexArrays(1, &handle);
}

// Bind the vertex array handle to the specific target
void VertexArrayObject::Bind() const
{
    Handle handle = GetHandle();
    glBindVertexArray(handle);
}

// Bind the null handle to the specific target
void VertexArrayObject::Unbind()
{
    Handle handle = NullHandle;
    glBindVertexArray(handle);
}

// Sets the VertexAttribute pointer and enables the VertexAttribute in that location
void VertexArrayObject::SetAttribute(GLuint location, const VertexAttribute& attribute, GLint offset, GLsizei stride)
{
    // Get the attribute properties in OpenGL expected format
    GLint components = attribute.GetComponents();
    GLenum type = static_cast<GLenum>(attribute.GetType());
    GLboolean normalized = attribute.IsNormalized() ? GL_TRUE : GL_FALSE;

    // Compute the attribute pointer
    const unsigned char* pointer = nullptr; // Actual base pointer is in VBO
    pointer += offset;
    glVertexAttribPointer(location, components, type, normalized, stride,pointer);
    glEnableVertexAttribArray(location);
    // (todo) 00.1: Set the VertexAttribute pointer in that location

    // (todo) 00.1: Finally, we enable the VertexAttribute in that location
}
