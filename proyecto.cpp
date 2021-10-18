#include <GL/glut.h>
#include <iostream>
#include<stdio.h>
#include<math.h>
#include<string>
#include <fstream>
#include <cstdlib>
using namespace std;

//Tamaño de ventana
GLsizei height = 800, width = 1200;

//circulo
int radius = 25;

float PI = 3.14285714286;

int nodeCount = -1;

int maxNodes = 20;

//Matrix adjacente
int adjMatrix[20][20];

//nodos seleccionados
int lineNodesCount = 0;

//Indica si el nodo Inicio ya fue seleccionado
int sourceNodeCreated = 0;

//Inidica si el nodo destino ya fue seleccionado
int destinationNodeCreated = 0;


struct node
{	
	int id;
	int xCoordinate;
	int yCoordinate;
}nodes[20], lineNodes[2], sourceNode, destinationNode;

//Estructura de ayuda para recorrer doc con vertices
typedef struct tempNode
{
	int id;
	int x;
	int y;	
} tempN;

//Estructura de ayuda para recorrer doc que guarda la distancia entre nodos
typedef struct record {
    int key;
	int a;
    int b;
    int distancia;
} rec;


  void drawText(const string& text, const unsigned int x, const unsigned int y)
  {

    glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(x, y);
    for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)c);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
  }
  
  void drawDist(const string& text, int x, int y){
  	glColor3f(1.0, 1.0, 1.0);
  	glRasterPos2i(x, y);
  	for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)c);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    //cout << "Distancia impresa: " << text << "\n";
    
    glEnd();
	glFlush();
  }


void drawLine(int a, int b,int distancia, char color){
	
	//color de la linea
	if(color == 'N' || color == 'R')
		glColor3f(0.3, 0.3, 0.3);

		
	glBegin(GL_LINES);

	glVertex2f(nodes[a].xCoordinate, nodes[a].yCoordinate);
	glVertex2f(nodes[b].xCoordinate, nodes[b].yCoordinate);
	
	//cout << "xA: " << nodes[a].xCoordinate << " yA: " << nodes[a].yCoordinate << "\n";
	//cout << "xB: " << nodes[b].xCoordinate << " yB: " << nodes[b].yCoordinate << "\n";
	
	glEnd();
	glFlush();	
	
	//obtener los id que sirven como posiciones de la matrix adjacente 
	int id1 = nodes[a].id;
	int id2 = nodes[b].id;
	
	//guardar en la matrix adjacente
	adjMatrix[id1][id2] = distancia;
	adjMatrix[id2][id1] = distancia;
	
	
	//Cordenadas para imprimir el texto
	int xText = abs(((nodes[b].xCoordinate-nodes[a].xCoordinate)/2)+nodes[a].xCoordinate);
	int yText = abs(((nodes[b].yCoordinate-nodes[a].yCoordinate)/2)+nodes[a].yCoordinate);
	//cout << "CoordTextX: " << xText << "\n";
	//cout << "CoordTextY: " << yText << "\n";
	string text = to_string(distancia);
	//cout << "Distancia a imprimir: " << text << "\n";
	drawDist(text,xText,yText);
	
}


void drawCircle(float cx, float cy, float r, float num_segments, char colorCharacter)
{
	
	//rad angulo en radianes
	//coordenada x
	//coordenada y
	float rad,x,y;
	string text;
	
	//agregar texto id al circulo
	//si es N entonces se pone color verde 
	if(colorCharacter == 'N'){
		glColor3f(0.0, 1.0, 0.0);
		text = to_string(nodeCount+1);
		drawText(text.data(),cx - 2, cy - 8);
		nodeCount++;
	}

	/// Esta linea sirve para conectar todos los puntos de abajo para formar el circulo
	glBegin(GL_LINE_LOOP);
 
	for(int i = 0; i < num_segments; i++){
		
		rad = 2.0f * PI * i / num_segments;
 		x = r * cosf(rad);
		y = r * sinf(rad);
 		
 		//dibujo el punto
		glVertex2f(x + cx, y + cy);
	}
	glEnd();

	
	glFlush();

}

