/**
 * @author     : Chen Wei
 * @date       : 2025-02-28
 * @description: opengl texture cube map
 * @version    : 1.0
 */

#include "gl_texture.h"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "opencv2/imgproc/types_c.h"

#include "core/log/log.h"

#include "gl_utility.h"

#include "gl_texture_cubemap.h"


namespace luna {

GLTextureCubeMap::GLTextureCubeMap(const std::string& positive_x_image_file,
                                   const std::string& negative_x_image_file,
                                   const std::string& positive_y_image_file,
                                   const std::string& negative_y_image_file,
                                   const std::string& positive_z_image_file,
                                   const std::string& negative_z_image_file)
{
    bool succ = this->init(positive_x_image_file,
                           negative_x_image_file,
                           positive_y_image_file,
                           negative_y_image_file,
                           positive_z_image_file,
                           negative_z_image_file);
    if (!succ)
    {
        LOGE("error: can not init GLTextureCubeMap");
        throw std::invalid_argument("error: can not init GLTextureCubeMap");
    }
}

GLTextureCubeMap::GLTextureCubeMap(const cv::Mat & positive_x_img,
                                   const cv::Mat & negative_x_img,
                                   const cv::Mat & positive_y_img,
                                   const cv::Mat & negative_y_img,
                                   const cv::Mat & positive_z_img,
                                   const cv::Mat & negative_z_img,
                                   GLenum format)
{
    bool succ = this->init(positive_x_img,
                           negative_x_img,
                           positive_y_img,
                           negative_y_img,
                           positive_z_img,
                           negative_z_img,
                           format);
    if (!succ)
    {
        LOGE("error: can not init GLTextureCubeMap");
        throw std::invalid_argument("error: can not init GLTextureCubeMap");
    }
}

GLTextureCubeMap::GLTextureCubeMap(const std::vector<std::string>& image_files)
{
    bool succ = this->init(image_files);
    if (!succ)
    {
        LOGE("error: can not init GLTextureCubeMap");
        throw std::invalid_argument("error: can not init GLTextureCubeMap");
    }
}

void GLTextureCubeMap::setupTexture()
{
    glGenTextures(1, &m_tex_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

template <typename Scale>
bool GLTextureCubeMap::updateTextureData(int width, int height, GLenum target, GLenum format, const Scale* data)
{
    if (m_tex_id == 0)
    {
        LOGE("error: invalid texture id: %d", m_tex_id);
        throw std::invalid_argument("error: invalid texture id");
        return false;
    }

    //for compatibility
    if (format == GL_LUMINANCE)
        format = GL_RED;

    this->bind();

    glVerify(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GLint internal_format = GLTexture::getInternalFormat(format, std::is_same_v<Scale, float>);


    if constexpr (std::is_same_v<Scale, unsigned char>)
    {
        if (format == GL_DEPTH_COMPONENT)
            glVerify(glTexImage2D(target, 0, internal_format, width, height, 0, format, GL_UNSIGNED_INT, data));
        else
            glVerify(glTexImage2D(target, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
    }
    else if constexpr (std::is_same_v<Scale, float>)
    {
        glVerify(glTexImage2D(target, 0, internal_format, width, height, 0, format, GL_FLOAT, data));
    }
    else
    {
        LOGE("error: unsupported data type");
        throw std::invalid_argument("error: unsupported data type");
        return false;
    }

    return true;
}


bool GLTextureCubeMap::init(const std::string& positive_x_image_file,
                            const std::string& negative_x_image_file,
                            const std::string& positive_y_image_file,
                            const std::string& negative_y_image_file,
                            const std::string& positive_z_image_file,
                            const std::string& negative_z_image_file)
{
    return init({positive_x_image_file,
                 negative_x_image_file,
                 positive_y_image_file,
                 negative_y_image_file,
                 positive_z_image_file,
                 negative_z_image_file});

}

bool GLTextureCubeMap::init(const std::vector<std::string> & image_files)
{
    if (image_files.size() != 6)
    {
        LOGE("error: image files size should be 6");
        throw std::invalid_argument("error: image files size should be 6");
        return false;
    }

    //destroy if necessary
    this->destroy();

    setupTexture();

    GLenum cubemap_format; 

    for (int i = 0; i < 6; ++i)
    {
        const std::string& image_file = image_files[i];

        int channel_num = 0;
        int width = 0;
        int height = 0;
        unsigned char* data = stbi_load(image_file.c_str(), &width, &height, &channel_num, 0);

        if (data == nullptr)
        {
            LOGE("error: can not load image file: %s", image_file.c_str());
            throw std::invalid_argument("error: can not load image file");
        }

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

        if (i == 0)
        {
            cubemap_format = format;
        }
        else
        {
            if (cubemap_format != format)
            {
                LOGE("error: cubemap format should be the same");
                throw std::invalid_argument("error: cubemap format should be the same");
                return false;
            }
        }

        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

        bool succ = updateTextureData(width, height, target, format, data);

        stbi_image_free(data);

        if (!succ)
        {
            LOGE("error: can not init texture");
            throw std::invalid_argument("error: can not init texture");
            return false;
        }
    }


    return true;
}

bool GLTextureCubeMap::init(const cv::Mat & positive_x_img,
                            const cv::Mat & negative_x_img,
                            const cv::Mat & positive_y_img,
                            const cv::Mat & negative_y_img,
                            const cv::Mat & positive_z_img,
                            const cv::Mat & negative_z_img,
                            GLenum format)
{
    if (positive_x_img.empty() || negative_x_img.empty() ||
        positive_y_img.empty() || negative_y_img.empty() ||
        positive_z_img.empty() || negative_z_img.empty())
    {
        LOGE("error: input image is empty");
        throw std::invalid_argument("error: input image is empty");
        return false;
    }

    if (positive_x_img.size() != negative_x_img.size() ||
        positive_x_img.size() != positive_y_img.size() ||
        positive_x_img.size() != negative_y_img.size() ||
        positive_x_img.size() != positive_z_img.size() ||
        positive_x_img.size() != negative_z_img.size())
    {
        LOGE("error: input image size should be the same");
        throw std::invalid_argument("error: input image size should be the same");
        return false;
    }

    //destroy if necessary
    this->destroy();

    setupTexture();

    std::vector<cv::Mat> images = {positive_x_img, negative_x_img,
                                   positive_y_img, negative_y_img,
                                   positive_z_img, negative_z_img};

    for (int i = 0; i < 6; ++i)
    {
        const cv::Mat& img = images[i];

        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

        bool succ =  false;

        if (img.depth() == CV_32F)
            succ = updateTextureData(img.cols, img.rows, target, format, img.ptr<float>());
        else
            succ =updateTextureData(img.cols, img.rows, target, format, img.data);

        if (!succ)
        {
            LOGE("error: can not init texture");
            throw std::invalid_argument("error: can not init texture");
            return false;
        }
    }

    return true;
}

GLTextureCubeMap::~GLTextureCubeMap()
{
    destroy();
}

void GLTextureCubeMap::destroy()
{
    if (m_tex_id != 0)
    {
        glDeleteTextures(1, &m_tex_id);
        m_tex_id = 0;
    }
}


GLTextureCubeMap::GLTextureCubeMap(GLTextureCubeMap&& rhs) noexcept
{
    m_tex_id = rhs.m_tex_id;
    rhs.m_tex_id = 0;
}

GLTextureCubeMap& GLTextureCubeMap::operator = (GLTextureCubeMap&& rhs) noexcept
{
    if (this != &rhs)
    {
        this->destroy();

        m_tex_id = rhs.m_tex_id;
        rhs.m_tex_id = 0;
    }

    return *this;
}

void GLTextureCubeMap::bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_id);
}

void GLTextureCubeMap::bind(GLuint tex_unit) const
{
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_id);
}

void GLTextureCubeMap::unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLuint GLTextureCubeMap::id() const
{
    return m_tex_id;
}

bool GLTextureCubeMap::isValid() const
{
    return m_tex_id != 0;
}

}//end of namespace luna