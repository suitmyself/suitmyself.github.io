/**
 * @author     : Chen Wei
 * @date       : 2025-03-14
 * @description: opengl uniform buffer (UBO)
 * @version    : 1.0
 */

#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "gl_include.h"

namespace luna{

class GLUniformBuffer
{
public:

    GLUniformBuffer();

    ~GLUniformBuffer();

    //disable copy
    GLUniformBuffer(const GLUniformBuffer& rhs) = delete;
    GLUniformBuffer& operator = (const GLUniformBuffer& rhs) = delete;

    //enable move
    GLUniformBuffer(GLUniformBuffer&& rhs) noexcept;
    GLUniformBuffer& operator = (GLUniformBuffer&& rhs) noexcept;

    void destroy();

    GLuint id() const;

    void update(const void* data, int size, GLenum usage = GL_STATIC_DRAW);

    void bind() const;

    void unbind() const;

private:
    GLuint m_ubo_id = 0;
};

using GLUBO = GLUniformBuffer;

}