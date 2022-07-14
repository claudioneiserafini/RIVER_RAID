/*River Raid*/

/*Includes*/
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

/*Constantes*/
/* Tamanho da janela*/
#define ALTURA_JANELA 600
#define LARGURA_JANELA 700
#define PI 3.1415926535898

#define QUANT_BLOCOS 3 /*Quantidade de blocos gerados*/
#define QUANT_OBJETOS 2 /*Quantidade de objetos por blocos*/
#define QUANT_BLOCOS_RIO 6 /*Quantidade de tipos de blocos de 'rio' qu podem ser gerados*/

/*Posicao X dos blocos do rio*/
float cY_bloco_rio[QUANT_BLOCOS] = { 450, 150, -150 };

/*Tamanho dos blocos do 'rio'*/
int pontosX_bloco_rio[QUANT_BLOCOS_RIO][4] = { {120, 120, -120, -120},
								{180, 180, -160, -160},
								{140, 140, -130, -130},
								{120, 120, -110, -110},
								{130, 130, -140, -140},
								{100, 100, -180, -180}
};

/*Tipo dos blocos, serao sorteados*/
int tipo_bloco_rio[QUANT_BLOCOS] = { 0, 1, 2 };

int cont_blocos = 0;/*A cada 10 blocos, a ponte é criada*/
int cY_ponte = 15000;

/*Posicao dos objetos(inimigos)*/
float cX_objeto[QUANT_BLOCOS][QUANT_OBJETOS] = { {-700, -700}, {-700, -700}, {-700, -700} };
float cY_objeto[QUANT_BLOCOS][QUANT_OBJETOS] = { {-700, -700}, {-700, -700}, {-700, -700} };
int tipo_objeto[QUANT_BLOCOS][QUANT_OBJETOS] = { {1, 2}, {0, 1}, {0, 2} }; /*Seleciona entre os inimigos*/
float velocidade_objeto[QUANT_BLOCOS][QUANT_OBJETOS] = { {0, 1}, {0, 0}, {0, 1} }; /*Velocidade do movimento dos inimigos*/

/*Posicao tanque de combustivel*/
float posicao_tanque_comb[QUANT_BLOCOS][2];
/*Quantidade de combustivel*/
float nivel_de_combustivel = 195;

/*Coordenada inicial do avião*/
float cX_aviao = 0;

/*Status de tiro*/
bool status_tiro = false;
/*Posicao dos tiros*/
float cX_tiro = 450;
float cY_tiro = -160;

int pontuacao = 0;
int status_jogo = 0;

float velocidade_fase = 0;

/*Prototipacao*/
void display();
void tela(GLsizei w, GLsizei h);
void keyboard(unsigned char tecla, int x, int y);

void animar(int valor);
void animar_objetos();
void sortear_objetos(int, int);

void desenhar();

void teste_colisao();
void colisao_parede(int);
void colisao_combustivel(int);
void colisao_objeto(int, int);
void colisao_tiro_combustivel(int);
void colisao_tiro_objeto(int, int);
void colisao_ponte();
void colisao_tiro_ponte();
void fim_combustivel();
void inicializar();

void desenhar_string(char*, int, int, float, float, float, void*);
void tela_jogo();
void tela_final();

/*Desenhos*/
void aviao();
void bloco_rio(int);
void barra_status();
void tiro();
void marcador_de_combustivel();
void navio();
void aviao_inimigo();
void tanque_de_combustivel();
void ponte();
void rio_inicio();
void plano_de_fundo(float, float, float);

/*Funcoes*/
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(LARGURA_JANELA, ALTURA_JANELA);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("River Raid");

	srand(time(NULL));

	glutReshapeFunc(tela);
	glutDisplayFunc(display);
	glutKeyboardFunc(&keyboard);

	glutTimerFunc(10, animar, 1);

	glutMainLoop();
	return(0);
}

void display() {
	teste_colisao();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glTranslatef(LARGURA_JANELA / 2, ALTURA_JANELA / 2, 0);

	desenhar();

	glutSwapBuffers();
}

void tela(GLsizei w, GLsizei h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, LARGURA_JANELA, 0, ALTURA_JANELA);
	glMatrixMode(GL_MODELVIEW);

	glutReshapeWindow(LARGURA_JANELA, ALTURA_JANELA);
}

