#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#define GLM_FORCE_RADIANS 1
#ifdef EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#endif

#include <iostream>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

float const screenWidth = 480.0f;
float const screenHeight = 480.0f;
unsigned int const fps = 30;

GLuint g_program;
GLint g_projectionMatrixLocation;
GLint g_modelviewMatrixLocation;

void draw() {
    static int frame = 0;
    frame++;

    // create frustum matrix
    // FIXME: this is constant!, remove from draw()
    glm::mat4 proj_mat = glm::frustum(-1.f, 1.f, -1.f, 1.f, 3.f, 10.f);
    glUniformMatrix4fv(g_projectionMatrixLocation, 1, GL_FALSE, &proj_mat[0][0]);

    // rotation
    glm::mat4 mv_mat = glm::rotate(
            glm::translate(glm::mat4(1.0f),
                glm::vec3(0.f, 0.f, -6.f)), frame * 0.1f, glm::vec3(0.f, 1.f, 0.f));
    glUniformMatrix4fv(g_modelviewMatrixLocation, 1, GL_FALSE, &mv_mat[0][0]);

//    std::cout << glm::to_string(mv_mat) << std::endl;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glFlush();
}

GLuint loadShader(
    GLenum type,
    const char *source)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        std::cerr << "Error creating shader" << std::endl;
        return 0;
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE) {
        char infoLogBuffer[1024];
        glGetShaderInfoLog(shader, 1024, 0, infoLogBuffer);
        std::cerr << "Shader compilation error (" << infoLogBuffer << ")" << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint buildProgram()
{
    auto vertexShaderSource =
#ifdef EMSCRIPTEN
        "precision mediump float;"
#else
        "#version 120"
#endif
        R"(
        uniform mat4 modelviewMatrix;
        uniform mat4 projectionMatrix;

        attribute vec3 vertex;
        attribute vec3 normal;

        varying vec3 e_normal;

        void main() {
            gl_Position = projectionMatrix * modelviewMatrix * vec4(vertex, 1.0);
            e_normal = vec3(modelviewMatrix * vec4(normal, 0.0));
        }
        )";

    auto fragmentShaderSource =
#ifdef EMSCRIPTEN
        "precision mediump float;"
#else
        "#version 120"
#endif
        R"(
        varying vec3 e_normal;

        void main() {
            vec3 n = normalize(e_normal);
            float l = abs(dot(n, normalize(vec3(-1, 1, 1))));
            gl_FragColor = vec4(l, l, l, 1.0);
        }
        )";

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint programObject = glCreateProgram();

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);

    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked) {
        char infoLogBuffer[1024];
        glGetProgramInfoLog(programObject, 1024, 0, infoLogBuffer);
        std::cerr << "Program link error (" << infoLogBuffer << ")" << std::endl;
        glDeleteProgram(programObject);
        programObject = 0;
    }
    if (vertexShader) glDeleteShader(vertexShader);
    if (fragmentShader) glDeleteShader(fragmentShader);

    return programObject;
}

void timer_cb(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000 / fps, timer_cb, 0);
};

int main(int argc, char * argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("EmGL");
    glEnable(GL_DEPTH_TEST);

    std::cout << "OpenGL version: "
        << glGetString(GL_VERSION)
        << std::endl;
    std::cout << "GLSL version: "
        << glGetString(GL_SHADING_LANGUAGE_VERSION)
        << std::endl;

    glutDisplayFunc(draw);
    glutTimerFunc(1000 / fps, timer_cb, 0);

    // init
    GLuint program = buildProgram();
    glUseProgram(program);

    // these values are determined adter glUseProgram()
    g_projectionMatrixLocation = glGetUniformLocation(program, "projectionMatrix");
    g_modelviewMatrixLocation = glGetUniformLocation(program, "modelviewMatrix");
    GLint vertexLocation = glGetAttribLocation(program, "vertex");
    GLint normalLocation = glGetAttribLocation(program, "normal");

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    GLuint buffers[2];
    glGenBuffers(2, buffers);
    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    GLfloat vertices[] = {-0.5f, -0.5f, 0.f, 0.5f, -0.5f, 0.f, 0.5f, 0.5f, 0.f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // normals
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    GLfloat normals[] = {0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vertexLocation);
    glEnableVertexAttribArray(normalLocation);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_TRUE, 0, 0);

    glutMainLoop();

    return 0;
}
