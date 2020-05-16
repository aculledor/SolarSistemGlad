

#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <math.h> 

#include "Sphere.h"

//Para las transformaciones
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define GL_PI 3.14f
#define RADIO .5f
#define GradosToRadianes 0.0175f
#define MaxToMinDist 0.7f
#define AnguloRecto 90.0f*GradosToRadianes
#define VELGLOB .1f

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void character_callback(GLFWwindow* window, unsigned int codepoint);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int ancho = 800;
int alto = 800;

int camara = 1;
int mostrarOrbitas = 1;
int colorSol = 0;
char foto[9][14] = { "fotos/sun.png","fotos/mer.png","fotos/ven.png","fotos/ear.png","fotos/moo.png","fotos/mar.png",
"fotos/jup.png","fotos/sat.png","fotos/stm.png" };

typedef struct {
	float R;
	float G;
	float B;
} vectores;

typedef struct {
	float distanciaMax;
	float distanciaMin;	//70% da distancia maxima para facer a orbita
	float vel_trans;
	float ang_trans;
	float vel_rot;
	float ang_rot;
	float tam;
	int	  listarender;
	float X;
	float Z;
	float ang_univ;
	vectores colorObj;
	unsigned int imagen;
} objetoCelestial;

objetoCelestial sol =		{   0,     0 * MaxToMinDist,   0 * VELGLOB, 0, 10 * VELGLOB, 0, 20.0f, 0,  50, 0,  0, {1.0f, 1.0f, 1.0f}, 0 };	//El color del sol es el color de la luz que emite
objetoCelestial mercurio =  {  50,    50 * MaxToMinDist,  30 * VELGLOB, 0, 50 * VELGLOB, 0,  5.0f, 0,  50, 0,  0, {1.0f, 0.0f, 0.0f}, 0 };
objetoCelestial venus =		{  80,    80 * MaxToMinDist,  14 * VELGLOB, 0, 14 * VELGLOB, 0,  8.0f, 0,  80, 0,  0, {0.0f, 1.0f, 0.0f}, 0 };
objetoCelestial tierra =	{ 150,   150 * MaxToMinDist,  20 * VELGLOB, 0, 10 * VELGLOB, 0, 10.0f, 0, 150, 0,  0, {0.0f, 0.0f, 0.8f}, 0 };
objetoCelestial luna =		{  25,    25 * MaxToMinDist, 200 * VELGLOB, 0, 20 * VELGLOB, 0,  2.0f, 0,  25, 0,  0, {0.8f, 0.8f, 0.8f}, 0 };
objetoCelestial ISS =		{  30,    30 * MaxToMinDist, 150 * VELGLOB, 0,  1 * VELGLOB, 0,  1.0f, 0,  30, 0, 30 * GradosToRadianes, {0.2f, 0.2f, 0.2f}, 0 };
objetoCelestial marte =		{ 250,   250 * MaxToMinDist,  35 * VELGLOB, 0, 10 * VELGLOB, 0, 12.0f, 0, 250, 0,  0, {0.8f, 0.8f, 0.8f}, 0 };
objetoCelestial jupiter =	{ 450,   450 * MaxToMinDist,  30 * VELGLOB, 0,  8 * VELGLOB, 0, 15.0f, 0, 450, 0,  0, {0.0f, 0.3f, 0.9f}, 0 };
objetoCelestial saturno =	{ 650,   650 * MaxToMinDist,  40 * VELGLOB, 0,  5 * VELGLOB, 0, 18.0f, 0, 650, 0,  0, {0.0f, 0.9f, 0.3f}, 0 };
objetoCelestial estrellas = { 0, 0, 0, 0, 0, 0, 1000.0f, 0, 0, 0,  0, {0.01f, 0.01f, 0.01f}, 0 };


extern GLuint setShaders(const char *nVertx, const char *nFrag);
GLuint shaderProgram, shaderProgramTex, IDShader;


// unsigned int VBO, VAO, EBO;
unsigned int VAO, orbitasVAO, VAOCuadrado;
Sphere sphere(1.0f, 18, 9);

