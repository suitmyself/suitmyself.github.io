/**
 * @author     : Chen Wei
 * @date       : 2023-05-10
 * @description: opengl frame buffer
 * @version    : 1.0
 */

#include <iostream>
#include <stdexcept>
#include <vector>

#include "stb/stb_image_write.h"

#include "opencv2/imgproc/types_c.h"

#include "core/log/log.h"

#include "gl_utility.h"

#include "gl_framebuffer.h"

namespace luna{

GLFrameBuffer::GLFrameBuffer(int width,
                             int height,
                             bool need_depth,
                             bool need_color,
                             bool use_float_color,
                             unsigned int multi_sample,
                             unsigned int color_attachment_num)
{
    bool succ = this->init(width, height, need_depth, need_color, use_float_color, multi_sample, color_attachment_num);
    if (!succ)
    {
        LOGE("error: can not init GLFrameBuffer");
        throw std::invalid_argument("error: can not init GLFrameBuffer");
    }
}

bool GLFrameBuffer::init(int width,
                         int height,
                         bool need_depth,
                         bool need_color,
                         bool use_float_color,
                         unsigned int multi_sample,
                         unsigned int color_attachment_num)
{
    //destroy if necessary
    this->destroy();

    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);

#if __IOS__
    if (multi_sample > 1)
    {
        multi_sample = 1;
        LOGE("Error: IOS don't support multi-sample texture, replaced by non-multi-sample texture");
    }

    GLenum target = GL_TEXTURE_2D;
#else
    GLenum target = multi_sample > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
#endif

    if (need_color)
    {
        GLint MAX_COLOR_ATTACHMENTS;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &MAX_COLOR_ATTACHMENTS);

        if (MAX_COLOR_ATTACHMENTS < color_attachment_num)
        {
            LOGE("error: MAX_COLOR_ATTACHMENTS(%d) is less than color_attachment_num(%d)", MAX_COLOR_ATTACHMENTS, color_attachment_num);
            throw std::invalid_argument("error: MAX_COLOR_ATTACHMENTS is less than color_attachment_num");
            return false;
        }

        m_fbo_color_tex_vec.resize(color_attachment_num);
        for (unsigned int i = 0; i < color_attachment_num; ++i)
        {
            if (use_float_color)
            {
                m_fbo_color_tex_vec[i].init(m_width, m_height, GL_RGBA, static_cast<float*>(nullptr), multi_sample);
#if __IOS__
                //NOTE(Chen Wei): for ios opengles 3.0, only GL_RGBA16F is supported for FBO with float-type color attachment
                m_fbo_color_tex_vec[i].bind();
                glVerify(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr));
                m_fbo_color_tex_vec[i].unbind();
                
#endif
            }
            else
            {
                m_fbo_color_tex_vec[i].init(m_width, m_height, GL_RGBA, static_cast<unsigned char*>(nullptr), multi_sample);
            }