/*Funcoes do teclado*/
void keyboard(unsigned char tecla, int x, int y)
{

	if (status_jogo == 0) {
		if (tecla == 'f' || tecla == 'F') {
			exit(0);
		}
		if (tecla == ' ') {
			status_jogo = 1;

		}
	}
	else if (status_jogo == 1) {
		if (tecla == 'f' || tecla == 'F') {
			exit(0);
		}
		if (tecla == 'a' || tecla == 'A') {
			cX_aviao = cX_aviao - 15;
		}
		if (tecla == 'd' || tecla == 'D') {
			cX_aviao = cX_aviao + 15;
		}
		if (tecla == ' ') {
			if (status_tiro != true) {
				status_tiro = true;
				cX_tiro = cX_aviao;
			}
		}
	}
	else if (status_jogo == 2) {
		if (tecla == ' ') {
			status_jogo = 0;
		}
		if (tecla == 'f' || tecla == 'F') {
			exit(0);
		}
	}

	glutPostRedisplay();
}


/*Funcoes de animacao*/
void animar(int valor) {

	if (status_jogo == 1) {
		for (int i = 0; i < QUANT_BLOCOS; i++) {
			cY_bloco_rio[i] -= 2;


			if (cY_bloco_rio[i] < -450) {
				cont_blocos += 1;

				cY_bloco_rio[i] = 445;
				tipo_bloco_rio[i] = rand() % QUANT_BLOCOS_RIO;

				posicao_tanque_comb[i][0] = (rand() % ((pontosX_bloco_rio[tipo_bloco_rio[i]][0] - 10) * 2)) + (pontosX_bloco_rio[tipo_bloco_rio[i]][2] + 10);
				posicao_tanque_comb[i][1] = (rand() % 260) - 130;

				for (int j = 0; j < QUANT_OBJETOS; j++) {
					sortear_objetos(i, j);
				}
			}
		}

		cY_ponte -= 2;

		if (cont_blocos > 9) {
			cY_ponte = 400;
			cont_blocos = 0;
		}

		if (status_tiro == true) {
			if (cY_tiro > 310) {
				status_tiro = false;
				cY_tiro = -200;
				cX_tiro = 450;
			}
			cY_tiro += 5;
		}

		nivel_de_combustivel -= 0.1 + (velocidade_fase / 10);

		animar_objetos();

		pontuacao += 1;
	}

	glutPostRedisplay();
	glutTimerFunc(10, animar, 1);
}

/*Mover objetos(inimigos)*/
void animar_objetos() {
	for (int i = 0; i < QUANT_BLOCOS; i++) {
		for (int j = 0; j < QUANT_OBJETOS; j++) {
			cX_objeto[i][j] = cX_objeto[i][j] + (velocidade_objeto[i][j] / 4);
			if (pontosX_bloco_rio[tipo_bloco_rio[i]][0] < cX_objeto[i][j] + 20 ||
				pontosX_bloco_rio[tipo_bloco_rio[i]][3] > cX_objeto[i][j] - 20) {
				velocidade_objeto[i][j] = -velocidade_objeto[i][j];
			}
		}
	}
}

/*Sortear posicao dos objetos*/
void sortear_objetos(int i, int j) {
	int x = (rand() % ((pontosX_bloco_rio[tipo_bloco_rio[i]][0] - 20) * 2)) + (pontosX_bloco_rio[tipo_bloco_rio[i]][2] + 20);
	int y = 0;

	if (j == 0) {
		y = -100;
	}
	else if (j == 1) {
		y = 100;
	}

	cX_objeto[i][j] = x;
	cY_objeto[i][j] = y;

	tipo_objeto[i][j] = rand() % 3;

	if (tipo_objeto[i][j] == 1) {
		velocidade_objeto[i][j] = 1 + velocidade_fase;
	}
	else if (tipo_objeto[i][j] == 2) {
		velocidade_objeto[i][j] = 2 + velocidade_fase;
	}
	else {
		velocidade_objeto[i][j] = 0;
	}
}

/*Montar de desenho na tela*/
void desenhar() {
		tela_jogo();
	if (status_jogo == 2) {
		inicializar();
		tela_final();
	}
}

/*Teste de colisao*/
void teste_colisao() {
	for (int i = 0; i < QUANT_BLOCOS; i++) {
		if (cY_bloco_rio[i] > -330 && cY_bloco_rio[i] < -50) {
			colisao_parede(i);
			colisao_combustivel(i);

			for (int j = 0; j < QUANT_OBJETOS; j++) {
				colisao_objeto(i, j);
			}
		}

		colisao_tiro_combustivel(i);

		for (int j = 0; j < QUANT_OBJETOS; j++) {
			colisao_tiro_objeto(i, j);
		}
	}

	colisao_ponte();
	colisao_tiro_ponte();
	fim_combustivel();
}

