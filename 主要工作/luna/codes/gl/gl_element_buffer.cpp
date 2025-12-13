/**
 * @author     : Chen Wei
 * @date       : 2023-05-11
 * @description: opengl element buffer (EBO)
 * @version    : 1.0
 */

#include "gl_element_buffer.h"

namespace luna {

GLElementBuffer::GLElementBuffer()
{
    glGenBuffers(1, &m_ebo_id);
}

GLElementBuffer::GLElementBuffer(GLElementBuffer&& rhs) noexcept
{
    m_ebo_id = rhs.m_ebo_id;
    rhs.m_ebo_id = 0;
}

GLElementBuffer& GLElementBuffer::operator = (GLElementBuffer&& rhs) noexcept
{
    if (this != &rhs)
    {
        glDeleteBuffers(1, &m_ebo_id);

        m_ebo_id = rhs.m_ebo_id;
        rhs.m_ebo_id = 0;
    }

    return *this;
}

GLElementBuffer::~GLElementBuffer()
{
    this->destroy();
}

void GLElementBuffer::destroy()
{
    if (m_ebo_id != 0)
    {
        glDeleteBuffers(1, &m_ebo_id);
        m_ebo_id = 0;
    }
}

GLuint GLElementBuffer::id() const
{
    return m_ebo_id;
}

void GLElementBuffer::update(const unsigned int* data, int size, GLenum usage)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLElementBuffer::update(const std::vector<unsigned int>& data, GLenum usage)
{
    this->update(data.data(), sizeof(unsigned int) * data.size(), usage);
}

void GLElementBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);
}

void GLElementBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}//end of namespace luna
