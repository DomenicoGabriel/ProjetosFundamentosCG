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

glm::vec2 playerVelocity(0.0f); // Apenas o eixo X será usado
float speed = 200.0f; // velocidade em pixels por segundo
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;
uniform vec4 subrect; // x, y, width, height

void main()
{
    gl_Position = projection * model * vec4(position, 1.0);
    TexCoord = subrect.xy + texCoord * subrect.zw;
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
    glm::vec4 subrect;  // x, y, width, height em coordenadas de textura (0 a 1)
    float rotation;       // rotação em graus

    public: 
    Sprite(GLuint vao, GLuint textureID, GLuint shaderID) :
    vao(vao), textureID(textureID), shaderID(shaderID),
    position(0.0f, 0.0f), scale(1.0f, 1.0f), rotation(0.0f),
    subrect(0.0f, 0.0f, 1.0f, 1.0f) {}

    glm::vec2 getPosition() const {
        return position;
    }

    void setPosition(float x, float y) {
        position = glm::vec2(x, y);
    }

    void setScale(float sx, float sy) {
        scale = glm::vec2(sx, sy);
    }

    void setSubrect(float x, float y, float width, float height) {
        subrect = glm::vec4(x, y, width, height);
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
        
        GLint locSubrect = glGetUniformLocation(shaderID, "subrect");
        glUniform4fv(locSubrect, 1, &subrect[0]);

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
    GLuint bgTex1 = loadTexture("../assets/sprites/cartoonForest/camada1.png");
    GLuint bgTex2 = loadTexture("../assets/sprites/cartoonForest/camada2.png");
    GLuint bgTex3 = loadTexture("../assets/sprites/cartoonForest/camada3.png");
    GLuint bgTex4 = loadTexture("../assets/sprites/cartoonForest/camada4.png");
    GLuint bgTex5 = loadTexture("../assets/sprites/cartoonForest/camada5.png");
    GLuint minotaurTex = loadTexture("../assets/sprites/minotauro.png");


    // Cria sprites
    Sprite layer1a(quadVAO, bgTex1, shaderProgram);
    Sprite layer1b(quadVAO, bgTex1, shaderProgram);

    Sprite layer2a(quadVAO, bgTex2, shaderProgram);
    Sprite layer2b(quadVAO, bgTex2, shaderProgram);

    Sprite layer3a(quadVAO, bgTex3, shaderProgram);
    Sprite layer3b(quadVAO, bgTex3, shaderProgram);

    Sprite layer4a(quadVAO, bgTex4, shaderProgram);
    Sprite layer4b(quadVAO, bgTex4, shaderProgram);

    Sprite layer5a(quadVAO, bgTex5, shaderProgram);
    Sprite layer5b(quadVAO, bgTex5, shaderProgram);

    Sprite minotaur(quadVAO, minotaurTex, shaderProgram);

    // Define projeção ortográfica (para que 0,0 seja canto inferior esquerdo)
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WIDTH),
                                      0.0f, static_cast<float>(HEIGHT),
                                      -1.0f, 1.0f);

    // Ajusta sprites na tela
    layer1a.setScale(WIDTH, HEIGHT);
    layer1a.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);
    
    layer1b.setScale(WIDTH, HEIGHT);
    layer1b.setPosition(WIDTH / 2.0f + WIDTH, HEIGHT / 2.0f);


    layer2a.setScale(WIDTH, HEIGHT);
    layer2a.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);

    layer2b.setScale(WIDTH, HEIGHT);
    layer2b.setPosition(WIDTH / 2.0f + WIDTH, HEIGHT / 2.0f);

    layer3a.setScale(WIDTH, HEIGHT);
    layer3a.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);

    layer3b.setScale(WIDTH, HEIGHT);
    layer3b.setPosition(WIDTH / 2.0f + WIDTH, HEIGHT / 2.0f);

    layer4a.setScale(WIDTH, HEIGHT);
    layer4a.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);

    layer4b.setScale(WIDTH, HEIGHT);
    layer4b.setPosition(WIDTH / 2.0f + WIDTH, HEIGHT / 2.0f);

    layer5a.setScale(WIDTH, HEIGHT);
    layer5a.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);

    layer5b.setScale(WIDTH, HEIGHT);
    layer5b.setPosition(WIDTH / 2.0f + WIDTH, HEIGHT / 2.0f);

    minotaur.setScale(400.0f, 400.0f);

    // Ativar blending uma única vez, no início (após criar o contexto OpenGL)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Fatores de parallax: valores menores se movem mais devagar
    const float parallaxFactors[5] = { 0.2f, 0.4f, 0.6f, 0.8f, 1.0f };

    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        // --- MOVIMENTO ESQUERDA/DIREITA ---
        playerVelocity.x = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            playerVelocity.x = -1.0f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            playerVelocity.x = 1.0f;

        glm::vec2 displacement = playerVelocity * speed * deltaTime;

        // Atualiza posição do personagem
        // personagem fixo no centro horizontal da tela e em y fixo
        minotaur.setPosition(300.0f, 180.0f);


        // Atualiza cada camada com base no fator de parallax
        auto updateLayer = [&](Sprite& a, Sprite& b, float parallaxFactor)
        {
            // Move
            a.setPosition(a.getPosition().x - displacement.x * parallaxFactor, a.getPosition().y);
            b.setPosition(b.getPosition().x - displacement.x * parallaxFactor, b.getPosition().y);

            // Verifica saída pela ESQUERDA e reposiciona à DIREITA
            if (a.getPosition().x + WIDTH / 2.0f <= 0)
                a.setPosition(b.getPosition().x + WIDTH, a.getPosition().y);
            if (b.getPosition().x + WIDTH / 2.0f <= 0)
                b.setPosition(a.getPosition().x + WIDTH, b.getPosition().y);

            // Verifica saída pela DIREITA e reposiciona à ESQUERDA
            if (a.getPosition().x - WIDTH / 2.0f >= WIDTH)
                a.setPosition(b.getPosition().x - WIDTH, a.getPosition().y);
            if (b.getPosition().x - WIDTH / 2.0f >= WIDTH)
                b.setPosition(a.getPosition().x - WIDTH, b.getPosition().y);
        };

        updateLayer(layer1a, layer1b, parallaxFactors[0]);
        updateLayer(layer2a, layer2b, parallaxFactors[1]);
        updateLayer(layer3a, layer3b, parallaxFactors[2]);
        updateLayer(layer4a, layer4b, parallaxFactors[3]);
        updateLayer(layer5a, layer5b, parallaxFactors[4]);


        // --- DESENHO ---
        layer1a.draw(projection);
        layer1b.draw(projection);

        layer2a.draw(projection);
        layer2b.draw(projection);

        layer3a.draw(projection);
        layer3b.draw(projection);

        layer4a.draw(projection);
        layer4b.draw(projection);

        minotaur.draw(projection);  // personagem fixo no centro

        layer5a.draw(projection);
        layer5b.draw(projection);

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
        if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
            playerVelocity.x = -speed;
        if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
            playerVelocity.x = speed;
    }
    if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
            playerVelocity.x = 0;
    }
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
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
