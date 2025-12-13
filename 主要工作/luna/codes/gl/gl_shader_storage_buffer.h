/**
 * @author     : Chen Wei
 * @date       : 2025-03-16
 * @description: opengl shader storage buffer (SSBO)
 * @version    : 1.0
 */

#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "gl_include.h"

#if !__IOS__

namespace luna{

class GLShaderStorageBuffer
{
public:

    GLShaderStorageBuffer();

    ~GLShaderStorageBuffer();

    //disable copy
    GLShaderStorageBuffer(const GLShaderStorageBuffer& rhs) = delete;
    GLShaderStorageBuffer& operator = (const GLShaderStorageBuffer& rhs) = delete;

    //enable move
    GLShaderStorageBuffer(GLShaderStorageBuffer&& rhs) noexcept;
    GLShaderStorageBuffer& operator = (GLShaderStorageBuffer&& rhs) noexcept;

    void destroy();

    GLuint id() const;

    void update(const void* data, int size, GLenum usage = GL_STATIC_DRAW);

    void bind() const;

    void unbind() const;

private:
    GLuint m_ssbo_id = 0;
};

using GLSSBO = GLShaderStorageBuffer;

}//end of namespace luna

#endif//__IOS__