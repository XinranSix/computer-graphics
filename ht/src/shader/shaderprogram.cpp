#include "shader/shader.h"
#include <format>
#include <memory>
#include <vector>

namespace ht {
    namespace opengl {
        class ShaderProgram::Impl {
        public:
            Impl() {
            }

            ~Impl() {
                glDeleteProgram(id);
            }

            bool addShader(Shader *shader) {
                shaders.push_back(std::shared_ptr<Shader>(shader));
                return true;
            }

            bool addShaderFromSourceCode(ShaderType type,
                                         std::string_view source) {
                auto temp = std::make_shared<Shader>(type);
                if (!temp->compileSourceCode(source)) {
                    logInfo = temp->log();
                    return false;
                }
                shaders.push_back(temp);
                return true;
            }

            bool addShaderFromSourceFile(ShaderType type,
                                         std::string_view fileName) {
                auto temp = std::make_shared<Shader>(type);
                if (!temp->compileSourceFile(fileName)) {
                    logInfo = temp->log();
                    return false;
                }
                shaders.push_back(temp);
                return true;
            }

            bool link() {
                for (auto shader : shaders) {
                    glAttachShader(id, shader->shaderId());
                }
                glLinkProgram(id);
                char infoLog[1024];
                glGetProgramiv(id, GL_LINK_STATUS, &linked);
                if (!linked) {
                    glGetProgramInfoLog(id, 1024, nullptr, infoLog);
                    logInfo = std::format(
                        "ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}\n",
                        infoLog);
                    return false;
                }
                return true;
            }

            bool isLinked() const {
                return linked != 0;
            }

            std::string log() const {
                return logInfo;
            }

            bool bind() {
                glUseProgram(id);
                return true;
            }

            bool create() {
                id = glCreateProgram();
                return true;
            }

            GLuint programId() const {
                return id;
            }

        private:
            GLuint id {};
            std::vector<std::shared_ptr<Shader>> shaders;
            std::string logInfo;
            int linked {};
        };

    }; // namespace opengl

    namespace opengl {
        ShaderProgram::ShaderProgram() : pImpl { std::make_unique<Impl>() } {
        }

        ShaderProgram::~ShaderProgram() = default;

        bool ShaderProgram::addShader(Shader *shader) {
            return pImpl->addShader(shader);
        }

        bool ShaderProgram::addShaderFromSourceCode(ShaderType type,
                                                    std::string_view source) {
            return pImpl->addShaderFromSourceCode(type, source);
        }

        bool ShaderProgram::addShaderFromSourceFile(ShaderType type,
                                                    std::string_view fileName) {
            return pImpl->addShaderFromSourceFile(type, fileName);
        }

        bool ShaderProgram::link() {
            return pImpl->link();
        }

        bool ShaderProgram::isLinked() const {
            return pImpl->isLinked();
        }

        std::string ShaderProgram::log() const {
            return pImpl->log();
        }

        bool ShaderProgram::bind() {
            return pImpl->bind();
        }

        bool ShaderProgram::create() {
            return pImpl->create();
        }

        GLuint ShaderProgram::programId() const {
            return pImpl->programId();
        }

    } // namespace opengl
};    // namespace ht
