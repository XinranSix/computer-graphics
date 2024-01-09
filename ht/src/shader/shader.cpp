#pragma once

#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader/shader.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <sstream>

namespace ht {
    namespace opengl {

        class Shader::Impl {
        public:
            Impl(ShaderType type) : type { type } {
            }

            ~Impl() {
                glDeleteShader(id);
            }

            ShaderType shaderType() const {
                return type;
            }

            bool compileSourceCode(std::string_view source) { // TODO: 待完成
                compileShader();
                return false;
            }

            bool compileSourceFile(std::string_view fileName) { // TODO: 待完成
                std::ifstream ifs;
                ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                try {
                    ifs.open(fileName.data());
                    std::ostringstream oss;
                    oss << ifs.rdbuf();
                    ifs.close();
                    shaderSourceCode = oss.str();
                    return compileShader();
                } catch (std::ifstream::failure &e) {
                    logInfo = std::string {
                        "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: "
                    } + e.what();
                    return false;
                }
                return true;
            }

            std::string sourceCode() const {
                return shaderSourceCode;
            }

            bool isCompiled() const {
                // TODO: 待完成
                return false;
            }

            std::string log() const {
                return logInfo;
            }

            GLuint shaderId() const {
                return id;
            }

        private:
            bool compileShader() {
                id = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(id, 1,
                               reinterpret_cast<const GLchar *const *>(
                                   shaderSourceCode.c_str()),
                               nullptr);
                return true;
            }

        private:
            GLuint id;
            ShaderType type;
            std::string shaderSourceCode;
            std::string logInfo;
        };

        Shader::Shader(Shader::ShaderType type)
            : pImpl { std::make_unique<Impl>(type) } {
        }

        Shader::~Shader() = default;

        Shader::ShaderType Shader::shaderType() const {
            return pImpl->shaderType();
        }

        bool Shader::compileSourceCode(std::string_view source) {
            return pImpl->compileSourceCode(source);
        }

        bool Shader::compileSourceFile(std::string_view fileName) {
            return pImpl->compileSourceFile(fileName);
        }

        std::string Shader::sourceCode() const {
            return pImpl->sourceCode();
        }

        bool Shader::isCompiled() const {
            return pImpl->isCompiled();
        }

        std::string Shader::log() const {
            return pImpl->log();
        }

        GLuint Shader::shaderId() const {
            return pImpl->shaderId();
        }

        bool Shader::hasOpenGLShaders(ShaderType type, GLFWwindow *context) {
            // FIXME: 待完成
            return false;
        }

    } // namespace opengl

} // namespace ht