void dibujaEjes() {
	unsigned int VBO, EBO;
	
	float vertices[] = {
		//Vertices          //Colores
		0.0f, 0.0f, 0.0f,	 0.0f, .0f, .0f,  // 0
		40.0f, 0.0f, 0.0f,	 1.0f, .0f, .0f,  //x
		0.0f, 40.0f, 0.0f,	.0f, 1.0f, 0.0f, // y
		0.0f, 40.0f, 0.0f,	 .0f, .0f, 1.0f,  // z  
		40.0f , 40.0f, 40.0f,	0.0f, .0f, .0f // 1,1,1 bueno realmente la mitad
	};
	unsigned int indices[] = {  // empieza desde cero
		0, 1,
		0, 2,
		0, 3,
		0, 4
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position Color

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

}

void orbitas() {
	float vertices[301 * 6];
	unsigned int VBO;
	for (int i = 0; i < 301; i++) {
		vertices[i * 6] = cos(2 * i * GL_PI / 300);
		vertices[(i * 6) + 1] = -sin(2 * i * GL_PI / 300)*MaxToMinDist;
		vertices[(i * 6) + 2] = 0.0f;
		vertices[(i * 6) + 3] = 0.0f;
		vertices[(i * 6) + 4] = 0.0f;
		vertices[(i * 6) + 5] = 1.0f;
	}

	glGenVertexArrays(1, &orbitasVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(orbitasVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
}

void myCamara(vectores *vista) {
	glViewport(0, 0, ancho, alto);

	//VIEW
	glm::mat4 view = glm::mat4(1); 
	view = glm::lookAt(glm::vec3(0.0f, 600.0f, 600.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	(*vista).R = 0.0f; (*vista).G = 600.0f; (*vista).B = 600.0f;

	//PROYECCION
	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(60.0f), (float)ancho / (float)alto, 0.1f, 2000.0f);
	unsigned int proyectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(proyectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void myTelescopio(objetoCelestial observador, objetoCelestial observado, vectores *vista) {
	glViewport(0, 0, ancho, alto);

	(*vista).R = observador.X; (*vista).G = 0.0f; (*vista).B = observador.Z;

	//VIEW 
	glm::mat4 view = glm::mat4(1);
	view = glm::lookAt(	glm::vec3(observador.X, 0, observador.Z),
						glm::vec3(observado.X, 0.0f, observado.Z),
						glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//PROYECCION
	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(60.0f), (float)ancho / (float)alto, 0.1f, 2000.0f);
	unsigned int proyectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(proyectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void mirarSatelite(objetoCelestial planeta, objetoCelestial satelite, vectores *vista) {
	glViewport(0, 0, ancho, alto);

	(*vista).R = planeta.X; (*vista).G = 0.0f; (*vista).B = planeta.Z;

	//VIEW 
	glm::mat4 view = glm::mat4(1);
	view = glm::lookAt(glm::vec3(planeta.X, 0.0f, planeta.Z),
		glm::vec3(planeta.X+satelite.X, 0.0f, planeta.Z + satelite.Z),
		glm::vec3(0.0f, 1.0f, 0.0f));

	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//PROYECCION
	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(90.0f), (float)ancho / (float)alto, 0.1f, 2000.0f);
	unsigned int proyectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(proyectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

unsigned int myCargaTexturas(char *nombre) {
	// load and create a texture 
	// -------------------------
	unsigned int textura;
	glGenTextures(1, &textura);
	glBindTexture(GL_TEXTURE_2D, textura); 
	
	//Parametros
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load(nombre, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); 
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return(textura);
}

void openGlInit() {
	//Habilito aqui el depth en vez de arriba para los usuarios de linux y mac mejor arriba
	//Incializaciones varias
	glClearDepth(1.0f); //Valor z-buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // valor limpieza buffer color
	glEnable(GL_DEPTH_TEST); // z-buffer
	glEnable(GL_CULL_FACE); //ocultacion caras back
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);
}

void Display();

double lastTime = glfwGetTime();
double currentTime;
double lapsoTime;
int nbFrames = 0;

void tiempo() {
	static float unSegundo = 0;
	currentTime = glfwGetTime();
	nbFrames++;
	lapsoTime = currentTime - lastTime;
	unSegundo = unSegundo - lapsoTime;

	if (unSegundo >= 1.0) {
		nbFrames = 0;
		unSegundo = 0;
	}
	lastTime = currentTime;
}

void calculoMov(objetoCelestial* planeta) {
	(*planeta).ang_trans += (*planeta).vel_trans*lapsoTime;
	if ((*planeta).ang_trans > 360) (*planeta).ang_trans -= 360;

	(*planeta).X =  (*planeta).distanciaMax * cos((*planeta).ang_trans*GradosToRadianes);
	(*planeta).Z = -(*planeta).distanciaMin * sin((*planeta).ang_trans*GradosToRadianes);


	(*planeta).ang_rot += (*planeta).vel_rot*lapsoTime;
	if ((*planeta).ang_rot > 360) (*planeta).ang_rot -= 360;
}

void movimiento() {
	calculoMov(&sol);

	calculoMov(&mercurio);

	calculoMov(&venus);

	calculoMov(&tierra);

	calculoMov(&luna);

	calculoMov(&ISS);

	calculoMov(&marte);

	calculoMov(&jupiter);

	calculoMov(&saturno);
}

void dibujarOrbita(glm::mat4 model, unsigned int modelLoc, objetoCelestial cuerpo) {
	model = glm::rotate(model, AnguloRecto, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, cuerpo.ang_univ, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(cuerpo.distanciaMax, cuerpo.distanciaMax, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(orbitasVAO);
	glDrawArrays(GL_LINE_STRIP, 0, 301);
	glBindVertexArray(0);
}

void crearPlaneta(glm::mat4 model, unsigned int modelLoc, objetoCelestial planeta) {
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);
	//Calculo la Matriz 
	model = glm::mat4(1); // identity matrix

	
	// Lo guro en funcion del tiempo
	if (mostrarOrbitas == 1) dibujarOrbita(model, modelLoc, planeta);
	glUniform3f(colorLoc, planeta.colorObj.R, planeta.colorObj.G, planeta.colorObj.B);
	model = glm::rotate(model, (float)planeta.ang_univ, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(planeta.X, 0.0f, planeta.Z));
	model = glm::rotate(model, (float)planeta.ang_rot, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(planeta.tam, planeta.tam, planeta.tam));
	model = glm::rotate(model, -55.0f*GradosToRadianes, glm::vec3(1.0f, 0.0f, 0.0f));
	
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeta.imagen);
	sphere.draw();
}

void crearSatelite(glm::mat4 model, unsigned int modelLoc, objetoCelestial planetaEje, objetoCelestial satelite) {
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);
	//Calculo la Matriz 
	model = glm::mat4(1); // identity matrix

	// Lo guro en funcion del tiempo
	model = glm::translate(model, glm::vec3(planetaEje.X, 0.0f, planetaEje.Z));
	if (mostrarOrbitas == 1) dibujarOrbita(model, modelLoc, satelite);
	glUniform3f(colorLoc, satelite.colorObj.R, satelite.colorObj.G, satelite.colorObj.B);
	model = glm::rotate(model, (float)satelite.ang_univ, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(satelite.X, 0.0f, satelite.Z));
	model = glm::rotate(model, (float)satelite.ang_rot, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(satelite.tam, satelite.tam, satelite.tam));
	
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, satelite.imagen);
	sphere.draw();
}

void Color_Sol() {
	switch (colorSol)
	{
		//Blanco
	case 0:
		sol.colorObj.R = 1.0f; sol.colorObj.G = 1.0f; sol.colorObj.B = 1.0f;
		break;
		//Rojo
	case 1:
		sol.colorObj.R = 1.0f; sol.colorObj.G = 0.0f; sol.colorObj.B = 0.0f;
		break;
		//Verde
	case 2:
		sol.colorObj.R = 0.0f; sol.colorObj.G = 1.0f; sol.colorObj.B = 0.0f;
		break;
		//Azul
	case 3:
		sol.colorObj.R = 0.0f; sol.colorObj.G = 0.0f; sol.colorObj.B = 1.0f;
		break;
	case 4:
		sol.colorObj.R = 1.0f; sol.colorObj.G = 1.0f; sol.colorObj.B = 0.0f;
		break;
	}
}

void Display() {
	Color_Sol();
	// render
		// ------
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //Borro el Buffer the la ventana
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Para dibujarlos en lineas
	vectores posicionCamara = { 0.0f, 0.0f, 0.0f };

	/*
	*	VARABLES DE LOS SHADERS
	*/
	glm::mat4 model;
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");

	//El color del objeto
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);

	//El color de la luz
	unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightColorLoc, sol.colorObj.R, sol.colorObj.G, sol.colorObj.B);

	// la posicion de la luz lo hago abajo poque va con el objeto que se mueve
	unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, 0.0f, 0.0f, 0.0f);

	unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	/*
	*
	*/

	switch (camara)
	{
	case 1:
		myCamara(&posicionCamara);
		break;
	case 2:
		myTelescopio(tierra, sol, &posicionCamara);
		break;
	case 3:
		mirarSatelite(tierra, luna, &posicionCamara);
		break;
	case 4:
		mirarSatelite(tierra, ISS, &posicionCamara);
		break;
	case 5:
		myTelescopio(venus, sol, &posicionCamara);
		break;
	case 6:
		myTelescopio(marte, sol, &posicionCamara);
		break;
	case 7:
		myTelescopio(jupiter, sol, &posicionCamara);
		break;
	case 8:
		myTelescopio(saturno, sol, &posicionCamara);
		break;
	}

	/*
	*	Actualizamos la posicion del observador para los brillos de la luz Especular
	*/
	glUniform3f(viewPosLoc, posicionCamara.R, posicionCamara.G, posicionCamara.B);

	//EJES
	model = glm::mat4(1); // identity matrix
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

	//MERCURIO
	crearPlaneta(model, modelLoc, mercurio);

	//Venus
	crearPlaneta(model, modelLoc, venus);

	//TIERRA
	if(camara != 3) crearPlaneta(model, modelLoc, tierra);

	//LUNA
	crearSatelite(model, modelLoc, tierra, luna);

	//ISS
	//glUniform3f(colorLoc, );
	crearSatelite(model, modelLoc, tierra, ISS);

	//MARTE
	crearPlaneta(model, modelLoc, marte);

	//JUPITER
	crearPlaneta(model, modelLoc, jupiter);

	//SATURNO
	crearPlaneta(model, modelLoc, saturno);
	
	//SOL
	crearPlaneta(model, modelLoc, sol);

	//SOL
	glFrontFace(GL_CW);
	crearPlaneta(model, modelLoc, estrellas);
	glFrontFace(GL_CCW);
}

void luces() {
	glUseProgram(shaderProgram);

	glm::mat4 transform; // es la matriz de transformacion	
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "model");


	//El color del objeto
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLoc, 0.3f, 0.3f, 0.9f);

	//El color de la luz
	unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightLoc, 0.3f, 0.3f, 0.9f);
}

