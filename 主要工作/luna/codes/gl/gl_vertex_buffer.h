/**
 * @author     : Chen Wei
 * @date       : 2023-05-11
 * @description: opengl vertex buffer (VBO)
 * @version    : 1.0
 */

#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "gl_include.h"

namespace luna{

class GLVertexBuffer
{
public:

    GLVertexBuffer();

    ~GLVertexBuffer();

    //disable copy
    GLVertexBuffer(const GLVertexBuffer& rhs) = delete;
    GLVertexBuffer& operator = (const GLVertexBuffer& rhs) = delete;

    //enable move
    GLVertexBuffer(GLVertexBuffer&& rhs) noexcept;
    GLVertexBuffer& operator = (GLVertexBuffer&& rhs) noexcept;

    void destroy();

    GLuint id() const;

    void update(const void* data, int size, GLenum usage = GL_STATIC_DRAW);

    void update(const std::vector<float>& data, GLenum usage = GL_STATIC_DRAW);

    void update(const std::vector<glm::vec2>& data, GLenum usage = GL_STATIC_DRAW);

    void update(const std::vector<glm::vec3> & data, GLenum usage = GL_STATIC_DRAW);

    void update(const std::vector<glm::vec4>& data, GLenum usage = GL_STATIC_DRAW);

    void bind() const;

    void unbind() const;

private:
    GLuint m_vbo_id = 0;
};

using GLVBO = GLVertexBuffer;

}