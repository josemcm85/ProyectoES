#include <GL/glut.h>
#include <iostream>
#include<stdio.h>
#include<math.h>
#include<string>
#include <fstream>
#include <cstdlib>
using namespace std;

//Almacena la cantidad de vértices
int V=0;
int dist[100];
bool visited[100]={0};
//Almacena el árbol que se utiliza para calcular las rutas
int parent[100];
#define INF 999

string streetName[50];
int streetA[50];
int streetB[50];

string building[50];

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
  
  
void drawBuildingName(const string& text, const unsigned int x, const unsigned int y)
  {

    glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(x, y);
    for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
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

 void drawStreetName(const string& text, int x, int y){
  	glColor3f(1.0, 1.0, 1.0);
  	glRasterPos2i(x, y);
  	for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)c);
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


void drawName(string nombre,const unsigned int x, const unsigned int y){

//Da formato al nombre y le agrega 25 a y
size_t ubicacionGuion=nombre.find("_");

while(ubicacionGuion!=string::npos){
	
	nombre.replace(ubicacionGuion,1," ");
	
	ubicacionGuion=nombre.find("_");
}

	drawBuildingName(nombre,x - 50, y + 45);
}

void streetNameParser(string nombre,const unsigned int x, const unsigned int y){

//Da formato al nombre y le agrega 25 a y
size_t ubicacionGuion=nombre.find("_");

while(ubicacionGuion!=string::npos){
	
	nombre.replace(ubicacionGuion,1," ");
	
	ubicacionGuion=nombre.find("_");
}

	drawStreetName(nombre,x, y);
}

// Recorre el documento con los vertices de cada nodo para dibujarlos
void drawNodes(){
		
	ifstream f ("vertices.txt");
	ifstream lugar ("lugares.txt");
	
	string nombre="";
	int idNodo;
	
	tempN v;
	f >> v.id;
	lugar >>idNodo;
	while(!f.eof()){
		//Para ir contando la cantidad de vértices que hay y almacenarlas en una variable global 
		V++;
		 
		 f >> v.x;
         f >> v.y;
        lugar >>nombre;
        building[v.id]=nombre;
        
		drawCircle(v.x, v.y, radius, 200, 'N');
		drawName(nombre,v.x,v.y);
		nodes[nodeCount].xCoordinate = v.x;
		nodes[nodeCount].yCoordinate = v.y;
		nodes[nodeCount].id = nodeCount;
		f >> v.id;	
		lugar >>idNodo;	
	}
	f.close();
}

//Recorre el documento con las distancias entre cada nodo para dibujarlas
void drawNodesLines(){
	ifstream f ("distancia.txt");
	ifstream calle("calles.txt");
	string nombre="";
	int idNodo;
	
	rec r;
	f >> r.key;
	calle>>idNodo;
	while(!f.eof()){
		
		f >> r.a;
		f >> r.b;
		f >> r.distancia;
		calle>>nombre;
		streetName[r.key]=nombre;
		streetA[r.key]=r.a;
		streetB[r.key]=r.b;
		int xText = abs(((nodes[r.b].xCoordinate-nodes[r.a].xCoordinate)/2)+nodes[r.a].xCoordinate);
		int yText = abs(((nodes[r.b].yCoordinate-nodes[r.a].yCoordinate)/2)+nodes[r.a].yCoordinate)-35;
		streetNameParser(nombre, xText, yText);
		//cout << "1: " << r.a << " 2: " << r.b << " dist: " << r.distancia << "\n";
		drawLine(r.a,r.b,r.distancia,'N');
			
		f >> r.key;
		calle>>idNodo;
	}
	f.close();
	calle.close();	
}


void init(){
	for(int i=0;i<V;i++){
		parent[i]=i;
		dist[i]=INF;
	}	
	dist[sourceNode.id]=0;	
}

int getNearest(){
	//basado en los datos de costo acumulado calculados en el método dijkstra(), se revisa cual 
	//de los vecinos es más barato para así en la siguiente corrida de dijkstra() agregarlo al árbol 
	//y seguir el ciclo, esta vez con los vecinos de ese vecino más cercano/barato.
	int minvalue=INF;
	int minnode=0;
	for(int i=0;i<V;i++){
		//Esto es porque la única forma de que el dist del 
		//nodo sea menor a INF sin haber sido visitados es si 
		//son vecinos del nodo anteriormente analizado y agregado 
		//al árbol
		if(!visited[i] && dist[i]<minvalue){
		//	cout<<"dist Node "<<i<<" ="<<dist[i]<<endl;
			//Lo que hace esto es revisar cual de los nodos no visitados tiene el menor costo
			minvalue=dist[i];
			minnode=i;
		}
	}
	
//	cout<<"minNode:"<<minnode<<" minVal"<<minvalue<<endl;
	return minnode;
}

