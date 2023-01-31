#include <ituGL/core/BufferObject.h>

// Create the object initially null, get object handle and generate 1 buffer
BufferObject::BufferObject() : Object(NullHandle)
{
    Handle& handle = GetHandle();
    glGenBuffers(1, &handle);
}

// Get object handle and delete 1 buffer
BufferObject::~BufferObject()
{
    Handle& handle = GetHandle();
    glDeleteBuffers(1, &handle);
}

// Bind the buffer handle to the specific target
void BufferObject::Bind(Target target) const
{
    Handle handle = GetHandle();
    glBindBuffer(target, handle);
}

// Bind the null handle to the specific target
void BufferObject::Unbind(Target target)
{
    Handle handle = NullHandle;
    glBindBuffer(target, handle);
}

// Get buffer Target and allocate buffer data
void BufferObject::AllocateData(size_t size, Usage usage)
{
    Target target = GetTarget();
    // (todo) 00.1: Allocate without initial data (you can use nullptr instead)
    glBufferData(target, size, nullptr, usage);
}

// Get buffer Target and allocate buffer data
void BufferObject::AllocateData(std::span<const std::byte> data, Usage usage)
{
    Target target = GetTarget();
    // (todo) 00.1: Allocate with initial data, specifying the size in bytes and the pointer to the data
    glBufferData(target, data.size_bytes(), data.data(), usage);
}

// Get buffer Target and set buffer subdata
void BufferObject::UpdateData(std::span<const std::byte> data, size_t offset)
{
    // (todo) 00.5: Update buffer subdata

}