            m_fbo_color_tex_vec[i].bind();
            glVerify(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, m_fbo_color_tex_vec[i].id(), 0));
            m_fbo_color_tex_vec[i].unbind();
        }
    }

    if (need_depth)
    {
#if __ANDROID__
        //NOTE(Chen Wei): use float-type depth texture for android get wrong result, why ?
        m_fbo_depth_tex.init(m_width, m_height, GL_DEPTH_COMPONENT, static_cast<unsigned char*>(nullptr), multi_sample);
#else
        m_fbo_depth_tex.init(m_width, m_height, GL_DEPTH_COMPONENT, static_cast<float*>(nullptr), multi_sample);
#endif

        m_fbo_depth_tex.bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, m_fbo_depth_tex.id(), 0);
        m_fbo_depth_tex.unbind();
    }

    //check frame buffer status
    if (openGLCheckCurFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGE("error: FBO is incomplete !");
        throw std::runtime_error("error: FBO is incomplete !");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

bool GLFrameBuffer::init(const GLTexture& color_tex)
{
    return this->init(color_tex.id(), color_tex.getWidth(), color_tex.getHeight());
}

bool GLFrameBuffer::init(GLuint color_tex_id, int width, int height)
{
    //destroy if necessary
    this->destroy();

    m_width = width;
    m_height = height;

    //NOTE(Chen Wei): not supported by OpenGL ES 3.0 & lower, need further consideration
    // glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_2D, &m_width);
    // glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_height);

    glGenFramebuffers(1, &m_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);

    GLTexture color_tex;
    color_tex.wrap(color_tex_id, m_width, m_height);

    color_tex.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex_id, 0);
    color_tex.unbind();

    m_fbo_color_tex_vec.push_back(std::move(color_tex));

    //check frame buffer status
    if (openGLCheckCurFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGE("error: FBO is incomplete !");
        std::exit(EXIT_FAILURE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

GLFrameBuffer::GLFrameBuffer(GLFrameBuffer&& rhs) noexcept
{
    m_fbo_id = rhs.m_fbo_id;
    m_width = rhs.m_width;
    m_height = rhs.m_height;

    m_fbo_color_tex_vec = std::move(rhs.m_fbo_color_tex_vec);
    m_fbo_depth_tex = std::move(rhs.m_fbo_depth_tex);

    rhs.m_fbo_id = 0;
    rhs.m_width = 0;
    rhs.m_height = 0;
}

GLFrameBuffer& GLFrameBuffer::operator = (GLFrameBuffer&& rhs) noexcept
{
    if (this == &rhs)
        return *this;

    this->destroy();

    m_fbo_id = rhs.m_fbo_id;
    m_width = rhs.m_width;
    m_height = rhs.m_height;

    m_fbo_color_tex_vec = std::move(rhs.m_fbo_color_tex_vec);
    m_fbo_depth_tex = std::move(rhs.m_fbo_depth_tex);

    rhs.m_fbo_id = 0;
    rhs.m_width = 0;
    rhs.m_height = 0;

    return *this;
}

GLFrameBuffer::~GLFrameBuffer()
{
    this->destroy();
}

void GLFrameBuffer::destroy()
{
    if (m_fbo_id != 0)
    {
        glDeleteFramebuffers(1, &m_fbo_id);
        m_fbo_id = 0;

        m_width = 0;
        m_height = 0;

        m_fbo_color_tex_vec.clear();
        m_fbo_depth_tex.destroy();
    }
}

void GLFrameBuffer::bind(bool set_viewport, bool clear_color_depth) const
{
    glVerify(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id));

    std::vector<GLuint> attachments;
    for (unsigned int i = 0; i < m_fbo_color_tex_vec.size(); ++i)
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);

    glVerify(glDrawBuffers(attachments.size(), attachments.data()));

    if (set_viewport)
        glViewport(0, 0, m_width, m_height);

    if (clear_color_depth)
    {
        glClearDepthf(1.0);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void GLFrameBuffer::unbind() const
{
    glVerify(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    //error: Invalid Operation, why ?
    //GLuint attachments[] = {GL_COLOR_ATTACHMENT0};
    //glVerify(glDrawBuffers(1, attachments));
}

GLuint GLFrameBuffer::id() const
{
    return m_fbo_id;
}

GLTexture& GLFrameBuffer::getColorTex(int id)
{
    return m_fbo_color_tex_vec[id];
}

const GLTexture & GLFrameBuffer::getColorTex(int id) const
{
    return m_fbo_color_tex_vec[id];
}

std::vector<GLTexture>& GLFrameBuffer::getColorTexVec()
{
    return m_fbo_color_tex_vec;
}

const std::vector<GLTexture>& GLFrameBuffer::getColorTexVec() const
{
    return m_fbo_color_tex_vec;
}

GLTexture& GLFrameBuffer::getDepthTex()
{
    return m_fbo_depth_tex;
}

const GLTexture & GLFrameBuffer::getDepthTex() const
{
    return m_fbo_depth_tex;
}

int GLFrameBuffer::getWidth() const
{
    return m_width;
}

int GLFrameBuffer::getHeight() const
{
    return m_height;
}

template <typename  Scale>
bool GLFrameBuffer::readFrameBufferData(Scale* data, GLenum format, int data_size_in_byte, int color_attachment_id) const
{
    if (data == nullptr)
    {
        LOGE("error: data is nullptr");
        throw std::invalid_argument("error: data is nullptr");
        return false;
    }

    if (m_fbo_id == 0)
    {
        LOGE("error: invalid fbo id: %d", m_fbo_id);
        throw std::invalid_argument("error: invalid fbo id");
        return false;
    }

    if (data_size_in_byte >= 0)
    {
        const int channel_num = GLTexture::getChannelNum(format);
        const int expected_size = m_width * m_height * channel_num * sizeof(float);
        if (data_size_in_byte < expected_size)
        {
            LOGE("error: size not match");
            throw std::invalid_argument("error: size not match");
            return false;
        }
    }

    //for compatibility
    if (format == GL_LUMINANCE)
        format = GL_RED;

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    //NOTE(Chen Wei): warning: GL_RGB may not supported by ios (gles 3.0)

    if constexpr (std::is_same<Scale, unsigned char>::value)
    {
        if (format == GL_DEPTH_COMPONENT)
        {
            glVerify(glReadPixels(0, 0, m_width, m_height, format, GL_UNSIGNED_INT, data));
        }
        else
        {
            glVerify(glReadBuffer(GL_COLOR_ATTACHMENT0 + color_attachment_id));
            glVerify(glReadPixels(0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, data));
            glVerify(glReadBuffer(GL_COLOR_ATTACHMENT0));
        }
    }
    else if constexpr (std::is_same<Scale, float>::value)
    {
        if (format == GL_DEPTH_COMPONENT)
        {
            glVerify(glReadPixels(0, 0, m_width, m_height, format, GL_FLOAT, data));
        }
        else
        {
            glVerify(glReadBuffer(GL_COLOR_ATTACHMENT0 + color_attachment_id));
            glVerify(glReadPixels(0, 0, m_width, m_height, format, GL_FLOAT, data));
            glVerify(glReadBuffer(GL_COLOR_ATTACHMENT0));
        }
    }
    else
    {
        LOGE("error: unsupported data type");
        throw std::invalid_argument("error: unsupported data type");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

bool GLFrameBuffer::read(unsigned char* data, GLenum format, int data_size_in_byte, int color_attachment_id) const
{
    return readFrameBufferData(data, format, data_size_in_byte, color_attachment_id);
}

bool GLFrameBuffer::read(float* data, GLenum format, int data_size_in_byte, int color_attachment_id) const
{
    return readFrameBufferData(data, format, data_size_in_byte, color_attachment_id);
}

bool GLFrameBuffer::read(cv::Mat& img, GLenum format, bool convert_to_bgr, bool vertical_flip, int color_attachment_id) const
{
    int type = CV_8UC3;
    if (format == GL_RED)
        type = CV_8UC1;
    else if (format == GL_LUMINANCE)
        type = CV_8UC1;
    else if (format == GL_RG)
        type = CV_8UC2;
    else if (format == GL_RGB)
        type = CV_8UC3;
    else if (format == GL_RGBA)
        type = CV_8UC4;

    if (format == GL_DEPTH_COMPONENT)
    {
#if __ANDROID__
        type = CV_32SC1;
#else
        type = CV_32FC1;
#endif
    }


    img.create(m_height, m_width, type);

    bool succ = this->read(img.data, format, -1, color_attachment_id);
    if (!succ)
    {
        LOGE("error: can not read frame buffer");
        return false;
    }

    if (convert_to_bgr)
    {
        if (format == GL_RGB)
            cv::cvtColor(img, img, CV_RGB2BGR);
        else if (format == GL_RGBA)
            cv::cvtColor(img, img, CV_RGBA2BGRA);
        else
            LOGE("error: unsupported format to convert_to_bgr");
    }

    if (vertical_flip)
        cv::flip(img, img, 0);

    return true;
}

bool GLFrameBuffer::save(const std::string& image_file, bool vertical_flip, int color_attachment_id) const
{
    std::vector<unsigned char> data(3 * m_height * m_width);

    bool succ = this->read(data.data(), GL_RGB, -1, color_attachment_id);
    if (!succ)
    {
        LOGE("error: can not read texture");
        return false;
    }

    stbi_flip_vertically_on_write(vertical_flip);
    stbi_write_png(image_file.c_str(), m_width, m_height, 3, data.data(), 3 * m_width);
    stbi_flip_vertically_on_write(false);

    return true;
}

}//end of namespace luna
