/**
 * @author     : Chen Wei
 * @date       : 2025-03-16
 * @description: opengl shader storage buffer (SSBO)
 * @version    : 1.0
 */

#include "gl_shader_storage_buffer.h"

#if !__IOS__

namespace luna {

GLShaderStorageBuffer::GLShaderStorageBuffer()
{
    glGenBuffers(1, &m_ssbo_id);
}

GLShaderStorageBuffer::GLShaderStorageBuffer(GLShaderStorageBuffer&& rhs) noexcept
{
    m_ssbo_id = rhs.m_ssbo_id;
    rhs.m_ssbo_id = 0;
}

GLShaderStorageBuffer& GLShaderStorageBuffer::operator=(GLShaderStorageBuffer&& rhs) noexcept
{
    if (this != &rhs)
    {
        glDeleteBuffers(1, &m_ssbo_id);

        m_ssbo_id = rhs.m_ssbo_id;
        rhs.m_ssbo_id = 0;
    }
    return *this;
}

GLShaderStorageBuffer::~GLShaderStorageBuffer()
{
    this->destroy();
}

void GLShaderStorageBuffer::destroy()
{
    if (m_ssbo_id != 0)
    {
        glDeleteBuffers(1, &m_ssbo_id);
        m_ssbo_id = 0;
    }
}

GLuint GLShaderStorageBuffer::id() const
{
    return m_ssbo_id;
}

void GLShaderStorageBuffer::update(const void* data, int size, GLenum usage)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
}

void GLShaderStorageBuffer::bind() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo_id);
}

void GLShaderStorageBuffer::unbind() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

}//end of namespace luna

#endif//__IOS__