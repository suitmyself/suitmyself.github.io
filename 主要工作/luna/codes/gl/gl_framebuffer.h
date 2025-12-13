/**
 * @author     : Chen Wei
 * @date       : 2023-05-10
 * @description: opengl frame buffer
 * @version    : 1.0
 */

#pragma once

#include "opencv2/opencv.hpp"

#include "gl_include.h"

#include "gl_texture.h"

namespace luna{

class GLFrameBuffer
{
public:

    GLFrameBuffer() = default;

    GLFrameBuffer(int width,
                  int height,
                  bool need_depth = false,
                  bool need_color = true,
                  bool use_float_color = false,
                  unsigned int multi_sample = 1,
                  unsigned int color_attachment_num = 1); //color_attachment_num is only used when need_color is true

    bool init(int width,
              int height,
              bool need_depth = false,
              bool need_color = true,
              bool use_float_color = false,
              unsigned int multi_sample = 1,
              unsigned int color_attachment_num = 1); //color_attachment_num is only used when need_color is true

    bool init(const GLTexture& color_tex);

    bool init(GLuint color_tex_id, int width, int height);

    ~GLFrameBuffer();

    void destroy();

    //disable copy
    GLFrameBuffer(const GLFrameBuffer& rhs) = delete;
    GLFrameBuffer& operator = (const GLFrameBuffer& rhs) = delete;

    //enable move
    GLFrameBuffer(GLFrameBuffer&& rhs) noexcept;
    GLFrameBuffer& operator = (GLFrameBuffer&& rhs) noexcept;

    //-----------

    void bind(bool set_viewport = true, bool clear_color_depth = true) const;

    void unbind() const;

    GLuint id() const;

    GLTexture& getColorTex(int id = 0);

    const GLTexture& getColorTex(int id = 0) const;

    std::vector<GLTexture>& getColorTexVec();

    const std::vector<GLTexture>& getColorTexVec() const;

    GLTexture& getDepthTex();

    const GLTexture& getDepthTex() const;

    int getWidth() const;

    int getHeight() const;

    bool read(unsigned char* data,
              GLenum format = GL_RGB,
              int data_size_in_byte = -1, //-1 means do not check
              int color_attachment_id = 0) const;

    bool read(float* data,
              GLenum format = GL_RGB,
              int data_size_in_byte = -1, //-1 means do not check
              int color_attachment_id = 0) const;

    bool read(cv::Mat& img,
              GLenum format = GL_RGB,
              bool convert_to_bgr = true,
              bool vertical_flip = true,
              int color_attachment_id = 0) const;

    bool save(const std::string& image_file,
              bool vertical_flip = true,
              int color_attachment_id = 0) const;

private:

    template <typename Scale>
    bool readFrameBufferData(Scale* data, GLenum format, int data_size_in_byte, int color_attachment_id = 0) const;

private:
    GLuint m_fbo_id = 0;

    std::vector<GLTexture> m_fbo_color_tex_vec;

    GLTexture m_fbo_depth_tex;

    int m_width = 0;
    int m_height = 0;
};

using GLFBO = GLFrameBuffer;

}//end of namespace luna
