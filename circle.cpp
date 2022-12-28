
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

GLfloat glfx = 1.0;
GLfloat glfy = -1.0;
// Переменные с индентификаторами ID
// ID шейдерной программы
GLuint Program;
// ID атрибута вершин
GLint Attrib_vertex;
// ID атрибута цвета
GLint Attrib_color;
// ID юниформ переменной цвета
GLint Unif_color;
// ID VBO вершин
GLuint VBO_position;
// ID VBO цвета
GLuint VBO_color;
// Вершина

struct Vertex
{
    GLfloat x;
    GLfloat y;
};

const char* VertexShaderSource = R"(
    #version 330 core
    in vec2 coord;
    in vec4 color;

    out vec4 vert_color;

    void main() {
        gl_Position = vec4(coord, 0.0, 1.0);
        vert_color = color;
    }
)";

const char* FragShaderSource = R"(
    #version 330 core
    in vec4 vert_color;

    out vec4 color;
    void main() {
        color = vert_color;
    }
)";


const GLfloat onTapOffset = (GLfloat)0.1;
const int triangleAmount = 54;


void Init();
void Draw();
void Release();
void InitVBO();
void checkOpenGLerror();
void ShaderLog(unsigned int shader);

int main() {
    sf::Window window(sf::VideoMode(600, 600), "Circle", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    glewInit();

    Init();

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case (sf::Keyboard::Right):
                    if (glfx < 1)
                        glfx += onTapOffset;
                    break;
                case (sf::Keyboard::Left):
                    if (glfx > 0)
                        glfx -= onTapOffset;
                    break;
                case (sf::Keyboard::Up):
                    if (glfy < 0)
                        glfy += onTapOffset;
                    break;
                case (sf::Keyboard::Down): 
                    if (glfy > -1)
                        glfy -= onTapOffset;
                    break;
                default: break;
                }
                InitVBO();
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.display();
    }

    Release();
    return 0;
}



void InitVBO()
{
    glGenBuffers(1, &VBO_position);
    glGenBuffers(1, &VBO_color);

    // построение круга
    GLfloat twicePi = 2.0f * 3.14;
    Vertex triangle[triangleAmount + 2];
    triangle[0] = { 0,0 };
    for (int i = 0; i <= triangleAmount; i++)
    {
        GLfloat x = glfx * cos(i * twicePi / triangleAmount);
        GLfloat y = glfy * sin(i * twicePi / triangleAmount);
        triangle[i + 1] = { x, y };
    }
    triangle[triangleAmount + 1] = { glfx * cos(0.0f), glfy * sin(0.0f) };

    float colors[triangleAmount + 2][4];

    // расчет цвета
    // нулевая точка - белая
    colors[0][0] = 1;
    colors[0][1] = 1;
    colors[0][2] = 1;
    colors[0][3] = 1;

    int k = 0;
    double multFactor = 0.11111111111;
    for (int i = 0; i <= 8; i++)
    {
        colors[i + 1][0] = 1;
        colors[i + 1][1] = 0;
        colors[i + 1][2] = (multFactor * k);
        colors[i + 1][3] = 1;
        k++;
    }

    k = 0;
    for (int i =9; i <=17; i++) 
    {
        colors[i + 1][0] = 1 - (multFactor * k);
        colors[i + 1][1] = 0;
        colors[i + 1][2] = 1;
        colors[i + 1][3] = 1;
        k++;
    }

    k = 0;
    for (int i = 18; i <= 26; i++) 
    {
        colors[i + 1][0] = 0;
        colors[i + 1][1] = (multFactor * k);
        colors[i + 1][2] = 1;
        colors[i + 1][3] = 1;
        k++;
    }

    k = 0;
    for (int i = 27; i <= 35; i++) 
    {
        colors[i + 1][0] = 0;
        colors[i + 1][1] = 1;
        colors[i + 1][2] = 1 - (multFactor * k);
        colors[i + 1][3] = 1;
        k++;
    }

    k = 0;
    for (int i = 36; i <= 44; i++) 
    {
        colors[i + 1][0] = (multFactor * k);
        colors[i + 1][1] = 1;
        colors[i + 1][2] = 0;
        colors[i + 1][3] = 1;
        k++;
    }

    k = 0;
    for (int i = 45; i <= 53; i++)
    {
        colors[i + 1][0] = 1;
        colors[i + 1][1] = 1 - (multFactor * k);
        colors[i + 1][2] = 0;
        colors[i + 1][3] = 1;
        k++;
    }
    colors[55][0] = 1;
    colors[55][1] = 0;
    colors[55][2] = 0;
    colors[55][3] = 1;

    // Передаем вершины в буфер
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    checkOpenGLerror();
}



void InitShader()
{
    // вершинный шейдер
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    ShaderLog(vShader);

    // фрагментный шейдер
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    ShaderLog(fShader);

    // Создаем программу и прикрепляем шейдеры к ней
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);

    // Линкуем шейдерную программу
    glLinkProgram(Program);
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        std::cout << "error attach shaders \n";
        return;
    }

    // Вытягиваем ID атрибута вершин из собранной программы
    Attrib_vertex = glGetAttribLocation(Program, "coord");
    if (Attrib_vertex == -1)
    {
        std::cout << "could not bind attrib coord" << std::endl;
        return;
    }

    // Вытягиваем ID атрибута цвета
    Attrib_color = glGetAttribLocation(Program, "color");
    if (Attrib_color == -1)
    {
        std::cout << "could not bind attrib color" << std::endl;
        return;
    }

    checkOpenGLerror();
}

void Init()
{
    InitShader();
    InitVBO();
}

void Draw()
{
    // Устанавливаем шейдерную программу текущей
    glUseProgram(Program);
    // Включаем массивы атрибутов
    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_color);

    // Подключаем VBO_position
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Подключаем VBO_color
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glVertexAttribPointer(Attrib_color, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Отключаем VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_POLYGON, 0, triangleAmount + 2);

    // Отключаем массивы атрибутов
    glDisableVertexAttribArray(Attrib_vertex);
    glDisableVertexAttribArray(Attrib_color);

    glUseProgram(0);
    checkOpenGLerror();
}


// Освобождение шейдеров
void ReleaseShader()
{
    glUseProgram(0);
    glDeleteProgram(Program);
}

// Освобождение буфера
void ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_position);
    glDeleteBuffers(1, &VBO_color);
}

void Release() 
{
    ReleaseShader();
    ReleaseVBO();
}

void checkOpenGLerror()
{
    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

void ShaderLog(unsigned int shader)
{
    int infologLen = 0;
    int charsWritten = 0;
    char* infoLog;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        infoLog = new char[infologLen];
        if (infoLog == NULL)
        {
            std::cout << "ERROR: Could not allocate InfoLog buffer" << std::endl;
            exit(1);
        }
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
        std::cout << "InfoLog: " << infoLog << "\n\n\n";
        delete[] infoLog;
    }
}