void drawInicioFinal(float cx, float cy,int id, float r, float num_segments, char colorCharacter)
{
	
	//rad angulo en radianes
	//coordenada x
	//coordenada y
	float rad,x,y;

	//agregar texto id al circulo
	string text;
	text = to_string(id);
	drawText(text.data(),cx - 2, cy - 8);
	// Estas dos if es cuando se selecciona el inicio y final
	if(colorCharacter == 'B')
		glColor3f(0.0, 0.0, 1.0);
	if(colorCharacter == 'R')
		glColor3f(1.0, 0.0, 0.0);


	/// Esta linea sirve para conectar todos los puntos de abajo para formar el circulo
	glBegin(GL_LINE_LOOP);
 
	for(int i = 0; i < num_segments; i++){
		
		rad = 2.0f * PI * i / num_segments;
 		x = r * cosf(rad);
		y = r * sinf(rad);
 		
 		//dibujo el punto
		glVertex2f(x + cx, y + cy);
	}
	
	glEnd();
	glFlush();

}


// Recorre el documento con los vertices de cada nodo para dibujarlos
void drawNodes(){
		
	ifstream f ("vertices.txt");
	
	tempN v;
	f >> v.id;
	while(!f.eof()){
		 
		 f >> v.x;
         f >> v.y;
		drawCircle(v.x, v.y, radius, 200, 'N');
		nodes[nodeCount].xCoordinate = v.x;
		nodes[nodeCount].yCoordinate = v.y;
		nodes[nodeCount].id = nodeCount;
		f >> v.id;		
	}
	f.close();
}

//Recorre el documento con las distancias entre cada nodo para dibujarlas
void drawNodesLines(){
	ifstream f ("distancia.txt");
	
	rec r;
	f >> r.key;
	while(!f.eof()){
		
		f >> r.a;
		f >> r.b;
		f >> r.distancia;
		//cout << "1: " << r.a << " 2: " << r.b << " dist: " << r.distancia << "\n";
		drawLine(r.a,r.b,r.distancia,'N');
			
		f >> r.key;
	}
	f.close();	
}


//Revisa si se esta seleccionando un nodo
int noRepeat(int x, int y) 
{	

	for(int i = 0; i < nodeCount; i++)
	{	
	
		if(x<=(nodes[i].xCoordinate+radius) && x>=(nodes[i].xCoordinate-radius) && y<=(nodes[i].yCoordinate+radius) && y>=(nodes[i].yCoordinate-radius)){
			
			//Agrega nodo a lista de seleccionados
			lineNodes[lineNodesCount] = nodes[i];
			return 0;
		}
		
	}
	//Ningun nodo fue seleccionado
	return 1;
}


//Seleccionar Inicio y Destino con el Mouse
void clickMouse(int btn, int state, int x, int y)
{
	//correccion en el sistema de coordenadas para poder verificar la seleccion de nodos
	y = height-y;
	
	
	
	//Seleccion de Inicio y Destino
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN && (!sourceNodeCreated || !destinationNodeCreated))
	{
		
		//Si hay nodos creados
		if(nodeCount)
		{
		
			//Si un nodo es seleccionado se va a asignar a lineNodes[0]
			if(!noRepeat(x, y))
			{
				
				//Cuando no se ha seleccionado el nodo Inicio
				if(!sourceNodeCreated)
				{
					
					sourceNode = lineNodes[0];
					//Nodo Incio Seleccionado
					sourceNodeCreated = 1;
					
					drawInicioFinal(sourceNode.xCoordinate, sourceNode.yCoordinate,sourceNode.id, radius, 200, 'B');
				}
				else 
				{
					//revisa que no se este seleccionando otra vez el nodo de inicio
					if(((sourceNode.xCoordinate - lineNodes[0].xCoordinate) != 0) || ((sourceNode.yCoordinate - lineNodes[0].yCoordinate) != 0 ))
					{
						
						destinationNode = lineNodes[0];
						//Nodo de destinado seleccionado
						destinationNodeCreated = 1;
						
						drawInicioFinal(destinationNode.xCoordinate, destinationNode.yCoordinate,destinationNode.id, radius, 200, 'R');
						
					} 
				}

			}
		}
	}
}

/*void verAdjMatrix(){
	for(int i=0;i<20;i++){
		for(int j=0;j<20;j++){
			cout << adjMatrix[i][j] << "\n";
		}
	}
}*/

void myInit()
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 1);
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(5);
	 
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Proyecto 1");
    myInit();
    glutMouseFunc(clickMouse);
    drawNodes();
	drawNodesLines();
//	verAdjMatrix();
    glutMainLoop();

    return 0;
}

