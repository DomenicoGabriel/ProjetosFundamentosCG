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

// Protótipo de funções:
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode); // --> Fechar janela com tecla ESC

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

int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Triangle", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

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

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	vector<GLuint> VAOs;
	VAOs.push_back(createTriangle(-0.9f, -0.5f, -0.7f, -0.1f, -0.5f, -0.5f));
    VAOs.push_back(createTriangle(-0.3f, -0.5f, -0.1f, -0.1f,  0.1f, -0.5f));
    VAOs.push_back(createTriangle( 0.3f, -0.5f,  0.5f, -0.1f,  0.7f, -0.5f));
    VAOs.push_back(createTriangle(-0.6f,  0.2f, -0.4f,  0.6f, -0.2f,  0.2f));
    VAOs.push_back(createTriangle( 0.0f,  0.2f,  0.2f,  0.6f,  0.4f,  0.2f));
	
	glUseProgram(shaderID);

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	// Matriz de projeção paralela ortográfica
	// mat4 projection = ortho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
	mat4 projection = ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	// Matriz de modelo: transformações na geometria (objeto)
	mat4 model = mat4(1); // matriz identidade
	// Translação
	// model = translate(model,vec3(400.0,300.0,0.0));

	// model = rotate(model,radians(45.0f),vec3(0.0,0.0,1.0));
	// Escala
	// model = scale(model,vec3(300.0,300.0,1.0));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	glUniform4f(colorLoc, 1.0f, 0.0f, 1.0f, 1.0f); // enviando cor para variável uniform inputColor
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

		for (int i = 0; i < VAOs.size(); i++)
		{
			glBindVertexArray(VAOs[i]); // Conectando ao buffer de geometria
			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		// Desenho com contorno (linhas)
		// glUniform4f(colorLoc, 1.0f, 0.0f, 1.0f, 1.0f); //enviando cor para variável uniform inputColor
		// glDrawArrays(GL_LINE_LOOP, 0, 3); //Desenha T0
		// glDrawArrays(GL_LINE_LOOP, 3, 3); //Desenha T1

		// Desenho só dos pontos (vértices)
		// glUniform4f(colorLoc, 1.0f, 1.0f, 0.0f, 1.0f); //enviando cor para variável uniform inputColor
		// glDrawArrays(GL_POINTS, 0, 6);

		glBindVertexArray(0); // Desconectando o buffer de geometria

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	//glDeleteVertexArrays(1, &VAO);
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

    // Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);
	// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando:
	//  Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	//  Numero de valores que o atributo tem (por ex, 3 coordenadas xyz)
	//  Tipo do dado
	//  Se está normalizado (entre zero e um)
	//  Tamanho em bytes
	//  Deslocamento a partir do byte zero
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

    // Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}