void dijkstra(){
	for(int i=0;i<V;i++){
		
		int nearest =getNearest();
		visited[nearest]=true;
		
		for(int adj=0; adj<V;adj++){
			//Si el costo no es INF, significa que ese es un nodo adyacente
			//Y revisa que el costo de dist[adj] sea mayor a  dist[nearest]+cost[nearest][adj]
			//porque si esa condición no se cumple, significa que ese vecino tiene otra ruta por donde se 
			//llega desde el origen a un menor costo 
			if(adjMatrix[nearest][adj] !=INF && dist[adj]> dist[nearest]+adjMatrix[nearest][adj]){
				
				//Así vamos acumulando los costos(lo cual podemos usar para en la siguiente ronda saber cual
				//vecino es el más cercano
				//Entonces lo que hacemos es revisar los vecinos del nodo actual y calcularles 
				//el costo, para ello tomamos el costo del nodo actual hasta el origen y le sumamos el costo del 
				//nodo actual a ese nodo vecino(lo cual está en el adjacency matrix)
				
				dist[adj]=dist[nearest]+adjMatrix[nearest][adj];
				
				//Aquí es donde se va creando el árbol 
				//Asignando en el espacio del vecino más cercano 
				//una referencia al nodo actual(para que asi actúe como 
				//la raíz de este vecino)
				parent[adj]=nearest;
				
			}
		}
	}
}

void display(){
	cout<<"Ruta para llegar a "<<destinationNode.id<<":"<<endl;
		cout<<destinationNode.id;
		int parnode= parent[destinationNode.id];
		int ruta[20];
		ruta[0]=destinationNode.id;
		int i=1;
		while(parnode!=sourceNode.id){
			ruta[i]=parnode;
			cout<<"<-" << parnode <<" ";
			parnode=parent[parnode];
			i++;
		}
		cout<<endl;
		
		for(int j=i-1;j>=0;j--){
			cout<<ruta[j]<<" ";
		}
		
		cout<<"distancia:"<<dist[destinationNode.id]<<endl;
}

/*

 void drawStreetName(const string& text, int x, int y){
  	glColor3f(1.0, 1.0, 1.0);
  	glRasterPos2i(x, y);
  	for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)c);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    //cout << "Distancia impresa: " << text << "\n";
    
    glEnd();
	glFlush();
  }
*/
void drawInstructions(){
		cout<<"Ruta para llegar a "<<destinationNode.id<<":"<<endl;
		string titulo="Ruta para llegar a "+ building[destinationNode.id];

		streetNameParser(titulo,400, 300);
		string distancia="Distancia: "+ to_string(dist[destinationNode.id])+"00m";
		streetNameParser(distancia,400, 275);
		
		cout<<destinationNode.id;
		int parnode= parent[destinationNode.id];
		int ruta[20];
		ruta[0]=destinationNode.id;
		int i=1;
		
		//streetA, streetB, streetName
		
		while(parnode!=sourceNode.id){
			ruta[i]=parnode;
			cout<<"<-" << parnode <<" ";
			parnode=parent[parnode];
			i++;
		}
		cout<<endl;
		
		int puntoARuta=sourceNode.id;
		int puntoBRuta;
		int distanciaNodos;
		string instruccion;
		int ubicacionYInstruccion=250;
		string distanciaInstruccion;
		for(int j=i-1;j>=0;j--){
			puntoBRuta=ruta[j];
			cout<<ruta[j]<<" ";
			
			for(int k=0;k<50;k++){
				if((streetA[k]==puntoARuta && streetB[k]==puntoBRuta) || (streetB[k]==puntoARuta && streetA[k]==puntoBRuta)){
					distanciaNodos=(dist[puntoBRuta]-dist[puntoARuta])*100;
					distanciaInstruccion=to_string(distanciaNodos);
					instruccion="Camine " +distanciaInstruccion+"m "+" hasta "+ building[puntoBRuta]+" sobre " + streetName[k];
						streetNameParser(instruccion,400, ubicacionYInstruccion);
						ubicacionYInstruccion=ubicacionYInstruccion-25;
				}
			}
			string streetName[50];
			string streetA[50];
			string streetB[50];
			
			puntoARuta=ruta[j];
			
		}
		
		
		cout<<"distancia:"<<dist[destinationNode.id]<<endl;
		
}

//Revisa si se esta seleccionando un nodo
int noRepeat(int x, int y) 
{	

	for(int i = 0; i < V; i++)
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
				cout<<"click1"<<endl;
			//Si un nodo es seleccionado se va a asignar a lineNodes[0]
			if(!noRepeat(x, y))
			{
						cout<<"click2"<<endl;
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
						init();
   						dijkstra();
   						drawInstructions();
					} 
				}

			}
		}
	}
}

void verAdjMatrix(){
	for(int i=0;i<20;i++){
		for(int j=0;j<20;j++){
			cout << adjMatrix[i][j] << " ";
		}
			cout << endl;
	}
}




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


void adjustMatrix(){
//Hace ajustes a la matriz de adjacencia generada para asegurar el buen funcionamiento de dijkstra 
	for(int i=0;i<20;i++){
		for(int j=0;j<20;j++){
			if(adjMatrix[i][j]==0 && i!=j){
				adjMatrix[i][j]=999;
			}
		}
			
	}

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
	adjustMatrix();
//	verAdjMatrix();
    glutMainLoop();

    return 0;
}

