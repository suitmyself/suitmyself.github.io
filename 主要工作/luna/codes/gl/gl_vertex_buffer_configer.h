/**
 * @author     : Chen Wei
 * @date       : 2025-03-02
 * @description: vertex buffer configer
 * @version    : 1.0
 */

#pragma once

#include <vector>

#include "core/macros/platform.h"

#include "gl_vertex_buffer.h"
#include "gl_utility.h"

namespace luna {

template <typename T>
class GLVertexBufferConfiger
{
public:
    GLVertexBufferConfiger() = default;
    ~GLVertexBufferConfiger() = default;

    //disable copy
    GLVertexBufferConfiger(const GLVertexBufferConfiger &) = delete;
    GLVertexBufferConfiger & operator = (const GLVertexBufferConfiger &) = delete;

    GLVertexBufferConfiger(GLVertexBufferConfiger && rhs) noexcept
    {
        this->m_value_num = rhs.m_value_num;
        this->m_use_uniform_value = rhs.m_use_uniform_value;
        this->m_uniform_value = rhs.m_uniform_value;
        this->m_VBO = std::move(rhs.m_VBO);
    }

    GLVertexBufferConfiger & operator = (GLVertexBufferConfiger && rhs) noexcept
    {
        if (this != &rhs)
        {
            this->m_value_num = rhs.m_value_num;
            this->m_use_uniform_value = rhs.m_use_uniform_value;
            this->m_uniform_value = rhs.m_uniform_value;
            this->m_VBO = std::move(rhs.m_VBO);
        }

        return *this;
    }

    void setUniformValue(const T & value, bool use_uniform_value = true)
    {
        m_use_uniform_value = use_uniform_value;
        m_uniform_value = value;
    }

    void setValues(const std::vector<T> & values)
    {
        m_use_uniform_value = false;
        m_value_num = values.size();

        m_VBO.update(values.data(), values.size() * sizeof(T));
    }

    void setValues(const T * values, unsigned int num)
    {
        m_use_uniform_value = false;
        m_value_num = num;

        m_VBO.update(values, num * sizeof(T));
    }

    bool isUseUniformValue() const
    {
        return m_use_uniform_value;
    }

    unsigned int valueNum() const
    {
        return m_value_num;
    }

    void configToVAO(unsigned int valid_num,
                    int           index,
                    int           size,
                    GLenum        type,
                    bool          for_instance_draw = false) const
    {
        int expect_bytes = 0;
        if (type == GL_FLOAT)
            expect_bytes = sizeof(float);
        else if (type == GL_INT)
            expect_bytes = sizeof(int);
        else if (type == GL_UNSIGNED_INT)
            expect_bytes = sizeof(unsigned int);
#if __PC_PLFTFORM__
        else if (type == GL_DOUBLE)
            expect_bytes = sizeof(double);
#endif
        else
            throw std::runtime_error("error: unsupported type");

        expect_bytes *= size;

        if (sizeof(T) != expect_bytes)
            throw std::runtime_error("error: size not match!");

        //----------------------------------------------

        if (m_use_uniform_value == true)
        {

            glVerify(glDisableVertexAttribArray(index));

            if (size == 1)
            {
                if (type == GL_FLOAT)
                    glVertexAttrib1f(index, reinterpret_cast<const float &>(m_uniform_value));
#if __PC_PLATFORM__
                else if (type == GL_INT)
                    glVertexAttribI1i(index, reinterpret_cast<const int &>(m_uniform_value));
                else if (type == GL_UNSIGNED_INT)
                    glVertexAttribI1ui(index, reinterpret_cast<const unsigned int &>(m_uniform_value));
                else if (type == GL_DOUBLE)
                    glVertexAttribL1d(index, reinterpret_cast<const double &>(m_uniform_value));
#endif
                else
                    throw std::runtime_error("error: unsupported type");
            }
            else if (size == 2)
            {
                if (type == GL_FLOAT)
                    glVertexAttrib2fv(index, reinterpret_cast<const float *>(&m_uniform_value));
#if __PC_PLATFORM__
                else if (type == GL_INT)
                    glVertexAttribI2iv(index, reinterpret_cast<const int *>(&m_uniform_value));
                else if (type == GL_UNSIGNED_INT)
                    glVertexAttribI2uiv(index, reinterpret_cast<const unsigned int *>(&m_uniform_value));
                else if (type == GL_DOUBLE)
                    glVertexAttribL2dv(index, reinterpret_cast<const double *>(&m_uniform_value));
#endif
                else
                    throw std::runtime_error("error: unsupported type");
            }
            else if (size == 3)
            {
                if (type == GL_FLOAT)
                    glVertexAttrib3fv(index, reinterpret_cast<const float *>(&m_uniform_value));
#if __PC_PLATFORM__
                else if (type == GL_INT)
                    glVertexAttribI3iv(index, reinterpret_cast<const int *>(&m_uniform_value));
                else if (type == GL_UNSIGNED_INT)
                    glVertexAttribI3uiv(index, reinterpret_cast<const unsigned int *>(&m_uniform_value));
                else if (type == GL_DOUBLE)
                    glVertexAttribL3dv(index, reinterpret_cast<const double *>(&m_uniform_value));
#endif
                else
                    throw std::runtime_error("error: unsupported type");
            }
            else if (size == 4)
            {
                if (type == GL_FLOAT)
                    glVertexAttrib4fv(index, reinterpret_cast<const float *>(&m_uniform_value));
#if __PC_PLATFORM__
                else if (type == GL_INT)
                    glVertexAttrib4iv(index, reinterpret_cast<const int *>(&m_uniform_value));
                else if (type == GL_UNSIGNED_INT)
                    glVertexAttrib4uiv(index, reinterpret_cast<const unsigned int *>(&m_uniform_value));
                else if (type == GL_DOUBLE)
                    glVertexAttribL4dv(index, reinterpret_cast<const double *>(&m_uniform_value));
#endif
                else
                    throw std::runtime_error("error: unsupported type");
            }
            else
            {
                throw std::runtime_error("error: unsupported size");
            }
        }
        else
        {
            if (valid_num != m_value_num)
                throw std::runtime_error("error: size not match!");

            m_VBO.bind();
            glVertexAttribPointer(index, size, type, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(index);
            m_VBO.unbind();
        }

        if (for_instance_draw)
            glVertexAttribDivisor(index, 1);
    }

private:
    unsigned int m_value_num = 1;

    bool m_use_uniform_value = true;
    T m_uniform_value = {};

    GLVertexBuffer m_VBO;
};

}//end of namespace luna
