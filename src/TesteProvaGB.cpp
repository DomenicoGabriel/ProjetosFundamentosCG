#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <vector>

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
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode); // --> Tratar múltiplas teclas

GLuint loadTexture(const char* path); // --> Carregar textura
GLuint createQuad(); // --> Criar quadrado
int setupShader(); // --> Retornar o identificador do programa de shader


const GLuint WIDTH = 1000, HEIGHT = 700;

// Matriz que armazena a informação das texturas de cada tile
/*int matrizTexturas[3][3] = {
3, 2, 2,
2, 2, 3,
3, 2, 3
};*/
int matrizTexturas[15][15] = {
    2, 2, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    2, 2, 2, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 4, 4,
    2, 2, 2, 2, 4, 4, 5, 4, 4, 5, 5, 5, 5, 4, 4,
    2, 2, 2, 4, 2, 4, 5, 4, 5, 5, 4, 4, 4, 4, 4,
    2, 3, 2, 2, 4, 4, 5, 4, 4, 4, 5, 5, 5, 4, 4,
    2, 3, 3, 2, 2, 4, 5, 5, 5, 5, 5, 5, 5, 4, 4,
    3, 2, 2, 3, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    2, 3, 3, 2, 3, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5,
    3, 2, 3, 2, 3, 3, 2, 2, 2, 2, 5, 5, 5, 5, 5,
    2, 3, 3, 3, 2, 3, 2, 2, 2, 2, 2, 2, 2, 5, 5,
    3, 2, 2, 2, 2, 3, 2, 2, 2, 3, 2, 5, 5, 2, 5,
    3, 3, 3, 3, 2, 3, 2, 3, 3, 2, 2, 2, 2, 2, 5,
    3, 3, 2, 3, 2, 3, 2, 3, 3, 3, 2, 2, 3, 2, 2,
    2, 2, 2, 3, 2, 3, 2, 2, 2, 2, 2, 2, 3, 2, 2,
    2, 2, 3, 2, 2, 3, 3, 2, 3, 3, 3, 3, 3, 2, 2
};
int matrizTraps[15][15] = {
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1,
    1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0,
    1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0
};

// Matriz que armazena a informação da posição geográfica X de cada tile 
/*int matrizPosX[3][3] = {
200, 300, 400,
300, 400, 500,
400, 500, 600,
};*/
/*int matrizPosX[3][3] = {
460, 480, 500,
480, 500, 520,
500, 520, 540
};*/
int matrizPosX[15][15] = {
    240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520,
    260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540,
    280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560,
    300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580,
    320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600,
    340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620,
    360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640,
    380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660,
    400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680,
    420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700,
    440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720,
    460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740,
    480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740, 760,
    500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740, 760, 780,
    520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740, 760, 780, 800
};

int matrizTrapPosX[5][10] = {
    360, 380, 400, 440, 460, 480, 520, 540, 540, 560,
    440, 480, 580, 600, 340, 400, 420, 600, 620, 440,
    500, 520, 540, 560, 460, 560, 420, 580, 600, 560,
    580, 600, 700, 540, 640, 660, 680, 720, 560, 680,
    700, 740, 500, 520, 640, 660, 680, 760, 540, 780
};


// Matriz que armazena a informação da posição geográfica Y de cada tile
/*int matrizPosY[3][3] = {
300, 350, 400,
250, 300, 350,
200, 250, 300
};*/
/*int matrizPosY[3][3] = {
350, 360, 370,
340, 350, 360,
330, 340, 350
};*/
int matrizPosY[15][15] = {
    360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500,
    350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490,
    340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480,
    330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470,
    320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460,
    310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450,
    300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440,
    290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430,
    280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420,
    270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410,
    260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400,
    250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390,
    240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380,
    230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370,
    220, 230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360
};

int matrizTrapPosY[5][10] = {
    420, 430, 440, 460, 470, 480, 480, 490, 470, 480,
    400, 400, 450, 460, 310, 340, 350, 440, 450, 340,
    370, 380, 390, 400, 310, 360, 270, 350, 360, 320,
    330, 340, 390, 290, 340, 350, 360, 380, 280, 340, 
    350, 370, 230, 240, 300, 310, 320, 360, 230, 350
};

bool keyW = false, keyA = false, keyS = false, keyD = false;
bool keyQ = false, keyE = false, keyZ = false, keyX = false;

bool trapActive = true;

// Configuração do spritesheet:
int nAnimations = 4; // número de linhas (animações)
int nFrames = 6;     // número de colunas (frames por animação)

