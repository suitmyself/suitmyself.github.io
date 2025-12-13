/**
 * @author     : Chen Wei
 * @date       : 2023-05-10
 * @description: opengl texture
 * @version    : 1.0
 */

#include <iostream>
#include <vector>

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "opencv2/imgproc/types_c.h"

#include "core/log/log.h"

#include "gl_utility.h"
#include "gl_framebuffer.h"

#include "gl_texture.h"

namespace luna{

GLTexture::GLTexture(int width, int height, GLenum format, const unsigned char* data, unsigned int multi_sample)
{
    bool succ = this->init(width, height, format, data, multi_sample);
    if (!succ)
    {
        LOGE("error: can not init GLTexture");
        throw std::invalid_argument("error: can not init GLTexture");
    }
}

GLTexture::GLTexture(int width, int height, GLenum format, const float* data, unsigned int multi_sample)
{
    bool succ = this->init(width, height, format, data, multi_sample);
    if (!succ)
    {
        LOGE("error: can not init GLTexture");
        throw std::invalid_argument("error: can not init GLTexture");
    }
}

GLTexture::GLTexture(const std::string& image_file, bool vertical_flip, unsigned int multi_sample)
{
    bool succ = this->init(image_file, vertical_flip, multi_sample);
    if (!succ)
    {
        LOGE("error: can not init GLTexture");
        throw std::invalid_argument("error: can not init GLTexture");
    }
}

GLTexture::GLTexture(const cv::Mat& img, GLenum format, bool vertical_flip, unsigned int multi_sample)
{
    bool succ = this->init(img, format, vertical_flip, multi_sample);
    if (!succ)
    {
        LOGE("error: can not init GLTexture");
        throw std::invalid_argument("error: can not init GLTexture");
    }
}

void GLTexture::setupTexture(unsigned int multi_sample)
{

#if __IOS__
    if (multi_sample > 1)
        LOGE("Error: IOS don't support multi-sample texture, replaced by non-multi-sample texture");

    m_multi_sample = 1;
    m_target = GL_TEXTURE_2D;

#else

    m_multi_sample = multi_sample;
    m_target = (multi_sample <= 1) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;

#endif

    glVerify(glGenTextures(1, &m_tex_id));

    if (multi_sample <= 1)
    {
        glBindTexture(GL_TEXTURE_2D, m_tex_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
#if __IOS__
        ////IOS don't support multi-sample texture
        assert(false);
#else
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_tex_id);

        //NOTE: for multisample texture, we can not set wrap mode

        //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
    }
}

bool GLTexture::init(int width, int height, GLenum format, const unsigned char* data, unsigned int multi_sample)
{
    //destroy if necessary
    this->destroy();

    this->setupTexture(multi_sample);

    bool succ = this->update(width, height, format, data);
    if (!succ)
    {
        LOGE("error: can not init texture");
        throw std::invalid_argument("error: can not init texture");
        return false;
    }

    return true;
}

bool GLTexture::init(int width, int height, GLenum format, const float* data, unsigned int multi_sample)
{
    //destroy if necessary
    this->destroy();

    this->setupTexture(multi_sample);

    bool succ = this->update(width, height, format, data);
    if (!succ)
    {
        LOGE("error: can not init texture");
        throw std::invalid_argument("error: can not init texture");
        return false;
    }

    return true;
}

bool GLTexture::init(const std::string& image_file, bool vertical_flip, unsigned int multi_sample)
{
    //destroy if necessary
    this->destroy();

    stbi_set_flip_vertically_on_load(vertical_flip);

    int channel_num = 0;
    unsigned char* data = stbi_load(image_file.c_str(), &m_width, &m_height, &channel_num, 0);

    if (data == nullptr)
    {
        LOGE("error: can not load image file: %s", image_file.c_str());
        throw std::invalid_argument("error: can not load image file");
    }

    stbi_set_flip_vertically_on_load(false);

    GLenum format = GL_RGB;
    if (channel_num == 1)
        format = GL_RED;
    else if (channel_num == 2)
        format = GL_RG;
    else if (channel_num == 3)
        format = GL_RGB;
    else if (channel_num == 4)
        format = GL_RGBA;
    else
        throw std::invalid_argument("error: unsupported channel num");

    this->setupTexture(multi_sample);

    bool succ = this->update(m_width, m_height, format, data);
    if (!succ)
    {
        LOGE("error: can not init texture");
        throw std::invalid_argument("error: can not init texture");
        return false;
    }

    stbi_image_free(data);

    return true;
}

bool GLTexture::init(const cv::Mat& img, GLenum format, bool vertical_flip, unsigned int multi_sample)
{
    //destroy if necessary
    this->destroy();

    this->setupTexture(multi_sample);

    bool succ = this->update(img, format, vertical_flip);
    if (!succ)
    {
        LOGE("error: can not init texture");
        throw std::invalid_argument("error: can not init texture");
        return false;
    }

    return true;
}

template <typename Scale>
bool GLTexture::updateTextureData(int width, int height, GLenum format, const Scale* data)
{
    if (m_tex_id == 0)
    {
        LOGE("error: invalid texture id: %d", m_tex_id);
        throw std::invalid_argument("error: invalid texture id");
        return false;
    }

    m_width = width;
    m_height = height;

    //for compatibility
    if (format == GL_LUMINANCE)
        format = GL_RED;

    this->bind();

    glVerify(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GLint internal_format = this->getInternalFormat(format, std::is_same_v<Scale, float>);

    if constexpr (std::is_same_v<Scale, unsigned char>)
    {
        if (m_multi_sample > 1)
        {
#if __IOS__
            //IOS don't support multi-sample texture
            assert(false);
#elif __ANDROID__
            if (format == GL_DEPTH_COMPONENT)
                internal_format = GL_DEPTH_COMPONENT24; //special case for multi-sample depth texture

            glVerify(glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_multi_sample, internal_format, m_width, m_height, GL_TRUE));
#else
            glVerify(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_multi_sample,
                                             internal_format, m_width, m_height, GL_TRUE));
#endif
        }
        else
        {
            if (format == GL_DEPTH_COMPONENT)
                glVerify(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_width, m_height, 0, format, GL_UNSIGNED_INT, data));
            else
                glVerify(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data));
        }
    }
    else if constexpr (std::is_same_v<Scale, float>)
    {
        if (m_multi_sample > 1)
        {
#if __IOS__
            //IOS don't support multi-sample texture
            assert(false);
#elif __ANDROID__
            glVerify(glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_multi_sample, internal_format, m_width, m_height, GL_TRUE));
#else
            glVerify(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_multi_sample,
                                             internal_format, m_width, m_height, GL_TRUE));
