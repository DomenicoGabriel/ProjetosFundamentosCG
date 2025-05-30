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

struct Triangle 
{
	vec3 position;
	vec3 dimensions;
	vec3 color;
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
	colors.push_back(vec3(200, 191, 231));
	colors.push_back(vec3(174, 217, 224));
	colors.push_back(vec3(181, 234, 215));
	colors.push_back(vec3(255, 241, 182));
	colors.push_back(vec3(255, 188, 188));
	colors.push_back(vec3(246, 193, 199));
	colors.push_back(vec3(255, 216, 190));
	colors.push_back(vec3(220, 198, 224));
	colors.push_back(vec3(208, 230, 165));
	colors.push_back(vec3(183, 201, 226));
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

	GLuint VAO = createTriangle(-0.5,-0.5,0.5,-0.5,0.0,0.5);
	
	Triangle tri;
	tri.position = vec3(400.0,300.0,0.0);
	tri.dimensions = vec3(100.0,100.0,1.0);
	tri.color = vec3(colors[iColor].r, colors[iColor].g, colors[iColor].b);
	iColor = (iColor + 1) % colors.size();
	triangles.push_back(tri);

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

        glBindVertexArray(VAO); // Conectando ao buffer de geometria

        for (int i = 0; i < triangles.size(); i++)
		{
			// Matriz de modelo: transformações na geometria (objeto)
			mat4 model = mat4(1); // matriz identidade
			// Translação
			model = translate(model,vec3(triangles[i].position.x,triangles[i].position.y,0.0));

			model = rotate(model,radians(180.0f),vec3(0.0,0.0,1.0));
			// Escala
			model = scale(model,vec3(triangles[i].dimensions.x,triangles[i].dimensions.y,1.0));
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

			glUniform4f(colorLoc, triangles[i].color.r, triangles[i].color.g, triangles[i].color.b, 1.0f); // enviando cor para variável uniform inputColor
			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

        glBindVertexArray(0); // Desconectando o buffer de geometria
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

/*  Se botão esquerdo do mouse (button = GLFW == GLFW_MOUSE_BUTTON_LEFT) clicar (action == GLFW_PRESS),
    obtém a posição do cursor na tela (glfwGetCursorPos()), imprime no console a posição onde o mouse clicou,
    cria uma nova instância de uma estrutura chamada Triangle utilizando as posições do cursor dadas ao clicar na tela
    e atualiza a cor */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Obtém a posição do cursos na tela e imprime no console a posição onde o mouse clicou
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		cout << xpos << "  " << ypos << endl;

        // Cria uma nova instância de uma estrutura chamada Triangle utilizando as posições do cursor dadas ao clicar na tela
		Triangle tri;
        // Define os atributos do novo triângulo
		tri.position = vec3(xpos,ypos,0.0);
		tri.dimensions = vec3(100.0,100.0,1.0);
		tri.color = vec3(colors[iColor].r, colors[iColor].g, colors[iColor].b);
        // Atualiza iColor para a próxima cor no vetor. Quando chegar no fim do vetor, volta para o começo
		iColor = (iColor + 1) % colors.size();

        // Adiciona o novo triângulo ao vetor triangles
		triangles.push_back(tri);
		
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
