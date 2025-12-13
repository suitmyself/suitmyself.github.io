/**
 * @author     : Chen Wei
 * @date       : 2023-05-10
 * @description: opengl texture
 * @version    : 1.0
 */

#pragma once

#include <string>

#include "opencv2/opencv.hpp"

#include "gl_include.h"

namespace luna {

class GLTexture
{
public:

    GLTexture() = default;

    GLTexture(int width, int height, GLenum format = GL_RGB, const unsigned char* data = nullptr, unsigned int multi_sample = 1);

    GLTexture(int width, int height, GLenum format, const float* data, unsigned int multi_sample = 1); //not clamped to [0, 1]

    GLTexture(const std::string& image_file, bool vertical_flip = true, unsigned int multi_sample = 1);

    explicit GLTexture(const cv::Mat& img, GLenum format = GL_RGB, bool vertical_flip = false, unsigned int multi_sample = 1);

    bool init(int width, int height, GLenum format = GL_RGB, const unsigned char* data = nullptr, unsigned int multi_sample = 1);

    bool init(int width, int height, GLenum format, const float* data, unsigned int multi_sample = 1);  //not clamped to [0, 1]

    bool init(const std::string& image_file, bool vertical_flip = true, unsigned int multi_sample = 1);

    bool init(const cv::Mat& img, GLenum format = GL_RGB, bool vertical_flip = false, unsigned int multi_sample = 1);

    bool update(int width, int height, GLenum format, const unsigned char* data);

    bool update(int width, int height, GLenum format, const float* data);  //not clamped to [0, 1]

    bool update(const cv::Mat& img, GLenum format = GL_RGB, bool vertical_flip = false);

    bool update(const GLTexture& src);

    bool wrap(GLuint tex_id, int width, int height);

    ~GLTexture();

    void destroy();

    //disable copy
    GLTexture(const GLTexture& rhs) = delete;
    GLTexture& operator = (const GLTexture& rhs) = delete;

    //enable move
    GLTexture(GLTexture&& rhs) noexcept;
    GLTexture& operator = (GLTexture&& rhs) noexcept;

    //-----------

    void setFilter(GLenum min_filter, GLenum mag_filter);

    void setWrapMode(GLenum wrap_s, GLenum wrap_t);

    void bind() const;

    void bind(GLuint tex_unit) const;

    void unbind() const;

    GLuint id() const;

    bool isValid() const;

    int getWidth() const;

    int getHeight() const;

    unsigned int getMultiSample() const;

    bool read(unsigned char* data, GLenum format = GL_RGB, int data_size_in_byte = -1) const; //-1 means do not check

    bool read(float* data, GLenum format = GL_RGB, int data_size_in_byte = -1) const;  //-1 means do not check

    bool read(cv::Mat& img, GLenum format = GL_RGB, bool convert_to_bgr = true, bool vertical_flip = false) const;

    bool save(const std::string& image_file, bool vertical_flip = false) const;

private:

    void setupTexture(unsigned int multi_sample);

    template <typename Scale>
    bool updateTextureData(int width, int height, GLenum format, const Scale* data);

    template <typename Scale>
    bool readTextureData(Scale * data, GLenum format = GL_RGB, int data_size_in_byte = -1) const;

public:
    static GLint getInternalFormat(GLenum format, bool use_float);

    static void checkChannelNum(const cv::Mat & mat, GLenum format);

    static int getChannelNum(GLenum format);

private:
    GLuint m_tex_id = 0;

    int m_width = 0;
    int m_height = 0;

    unsigned int m_multi_sample = 1;

    GLenum m_target = GL_TEXTURE_2D;

    bool m_own_texture = true;
};

}//end of namespace luna
