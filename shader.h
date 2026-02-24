#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. Read files
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.open(vertexPath);
        if (!vShaderFile.is_open()) {
            std::cerr << "Failed to open vertex shader file: " << vertexPath << std::endl;
            ID = 0;
            return;
        }
        fShaderFile.open(fragmentPath);
        if (!fShaderFile.is_open()) {
            std::cerr << "Failed to open fragment shader file: " << fragmentPath << std::endl;
            ID = 0;
            return;
        }
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // Remove UTF-8 BOM if present (0xEF 0xBB 0xBF)
        auto stripBOM = [](std::string& s) {
            if (s.size() >= 3
                && static_cast<unsigned char>(s[0]) == 0xEF
                && static_cast<unsigned char>(s[1]) == 0xBB
                && static_cast<unsigned char>(s[2]) == 0xBF) {
                s.erase(0, 3);
            }
            };
        stripBOM(vertexCode);
        stripBOM(fragmentCode);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
       

        // 2. Compile vertex shader
        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        int success;
        char infoLog[1024];
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
            std::cerr << "VERTEX SHADER COMPILATION FAILED:\n" << infoLog << std::endl;
            glDeleteShader(vertex);
            ID = 0;
            return;
        }

        // 3. Compile fragment shader
        unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
            std::cerr << "FRAGMENT SHADER COMPILATION FAILED:\n" << infoLog << std::endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            ID = 0;
            return;
        }
		//4. Link shaders into a program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 1024, NULL, infoLog);
            std::cerr << "SHADER PROGRAM LINKING FAILED:\n" << infoLog << std::endl;
            glDeleteProgram(ID);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            ID = 0;
            return;
        }
		// 5. Clean up shaders 
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() { if (ID) glUseProgram(ID); }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        unsigned int loc = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }

    // setInt to assign integer uniforms (e.g., texture unit indices)
    void setInt(const std::string& name, int value) const {
        if (!ID) return;
        int loc = glGetUniformLocation(ID, name.c_str());
        if (loc == -1) {
            std::cerr << "Warning: uniform '" << name << "' not found or unused in shader.\n";
            return;
        }
        glUniform1i(loc, value);
    }

    void setBool(const std::string& name, bool value) {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
};
