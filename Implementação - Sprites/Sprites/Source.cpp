#include <iostream>
#include <string>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

using namespace std;

#include "Shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupGeometry();
void drawScene(int VAO, GLuint backgroundTexID, GLuint charTexID, Shader& shader);
GLuint loadTexture(string texturePath);

const GLuint WIDTH = 800, HEIGHT = 600;
float rotation = 0.0f;
float scale = 1.0f;

int main()
{
    // Inicializa��o da GLFW
    glfwInit();

    // Cria��o da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Texturas", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Fazendo o registro da fun��o de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
    }

    // Obtendo as informa��es de vers�o
    const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Compilando e buildando o programa de shader
    Shader shader("../shaders/tex_vert.glsl", "../shaders/tex_frag.glsl");

    GLuint VAO = setupGeometry();

    GLuint backgroundTexID = loadTexture("../../Textures/background.png");
    GLuint charTexID = loadTexture("../../Textures/char.png");

    // Ativando o buffer de textura 0 da opengl
    glActiveTexture(GL_TEXTURE0);

    shader.Use();

    // Matriz de proje��o paralela ortogr�fica
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    shader.setMat4("projection", glm::value_ptr(projection));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Loop da aplica��o - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height); // unidades de tela: pixel

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        // Chamadas de desenho da cena
        drawScene(VAO, backgroundTexID, charTexID, shader);

        // Troca os buffers da tela
        glfwSwapBuffers(window);

        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
        glfwPollEvents();
    }
    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);
    // Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        scale += 0.1f;
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        scale -= 0.1f;
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        rotation += 10.0f;
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        rotation -= 10.0f;
}

int setupGeometry()
{
    GLfloat vertices[] = {
        // Tri�ngulo 0
        -0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,  // v0
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // v1
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // v2
         // Tri�ngulo 1
         -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // v1
          0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  // v3
          0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f   // v2
    };

    GLuint VBO, VAO;
    // Gera��o do identificador do VBO
    glGenBuffers(1, &VBO);
    // Faz a conex�o (vincula) do buffer como um buffer de array
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Envia os dados do array de floats para o buffer da OpenGl
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Gera��o do identificador do VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);
    // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
    // e os ponteiros para os atributos 
    glBindVertexArray(VAO);

    // Atributo 0 - posi��o
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1 - cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Atributo 2 - coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

void drawScene(int VAO, GLuint backgroundTexID, GLuint charTexID, Shader& shader)
{
    glBindTexture(GL_TEXTURE_2D, backgroundTexID);
    glBindVertexArray(VAO);

    // Desenho do fundo
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(400.0f, 300.0f, 0.0f));
    model = glm::scale(model, glm::vec3(800.0f, 600.0f, 1.0f));
    shader.setMat4("model", glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Desenho do sprite (char)
    glBindTexture(GL_TEXTURE_2D, charTexID);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(400.0f, 300.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(100.0f * scale, 100.0f * scale, 0.5f));
    shader.setMat4("model", glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

GLuint loadTexture(string texturePath)
{
    GLuint texID;

    // Gera o identificador da textura na mem�ria 
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Configura��o do par�metro WRAPPING nas coords s e t
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Configura��o do par�metro FILTERING na minifica��o e magnifica��o da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true); // Inverte a imagem ao carregar
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) // jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else // png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}
