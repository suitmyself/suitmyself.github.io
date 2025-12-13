/**
 * @author     : Chen Wei
 * @date       : 2023-05-11
 * @description: opengl element buffer (EBO)
 * @version    : 1.0
 */

#pragma once

#include <vector>

#include "gl_include.h"

namespace luna{

class GLElementBuffer
{
public:

    GLElementBuffer();

    ~GLElementBuffer();

    //disable copy
    GLElementBuffer(const GLElementBuffer& rhs) = delete;
    GLElementBuffer& operator = (const GLElementBuffer& rhs) = delete;

    //enable move
    GLElementBuffer(GLElementBuffer&& rhs) noexcept;
    GLElementBuffer& operator = (GLElementBuffer&& rhs) noexcept;

    void destroy();

    GLuint id() const;

    void update(const unsigned int* data, int size, GLenum usage = GL_STATIC_DRAW);

    void update(const std::vector<unsigned int>& data, GLenum usage = GL_STATIC_DRAW);

    void bind() const;

    void unbind() const;

private:
    GLuint m_ebo_id = 0;
};

using GLEBO = GLElementBuffer;

}