#endif
        }
        else
        {
            glVerify(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_width, m_height, 0, format, GL_FLOAT, data));
        }
    }
    else
    {
        LOGE("error: unsupported data type");
        throw std::invalid_argument("error: unsupported data type");
        return false;
    }

    return true;
}

bool GLTexture::update(int width, int height, GLenum format, const unsigned char * data)
{
    return this->updateTextureData(width, height, format, data);
}

bool GLTexture::update(int width, int height, GLenum format, const float * data)
{
    return this->updateTextureData(width, height, format, data);
}

bool GLTexture::update(const cv::Mat& img, GLenum format, bool vertical_flip)
{
    checkChannelNum(img, format);

    if(vertical_flip)
    {
        cv::Mat flip_img;
        cv::flip(img, flip_img, 0);

        if (flip_img.depth() == CV_32F)
            return this->update(flip_img.cols, flip_img.rows, format, flip_img.ptr<float>());
        else
            return this->update(flip_img.cols, flip_img.rows, format, flip_img.data);
    }
    else
    {
        if (img.depth() == CV_32F)
            return this->update(img.cols, img.rows, format, img.ptr<float>());
        else
            return this->update(img.cols, img.rows, format, img.data);
    }
}

bool GLTexture::update(const GLTexture& src)
{
    if (m_width != src.getWidth() || m_height != src.getHeight())
    {
        LOGE("error: texture size not match");
        throw std::invalid_argument("error: texture size not match");
        return false;
    }

    if (m_target != GL_TEXTURE_2D)
    {
        LOGE("error: invalid texture target, current only support GL_TEXTURE_2D");
        throw std::invalid_argument("error: invalid texture target, current only support GL_TEXTURE_2D");
        return false;
    }

    //NOTE(Chen Wei): need further consideration

#if WIN32 || GL_ES_VERSION_3_2
    glVerify(glCopyImageSubData(src.id(), GL_TEXTURE_2D, 0, 0, 0, 0,
                                m_tex_id, GL_TEXTURE_2D, 0, 0, 0, 0,
                                m_width, m_height, 1));
#else
    GLFrameBuffer fbo;
    fbo.init(src);
    fbo.bind(false, false);
    glBindTexture(GL_TEXTURE_2D, m_tex_id);
    glVerify(glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_width, m_height, 0));
    fbo.unbind();