float ds = 1.0f / nFrames;
float dt = 1.0f / nAnimations;

// Configuração do tile
//int tileQueue = 1;
int tileTextures = 7; // número de texturas

// Estado atual da animação
int iAnimation = 0; // linha atual
int iFrame = 0;     // frame atual na linha

float timeSinceLastFrame = 0.0f;
float frameTime = 1.0f / 12.0f; // controla a velocidade da animação (12 FPS)

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;

// Novos uniforms para controle do frame do spritesheet
uniform vec2 texOffset;
uniform vec2 texScale;

void main()
{
    gl_Position = projection * model * vec4(position, 1.0);
    TexCoord = texOffset + texCoord * texScale;
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

class Sprite
{
    protected:
        GLuint vao;
        GLuint textureID;
        GLuint shaderID;

        glm::vec2 position;
        glm::vec2 scale;
        float rotation;

        glm::vec2 texOffset = glm::vec2(0.0f, 0.0f);
        glm::vec2 texScale = glm::vec2(1.0f, 1.0f);

        // Animação por spritesheet
        int rows = 1, cols = 1;
        int currentFrame = 0;
        float frameTime = 0.1f; // tempo entre frames
        float frameTimer = 0.0f;

    public:
        Sprite() : position(0.0f), scale(1.0f), rotation(0.0f)
        {}

        Sprite(GLuint vao, GLuint textureID, GLuint shaderID)
            : vao(vao), textureID(textureID), shaderID(shaderID),
            position(0.0f), scale(1.0f), rotation(0.0f) {}

        void inicializa(GLuint pVao, GLuint pTextureID, GLuint pShaderID){
            vao = pVao;
            textureID = pTextureID;
            shaderID = pShaderID;
        }

        void setPosition(float x, float y) {
            position = glm::vec2(x, y);
        }

        glm::vec2 getPosition() const {
            return position;
        }

        void setScale(float sx, float sy) {
            scale = glm::vec2(sx, sy);
        }

        glm::vec2 getScale() const
        {
            return scale;
        }

        void setRotation(float angleDegrees) {
            rotation = angleDegrees;
        }

        void setTexOffset(float x, float y) {
        texOffset = glm::vec2(x, y);
        }

        void setTexScale(float x, float y) {
            texScale = glm::vec2(x, y);
        }

        void setAnimation(int rows, int cols, float frameTime = 0.1f) {
            this->rows = rows;
            this->cols = cols;
            this->frameTime = frameTime;
            this->currentFrame = 0;
        }

        void updateAnimation(float deltaTime) {
            frameTimer += deltaTime;
            if (frameTimer >= frameTime) {
                frameTimer = 0.0f;
                currentFrame = (currentFrame + 1) % (rows * cols);
            }
        }

        void draw(const glm::mat4& projection)
        {
            glUseProgram(shaderID);

            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(position, 0.0f));
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0,0,1));
            model = glm::scale(model, glm::vec3(scale, 1.0f));

            GLint locModel = glGetUniformLocation(shaderID, "model");
            GLint locProjection = glGetUniformLocation(shaderID, "projection");
            GLint locTexOffset = glGetUniformLocation(shaderID, "texOffset");
            GLint locTexScale = glGetUniformLocation(shaderID, "texScale");

            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(locProjection, 1, GL_FALSE, &projection[0][0]);

            // Passa as variáveis do spritesheet
            glUniform2fv(locTexOffset, 1, &texOffset[0]);
            glUniform2fv(locTexScale, 1, &texScale[0]);

            // Passa a textura para o shader (bind na unidade 0)
            glUniform1i(glGetUniformLocation(shaderID, "ourTexture"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glUseProgram(0);
        }
};

class CharacterController : public Sprite
{
    private:
        float speed;

        // Fundo
        float backgroundOffsetX = 0.0f;
        float backgroundScrollSpeed = 0.1f;

        // Animação
        int frameX = 0;
        int frameY = 0;
        int maxFrames = 6;
        int totalRows = 4;
        float frameTime = 0.15f;
        float timeSinceLastFrame = 0.0f;

        // Posição inicial na matriz
        int currentLinha = 0, currentColuna = 0;

        bool estaVivo = true;

    public:
        CharacterController(GLuint vao, GLuint textureID, GLuint shaderID)
            : Sprite(vao, textureID, shaderID), speed(100.0f)
        {
            setTexScale(1.0f / maxFrames, 1.0f / totalRows);
        }

        int getCurrentLinha(){
            return currentLinha;
        }

        int getCurrentColuna(){
            return currentColuna;
        }

