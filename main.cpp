#define _USE_MATH_DEFINES

#include <stdio.h>
#include <iostream>
#include <string>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"

using namespace std;

// informacoes do xml
float corFundoR;
float corFundoG;
float corFundoB;
int windowWidth;
int windowHeight;
string titulo;
float raio;
float corCirculoR;
float corCirculoG;
float corCirculoB;

int gx;
int gy;

// coordenadas do mouse
int mousePosX;
int mousePosY;

// coordenadas do centro do circulo
float centerX;
float centerY;

// mouse dentro da area do circulo
bool onCircle;

// mouse se movendo em direcao ao centro do circulo
bool towardsCenter;

// states do mouse
bool leftButtonPressed;
bool rightButtonPressed;

// circulo desenhado
bool circleDrawn;

// movimento ilegal: quando o usuario clica fora do circulo e vai mexendo o mouse sem soltar o botao ate encostar no circulo
// caso isso aconteca, o circulo nao pode se alterar pois o usuario comecou a clicar fora dele
bool illegalMove;

// distance do mouse ao centro do circulo
float distanceFromCenter = -1;

// Para eventos de movimento do mouse quando um ou mais botoes do mesmo estao pressionados
void mouseMove(int x, int y)
{
	// Caso uma distancia anterior tenha sido calculada
	if (distanceFromCenter != -1)
	{
		// Calcula uma nova distancia
		float newDistance = sqrt((x - centerX)*(x - centerX) + (y - centerY)*(y - centerY));

		// Verifica, comparando as duas distancias, se o mouse esta se movendo em direcao ao centro ou na direcao oposta
		if (newDistance < distanceFromCenter)
		{
			towardsCenter = true;
		}
		else
		{
			towardsCenter = false;
		}
	}

	// Calcula a distancia atual ao centro do circulo
	distanceFromCenter = sqrt((x - centerX)*(x - centerX) + (y - centerY)*(y - centerY));

	// Verifica se o mouse esta dentro da area do circulo
	if (abs(distanceFromCenter) <= raio)
	{
		onCircle = true;
	}
	else if (abs(distanceFromCenter) > raio)
	{
		onCircle = false;
	}

	// Verifica se um dos botoes foi pressionado fora do circulo, para impedir que o mesmo consiga ser arrastado quando o mouse atingi-lo
	if ((leftButtonPressed || rightButtonPressed) && !onCircle)
	{
		illegalMove = true;
	}

	// Caso o movimento nao seja ilegal e o mouse esteja dentro da area do circulo
	if (!illegalMove && onCircle)
	{
		// Caso os dois botoes do mouse estejam sendo pressionados, a acao nao acontece
		// Circulo se move quando arrasta-se o mouse com o botao esquerdo pressionado
		if (leftButtonPressed && !rightButtonPressed)
		{
			gx = x - mousePosX;
			gy = y - mousePosY;
		}
		// Circulo diminiu o raio quando arrasta-se o mouse em direcao ao centro com o botao direito
		else if (rightButtonPressed && !leftButtonPressed && towardsCenter)
		{
			raio -= 0.25f;
		}
		// Circulo diminiu o raio quando arrasta-se o mouse em direcao as bordas do circulo com o botao direito
		else if (rightButtonPressed && !leftButtonPressed && !towardsCenter)
		{
			raio += 0.25f;
		}
	}

	glutPostRedisplay();
}

// Para eventos de clique do mouse
void mouseClick(int button, int state, int x, int y)
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                leftButtonPressed = true;

                // so funciona se tiver aqui... senao ele ignora o clique
                if (!circleDrawn)
                {
                    circleDrawn = true;

                    mousePosX = x;
                    mousePosY = y;

                    glutPostRedisplay();
                }
            }
            if (state == GLUT_UP)
            {
                leftButtonPressed = false;
				illegalMove = false;
            }
            break;

        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
            {
                rightButtonPressed = true;
            }
            if (state == GLUT_UP)
            {
                rightButtonPressed = false;
				illegalMove = false;
            }
            break;

        default:
            break;
    }
}

void draw()
{
	int coordX, coordY;
	int X, Y;
	int vertices = 40;

	// Divide o circulo em 40 fatias (triangulos)
	for (int i = 0; i <= vertices; i++)
	{
		// Coordenadas x e y para cada fatia
		coordX = raio*cosf(i*2.0f*M_PI / vertices);
		coordY = raio*sinf(i*2.0f*M_PI / vertices);

		// Guarda valores do centro do circulo
		if (i == 0)
		{
			centerX = coordX + mousePosX + gx - raio;
			centerY = coordY + mousePosY + gy;
		}

		X = coordX + mousePosX + gx;
		Y = coordY + mousePosY + gy;

		glVertex2f(X, Y);
	}
}

void display()
{
	// Limpar todos os pixels
	glClear(GL_COLOR_BUFFER_BIT);

	if (circleDrawn)
	{
		// Desenhar um circulo
		glBegin(GL_TRIANGLE_FAN);
		glColor3f(corCirculoR, corCirculoG, corCirculoB);

		draw();

		glEnd();
	}

	// Nao esperar
	glFlush();
}

void init()
{
	// Selecionar cor de fundo
	glClearColor(corFundoR, corFundoG, corFundoB, 1.0);

	// Inicializar sistema de viz
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, windowWidth, windowHeight, 0.0, -1.0, 1.0);
}

void parsing(const char* loc)
{
	TiXmlDocument doc(loc);
	bool loadOkay = doc.LoadFile();

	if (!loadOkay)
	{
		printf("Could not load file\n");
  	}

	TiXmlHandle xmlHandler(&doc);
	TiXmlElement *aplicacao, *janela, *circulo;

	aplicacao = doc.FirstChildElement("aplicacao");
	janela = aplicacao->FirstChildElement("janela");
	circulo = janela->NextSiblingElement();

	windowWidth = atoi(janela->FirstChildElement("largura")->GetText());
	windowHeight = atoi(janela->FirstChildElement("altura")->GetText());

	corFundoR = atof(janela->FirstChildElement("fundo")->Attribute("corR"));
	corFundoG = atof(janela->FirstChildElement("fundo")->Attribute("corG"));
	corFundoB = atof(janela->FirstChildElement("fundo")->Attribute("corB"));

	titulo = janela->FirstChildElement("titulo")->GetText();

	raio = atof(circulo->Attribute("raio"));

	corCirculoR = atof(circulo->Attribute("corR"));
	corCirculoG = atof(circulo->Attribute("corG"));
	corCirculoB = atof(circulo->Attribute("corB"));
}

int main(int argc, char** argv)
{
	if (argc != 2)
    {
        printf("Modelo: %s local_do_arquivo\n", argv[0]);
        return 1;
    }

	string s = argv[1];
	string filename = "config.xml";

	string path = s + filename;
	const char* localizacao = path.c_str();
	parsing(localizacao);
	const char* title = titulo.c_str();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(title);
	init();

	// funcoes de callback
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutDisplayFunc(display);

	glutMainLoop();

	return 0;
}