#endif

    return true;
}

bool GLTexture::wrap(GLuint tex_id, int width, int height)
{
    //destroy if necessary
    this->destroy();

    m_tex_id = tex_id;
    m_width = width;
    m_height = height;

    m_multi_sample = 1;
    m_target = GL_TEXTURE_2D;

    m_own_texture = false;

    return true;
}

GLTexture::~GLTexture()
{
    this->destroy();
}

void GLTexture::destroy()
{
    if (m_own_texture && m_tex_id != 0)
    {
        glDeleteTextures(1, &m_tex_id);
        m_tex_id = 0;

        m_width = 0;
        m_height = 0;

        m_multi_sample = 1;
        m_target = GL_TEXTURE_2D;
    }
}

GLTexture::GLTexture(GLTexture&& rhs) noexcept
{
    m_tex_id = rhs.m_tex_id;
    m_width = rhs.m_width;
    m_height = rhs.m_height;
    m_multi_sample = rhs.m_multi_sample;
    m_target = rhs.m_target;
    m_own_texture = rhs.m_own_texture;

    rhs.m_tex_id = 0;
    rhs.m_width = 0;
    rhs.m_height = 0;
    rhs.m_multi_sample = 1;
    rhs.m_target = GL_TEXTURE_2D;
    rhs.m_own_texture = true;
}

GLTexture& GLTexture::operator = (GLTexture&& rhs) noexcept
{
    if (this != &rhs)
    {
        this->destroy();

        m_tex_id = rhs.m_tex_id;
        m_width = rhs.m_width;
        m_height = rhs.m_height;
        m_multi_sample = rhs.m_multi_sample;
        m_target = rhs.m_target;
        m_own_texture = rhs.m_own_texture;

        rhs.m_tex_id = 0;
        rhs.m_width = 0;
        rhs.m_height = 0;
        rhs.m_multi_sample = 1;
        rhs.m_target = GL_TEXTURE_2D;
        rhs.m_own_texture = true;
    }
    return *this;
}

void GLTexture::setFilter(GLenum min_filter, GLenum mag_filter)
{
    glBindTexture(m_target, m_tex_id);
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void GLTexture::setWrapMode(GLenum wrap_s, GLenum wrap_t)
{
    glBindTexture(m_target, m_tex_id);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, wrap_t);
}

void GLTexture::bind() const
{
    glBindTexture(m_target, m_tex_id);
}

void GLTexture::bind(GLuint tex_unit) const
{
    glActiveTexture(tex_unit);
    glBindTexture(m_target, m_tex_id);
}

void GLTexture::unbind() const
{
    glBindTexture(m_target, 0);
}

GLuint GLTexture::id() const
{
    return m_tex_id;
}

bool GLTexture::isValid() const
{
    return m_tex_id != 0;
}

int GLTexture::getWidth() const
{
#if (!NDEBUG) && (WIN32 || __MACOS__)
    GLint pre_bind_tex_id = 0;
    GLenum binding_target = (m_target == GL_TEXTURE_2D ? GL_TEXTURE_BINDING_2D : GL_TEXTURE_BINDING_2D_MULTISAMPLE);
    glGetIntegerv(binding_target, &pre_bind_tex_id);
    glBindTexture(m_target, m_tex_id);
    int width = 0;
    glGetTexLevelParameteriv(m_target, 0, GL_TEXTURE_WIDTH, &width);
    glBindTexture(m_target, pre_bind_tex_id);
    assert(width == m_width);
#endif

    return m_width;
}

