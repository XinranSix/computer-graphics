#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ht {
    namespace opengl {
        class Shader {
        public:
            enum class ShaderType {
                VERTEX,
                FRAGMENT,
                GEOMETRY,
                TESSELLATION_CONTROL,
                TESSELLATION_EVALUATION,
                COMPUTE
            };

            explicit Shader(ShaderType type);
            ~Shader();

            ShaderType shaderType() const;

            bool compileSourceCode(std::string_view source);
            bool compileSourceFile(std::string_view fileName);

            std::string sourceCode() const;

            bool isCompiled() const;
            std::string log() const;

            GLuint shaderId() const;

            static bool hasOpenGLShaders(ShaderType type,
                                         GLFWwindow *context = nullptr);

        private:
            class Impl;
            std::unique_ptr<Impl> pImpl;
        };

#if 0
        class ShaderProgram {

        public:
            explicit ShaderProgram();
            ~ShaderProgram();

            bool addShader(Shader *shader);
            void removeShader(Shader *shader);
            std::vector<Shader *> shaders() const;

            bool addShaderFromSourceCode(Shader::ShaderType type,
                                         std::string_view source);
            bool addShaderFromSourceFile(Shader::ShaderType type,
                                         std::string_view &fileName);

            /*  bool addCacheableShaderFromSourceCode(Shader::ShaderType type,
                                                   std::string_view source);
             bool addCacheableShaderFromSourceFile(Shader::ShaderType type,
                                                   std::string_view fileName);
           */

            void removeAllShaders();

            virtual bool link();
            bool isLinked() const;
            std::string log() const;

            bool bind();
            void release();

            bool create();

            GLuint programId() const;

            /*     int maxGeometryOutputVertices() const;

                void setPatchVertexCount(int count);
                int patchVertexCount() const; */

            /* void
            setDefaultOuterTessellationLevels(const std::vector<float> &levels);
            std::vector<float> defaultOuterTessellationLevels() const;

            void
            setDefaultInnerTessellationLevels(const std::vector<float> &levels);
            std::vector<float> defaultInnerTessellationLevels() const; */
            /*
                    void bindAttributeLocation(std::string_view name, int
               location);

                    int attributeLocation(std::string_view name) const;

                    void setAttributeValue(int location, GLfloat value);
                    void setAttributeValue(int location, GLfloat x, GLfloat y);
                    void setAttributeValue(int location, GLfloat x, GLfloat y,
               GLfloat z); void setAttributeValue(int location, GLfloat x,
               GLfloat y, GLfloat z, GLfloat w); void setAttributeValue(int
               location, const glm::vec2 &value); void setAttributeValue(int
               location, const glm::vec3 &value); void setAttributeValue(int
               location, const glm::vec4 &value); void setAttributeValue(int
               location, const GLfloat *values, int columns, int rows);

                    void setAttributeValue(std::string_view name, GLfloat
               value); void setAttributeValue(std::string_view name, GLfloat x,
               GLfloat y); void setAttributeValue(std::string_view name, GLfloat
               x, GLfloat y, GLfloat z); void setAttributeValue(std::string_view
               name, GLfloat x, GLfloat y, GLfloat z, GLfloat w); void
               setAttributeValue(std::string_view name, const glm::vec2 &value);
                    void setAttributeValue(std::string_view name, const
               glm::vec3 &value); void setAttributeValue(std::string_view name,
               const glm::vec4 &value); void setAttributeValue(std::string_view
               name, const GLfloat *values, int columns, int rows);

                    void setAttributeArray(int location, const GLfloat *values,
                                           int tupleSize, int stride = 0);
                    void setAttributeArray(int location, const glm::vec2
               *values, int stride = 0); void setAttributeArray(int location,
               const glm::vec3 *values, int stride = 0); void
               setAttributeArray(int location, const glm::vec4 *values, int
               stride = 0); void setAttributeArray(int location, GLenum type,
               const void *values, int tupleSize, int stride = 0); void
               setAttributeArray(std::string_view name, const GLfloat *values,
               int tupleSize, int stride = 0); void
               setAttributeArray(std::string_view name, const glm::vec2 *values,
               int stride = 0); void setAttributeArray(std::string_view name,
               const glm::vec3 *values, int stride = 0); void
               setAttributeArray(std::string_view name, const glm::vec4 *values,
               int stride = 0); void setAttributeArray(std::string_view name,
               GLenum type, const void *values, int tupleSize, int stride = 0);

                    void setAttributeBuffer(int location, GLenum type, int
               offset, int tupleSize, int stride = 0); void
               setAttributeBuffer(std::string_view name, GLenum type, int
               offset, int tupleSize, int stride = 0);

                    void enableAttributeArray(int location);
                    void enableAttributeArray(std::string_view name);
                    void disableAttributeArray(int location);
                    void disableAttributeArray(std::string_view name); */

            int uniformLocation(std::string_view name) const;

            void setUniformValue(int location, GLfloat value);
            void setUniformValue(int location, GLint value);
            void setUniformValue(int location, GLuint value);
            void setUniformValue(int location, GLfloat x, GLfloat y);
            void setUniformValue(int location, GLfloat x, GLfloat y, GLfloat z);
            void setUniformValue(int location, GLfloat x, GLfloat y, GLfloat z,
                                 GLfloat w);
            void setUniformValue(int location, const glm::vec2 &value);
            void setUniformValue(int location, const glm::vec3 &value);
            void setUniformValue(int location, const glm::vec4 &value);
            /*         void setUniformValue(int location, const QColor &color);
                    void setUniformValue(int location, const QPoint &point);
                    void setUniformValue(int location, const QPointF &point);
                    void setUniformValue(int location, const QSize &size);
                    void setUniformValue(int location, const QSizeF &size); */

            void setUniformValue(int location, const glm::mat2x2 &value);
            void setUniformValue(int location, const glm::mat2x3 &value);
            void setUniformValue(int location, const glm::mat2x4 &value);
            void setUniformValue(int location, const glm::mat3x2 &value);
            void setUniformValue(int location, const glm::mat3x3 &value);
            void setUniformValue(int location, const glm::mat3x4 &value);
            void setUniformValue(int location, const glm::mat4x2 &value);
            void setUniformValue(int location, const glm::mat4x3 &value);
            void setUniformValue(int location, const glm::mat4x4 &value);
            void setUniformValue(int location, const GLfloat value[2][2]);
            void setUniformValue(int location, const GLfloat value[3][3]);
            void setUniformValue(int location, const GLfloat value[4][4]);
            /*         void setUniformValue(int location, const QTransform
             * &value);
             */

            void setUniformValue(std::string_view name, GLfloat value);
            void setUniformValue(std::string_view name, GLint value);
            void setUniformValue(std::string_view name, GLuint value);
            void setUniformValue(std::string_view name, GLfloat x, GLfloat y);
            void setUniformValue(std::string_view name, GLfloat x, GLfloat y,
                                 GLfloat z);
            void setUniformValue(std::string_view name, GLfloat x, GLfloat y,
                                 GLfloat z, GLfloat w);
            void setUniformValue(std::string_view name, const glm::vec2 &value);
            void setUniformValue(std::string_view name, const glm::vec3 &value);
            void setUniformValue(std::string_view name, const glm::vec4 &value);
            /*         void setUniformValue(std::string_view name, const QColor
               &color); void setUniformValue(std::string_view name, const QPoint
               &point); void setUniformValue(std::string_view name, const
               QPointF &point); void setUniformValue(std::string_view name,
               const QSize &size); void setUniformValue(std::string_view name,
               const QSizeF &size); */
            void setUniformValue(std::string_view name,
                                 const glm::mat2x2 &value);
            void setUniformValue(std::string_view name,
                                 const glm::mat2x3 &value);
            void setUniformValue(std::string_view name,
                                 const glm::mat2x4 &value);
            void setUniformValue(std::string_view name,
                                 const glm::mat3x2 &value);
            void setUniformValue(std::string_view name,
                                 const glm::mat3x3 &value);
            void setUniformValue(std::string_view name,
                                 const glm::mat3x4 &value);
            void setUniformValue(std::string_view name,
                                 const glm::mat4x2 &value);
            void setUniformValue(std::string_view name,
                                 const glm::mat4x3 &value);
            void setUniformValue(std::string_view name,
                                 const glm::mat4x4 &value);
            void setUniformValue(std::string_view name,
                                 const GLfloat value[2][2]);
            void setUniformValue(std::string_view name,
                                 const GLfloat value[3][3]);
            void setUniformValue(std::string_view name,
                                 const GLfloat value[4][4]);
            /*         void setUniformValue(std::string_view name, const
             * QTransform &value); */

            void setUniformValueArray(int location, const GLfloat *values,
                                      int count, int tupleSize);
            void setUniformValueArray(int location, const GLint *values,
                                      int count);
            void setUniformValueArray(int location, const GLuint *values,
                                      int count);
            void setUniformValueArray(int location, const glm::vec2 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::vec3 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::vec4 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat2x2 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat2x3 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat2x4 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat3x2 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat3x3 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat3x4 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat4x2 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat4x3 *values,
                                      int count);
            void setUniformValueArray(int location, const glm::mat4x4 *values,
                                      int count);

            void setUniformValueArray(std::string_view name,
                                      const GLfloat *values, int count,
                                      int tupleSize);
            void setUniformValueArray(std::string_view name,
                                      const GLint *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const GLuint *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::vec2 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::vec3 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::vec4 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat2x2 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat2x3 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat2x4 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat3x2 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat3x3 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat3x4 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat4x2 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat4x3 *values, int count);
            void setUniformValueArray(std::string_view name,
                                      const glm::mat4x4 *values, int count);

            static bool hasOpenGLShaderPrograms(GLFWwindow *context = nullptr);

        private:
            bool init();

            class Impl;
            std::unique_ptr<Impl> pImpl;
        };
#endif
    } // namespace opengl

} // namespace ht