        bool getEstaVivo(){
            return estaVivo;
        }

        void updateMovement()
        {
            int lCurrentX = currentColuna, lCurrentY = currentLinha; // --> Guarda posição antes da checagem do teclado

            /*
                Matriz foi imaginada na diagonal para facilitar tratamento do movimento do personagem
            //                [2]
            //              [1] [5]
            //            [0] [4] [8]
            //    Linha0    [3] [7]
            //      Linha1    [6]
            //        Linha2
                
                Ex:
                    Estado inicial: matriz[1][1]
                    Q sobe --> pula da linha 1 para a linha 0, se mantendo na coluna 1 (movimento visual na diagonal)
                    W sobe direita --> pula da linha 1 para a linha 0, da coluna 1 para a coluna 2 (movimento visual subindo) 
            */
            if (keyQ) {
                currentLinha--; // sobe
                iAnimation = 2;
            } else if (keyZ) {
                currentColuna--; // vai para esquerda
                iAnimation = 2;
            } else if (keyX) {
                currentLinha++; // desce
                iAnimation = 3;
            } else if (keyE) {
                currentColuna++; // vai para direita
                iAnimation = 3;
            } else if (keyA) {
                currentLinha--;  // sobe
                currentColuna--; // esquerda
                iAnimation = 2;
            } else if (keyW) {
                currentLinha--;  // sobe
                currentColuna++; // direita
                iAnimation = 1;
            } else if (keyS) {
                currentLinha++;  // desce
                currentColuna--; // esquerda
                iAnimation = 0;
            } else if (keyD) {
                currentLinha++;  // desce
                currentColuna++; // direita
                iAnimation = 3;
            }

            // Impedir sair dos limites da matriz
            if (currentLinha < 0) currentLinha = 0;
            if (currentLinha > 14) currentLinha = 14;
            if (currentColuna < 0) currentColuna = 0;
            if (currentColuna > 14) currentColuna = 14;

            /*// Checa se a textura do tile para o qual se moveu é a textura 3 (lava). Se sim, desfaz ação, pois não é permitido 
            if(matrizTexturas[currentLinha][currentColuna] == 3){
                currentColuna = lCurrentX;
                currentLinha = lCurrentY;
            }

            // Checa se a textura do tile para o qual se moveu é a textura 5 (água). Se sim, desfaz ação, pois não é permitido 
            if(matrizTexturas[currentLinha][currentColuna] == 5){
                currentColuna = lCurrentX;
                currentLinha = lCurrentY;
            }*/
            if(matrizTexturas[currentLinha][currentColuna] == 3 || matrizTexturas[currentLinha][currentColuna] == 5)
            {
                currentColuna = lCurrentX;
                currentLinha = lCurrentY;
            }

            if(matrizTraps[currentLinha][currentColuna] == 1 && trapActive == true)
            {
                estaVivo = false;
            }



            // Resetar flags
            keyW = keyA = keyS = keyD = keyQ = keyE = keyZ = keyX = false;
        }


        void updateAnimation(float deltaTime)
        {
            timeSinceLastFrame += deltaTime;

            if (timeSinceLastFrame >= frameTime) {
                timeSinceLastFrame = 0.0f;
                iFrame = (iFrame + 1) % nFrames;

                float offsetS = iFrame * ds;
                float offsetT = 1.0f - (iAnimation + 1) * dt;
                setTexOffset(offsetS, offsetT);
            }
        }

