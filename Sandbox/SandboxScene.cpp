/**
 * @file SandboxScene.cpp
 * @author sumin.park
 * @brief  Example Scene class
 * @version 0.1
 * @date 1/8/2026 10:15:55 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "SandboxScene.h"

#include <filesystem>
#include <fstream>
#include <sstream>

GLuint CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        printf("Shader compile error: %s\n", info);
    }

    return shader;
}

GLuint CreateProgram(const char* vertSrc, const char* fragSrc) {
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

std::string LoadTextFile(const char* path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void SetTriangleMesh(rm::Mesh* mesh) {
    rm::Vertex vertices[] = {{{-0.5f, -0.5f}}, {{0.5f, -0.5f}}, {{0.0f, 0.5f}}};

    unsigned int indices[] = {0, 1, 2};

    mesh->indexCount = 3;

    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);

    glGenBuffers(1, &mesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // layout(location = 0) vec2 position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(rm::Vertex), (void*) 0);

    glBindVertexArray(0);
}

SandboxScene::~SandboxScene() {
}

void SandboxScene::OnInit() {
    rm::GameObject* gameObject = CreateGameObject();
    auto mr = gameObject->AddComponent<rm::MeshRenderer>();
    
    auto mat = new rm::Material();
    auto mesh = new rm::Mesh();

    LOG_INFO("CWD: {}", std::filesystem::current_path().string());

    auto vertSrc = LoadTextFile("../RM-Engine/src/Shaders/triangle.vert");
    auto fragSrc = LoadTextFile("../RM-Engine/src/Shaders/triangle.frag");
    
    mat->program = CreateProgram(vertSrc.c_str(), fragSrc.c_str());
    
    SetTriangleMesh(mesh);

    mr->SetMaterial(mat);
    mr->SetMesh(mesh);
}