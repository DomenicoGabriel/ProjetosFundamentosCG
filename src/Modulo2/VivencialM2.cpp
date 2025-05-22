#include <iostream>
#include <string>
#include <assert.h>
#include <vector>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include <cmath>
// Protótipo de funções callback:
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode); // --> Fechar janela com tecla ESC
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods); // --> Criar triângulo com o clique do mouse

// Protótipo de funções:
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2); // --> Criar triângulo
int setupShader(); // --> Retornar o identificador do programa de shader

// Varáveis constantes que determinam tamanho da janela
const GLuint WIDTH = 800, HEIGHT = 600;


// Código fonte vertex shader
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
uniform mat4 projection;
uniform mat4 model;
void main()	
{
	gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
}
)";

// Código fonte do fragment shader
const GLchar *fragmentShaderSource = R"(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main()
{
	color = inputColor;
}
)";

/*
struct Triangle 
{
	vec3 position;
	vec3 dimensions;
	vec3 color;
};*/
struct Triangle {
    vec3 position;
    vec3 dimensions;
    vec3 color;
    GLuint vao; // <-- VAO exclusivo para cada triângulo
};


vector<Triangle> triangles;

vector <vec3> colors;
int iColor = 0;

int main()
{
	// Inicialização da GLFW
	glfwInit();

    // Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Triangle", nullptr, nullptr);
	glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    // GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

    // Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte *version = glGetString(GL_VERSION);	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

    //Inicializando paleta de cores
	colors.push_back(vec3(127, 127, 127));
	colors.push_back(vec3(0, 255, 0));
	colors.push_back(vec3(255, 0, 0));
	colors.push_back(vec3(0, 0, 255));
	colors.push_back(vec3(255, 255, 0));
	colors.push_back(vec3(0, 255, 255));
	colors.push_back(vec3(255, 0, 255));
	colors.push_back(vec3(127, 0, 127));
	colors.push_back(vec3(127, 0, 0));
	colors.push_back(vec3(0, 0, 127));
    // Normalizar as cores entre 0 e 1
	for (int i=0; i<colors.size(); i++)
	{
		colors[i].r /= 255.0;
		colors[i].g /= 255.0;
		colors[i].b /= 255.0;
	}

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

    // Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	glUseProgram(shaderID);

    // Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

    // Matriz de projeção paralela ortográfica
	// mat4 projection = ortho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
	mat4 projection = ortho(0.0, 800.0, 600.0, 0.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

        // Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

        glLineWidth(10);
		glPointSize(20);

        for (int i = 0; i < triangles.size(); i++)
        {
            mat4 model = mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
    
            glUniform4f(colorLoc, triangles[i].color.r, triangles[i].color.g, triangles[i].color.b, 1.0f);

            glBindVertexArray(triangles[i].vao); // <-- Usa o VAO específico
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glBindVertexArray(0); // Limpa depois do loop
        glfwSwapBuffers(window); // Troca os buffers da tela
	}

    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

/*  Se a tecla ESC (key == GLFW_KEY_ESCAPE) for apertada (action == GLFW_PRESS),
    janela deve fechar (glfwSetWindowShouldClose(nomeJanlea, GL_TRUE)) */
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

/*  Se o botão esquerdo do mouse for apertado, guardar as coordenadas do cursor e armazenar em um vetor.
    Quando os 3 vértices do triângulo forem dados, criar o triêngulo e limpar o vetor para poder começar de novo.
    Diferente do outro exercício, nesse cada triângulo deve ter um VAO próprio pois terão geometrias diferentes. */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        cout << xpos << "  " << ypos << endl;

        static std::vector<vec2> clickPoints; // Vetor que armazena as coordenadas dos cliques para montar o triângulo
        clickPoints.push_back(vec2(xpos, ypos)); // Insere no vetor as coordenadas

        if (clickPoints.size() == 3)
        {
            // Seleciona os pontos X e Y dos vértices no vetor
            vec2 p0 = clickPoints[0];
            vec2 p1 = clickPoints[1];
            vec2 p2 = clickPoints[2];
            GLuint vao = createTriangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);

            Triangle tri;
            tri.position = vec3(0.0f);
            tri.dimensions = vec3(1.0f);
            tri.color = vec3(colors[iColor].r, colors[iColor].g, colors[iColor].b);
            tri.vao = vao;
            triangles.push_back(tri);

            iColor = (iColor + 1) % colors.size();

            // Limpa os pontos para o próximo triângulo
            clickPoints.clear();
        }
    }
}

/*  Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
    shader simples e único neste exemplo de código
    O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
    fragmentShader source no iniçio deste arquivo
    A função retorna o identificador do programa de shader  */
int setupShader()
{
    // Vertex shader                                                    ____
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);                 //
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);             //
	glCompileShader(vertexShader);                                          //
    //                                                                      //
    // Checando erros de compilação (exibição via log no terminal)          //
	GLint success;                                                          //
	GLchar infoLog[512];                                                    //  BLOCO REFERENTE AO VERTEX SHADER
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);               //
	if (!success)                                                           //
	{                                                                       //
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);               //
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"          //
				  << infoLog << std::endl;                                  //
	}//                                                                 ____

    // Fragment shader                                                  ____
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);             //
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);         //
	glCompileShader(fragmentShader);                                        //
    //                                                                      //
    // Checando erros de compilação (exibição via log no terminal)          //
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);             //  BLOCO REFERENTE AO FRAGMENT SHADER
	if (!success)                                                           //
	{                                                                       //
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);             //
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"        //
				  << infoLog << std::endl;                                  //
	}//                                                                 ____


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

    return shaderProgram; // Retorna o identificador do programa de shader
}

/*  Cria triângulo

    VAO e VBO possuem lógica parecida em questão de métodos:
    VBO{
        glGenBuffers()  --> glGen
        glBindBuffer()  --> glBind
        glBufferData()
    }
    
    VAO{
        glGenVertexArrays()  --> glGen
        glBindVertexArray()  --> glBind
    }
    
    Depois de registrados, devem ser desvinculados: glBind de ambos deve receber parâmetro 0 ao invés dos VBO e VAO */
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2)
{
    GLuint VAO, VBO;

    GLfloat vertices[] = {
		x0, y0, 0.0, // v0
		x1, y1, 0.0, // v1
		x2, y2, 0.0, // v2
	};

    
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}