void colisao_parede(int i) {
	if ((cX_aviao + 19) >= pontosX_bloco_rio[tipo_bloco_rio[i]][0] ||
		cX_aviao - 19 <= pontosX_bloco_rio[tipo_bloco_rio[i]][2])
	{
		status_jogo = 2;
	}
}

void colisao_combustivel(int i) {
	if (cX_aviao + 19 >= (posicao_tanque_comb[i][0] - 10) &&
		cX_aviao - 19 <= (posicao_tanque_comb[i][0] + 10) &&
		((posicao_tanque_comb[i][1] + cY_bloco_rio[i]) >= -200 &&
			(posicao_tanque_comb[i][1] + cY_bloco_rio[i]) <= -160))
	{
		nivel_de_combustivel = 195;
		posicao_tanque_comb[i][0] = 400;
		posicao_tanque_comb[i][1] = 400;
	}
}

void colisao_objeto(int i, int j) {
	if (cX_aviao + 19 >= (cX_objeto[i][j] - 20) &&
		cX_aviao - 19 <= (cX_objeto[i][j] + 20) &&
		((cY_objeto[i][j] + cY_bloco_rio[i]) >= -200 &&
			(cY_objeto[i][j] + cY_bloco_rio[i]) <= -160))
	{
		status_jogo = 2;
	}
}

void colisao_tiro_combustivel(int i) {
	if (cX_tiro >= (posicao_tanque_comb[i][0] - 10)
		&& cX_tiro <= (posicao_tanque_comb[i][0] + 10)
		&& ((posicao_tanque_comb[i][1] + cY_bloco_rio[i]) >= (cY_tiro - 3))
		&& ((posicao_tanque_comb[i][1] + cY_bloco_rio[i]) <= (cY_tiro + 3)))
	{
		posicao_tanque_comb[i][0] = 400;
		posicao_tanque_comb[i][1] = 400;
		status_tiro = false;
		cX_tiro = 450;
		cY_tiro = -160;
	}
}

void colisao_tiro_objeto(int i, int j) {
	if (cX_tiro >= (cX_objeto[i][j] - 20)
		&& cX_tiro <= (cX_objeto[i][j] + 20)
		&& ((cY_objeto[i][j] + cY_bloco_rio[i]) >= (cY_tiro - 3))
		&& ((cY_objeto[i][j] + cY_bloco_rio[i]) <= (cY_tiro + 3)))
	{
		cX_objeto[i][j] = 400;
		cY_objeto[i][j] = 400;
		status_tiro = false;
		cX_tiro = 450;
		cY_tiro = -160;
	}
}

void colisao_ponte() {
	if (cY_ponte < -155) {
		status_jogo = 2;
	}
}

void colisao_tiro_ponte() {
	if ((cY_tiro + 3) > (cY_ponte - 25) && (cY_tiro - 3) < (cY_ponte + 25) && cX_tiro < 450) {
		cY_ponte = 15000;
		status_tiro = false;
		cX_tiro = 450;
		cY_tiro = -160;

		velocidade_fase += 0.5;
	}
}

void fim_combustivel() {
	if (nivel_de_combustivel < 0.1) {
		status_jogo = 2;
	}
}

void inicializar() {
	cY_bloco_rio[0] = 450;
	cY_bloco_rio[1] = 150;
	cY_bloco_rio[2] = -150;

	cont_blocos = 0; /*A cada 5 blocos, a ponte é criada*/
	cY_ponte = 15000;

	/*Posicao dos objetos(inimigos)*/
	for (int i = 0; i < QUANT_BLOCOS; i++) {
		for (int j = 0; j < QUANT_OBJETOS; j++) {
			cX_objeto[i][j] = -700;
			cY_objeto[i][j] = -700;
		}
	}

	nivel_de_combustivel = 195;

	cX_aviao = 0;

	pontuacao = 0;

	velocidade_fase = 0;
}

/*Desenha palavras na tela*/
void desenhar_string(char* string, int posicao_X, int posicao_Y, float red, float green, float blue, void* fonte) {
	glColor3f(red, green, blue);

	glRasterPos2i(posicao_X, posicao_Y);
	for (char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(fonte, *c);
	}
}

