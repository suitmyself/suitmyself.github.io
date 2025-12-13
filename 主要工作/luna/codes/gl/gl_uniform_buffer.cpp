/**
 * @author     : Chen Wei
 * @date       : 2025-03-14
 * @description: opengl uniform buffer (UBO)
 * @version    : 1.0
 */

#include "gl_uniform_buffer.h"

namespace luna {

GLUniformBuffer::GLUniformBuffer()
{
    glGenBuffers(1, &m_ubo_id);
}

GLUniformBuffer::GLUniformBuffer(GLUniformBuffer&& rhs) noexcept
{
    m_ubo_id = rhs.m_ubo_id;
    rhs.m_ubo_id = 0;
}

GLUniformBuffer& GLUniformBuffer::operator=(GLUniformBuffer&& rhs) noexcept
{
    if (this != &rhs)
    {
        glDeleteBuffers(1, &m_ubo_id);

        m_ubo_id = rhs.m_ubo_id;
        rhs.m_ubo_id = 0;
    }
    return *this;
}

GLUniformBuffer::~GLUniformBuffer()
{
    this->destroy();
}

void GLUniformBuffer::destroy()
{
    if (m_ubo_id != 0)
    {
        glDeleteBuffers(1, &m_ubo_id);
        m_ubo_id = 0;
    }
}

GLuint GLUniformBuffer::id() const
{
    return m_ubo_id;
}

void GLUniformBuffer::update(const void* data, int size, GLenum usage)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_id);
    glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
}

void GLUniformBuffer::bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_id);
}

void GLUniformBuffer::unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

}//end of namespace luna