        float getBackgroundOffsetX() const
        {
            return backgroundOffsetX;
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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Prova Grau B", nullptr, nullptr);
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
    GLuint backgroundTex = loadTexture("../assets/sprites/cave7.png");
    GLuint orcTex = loadTexture("../assets/sprites/orc3_walk_full.png");

    GLuint tileTex = loadTexture("../assets/tilesets/tileset2.png");
    GLuint trapOnTex = loadTexture("../assets/tilesets/trapOn.png");
    GLuint trapOffTex = loadTexture("../assets/tilesets/trapOff.png");

    GLuint chaveCongeladaTex = loadTexture("../assets/sprites/chaveCongelada.png");
    GLuint chaveLavaTex = loadTexture("../assets/sprites/chaveLava.png");

    GLuint cadeadoCongeladoTex = loadTexture("../assets/sprites/cadeadoCongelado.png");
    GLuint cadeadoLavaTex = loadTexture("../assets/sprites/cadeadoLava.png");

    GLuint portaFinalTex = loadTexture("../assets/sprites/portaFinal.png");
    GLuint portaFinalFechadaTex = loadTexture("../assets/sprites/portaFinalFechada.png");

    // Cria sprites
    Sprite background(quadVAO, backgroundTex, shaderProgram);
    CharacterController orc(quadVAO, orcTex, shaderProgram);

    Sprite chaveCongelada(quadVAO, chaveCongeladaTex, shaderProgram);
    Sprite chaveLava(quadVAO, chaveLavaTex, shaderProgram);

    Sprite cadeadoCongelado(quadVAO, cadeadoCongeladoTex, shaderProgram);
    Sprite cadeadoLava(quadVAO, cadeadoLavaTex, shaderProgram);

    Sprite portaFinal(quadVAO, portaFinalTex, shaderProgram);
    Sprite portaFinalFechada(quadVAO, portaFinalFechadaTex, shaderProgram);

    Sprite trapOn(quadVAO, trapOnTex, shaderProgram);
    Sprite trapOff(quadVAO, trapOffTex, shaderProgram);

    int lNumTex;
    Sprite tileMatrix[15][15]; // Matriz onde os tiles serão criados e armazenados
    for(int row=0; row<15; row++){
        for(int col=0; col<15; col++){
            tileMatrix[row][col].inicializa(quadVAO, tileTex, shaderProgram);

            // Define fração da textura
            float texW = 1.0f / tileTextures; // 7 texturas
            float texH = 1.0f / 1; // Só tem uma linha

            lNumTex = matrizTexturas[row][col];

            tileMatrix[row][col].setTexOffset(texW * lNumTex, 0.0f);  // Deslocamento na linha
            tileMatrix[row][col].setTexScale(texW, texH);

            tileMatrix[row][col].setPosition(matrizPosX[row][col], matrizPosY[row][col]);
            tileMatrix[row][col].setScale(40.0f, 20.0f); // Tamanho do quad na tela
        }
    }

    Sprite trapOnMatrix[5][10]; // Matriz onde os tiles serão criados e armazenados
    for(int row=0; row<5; row++){
        for(int col=0; col<10; col++){
            trapOnMatrix[row][col].inicializa(quadVAO, trapOnTex, shaderProgram);

            // Define fração da textura
            float texW = 1.0f / 1; // 7 texturas
            float texH = 1.0f / 1; // Só tem uma linha

            //lNumTex = matrizTexturas[row][col];

            trapOnMatrix[row][col].setTexOffset(0.0f, 0.0f);  // Deslocamento na linha
            trapOnMatrix[row][col].setTexScale(texW, texH);

            trapOnMatrix[row][col].setPosition(matrizTrapPosX[row][col], matrizTrapPosY[row][col]);
            trapOnMatrix[row][col].setScale(40.0f, 20.0f); // Tamanho do quad na tela
        }
    }

    Sprite trapOffMatrix[5][10]; // Matriz onde os tiles serão criados e armazenados
    for(int row=0; row<5; row++){
        for(int col=0; col<10; col++){
            trapOffMatrix[row][col].inicializa(quadVAO, trapOffTex, shaderProgram);

            // Define fração da textura
            float texW = 1.0f / 1; // 7 texturas
            float texH = 1.0f / 1; // Só tem uma linha

            //lNumTex = matrizTexturas[row][col];

            trapOffMatrix[row][col].setTexOffset(0.0f, 0.0f);  // Deslocamento na linha
            trapOffMatrix[row][col].setTexScale(texW, texH);

            trapOffMatrix[row][col].setPosition(matrizTrapPosX[row][col], matrizTrapPosY[row][col]);
            trapOffMatrix[row][col].setScale(40.0f, 20.0f); // Tamanho do quad na tela
        }
    }


    // Define projeção ortográfica
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WIDTH),
                                      0.0f, static_cast<float>(HEIGHT),
                                      -1.0f, 1.0f);

    // Ajusta background e personagem
    background.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);
    background.setScale(WIDTH, HEIGHT);

    chaveCongelada.setPosition(440.0f, 420.0f);
    chaveCongelada.setScale(30.0f, 30.0f);

    chaveLava.setPosition(520.0f, 220.0f);
    chaveLava.setScale(30.0f, 30.0f);

    cadeadoCongelado.setPosition(850.0f, 420.0f);
    cadeadoCongelado.setScale(30.0f, 30.0f);

    cadeadoLava.setPosition(850.0f, 390.0f);
    cadeadoLava.setScale(30.0f, 30.0f);

    portaFinal.setPosition(800.0f, 405.0f);
    portaFinal.setScale(100.0f, 110.0f);

    portaFinalFechada.setPosition(800.0f, 405.0f);
    portaFinalFechada.setScale(100.0f, 110.0f);

    trapOn.setScale(40.0f, 20.0f);
    

    //orc.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);
    orc.setScale(60.0f, 60.0f);
    

    // Ativar blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Controle de tempo para deltaTime
    float lastFrameTime = 0.0f;
    float trapStartTime = glfwGetTime();

    bool chaveAtiva = true;
    bool chaveAtiva1 = true;
    int contadorChave = 0;

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        float elapsed = glfwGetTime() - trapStartTime;

        // Atualiza estado das teclas
        bool keyW = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        bool keyS = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        bool keyA = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        bool keyD = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

        bool keyQ = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
        bool keyE = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
        bool keyZ = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
        bool keyX = glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS;

        background.setTexOffset(orc.getBackgroundOffsetX(), 0.0f);

        // Limpa tela
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenha background com offset atualizado
        background.draw(projection);

        portaFinalFechada.draw(projection);

        if(contadorChave == 2) portaFinal.draw(projection);
        

        //tile.draw(projection);
        for(int row=0; row<15; row++){
            for(int col=0; col<15; col++){
                tileMatrix[row][col].draw(projection);
            }
        }

        
        if (trapActive && elapsed >= 1.0f) {
            trapActive = false;
            trapStartTime = glfwGetTime(); // reinicia tempo
        }
        else if (!trapActive && elapsed >= 2.0f) {
            trapActive = true;
            trapStartTime = glfwGetTime(); // reinicia tempo
        }

        for(int row=0; row<5; row++){
            for(int col=0; col<10; col++){
                if (trapActive)
                    trapOnMatrix[row][col].draw(projection);
                else
                    trapOffMatrix[row][col].draw(projection);
            }
        }

        // Atualiza movimento e animação do personagem
        orc.updateMovement();
        orc.setPosition(matrizPosX[orc.getCurrentLinha()][orc.getCurrentColuna()], matrizPosY[orc.getCurrentLinha()][orc.getCurrentColuna()]+5);
        orc.updateAnimation(deltaTime);
        
        if(orc.getEstaVivo() == false)
        {
            cout << "Fim de jogo! Voce pisou em uma armadilha!\n";
        	break;
        }

        if(chaveAtiva)
        {
            chaveCongelada.draw(projection);
            cadeadoCongelado.draw(projection);
        }

        if(chaveAtiva1)
        {
            chaveLava.draw(projection);
            cadeadoLava.draw(projection);
        }

        if((orc.getCurrentLinha() == 2) && (orc.getCurrentColuna() == 8 && chaveAtiva))
        {
            chaveAtiva = false;
            contadorChave++;
        } 
        if((orc.getCurrentLinha() == 14) && (orc.getCurrentColuna() == 0 && chaveAtiva1))
        {
            chaveAtiva1 = false;
            contadorChave++;
        }

        if(orc.getCurrentLinha() >= 12 && orc.getCurrentColuna() == 14)
        {
            if(contadorChave == 2)
            {
                cout << "Fim de jogo! Parabens por ter completado a fase!\n";
        	    break;
            }
        }

        // Desenha personagem
        orc.draw(projection);

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
    // Fechar janela ao pressionar ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // Atualizar flags ao pressionar ou soltar teclas WASD e QEZX
    bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    bool isReleased = (action == GLFW_RELEASE);

    switch (key)
    {
        case GLFW_KEY_W:
            if (isPressed) keyW = true;
            if (isReleased) keyW = false;
            break;

        case GLFW_KEY_A:
            if (isPressed) keyA = true;
            if (isReleased) keyA = false;
            break;

        case GLFW_KEY_S:
            if (isPressed) keyS = true;
            if (isReleased) keyS = false;
            break;

        case GLFW_KEY_D:
            if (isPressed) keyD = true;
            if (isReleased) keyD = false;
            break;
        case GLFW_KEY_Q:
            if (isPressed) keyQ = true;
            if (isReleased) keyQ = false;
            break;

        case GLFW_KEY_E:
            if (isPressed) keyE = true;
            if (isReleased) keyE = false;
            break;

        case GLFW_KEY_Z:
            if (isPressed) keyZ = true;
            if (isReleased) keyZ = false;
            break;

        case GLFW_KEY_X:
            if (isPressed) keyX = true;
            if (isReleased) keyX = false;
            break;
        default:
            break;
    }
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

    stbi_set_flip_vertically_on_load(true); // Inverte verticalmente a imagem

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // **Alteração para permitir repetição no wrap**
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