void tela_jogo() {
	plano_de_fundo(0.0, 0.5, 0.0);

	for (int i = 0; i < QUANT_BLOCOS; i++) {
		glPushMatrix();
		glTranslatef(0, cY_bloco_rio[i], 0);
		bloco_rio(tipo_bloco_rio[i]);
		glPushMatrix();
		glTranslatef(posicao_tanque_comb[i][0], posicao_tanque_comb[i][1], 0);
		tanque_de_combustivel();
		glPopMatrix();
		for (int j = 0; j < QUANT_OBJETOS; j++) {
			glPushMatrix();
			glTranslatef(cX_objeto[i][j], cY_objeto[i][j], 0);
			int rotacao = 0;
			if (velocidade_objeto[i][j] > 0) {
				rotacao = 180;
			}
			glRotatef(rotacao, 0, 0, 0);
			if (tipo_objeto[i][j] == 0) {
				navio();
			}
			else if (tipo_objeto[i][j] == 1) {
				aviao_inimigo();
			}else {
				aviao_inimigo();
				}

			glPopMatrix();
		}
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(0, cY_ponte, 0);
	ponte();
	glPopMatrix();

	if (status_tiro == true) {
		glPushMatrix();
		glTranslatef(cX_tiro, cY_tiro, 0);
		tiro();
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(cX_aviao, -180, 0);
	aviao();
	glPopMatrix();
	barra_status();
}

void tela_final() {
	plano_de_fundo(0.1, 0.0, 0.0);

	desenhar_string((char*)"GAME OVER", -60, 0, 0.8, 0, 0, GLUT_BITMAP_HELVETICA_18);

	desenhar_string((char*)"Precione 'Espace' para iniciar.", -130, -230, 1, 1, 1, GLUT_BITMAP_9_BY_15);
	desenhar_string((char*)"Precione   'F'    para fechar.", -130, -250, 1, 1, 1, GLUT_BITMAP_9_BY_15);
}

/*Desenhos*/
void aviao() {
	glBegin(GL_POLYGON);
	glColor3f(0.2, 0.0, 0.0); //red
	glVertex2f(0, 20);
	glColor3f(0.1, 0.1, 0.1);
	glVertex2f(22, -22);
	glVertex2f(17, -23);
	glVertex2f(10, -23);
	glColor3f(1.0, 0.0, 0.0); //red
	glVertex2f(5, -16);
	glVertex2f(0, -25);
	glVertex2f(-5, -16);
	glColor3f(0.1, 0.1, 0.1); //black
	glVertex2f(-10, -23);
	glVertex2f(-17, -23);
	glVertex2f(-22, -22);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.15, 0.0, 0.0);; //red
	glVertex2f(-10, 2);
	glVertex2f(-10, -30);
	glVertex2f(10, 2);
	glVertex2f(10, -30);
	glEnd();
}

void tiro() {
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10);
	glBegin(GL_POINTS);
	glColor3f(0.9, 0.2, 0.15);
	glVertex2f(0, 0);
	glEnd();
}

void bloco_rio(int cX_bloco) {
	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 1.0);
	glVertex2f(pontosX_bloco_rio[cX_bloco][0], -150);
	glVertex2f(pontosX_bloco_rio[cX_bloco][1], 150);
	glVertex2f(pontosX_bloco_rio[cX_bloco][2], 150);
	glVertex2f(pontosX_bloco_rio[cX_bloco][3], -150);
	glEnd();
}

void barra_status() {
	glBegin(GL_QUADS);
	glColor3f(0.8, 0.8, 0.8);
	glVertex2f(-1000, -230);
	glVertex2f(1000, -230);
	glVertex2f(1000, -300);
	glVertex2f(-1000, -300);
	glEnd();

	marcador_de_combustivel();

	desenhar_string((char*)"River Raid", -320, -270, 0.2, 0.4, 0.7, GLUT_BITMAP_9_BY_15);
}

void marcador_de_combustivel() {
	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(290, -285);
	glVertex2f(90, -285);
	glVertex2f(90, -290);
	glVertex2f(290, -290);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(290, -290);
	glVertex2f(295, -290);
	glVertex2f(295, -240);
	glVertex2f(290, -240);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(90, -290);
	glVertex2f(85, -290);
	glVertex2f(85, -240);
	glVertex2f(90, -240);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(188, -290);
	glVertex2f(192, -290);
	glVertex2f(192, -260);
	glVertex2f(188, -260);
	glEnd();

	glPushMatrix();
	glTranslatef(nivel_de_combustivel, 0, 0);
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(90, -290);
	glVertex2f(95, -290);
	glVertex2f(95, -250);
	glVertex2f(90, -250);
	glEnd();
	glPopMatrix();
}

