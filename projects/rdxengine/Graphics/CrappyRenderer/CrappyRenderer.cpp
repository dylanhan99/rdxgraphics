#include "CrappyRenderer.h"
#include "ServiceLayer.h"
#include <gl/glew.h>

using namespace rdx;

namespace { // Anonymous ns to keep things local to this translation unit
    GLuint tempVAO{};
    GLuint shaderProgram{};

    std::vector<GLuint> indices{
        0, 1, 2, 2, 3, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        6, 5, 2, 2, 1, 6, // Bottom face
        0, 3, 4, 4, 7, 0, // Top face
        7, 6, 1, 1, 0, 7, // Left face
        3, 2, 5, 5, 4, 3  // Right face
    };
}

bool CrappyRenderer::InitImpl()
{
    // Set glewExperimental to true before initialization
    glewExperimental = GL_TRUE;

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        RX_CRITICAL("Failed to initialize GLEW: {}", (const char*)glewGetErrorString(glewError));
        return false;
    }

    // Clear the initial GLEW error that occurs with glewExperimental
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        //RX_WARN("Initial GLEW error (normal): {}", err);
    }

    // Print OpenGL context info
    //RX_INFO("OpenGL Version: {}", glGetString(GL_VERSION));
    //RX_INFO("GLSL Version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));
    //RX_INFO("Vendor: {}", glGetString(GL_VENDOR));
    //RX_INFO("Renderer: {}", glGetString(GL_RENDERER));

    SetupGlewDefaults();
    SetupDefaultAssets();

    // Verify everything was created
    if (tempVAO == 0 || shaderProgram == 0) {
        //RX_CRITICAL("Failed to create OpenGL assets");
        return false;
    }

    //RX_INFO("Renderer initialized successfully");
    return true;
}

bool CrappyRenderer::TerminateImpl()
{
    // Clean up OpenGL resources
    if (tempVAO != 0) {
        glDeleteVertexArrays(1, &tempVAO);
        tempVAO = 0;
    }

    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    //RX_INFO("Renderer terminated successfully");
    return true;
}

void CrappyRenderer::DrawImpl()
{
    static int drawCount = 0;
    drawCount++;

    // Debug output
    //RX_TRACE("Draw call #{}", drawCount);

    // Check for OpenGL errors before drawing
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        //RX_ERROR("OpenGL error at start of Draw #{}: {}", drawCount, error);
    }

    glm::vec4 m_BackbufferColor{ 0.2f, 0.3f, 0.3f, 1.0f };
    glClearColor(m_BackbufferColor.r, m_BackbufferColor.g, m_BackbufferColor.b, m_BackbufferColor.a);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        //RX_ERROR("OpenGL error after glClearColor: {}", error);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        //RX_ERROR("OpenGL error after glClear: {}", error);
    }

    glBindVertexArray(tempVAO);
    glUseProgram(shaderProgram);

    //auto view = ServiceLayer::EntityComponentService()->View<TransformComponent>();
    //for (auto [eid, xform] : view.each())
    RX_ECS_VIEWEACH(TransformComponent)(
        [](EntityID eid, TransformComponent& xform)
        {
            // glm::mat4 const& modelXform = xform.GetTransformMatrix();
            //glm::mat4 modelXform{1.0}; // identity
            //modelXform = glm::translate(modelXform, glm::vec3{1.f,0.f,0.f});
            glm::mat4 modelXform = glm::translate(glm::mat4{ 1.f }, xform.Position);
            GLint loc = glGetUniformLocation(shaderProgram, "model");
            if (loc == -1)
                RX_WARN("Failed to locate '{}' in shader.", "model");
            glUniformMatrix4fv(loc, 1, GL_FALSE, &glm::value_ptr(modelXform)[0]);

            // FIXED: Draw all indices with correct parameters
            glDrawElements(
                GL_TRIANGLES,
                static_cast<GLsizei>(indices.size()),  // Number of indices to draw
                GL_UNSIGNED_INT,
                nullptr  // We're using bound EBO, so no pointer needed
            );
        });
}

void CrappyRenderer::SetupGlewDefaults()
{
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set a default viewport (you might want to make this dynamic)
    glViewport(0, 0, 1280, 720); // Adjust to your window size

    //RX_INFO("OpenGL defaults configured");
}

void CrappyRenderer::SetupDefaultAssets()
{
    // Create and bind VAO
    glGenVertexArrays(1, &tempVAO);
    glBindVertexArray(tempVAO);

    // Vertex data
    std::vector<glm::vec3> vertices{
        { -0.5f,  0.5f,  0.5f }, // 0
        { -0.5f, -0.5f,  0.5f }, // 1
        {  0.5f, -0.5f,  0.5f }, // 2
        {  0.5f,  0.5f,  0.5f }, // 3
        {  0.5f,  0.5f, -0.5f }, // 4
        {  0.5f, -0.5f, -0.5f }, // 5
        { -0.5f, -0.5f, -0.5f }, // 6
        { -0.5f,  0.5f, -0.5f }  // 7
    };

    // Setup Element Buffer Object (EBO) for indices
    GLuint ebo{};
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW);

    // Setup Vertex Buffer Object (VBO) for vertex positions
    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(glm::vec3),
        vertices.data(),
        GL_STATIC_DRAW);

    // Setup vertex attribute pointer for position (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                    // location = 0
        3,                    // 3 components (x, y, z)
        GL_FLOAT,             // type
        GL_FALSE,             // normalized
        sizeof(glm::vec3),    // stride (12 bytes)
        (void*)0);            // offset (0 bytes)

    // Unbind VAO first (important!), then other buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Note: Don't unbind EBO while VAO is active, as VAO stores the EBO binding

    //RX_INFO("VAO, VBO, and EBO created successfully");

    // Setup shaders
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 model;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = model * vec4(aPos, 1.0);\n"
        "}\0";

    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    //CheckShaderCompileErrors(vertexShader, "VERTEX");

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    //CheckShaderCompileErrors(fragmentShader, "FRAGMENT");

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    //CheckShaderLinkErrors(shaderProgram);

    // Clean up shaders (they're linked into the program now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //RX_INFO("Shaders compiled and linked successfully");
}

//void CrappyRenderer::CheckShaderCompileErrors(GLuint shader, const std::string& type)
//{
//    GLint success;
//    GLchar infoLog[1024];
//    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//    if (!success) {
//        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
//        //RX_CRITICAL("SHADER_COMPILATION_ERROR of type: {} \n {}", type, infoLog);
//    }
//    else {
//        //RX_TRACE("{} shader compiled successfully", type);
//    }
//}
//
//void CrappyRenderer::CheckShaderLinkErrors(GLuint program)
//{
//    GLint success;
//    GLchar infoLog[1024];
//    glGetProgramiv(program, GL_LINK_STATUS, &success);
//    if (!success) {
//        glGetProgramInfoLog(program, 1024, NULL, infoLog);
//        //RX_CRITICAL("PROGRAM_LINKING_ERROR: \n {}", infoLog);
//    }
//    else {
//        //RX_TRACE("Shader program linked successfully");
//    }
//}