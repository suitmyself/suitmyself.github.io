/**
 * @author     : Chen Wei
 * @date       : 2023-05-11
 * @description: opengl vertex attrib array (VAO)
 * @version    : 1.0
 */

#include "gl_vertex_attrib_array.h"

namespace luna {

GLVertexAttribArray::GLVertexAttribArray()
{
    glGenVertexArrays(1, &m_vao_id);
}

GLVertexAttribArray::GLVertexAttribArray(GLVertexAttribArray&& rhs) noexcept
{
    m_vao_id = rhs.m_vao_id;
    rhs.m_vao_id = 0;
}

GLVertexAttribArray& GLVertexAttribArray::operator = (GLVertexAttribArray&& rhs) noexcept
{
    if (this != &rhs)
    {
        glDeleteVertexArrays(1, &m_vao_id);

        m_vao_id = rhs.m_vao_id;
        rhs.m_vao_id = 0;
    }
    return *this;
}

GLVertexAttribArray::~GLVertexAttribArray()
{
    this->destroy();
}

void GLVertexAttribArray::destroy()
{
    if (m_vao_id != 0)
    {
        glDeleteVertexArrays(1, &m_vao_id);
        m_vao_id = 0;
    }
}

GLuint GLVertexAttribArray::id() const
{
    return m_vao_id;
}

void GLVertexAttribArray::bind() const
{
    glBindVertexArray(m_vao_id);
}

void GLVertexAttribArray::unbind() const
{
    glBindVertexArray(0);
}

}//end of namespace luna