void navio() {
	/*Parte verde*/
	glBegin(GL_QUADS);
	glColor3f(0.0, 0.4, 0.0);
	glVertex2f(-5, 60);
	glVertex2f(2, 60);
	glVertex2f(2, 8);
	glVertex2f(-5, 8);
	glEnd();
	/*Parte Branca*/
	glBegin(GL_QUADS);
	glColor3f(255, 255, 255);
	glVertex2f(-20, 45);
	glVertex2f(15, 45);
	glVertex2f(15, 30);
	glVertex2f(-20, 30);
	glEnd();
	/*Parte preta*/
	glBegin(GL_QUADS);
	glColor3f(0.10, 0.0, 0.0);
	glVertex2f(-20, 0);
	glVertex2f(20, 0);
	glVertex2f(18, -10);
	glVertex2f(-18, -10);
	glEnd();
	/*Parte vermelha*/
	glBegin(GL_QUADS);
	glColor3f(0.20, 0.0, 0.0);
	glVertex2f(-20, 15);
	glVertex2f(20, 10);
	glVertex2f(20, 0);
	glVertex2f(-20, 0);
	glEnd();
}

void aviao_inimigo() {
	/*Corpo*/
	glBegin(GL_POLYGON);
	glColor3f(0.5, 0.4, 0.0);
	glVertex2f(-20, 0);
	glVertex2f(-17, 5);
	glVertex2f(20, 5);
	glVertex2f(20, -5);
	glVertex2f(-17, -5);
	glEnd();
	/*Asa*/
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.4, 0.0);
	glVertex2f(-6, 0);
	glVertex2f(-4, 20);
	glVertex2f(4, 21);
	glVertex2f(4, -21);
	glVertex2f(-4, -20);
	glEnd();
	/*Asa traseira*/
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.4, 0.0);
	glVertex2f(16, 0);
	glVertex2f(18, 10);
	glVertex2f(20, 12);
	glVertex2f(20, -12);
	glVertex2f(18, -10);
	glEnd();
}

void tanque_de_combustivel() {
	/*Parte preta cima*/
	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(-8.5, 20); // p b esq
	glVertex2f(11, 20); // p b dir
	glVertex2f(7, 27); // p c dir
	glVertex2f(-5, 27); // p c esq
	glEnd();

	/*Parte preta*/
	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(-8, 0);
	glVertex2f(11, 0);
	glVertex2f(11, -1.5);
	glVertex2f(-8, -1.5);
	glEnd();

	/*Parte vermelha*/
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.5, 0.0);
	glVertex2f(-8, 20);
	glVertex2f(11, 20);
	glVertex2f(11, 0);
	glVertex2f(-8, 0);
	glEnd();

	/*Parte branca*/
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glVertex2f(12, 20);
	glVertex2f(15, 0);
	glVertex2f(15, 15);
	glVertex2f(12, 10);
	glEnd();
}

void ponte() {
	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex2f(700, -25);
	glVertex2f(700, 25);
	glVertex2f(-700, 25);
	glVertex2f(-700, -25);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.8, 0.8, 0.0);
	glVertex2f(700, -2);
	glVertex2f(700, 2);
	glVertex2f(-700, 2);
	glVertex2f(-700, -2);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(700, -27);
	glVertex2f(700, -24);
	glVertex2f(-700, -24);
	glVertex2f(-700, -27);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(700, 24);
	glVertex2f(700, 27);
	glVertex2f(-700, 27);
	glVertex2f(-700, 24);
	glEnd();
}

void rio_inicio() {
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 0.3);
	glVertex2f(100, -300);
	glVertex2f(-100, -300);
	glVertex2f(-100, 50);
	glVertex2f(-200, 300);
	glVertex2f(0, 300);
	glVertex2f(100, 70);
	glEnd();
}

void plano_de_fundo(float red, float green, float blue) {
	glBegin(GL_QUADS);
	glColor3f(red, green, blue);
	glVertex2f(1000, -1000);
	glVertex2f(1000, 1000);
	glVertex2f(-1000, 1000);
	glVertex2f(-1000, -1000);
	glEnd();
}