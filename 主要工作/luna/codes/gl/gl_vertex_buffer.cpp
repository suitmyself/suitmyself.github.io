/**
 * @author     : Chen Wei
 * @date       : 2023-05-11
 * @description: opengl vertex buffer (VBO)
 * @version    : 1.0
 */

#include "gl_vertex_buffer.h"

namespace luna {

GLVertexBuffer::GLVertexBuffer()
{
    glGenBuffers(1, &m_vbo_id);
}

GLVertexBuffer::GLVertexBuffer(GLVertexBuffer&& rhs) noexcept
{
    m_vbo_id = rhs.m_vbo_id;
    rhs.m_vbo_id = 0;
}

GLVertexBuffer& GLVertexBuffer::operator=(GLVertexBuffer&& rhs) noexcept
{
    if (this != &rhs)
    {
        glDeleteBuffers(1, &m_vbo_id);

        m_vbo_id = rhs.m_vbo_id;
        rhs.m_vbo_id = 0;
    }
    return *this;
}

GLVertexBuffer::~GLVertexBuffer()
{
    this->destroy();
}

void GLVertexBuffer::destroy()
{
    if (m_vbo_id != 0)
    {
        glDeleteBuffers(1, &m_vbo_id);
        m_vbo_id = 0;
    }
}

GLuint GLVertexBuffer::id() const
{
    return m_vbo_id;
}

void GLVertexBuffer::update(const void* data, int size, GLenum usage)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void GLVertexBuffer::update(const std::vector<float>& data, GLenum usage)
{
    this->update(data.data(), sizeof(float) * data.size(), usage);
}

void GLVertexBuffer::update(const std::vector<glm::vec2>& data, GLenum usage)
{
    this->update(data.data(), sizeof(glm::vec2) * data.size(), usage);
}

void  GLVertexBuffer::update(const std::vector<glm::vec3>& data, GLenum usage)
{
    this->update(data.data(), sizeof(glm::vec3) * data.size(), usage);
}

void GLVertexBuffer::update(const std::vector<glm::vec4>& data, GLenum usage)
{
    this->update(data.data(), sizeof(glm::vec4) * data.size(), usage);
}

void GLVertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
}

void GLVertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}//end of namespace luna