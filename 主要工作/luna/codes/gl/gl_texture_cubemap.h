/**
 * @author     : Chen Wei
 * @date       : 2025-02-28
 * @description: opengl texture cube map
 * @version    : 1.0
 */

 #pragma once

 #include <string>

 #include "opencv2/opencv.hpp"

 #include "gl_include.h"

 namespace luna {

 class GLTextureCubeMap
 {
 public:

    GLTextureCubeMap() = default;

    GLTextureCubeMap(const std::string& positive_x_image_file,
                     const std::string& negative_x_image_file,
                     const std::string& positive_y_image_file,
                     const std::string& negative_y_image_file,
                     const std::string& positive_z_image_file,
                     const std::string& negative_z_image_file);

    GLTextureCubeMap(const cv::Mat & positive_x_img,
                     const cv::Mat & negative_x_img,
                     const cv::Mat & positive_y_img,
                     const cv::Mat & negative_y_img,
                     const cv::Mat & positive_z_img,
                     const cv::Mat & negative_z_img,
                     GLenum format = GL_RGB);

    GLTextureCubeMap(const std::vector<std::string>& image_files);

    bool init(const std::string& positive_x_image_file,
              const std::string& negative_x_image_file,
              const std::string& positive_y_image_file,
              const std::string& negative_y_image_file,
              const std::string& positive_z_image_file,
              const std::string& negative_z_image_file);

    bool init(const cv::Mat & positive_x_img,
              const cv::Mat & negative_x_img,
              const cv::Mat & positive_y_img,
              const cv::Mat & negative_y_img,
              const cv::Mat & positive_z_img,
              const cv::Mat & negative_z_img,
              GLenum format = GL_RGB);

    bool init(const std::vector<std::string>& image_files);

    ~GLTextureCubeMap();

    void destroy();

    //disable copy
    GLTextureCubeMap(const GLTextureCubeMap& rhs) = delete;
    GLTextureCubeMap& operator = (const GLTextureCubeMap& rhs) = delete;

    //enable move
    GLTextureCubeMap(GLTextureCubeMap&& rhs) noexcept;
    GLTextureCubeMap& operator = (GLTextureCubeMap&& rhs) noexcept;

    //-----------

    void bind() const;

    void bind(GLuint tex_unit) const;

    void unbind() const;

    GLuint id() const;

    bool isValid() const;


 private:
    void setupTexture();

    template <typename Scale>
    bool updateTextureData(int width, int height, GLenum target, GLenum format, const Scale* data);

private:
    GLuint m_tex_id = 0;
 };

 }//end of namespace luna
