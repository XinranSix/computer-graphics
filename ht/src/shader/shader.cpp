#pragma once

#include <format>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader/shader.h"
#include <memory>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include "magic_enum.hpp"

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

            bool compileSourceCode(std::string_view source) {
                this->shaderSourceCode = source;
                return compileShader();
            }

            bool compileSourceFile(std::string_view fileName) {
                std::ifstream ifs;
                ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                try {
                    ifs.open(fileName.data());
                    std::ostringstream oss;
                    oss << ifs.rdbuf();
                    ifs.close();
                    this->shaderSourceCode = oss.str();
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
                return success != 0;
            }

            std::string log() const {
                return logInfo;
            }

            GLuint shaderId() const {
                return id;
            }

        private:
            bool compileShader() {

                int shaderType {};

                if (type == ShaderType::VERTEX) {
                    shaderType = GL_VERTEX_SHADER;
                } else if (type == ShaderType::FRAGMENT) {
                    shaderType = GL_FRAGMENT_SHADER;
                } else if (type == ShaderType::GEOMETRY) {
                    shaderType = GL_GEOMETRY_SHADER;
                } else if (type == ShaderType::TESSELLATION_CONTROL) {
                    shaderType = GL_TESS_CONTROL_SHADER;
                } else if (type == ShaderType::TESSELLATION_EVALUATION) {
                    shaderType = GL_TESS_EVALUATION_SHADER;
                } else if (type == ShaderType::COMPUTE) {
                    shaderType = GL_COMPUTE_SHADER;
                } else {
                    return false;
                }
                id = glCreateShader(shaderType);

                const char *shaderCodeCStr = shaderSourceCode.c_str();

                glShaderSource(id, 1, &shaderCodeCStr, nullptr);

                glCompileShader(id);
                return checkShaderCompileErrors();
            }

            bool checkShaderCompileErrors() {
                char infoLog[1024];
                glGetShaderiv(id, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(id, 1024, nullptr, infoLog);
                    logInfo = std::format(
                        "ERROR::SHADER_COMPILATION_ERROR of type: {}\n{}\n -- "
                        "--------------------------------------------------- "
                        "-- ",
                        magic_enum::enum_name(type), infoLog);
                    return false;
                }
                return true;
            }

        private:
            GLuint id {};
            ShaderType type;
            std::string shaderSourceCode;
            std::string logInfo;
            int success {};
        };
    } // namespace opengl

    namespace opengl {

        Shader::Shader(ShaderType type)
            : pImpl { std::make_unique<Impl>(type) } {
        }

        Shader::~Shader() noexcept = default;

        ShaderType Shader::shaderType() const {
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

        /*   bool Shader::hasOpenGLShaders(ShaderType type, GLFWwindow *context)
          {
              // FIXME: 待完成
              return false;
          } */

    } // namespace opengl
} // namespace ht