int GLTexture::getHeight() const
{
#if (!NDEBUG) && (WIN32 || __MACOS__)
    GLint pre_bind_tex_id = 0;
    GLenum binding_target = (m_target == GL_TEXTURE_2D ? GL_TEXTURE_BINDING_2D : GL_TEXTURE_BINDING_2D_MULTISAMPLE);
    glGetIntegerv(binding_target, &pre_bind_tex_id);
    glBindTexture(m_target, m_tex_id);
    int height = 0;
    glGetTexLevelParameteriv(m_target, 0, GL_TEXTURE_HEIGHT, &height);
    glBindTexture(m_target, pre_bind_tex_id);
    assert(height == m_height);
#endif

    return m_height;
}

unsigned int GLTexture::getMultiSample() const
{
    return m_multi_sample;
}

template <typename Scale>
bool GLTexture::readTextureData(Scale* data, GLenum format, int data_size_in_byte) const
{
    if (data == nullptr)
    {
        LOGE("error: data is nullptr");
        throw std::invalid_argument("error: data is nullptr");
        return false;
    }

    if (m_tex_id == 0)
    {
        LOGE("error: invalid texture id: %d", m_tex_id);
        throw std::invalid_argument("error: invalid texture id");
        return false;
    }

    if (m_target != GL_TEXTURE_2D)
    {
        LOGE("error: invalid texture target, current only support GL_TEXTURE_2D");
        throw std::invalid_argument("error: invalid texture target, current only support GL_TEXTURE_2D");
        return false;
    }

    if (data_size_in_byte >= 0)
    {
        const int channel_num = this->getChannelNum(format);
        const int expected_size = m_width * m_height * channel_num;
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

#if WIN32 || __MACOS__
    this->bind();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    if constexpr (std::is_same_v<Scale, unsigned char>)
    {
        glVerify(glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, data));
    }
    else if constexpr (std::is_same_v<Scale, float>)
    {
        glVerify(glGetTexImage(GL_TEXTURE_2D, 0, format, GL_FLOAT, data));
    }
    else
    {
        LOGE("error: unsupported data type");
        throw std::invalid_argument("error: unsupported data type");
        return false;
    }
    glVerify(glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, data));
    this->unbind();
#else
    //NOTE(Chen Wei): OpenGLES has no glGetTexImage
    GLFrameBuffer fbo;
    fbo.init(*this);
    fbo.bind(false, false);
    fbo.read(data, format);
    fbo.unbind();
#endif

    return true;
}

bool GLTexture::read(unsigned char* data, GLenum format, int data_size_in_byte) const
{
    return this->readTextureData(data, format, data_size_in_byte);
}

bool GLTexture::read(float* data, GLenum format, int data_size_in_byte) const
{
    return this->readTextureData(data, format, data_size_in_byte);
}

bool GLTexture::read(cv::Mat& img, GLenum format, bool convert_to_bgr, bool vertical_flip) const
{
    //for compatibility
    if (format == GL_LUMINANCE)
        format = GL_RED;

    int type = CV_8UC3;
    if (format == GL_RED)
        type = CV_8UC1;
    else if (format == GL_RG)
        type = CV_8UC2;
    else if (format == GL_RGB)
        type = CV_8UC3;
    else if (format == GL_RGBA)
        type = CV_8UC4;

    //NOTE(Chen Wei): warning: GL_RGB may not support by ios (gles 3.0), so we change to read RGBA
    //                opengl es may only support GL_RGBA, compatibility need further consideration
#if __IOS__
    bool need_convert_back = false;
    if (format == GL_RGB)
    {
        format = GL_RGBA;
        type = CV_8UC4;

        need_convert_back = true;
    }
#endif

    img.create(m_height, m_width, type);

    bool succ = this->read(img.data, format, img.total() * img.elemSize());
    if (!succ)
    {
        LOGE("error: can not read texture");
        throw std::invalid_argument("error: can not read texture");
        return false;
    }

#if __IOS__
    if (need_convert_back)
    {
        cv::cvtColor(img, img, CV_RGBA2RGB);
    }
#endif

    if (convert_to_bgr)
    {
        if (format == GL_RGB)
        {
            cv::cvtColor(img, img, CV_RGB2BGR);
        }
        else if (format == GL_RGBA)
        {
            cv::cvtColor(img, img, CV_RGBA2BGRA);
        }
        else
        {
            LOGE("error: unsupported format to convert_to_bgr");
            throw std::invalid_argument("error: unsupported format to convert_to_bgr");
            return false;
        }
    }

    if (vertical_flip)
        cv::flip(img, img, 0);

    return true;
}