void inicializarImagenes(){
	sol.imagen = myCargaTexturas(foto[0]);
	mercurio.imagen = myCargaTexturas(foto[1]);
	venus.imagen = myCargaTexturas(foto[2]);
	tierra.imagen = myCargaTexturas(foto[3]);
	luna.imagen = myCargaTexturas(foto[4]);
	ISS.imagen = myCargaTexturas(foto[4]);
	marte.imagen = myCargaTexturas(foto[5]);
	jupiter.imagen = myCargaTexturas(foto[6]);
	saturno.imagen = myCargaTexturas(foto[7]);
	estrellas.imagen = myCargaTexturas(foto[8]);
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//Creo la ventana
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Planetario", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Fallo Crear Ventana" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCharCallback(window, character_callback);

	// glad: 
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	openGlInit();

	shaderProgram = setShaders("shaderPres.vert", "shaderPres.frag");
	//shaderProgramTex = setShaders("shaderTex.vert", "shaderTex.frag");
	glUniform1i(glGetUniformLocation(shaderProgram, "Tex"), 0);
	inicializarImagenes();

	orbitas();

	glUseProgram(shaderProgram); 

	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);
		tiempo();
		movimiento();
		Display();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
	if(codepoint == '1') camara = 1;
	if(codepoint == '2') camara = 2;
	if(codepoint == '3') camara = 3;
	if(codepoint == '4') camara = 4;
	if(codepoint == '5') camara = 5;
	if(codepoint == '6') camara = 6;
	if(codepoint == '7') camara = 7;
	if(codepoint == '8') camara = 8;
	if(codepoint == '9') camara = 9;	//Soltar camara
	if(codepoint == '0') mostrarOrbitas = -mostrarOrbitas;
	if (codepoint == 'e') { colorSol++; if (colorSol > 4) colorSol -= 5; }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glfwGetWindowSize(window, &ancho, &alto);
}
