/**
 * @author     : Chen Wei
 * @date       : 2023-08-12
 * @description: opengl vertex attrib array (VAO)
 * @version    : 1.0
 */

#pragma once

#include "gl_include.h"

namespace luna{

class GLVertexAttribArray
{
public:

    GLVertexAttribArray();

    ~GLVertexAttribArray();

    //disable copy
    GLVertexAttribArray(const GLVertexAttribArray& rhs) = delete;
    GLVertexAttribArray& operator = (const GLVertexAttribArray& rhs) = delete;

    //enable move
    GLVertexAttribArray(GLVertexAttribArray&& rhs) noexcept;
    GLVertexAttribArray& operator = (GLVertexAttribArray&& rhs) noexcept;

    void destroy();

    GLuint id() const;

    void bind() const;

    void unbind() const;

private:
    GLuint m_vao_id = 0;
};

using GLVAO = GLVertexAttribArray;

}