bool GLTexture::save(const std::string& image_file, bool vertical_flip) const
{
    std::vector<unsigned char> data(3 * m_height * m_width);

    bool succ = this->read(data.data(), GL_RGBA);
    if (!succ)
    {
        LOGE("error: can not read texture");
        throw std::invalid_argument("error: can not read texture");
        return false;
    }

    stbi_flip_vertically_on_write(vertical_flip);
    stbi_write_png(image_file.c_str(), m_width, m_height, 3, data.data(), 3 * m_width);
    stbi_flip_vertically_on_write(false);

    return true;
}

GLint GLTexture::getInternalFormat(GLenum format, bool use_float)
{
    //NOTE(Chen Wei): opengl internal format can not be GL_BGR/GL_BGRA

#if WIN32 || __MACOS__
    //NOTE(Chen Wei): OpenGLES has no GL_BGR or GL_BGRA
    if (format == GL_BGR)
        return GL_RGB;

    if (format == GL_BGRA)
        return GL_RGBA;
#endif

    //NOTE(Chen Wei): need further considerations

    if (use_float)
    {
        if (format == GL_RED)
            return GL_R32F;
        if (format == GL_RG)
            return GL_RG32F;
        if (format == GL_RGB)
            return GL_RGB32F;
        if (format == GL_RGBA)
            return GL_RGBA32F;
        if (format == GL_DEPTH_COMPONENT)
            return GL_DEPTH_COMPONENT32F;

        throw std::invalid_argument("error: unsupported format");
    }
    else
    {
        if (format == GL_RED)
            return GL_R8;
        if (format == GL_RG)
            return GL_RG8;
        if (format == GL_RGB)
            return GL_RGB8;
        if (format == GL_RGBA)
            return GL_RGBA8;
        if (format == GL_DEPTH_COMPONENT)
            return GL_DEPTH_COMPONENT;

        throw std::invalid_argument("error: unsupported format");
    }
}

void GLTexture::checkChannelNum(const cv::Mat& mat, GLenum format)
{
    int channel_num = mat.channels();

    if (format == GL_RED && channel_num != 1)
        throw std::invalid_argument("error: channel num not match");

    if (format == GL_BLUE && channel_num != 1)
        throw std::invalid_argument("error: channel num not match");

    if (format == GL_GREEN && channel_num != 1)
        throw std::invalid_argument("error: channel num not match");

    if (format == GL_LUMINANCE && channel_num != 1)
        throw std::invalid_argument("error: channel num not match");

    if (format == GL_RG && channel_num != 2)
        throw std::invalid_argument("error: channel num not match");

    if (format == GL_RGB && channel_num != 3)
        throw std::invalid_argument("error: channel num not match");

    if (format == GL_RGBA && channel_num != 4)
        throw std::invalid_argument("error: channel num not match");

#if WIN32 || __MACOS__

    if (format == GL_BGR && channel_num != 3)
        throw std::invalid_argument("error: channel num not match");

    if (format == GL_BGRA && channel_num != 4)
        throw std::invalid_argument("error: channel num not match");

#endif
}

int GLTexture::getChannelNum(GLenum format)
{
    if (format == GL_RED)
        return 1;
    if (format == GL_BLUE)
        return 1;
    if (format == GL_GREEN)
        return 1;
    if (format == GL_LUMINANCE)
        return 1;
    if (format == GL_RG)
        return 2;
    if (format == GL_RGB)
        return 3;
    if (format == GL_RGBA)
        return 4;

#if WIN32 || __MACOS__

    if (format == GL_BGR)
        return 3;

    if (format == GL_BGRA)
        return 4;

#endif

    throw std::invalid_argument("error: unsupported format");
}

}//end of namespace luna
