#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Protótipo de funções callback:
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode); // --> Fechar janela com tecla ESC

GLuint loadTexture(const char* path); // --> Carregar textura
GLuint createQuad(); // --> Criar quadrado
int setupShader(); // --> Retornar o identificador do programa de shader


const GLuint WIDTH = 800, HEIGHT = 600;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * model * vec4(position, 1.0);
    TexCoord = texCoord;
}
)";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"(
#version 400
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
    color = texture(ourTexture, TexCoord);
}
)";

class Sprite{
    private:
    GLuint vao;           // Buffer de geometria (quadrado 1x1 centrado)
    GLuint textureID;     // ID da textura
    GLuint shaderID;      // ID do shader usado para desenhar

    glm::vec2 position;   // posição na tela (pixels)
    glm::vec2 scale;      // escala (multiplicador)
    float rotation;       // rotação em graus

    public: 
    Sprite(GLuint vao, GLuint textureID, GLuint shaderID) :
    vao(vao), textureID(textureID), shaderID(shaderID),
    position(0.0f, 0.0f), scale(1.0f, 1.0f), rotation(0.0f) {}

    void setPosition(float x, float y) {
        position = glm::vec2(x, y);
    }

    void setScale(float sx, float sy) {
        scale = glm::vec2(sx, sy);
    }

    void setRotation(float angleDegrees) {
        rotation = angleDegrees;
    }

    void draw(const glm::mat4& projection) {
        glUseProgram(shaderID);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0,0,1));
        model = glm::scale(model, glm::vec3(scale, 1.0f));

        GLint locModel = glGetUniformLocation(shaderID, "model");
        GLint locProjection = glGetUniformLocation(shaderID, "projection");
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(locProjection, 1, GL_FALSE, &projection[0][0]);

        // Passa a textura para o shader (bind na unidade 0)
        glUniform1i(glGetUniformLocation(shaderID, "ourTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // usa indices com EBO agora
        glBindVertexArray(0);

        glUseProgram(0);
    }

};

int main()
{
    // Inicializa GLFW
    if (!glfwInit())
    {
        cout << "Falha ao inicializar GLFW" << endl;
        return -1;
    }

    // Configura a versão do OpenGL (4.0)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Cria a janela
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Minotauro x party", nullptr, nullptr);
    if (!window)
    {
        cout << "Falha ao criar janela GLFW" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicializa GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Falha ao inicializar GLAD" << endl;
        return -1;
    }

    // Define o viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    // Registra callback do teclado
    glfwSetKeyCallback(window, key_callback);

    // Compila e linka shader
    GLuint shaderProgram = setupShader();

    // Cria geometria do quad
    GLuint quadVAO = createQuad();

    // Carrega texturas
    GLuint backgroundTex = loadTexture("../assets/sprites/background.jpg");
    GLuint knightTex = loadTexture("../assets/sprites/cavaleiro.png");
    GLuint knight2Tex = loadTexture("../assets/sprites/cavaleiro2.png");
    GLuint mageTex = loadTexture("../assets/sprites/mago2.png");
    GLuint minotaurTex = loadTexture("../assets/sprites/minotauro.png");


    // Cria sprites
    Sprite background(quadVAO, backgroundTex, shaderProgram);
    Sprite knight(quadVAO, knightTex, shaderProgram);
    Sprite knight2(quadVAO, knight2Tex, shaderProgram);
    Sprite mage(quadVAO, mageTex, shaderProgram);
    Sprite minotaur(quadVAO, minotaurTex, shaderProgram);

    // Define projeção ortográfica (para que 0,0 seja canto inferior esquerdo)
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WIDTH),
                                      0.0f, static_cast<float>(HEIGHT),
                                      -1.0f, 1.0f);

    // Ajusta sprites na tela
    background.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);
    background.setScale(WIDTH, HEIGHT);

    //knight.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);
    knight.setPosition(550.0f, 250.0f);
    knight.setScale(100.0f, 100.0f); // Cavaleiro pequeno (100x100 pixels)
    
    knight2.setPosition(600.0f, 350.0f);
    knight2.setScale(100.0f, 100.0f); // Cavaleiro pequeno (100x100 pixels)
	
    mage.setPosition(700.0f, 300.0f);
    mage.setScale(100.0f, 100.0f); // Cavaleiro pequeno (100x100 pixels)

    minotaur.setPosition(200.0f, 350.0f);
    minotaur.setScale(200.0f, 200.0f);

    // Ativar blending uma única vez, no início (após criar o contexto OpenGL)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        background.draw(projection);
        knight.draw(projection);
        knight2.draw(projection);
        mage.draw(projection);
        minotaur.draw(projection);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLuint createQuad()
{
    GLuint VAO, VBO, EBO;

    // Vertices: posição (x,y,z) + textura (s,t)
    GLfloat vertices[] = {
        // positions        // texture coords
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f   // top-right
    };

    GLuint indices[] = {
        0, 1, 2,  // first triangle
        0, 2, 3   // second triangle
    };

    // Gera buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // VBO - vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO - indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Posição - layout location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Textura - layout location 1
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Desvincula VAO (mas mantém EBO vinculado ao VAO!)
    glBindVertexArray(0);

    // Desvincula buffers para evitar bugs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return VAO;
}

GLuint loadTexture(const char* path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Por padrão, imagens carregadas com stb_image têm a origem no canto superior esquerdo. O OpenGL espera a origem da imagem no canto inferior esquerdo.
    stbi_set_flip_vertically_on_load(true); // --> Gira verticalmente a imagem


    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Parâmetros da textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Erro ao carregar textura: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
