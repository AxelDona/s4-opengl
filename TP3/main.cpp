#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>
#include <vector>

int window_width  = 1280;
int window_height = 720;

static void key_callback(GLFWwindow* /*window*/, int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/)
{
}

static void mouse_button_callback(GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/)
{
}

static void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double /*yoffset*/)
{
}

static void cursor_position_callback(GLFWwindow* /*window*/, double /*xpos*/, double /*ypos*/)
{
}

static void size_callback(GLFWwindow* /*window*/, int width, int height)
{
    window_width  = width;
    window_height = height;
}

struct Vertex2DUV{

    glm::vec2 position{};
    glm::vec2 texture{};

    Vertex2DUV()=default;

    Vertex2DUV(float x, float y){
        position = glm::vec2 {x, y};
        texture  = glm::vec2 {0, 0};
    }

    Vertex2DUV(glm::vec2 posVec, glm::vec2 textureVec){
        position = posVec;
        texture  = textureVec;
    }
};

glm::mat3 rotate(float a){
    float r = glm::radians(a);
    return glm::mat3(glm::vec3(cos(r), sin(r), 0), glm::vec3(-sin(r),cos(r),0), glm::vec3(0,0,1));
}

glm::mat3 translate (float tx, float ty){
    return glm::mat3(glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(tx,ty,1));
}

glm::mat3 scale (float sx, float sy){
    return glm::mat3(glm::vec3(sx,0,0), glm::vec3(0,sy,0), glm::vec3(0,0,1));
}

int main(int, char** argv)
{
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a window and its OpenGL context */
#ifdef __APPLE__
    /* We need to explicitly ask for a 3.3 context on Mac */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP3", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Intialize glad (loads the OpenGL functions) */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    glimac::FilePath applicationPath(argv[0]);
    glimac::Program program = loadProgram(applicationPath.dirPath() + "TP3/shaders/tex2D.vs.glsl",
                                           applicationPath.dirPath() + "TP3/shaders/tex2D.fs.glsl");
    program.use();

    GLint uModelMatrixLocation = glGetUniformLocation(program.getGLId(), "uModelMatrix");
    GLint uColorLocation = glGetUniformLocation(program.getGLId(), "uColor");

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    std::vector<Vertex2DUV> vertices = {Vertex2DUV(-1.0, -1.0), Vertex2DUV(1.0, -1.0), Vertex2DUV(0.0, 1.0)};
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex2DUV)*2, vertices.data(), GL_STATIC_DRAW); // on transmet les valeurs du tableau à la cible, GL_ARRAY_BUFFER. Le 2e paramètre est la taille du tableau en octets qui est le nombre de coordonnées * la taille d'un float (en général)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_TEXTURE_POSITION = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION); // 0 correspond par défaut à l'attribut position, ici on utilise 3 ce qui est précisé dans le fichier de shader
    glEnableVertexAttribArray(VERTEX_TEXTURE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*)offsetof(Vertex2DUV, position)); // ID de position, taille de l'attribut (2 pour les coordonnées), composants de type float, osef, stride qui est le nobre d'octets de l'attribut pour un sommet, pointeur qui est le décalage par rapport au début de l'attribut (ici 0 car première instance dans l'attribut)
    glVertexAttribPointer(VERTEX_TEXTURE_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*)offsetof(Vertex2DUV, texture)); // ID position, taille (3 pour la couleur), ..., ..., stride identique car même attribut, pointeur différent car la couleur arrive après la position dans l'attribut, donc on met la taille de la position
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float rotateAngle = 0.0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 1.f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rotate(rotateAngle/2) * translate(-0.5, 0.5) * rotate(rotateAngle) * scale(0.25,0.25)));
        glUniform3f(uColorLocation, 0.5, 0, 0.7);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rotate(rotateAngle/2) * translate(0.5, 0.5) * rotate(-rotateAngle) * scale(0.25,0.25)));
        glUniform3f(uColorLocation, 0, 1, 0.4);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rotate(rotateAngle/2) * translate(-0.5, -0.5) * rotate(-rotateAngle) * scale(0.25,0.25)));
        glUniform3f(uColorLocation, 0.7, 0.2, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rotate(rotateAngle/2) * translate(0.5, -0.5) * rotate(rotateAngle) * scale(0.25,0.25)));
        glUniform3f(uColorLocation, 0.5, 0.8, 0.1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        rotateAngle += 0.5;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwTerminate();
    return 0;
}