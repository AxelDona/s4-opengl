#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>
#include <vector>
#include <cstddef>

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

class Vertex2DColor{


public :

    glm::vec2 position{};
    glm::vec3 color{};


    Vertex2DColor()=default;

    Vertex2DColor(glm::vec2 posVec, glm::vec3 colorVec){
        position = posVec;
        color = colorVec;
    }
};

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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP1", nullptr, nullptr);
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

    /*à partir d'ici initialisation : création des meshes, etc, tout ce qui est avant la boucle while*/

    glimac::FilePath applicationPath(argv[0]);
    glimac::Program program = loadProgram(applicationPath.dirPath() + "TP1/shaders/triangle.vs.glsl",
                                           applicationPath.dirPath() + "TP1/shaders/triangle.fs.glsl");
    program.use();

    // Créer et modifier le VBO
    GLuint vbo; // variable dans laquelle sera stockée l'identifiant du VBO créé (tableaux si on crée plusieurs)
    glGenBuffers(1, &vbo); // on crée 1 vbo, son ID stocké dans l'unsigned int "vbo"
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // pour pouvoir modifier notre VBO, on le bind à la cible GL_ARRAY_BUFFER (qui utilisée pour ça spécifiquement)
    // on stocke les coordonnées 2D des 3 vertex du triangle (soit 2 valeurs par vertice pour les coordonnées, soit 5 pour avoir la couleur avec)
    //    GLfloat vertices[] = {-0.5f, -0.5f, 1.f, 0.f, 0.f, // premier sommet
    //                          0.5f, -0.5f, 0.f, 1.f, 0.f, // deuxième sommet
    //                          0.0f, 0.5f, 0.f, 0.f, 1.f // troisème sommet
    //    };
    std::vector<Vertex2DColor> vertices;

    int n_sectors = 200;
    float R = 0.5;

    // centre
    vertices.push_back(Vertex2DColor(glm::vec2(0,0), glm::vec3(0, 0, 1)));

    for (int i = 0; i <= n_sectors + 1; i++) {

        // sommet i
        vertices.push_back(Vertex2DColor(
            glm::vec2(R * glm::cos(2 * i * glm::pi<float>() / (float)n_sectors),
                      R * glm::sin(2 * i * glm::pi<float>() / (float)n_sectors)),
            glm::vec3(1, 0, 0)));
    }

    // => Tableau d'indices: ce sont les indices des sommets à dessiner
    // Chaque indice correspond au sommet correspondant dans le VBO
    std::vector<GLuint> indices;
    for (int i = 1; i < n_sectors; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(n_sectors);
    indices.push_back(1);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW); // on transmet les valeurs du tableau à la cible, GL_ARRAY_BUFFER. Le 2e paramètre est la taille du tableau en octets qui est le nombre de coordonnées * la taille d'un float (en général)
    glBindBuffer(GL_ARRAY_BUFFER, 0); // on débind une fois nos modifications effectuées

    // => Creation du IBO
    GLuint ibo;
    glGenBuffers(1, &ibo);

    // => On bind sur GL_ELEMENT_ARRAY_BUFFER, cible reservée pour les IBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // => On remplit l'IBO avec les indices:
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
                 indices.data(), GL_STATIC_DRAW);

    // => Comme d'habitude on debind avant de passer à autre chose
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Créer le VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); // on bind, cette fois 1 seul paramètre car les VAO ne vont que sur une seule cible

    // => On bind l'IBO sur GL_ELEMENT_ARRAY_BUFFER; puisqu'un VAO est
    // actuellement bindé, cela a pour effet d'enregistrer l'IBO dans le VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_COLOR_POSITION = 8;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION); // 0 correspond par défaut à l'attribut position, ici on utilise 3 ce qui est précisé dans le fichier de shader
    glEnableVertexAttribArray(VERTEX_COLOR_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, position)); // ID de position, taille de l'attribut (2 pour les coordonnées), composants de type float, osef, stride qui est le nobre d'octets de l'attribut pour un sommet, pointeur qui est le décalage par rapport au début de l'attribut (ici 0 car première instance dans l'attribut)
    glVertexAttribPointer(VERTEX_COLOR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, color)); // ID position, taille (3 pour la couleur), ..., ..., stride identique car même attribut, pointeur différent car la couleur arrive après la position dans l'attribut, donc on met la taille de la position
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        /*Code de rendu ici*/
        // glDrawArrays(GL_TRIANGLES, 0, 3 * n * sizeof(Vertex2DColor));
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

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