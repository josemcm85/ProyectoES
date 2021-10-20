#include <GL/glut.h>
#include <iostream>
#include<stdio.h>
#include<math.h>
#include<string>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <unistd.h>
using namespace std;

//Almacena la cantidad de vértices
int V=0;
int dist[100];
bool visited[100]={0};
//Almacena el árbol que se utiliza para calcular las rutas
int parent[100];
#define INF 999

string selectedMap="escazu";

bool darkMode=true;
bool rotated=false;
//Tamaño de ventana
GLsizei height = 800, width = 1200;

//circulo
//int radius = 25;
int radius=10;

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


//coordenadas del botón reset(mata redonda)
int resetX1=50;
int resetY1=175;
int resetX2=200;
int resetY2=250;


//coordenadas del botón para seleccionar Escazu
int escazuX1=50;
int escazuY1=275;
int escazuX2=200;
int escazuY2=350;

//coordenadas del botón toggleDarkMode
int darkToggleX1=1050;
int darkToggleY1=175;
int darkToggleRadius=50;

//coordenadas del botón rotate
int rotateX1=1050;
int rotateY1=300;
int rotateRadius=50;

struct node
{	
	int id;
	int xCoordinate;
	int yCoordinate;
}nodes[20], lineNodes[2], sourceNode, destinationNode;


//Estructura para almacenar nombres de edificios y sus ubicaciones
struct buildingStruct
{
	string name;
	int x;
	int y;
}building[50];

//Estructura para almacenar el nombre de calles y de que nodo a qué nodo van
struct streetStruct
{
	string name;
	int A;
	int B;
}street[50];

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



  //Método para mostrar el texto con el nombre de edificios
void drawBuildingName(const string& text, const unsigned int x, const unsigned int y)
  {

    //glColor3f(1.0, 1.0, 0.0);
    
    if(darkMode){
    	glColor3f(0.776f,0.564f,0.427f);
	}else{
		glColor3f(0.408f,0.419f,0.498f);
	}
 

    glRasterPos2i(x, y);
    for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnd();
    glFlush();
  }
  
    //Método para mostrar el texto con el nombre de calles
 void drawStreetName(const string& text, int x, int y){
 	if(darkMode){
 		glColor3f(1.0, 1.0, 1.0);
	 }else{
	 	glColor3f(0.3f,0.3f,0.3f);
	 }

  	glRasterPos2i(x, y);
  	for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)c);
    
    glEnd();
	glFlush();
  }



 void drawInfoBackground(){
 	
 	int infoX1=0;
 	int infoX2=1200;
 	int infoY1=400;
 	int infoY2=0;
 	
 	if(darkMode){
 		glColor3f(0.211f, 0.29f, 0.349f);
	 }else{
	 	//glColor3f(0.811f, 0.89f, 0.949f);
	 	glColor3f(0.149f,0.274f,0.325f);
	 }

		 
	
	glBegin(GL_QUADS);
		
	glVertex2f(infoX1, infoY1);
	glVertex2f(infoX2, infoY1);
	glVertex2f(infoX2, infoY2);
	glVertex2f(infoX1, infoY2);
	glEnd();

    glFlush();
 }
  
 void drawDivider(int x1,int y1, int x2, int y2){
 	if(darkMode){
 		glColor3f(0.235f,0.251f,0.262f);
	 }else{
	 	glColor3f(0.635f,0.651f,0.662f);
	 }
 
 	glBegin(GL_LINES);

	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	
	glEnd();
	glFlush();
 }
  
    //Método que dibuja las líneas que representan las calles
  //también va armando la matriz de adyacencia del grafo
 
void drawLine(int a, int b,int distancia, char color){
	
	//color de la linea
	if(color == 'N')
		//glColor3f(0.3, 0.3, 0.3);
		//glColor3f(1.0, 1.0, 1.0);
		//glColor3f(0.24f,0.255f,0.259f);
		if(darkMode){
			glColor3f(0.137f,0.415f,0.243f);	
		}else{
			glColor3f(0.9f,0.9f,0.9f);
		}
		
	if(color== 'R'){
		//glColor3f(1.0, 0.0, 0.0);
		glColor3f(0.4,0.615,0.964);
	}
		
	glBegin(GL_LINES);

	glVertex2f(nodes[a].xCoordinate, nodes[a].yCoordinate);
	glVertex2f(nodes[b].xCoordinate, nodes[b].yCoordinate);
	
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

	string text = to_string(distancia);

}



void drawRotateButton(){

if(darkMode){
		glColor3f(0.188f, 0.192f, 0.208f);
	}else{
		glColor3f(0.906f, 0.435f, 0.317f);
	}
        glBegin(GL_TRIANGLE_FAN);
        
        glVertex2f(rotateX1, rotateY1); // Center
        for(int i = 0.0f; i <= 360; i++)
                glVertex2f(rotateRadius*cos(M_PI * i / 180.0) + rotateX1, rotateRadius*sin(M_PI * i / 180.0) + rotateY1);
        
        glEnd();
        glFlush();

int textX=rotateX1-25;
int textY=rotateY1-10;
    
    string text1="Rotar";

    if(darkMode){
    	glColor3f(0.498f, 0.655f, 0.788f);
    
	}else{
		glColor3f(0.965f, 0.973f, 0.976f);
	
	}
	
  
  	glRasterPos2i(textX,textY);

  	for (const char c : text1)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
 
    glEnd();
	glFlush();
}

void drawToggleDarkButton(){
	
    if(darkMode){
		glColor3f(0.188f, 0.192f, 0.208f);
	}else{
		glColor3f(0.906f, 0.435f, 0.317f);
	}
        glBegin(GL_TRIANGLE_FAN);
        
        glVertex2f(darkToggleX1, darkToggleY1); // Center
        for(int i = 0.0f; i <= 360; i++)
                glVertex2f(darkToggleRadius*cos(M_PI * i / 180.0) + darkToggleX1, darkToggleRadius*sin(M_PI * i / 180.0) + darkToggleY1);
        
        glEnd();
        glFlush();

int textX=darkToggleX1-25;
int textY=darkToggleY1+5;
    
    string text1="Modo";
    string text2="";
    if(darkMode){
    	glColor3f(0.498f, 0.655f, 0.788f);
    	text2="Claro";
	}else{
		glColor3f(0.965f, 0.973f, 0.976f);
		text2="Oscuro";
	}
	
  
  	glRasterPos2i(textX,textY);

  	for (const char c : text1)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
 
 	textY=textY-25;
 
 	glRasterPos2i(textX,textY);

  	for (const char c : text2)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
 
    glEnd();
	glFlush();
	
	

}


void drawEscazuButton(){
	
	if(darkMode){
		glColor3f(0.188f, 0.192f, 0.208f);
	}else{
		glColor3f(0.906f, 0.435f, 0.317f);
	}

	
	
	glBegin(GL_QUADS);
		
	glVertex2f(escazuX1, escazuY1);
	glVertex2f(escazuX2, escazuY1);
	glVertex2f(escazuX2, escazuY2);
	glVertex2f(escazuX1, escazuY2);
	glEnd();

    glFlush();


int textX=escazuX1+50;
int textY=escazuY1+25;
    
    if(darkMode){
    	glColor3f(0.498f, 0.655f, 0.788f);
	}else{
		glColor3f(0.965f, 0.973f, 0.976f);
	}
  
  	glRasterPos2i(textX,textY);
  	string text="Escazu";
  	for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
 
    glEnd();
	glFlush();

}

//Método que dibuja el botón de reset(y su texto)

void drawMataRedondaButton(){
	
	if(darkMode){
		glColor3f(0.188f, 0.192f, 0.208f);
	}else{
		glColor3f(0.906f, 0.435f, 0.317f);
	}

	
	
	glBegin(GL_QUADS);
		
	glVertex2f(resetX1, resetY1);
	glVertex2f(resetX2, resetY1);
	glVertex2f(resetX2, resetY2);
	glVertex2f(resetX1, resetY2);
	glEnd();

    glFlush();


int textX=resetX1+15;
int textY=resetY1+25;
    
    if(darkMode){
    	glColor3f(0.498f, 0.655f, 0.788f);
	}else{
		glColor3f(0.965f, 0.973f, 0.976f);
	}
  
  	glRasterPos2i(textX,textY);
  	string text="Mata Redonda";
  	for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
 
    glEnd();
	glFlush();

}

void drawSearchIcon(){
	
	float cx=25;
	float cy=375;
	float r=15;
	
	float x1=cx+5+5;
	float y1=cy-5-5;
	float x2=cx+40+5;
	float y2=cy-25-5;
		
	glColor3f(0.96f,0.522f,0.486f);
		glBegin(GL_LINE_LOOP);
 
 	int num_segments=200;
 	float x=0;
 	float y=0;
 	float rad=0;
	for(int i = 0; i < num_segments; i++){
		
		rad = 2.0f * M_PI * i / num_segments;
 		x = r * cosf(rad);
		y = r * sinf(rad);
 		
 		//dibujo el punto
		glVertex2f(x + cx, y + cy);
	}
	
	glEnd();
	glFlush();
	     /*
        glBegin(GL_TRIANGLE_FAN);
        
        
        glVertex2f(cx, cy); // Center
        for(int i = 0.0f; i <= 360; i++)
                glVertex2f(r*cos(M_PI * i / 180.0) + cx, r*sin(M_PI * i / 180.0) + cy);
        
        glEnd();
        glFlush();
        */
  	glColor3f(0.96f,0.522f,0.486f);
	
 
 	glBegin(GL_LINES);

	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	
	glEnd();
	glFlush();
	
	
	int textX=cx+50;
	int textY=cy-10;
    

  	glColor3f(1.0f, 1.0f, 1.0f);
  	glRasterPos2i(textX,textY);
  	
  	string text="";
  	if(selectedMap=="escazu"){
  		text="Escazu";
	  }
	  
	if(selectedMap=="mataRedonda"){
		text="Mata Redonda";
	}

  	for (const char c : text)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
 
    glEnd();
	glFlush();

}

//Método que dibuja los circulos que representan cada edificio
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
	//	glColor3f(0.3, 0.3, 0.3);
		glColor3f(1.0, 1.0, 1.0);
		text = to_string(nodeCount+1);

		nodeCount++;
	}
//	
	if(darkMode){
		glColor3f(0.96f,0.522f,0.486f);
	}else{
		glColor3f(0.3, 0.3, 0.3);
	}

	     
        glBegin(GL_TRIANGLE_FAN);
        
        
        glVertex2f(cx, cy); // Center
        for(int i = 0.0f; i <= 360; i++)
                glVertex2f(r*cos(M_PI * i / 180.0) + cx, r*sin(M_PI * i / 180.0) + cy);
        
        glEnd();
        glFlush();

}

//Método que redibuja los circulos que representan cada edificio
//Basado en si son el punto A o B de la ruta
void drawInicioFinal(float cx, float cy,int id, float r, float num_segments, char colorCharacter)
{
	
	//rad angulo en radianes
	//coordenada x
	//coordenada y
	float rad,x,y;

	//agregar texto id al circulo
	string text;
	text = to_string(id);

	// Estas dos if es cuando se selecciona el inicio y final
	if(colorCharacter == 'B')
	//	glColor3f(0.0, 1.0, 0.0);
		glColor3f(0.949f,0.674f,0.07f);
	if(colorCharacter == 'R')
	//	glColor3f(1.0, 0.0, 0.0);
		glColor3f(0.635f,0.102f,0.078f);

	 glBegin(GL_TRIANGLE_FAN);
        
        
        glVertex2f(cx, cy); // Center
        for(int i = 0.0f; i <= 360; i++)
                glVertex2f(r*cos(M_PI * i / 180.0) + cx, r*sin(M_PI * i / 180.0) + cy);
        
        glEnd();
		glFlush();
}



//Método que da formato al nombre del edificio(quitando los "_" y reemplazándolos por espacios)
void drawName(string nombre,const unsigned int x, const unsigned int y){

//Da formato al nombre 
size_t ubicacionGuion=nombre.find("_");

while(ubicacionGuion!=string::npos){
	
	nombre.replace(ubicacionGuion,1," ");
	
	ubicacionGuion=nombre.find("_");
}

	drawBuildingName(nombre,x - 50, y + 25);
}


//Método que da formato al nombre de la calle(quitando los "_" y reemplazándolos por espacios)
void streetNameParser(string nombre,const unsigned int x, const unsigned int y){


size_t ubicacionGuion=nombre.find("_");

while(ubicacionGuion!=string::npos){
	
	nombre.replace(ubicacionGuion,1," ");
	
	ubicacionGuion=nombre.find("_");
}

	drawStreetName(nombre,x, y);
}

// Recorre el documento con los vertices de cada nodo para dibujarlos
//Para que aunque haya cambios en la ruta, los nodos y el texto sigan encima 
void redrawNodes(){
	
		string verticeFile="";
	string lugarFile="";
	if(selectedMap=="mataRedonda"){
		verticeFile="verticesMataRedonda.txt";
		lugarFile="lugaresMataRedonda.txt";	
	}	
	
	if(selectedMap=="escazu"){
		verticeFile="verticesEscazu.txt";
		lugarFile="lugaresEscazu.txt";	
	}
	
	ifstream f (verticeFile);
	ifstream lugar (lugarFile);
	string nombre="";
	int idNodo;
	
	tempN v;
	f >> v.id;
	lugar >>idNodo;
	while(!f.eof()){
		//Para ir contando la cantidad de vértices que hay y almacenarlas en una variable global 
	
		 f >> v.x;
         f >> v.y;
        lugar >>nombre;
           if(rotated){
        	v.x=1200-v.x;
        	v.y=(800-v.y)+375;
		}
      
	bool nodoInicioOFin=false;

		if(sourceNodeCreated==1){
			if(sourceNode.id==v.id){
				nodoInicioOFin=true;
				drawInicioFinal(sourceNode.xCoordinate, sourceNode.yCoordinate,sourceNode.id, radius, 200, 'B');
			}
		}
		
		if(destinationNodeCreated==1){
			if(destinationNode.id==v.id){
				nodoInicioOFin=true;
				drawInicioFinal(destinationNode.xCoordinate, destinationNode.yCoordinate,destinationNode.id, radius, 200, 'R');
			}
		}
		
	
        drawName(nombre,v.x,v.y);
        
        if(nodoInicioOFin==false){
        	drawCircle(v.x, v.y, radius, 200, 'N');
		}
	
		
		f >> v.id;	
		lugar >>idNodo;	
	}
	f.close();
	
}


//Método que recorre los documentos vertices.txt y lugares.txt para 
//llamar a los métodos drawCircle y drawName los cuales dibujan en pantalla 
//una representación visual del edificio y el nombre del mismo 
//Además, almacena los datos de cada edificio en un objeto de tipo buildingStruct
void drawNodes(){

	string verticeFile="";
	string lugarFile="";
	if(selectedMap=="mataRedonda"){
		verticeFile="verticesMataRedonda.txt";
		lugarFile="lugaresMataRedonda.txt";	
	}	
	
	if(selectedMap=="escazu"){
		verticeFile="verticesEscazu.txt";
		lugarFile="lugaresEscazu.txt";	
	}
	
	ifstream f (verticeFile);
	ifstream lugar (lugarFile);
	
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
        building[v.id].name=nombre;
        
        if(rotated){
        	v.x=1200-v.x;
        	v.y=(800-v.y)+375;
		}
        
        building[v.id].x=v.x;
        building[v.id].y=v.y;
     
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

//Método que recorre los documentos distancia.txt y calles.txt para 
//llamar a los métodos drawLine y streetNameParser los cuales dibujan en pantalla 
//la calle  y el nombre de la misma 
//Además, almacena los datos de cada calle en un objeto de tipo streetStruct
void drawNodesLines(){
	string distanciaFile="";
	string calleFile="";
	if(selectedMap=="mataRedonda"){
		distanciaFile="distanciaMataRedonda.txt";
		calleFile="callesMataRedonda.txt";	
	}	
	
	if(selectedMap=="escazu"){
		distanciaFile="distanciaEscazu.txt";
		calleFile="callesEscazu.txt";	
	}
	
	
	ifstream f (distanciaFile);
	ifstream calle(calleFile);
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
		street[r.key].name=nombre;
		street[r.key].A=r.a;
		street[r.key].B=r.b;
		int xText = abs(((nodes[r.b].xCoordinate-nodes[r.a].xCoordinate)/2)+nodes[r.a].xCoordinate);
		int yText = abs(((nodes[r.b].yCoordinate-nodes[r.a].yCoordinate)/2)+nodes[r.a].yCoordinate)-35;
		
		yText=yText+40;
		xText=xText-65;
		streetNameParser(nombre, xText, yText);
	
		drawLine(r.a,r.b,r.distancia,'N');
			
		f >> r.key;
		calle>>idNodo;
	}
	f.close();
	calle.close();	
}


//Método para inicializar dijkstra
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


//Imprime el texto de las instrucciones de viaje y le da formato a este(elimina los underscore en caso de que existan)
//según el tipo utiliza un font distinto
//I:Para las instrucciones 
//T:Para el titulo
//D:Para la línea que muestra la distancia
 void printRouteInstructions(string text, int x, int y, char textType){
 	
 	if(darkMode){
 		glColor3f(1.0, 1.0, 1.0);
	 }else{
	 	//glColor3f(0.3, 0.3, 0.3);
	 	glColor3f(0.965f, 0.973f, 0.976f);
	 }

  	glRasterPos2i(x, y);
  
  	
	size_t ubicacionGuion=text.find("_");
	
	while(ubicacionGuion!=string::npos){
		
		text.replace(ubicacionGuion,1," ");
		
		ubicacionGuion=text.find("_");
	}

  	for (const char c : text){
  	
	  	if(textType=='I'){
	  		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)c);	
		}
		
		if(textType=='T'){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)c);	
		}
		
		if(textType=='D'){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)c);
		}
	
	  }
  	
     
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
 
    glEnd();
	glFlush();
  }


//Compara las coordenadas de 2 edificios y retorna la ubicación del edificio B(puntoB) respecto al edificio A 
string getDirection(int puntoA, int puntoB){
	
	int puntoAX=building[puntoA].x;
	int puntoAY=building[puntoA].y;
	int puntoBX=building[puntoB].x;
	int puntoBY=building[puntoB].y;
	
	string direccion="";
	
	if((puntoBX>puntoAX) && (puntoBY==puntoAY)){
		direccion="este";
	}
	
	if((puntoBX<puntoAX) && (puntoBY==puntoAY)){
		direccion="oeste";
	}
	
	if((puntoBX==puntoAX) && (puntoBY>puntoAY)){
		direccion="norte";
	}
	
	if((puntoBX==puntoAX) && (puntoBY<puntoAY)){
		direccion="sur";
	}
	
	if((puntoBX>puntoAX) && (puntoBY<puntoAY)){
		direccion="sureste";
	}
	
	if((puntoBX<puntoAX) && (puntoBY<puntoAY)){
		direccion="suroeste";
	}
	
	if((puntoBX>puntoAX) && (puntoBY>puntoAY)){
		direccion="noreste";
	}
	
	if((puntoBX<puntoAX) && (puntoBY>puntoAY)){
		direccion="noroeste";
	}
	
	return direccion;
}


void drawInstructions(){
	
		string titulo="Ruta para llegar a "+ building[destinationNode.id].name;

		printRouteInstructions(titulo,275,365,'T');

		string distancia="Distancia: "+ to_string(dist[destinationNode.id])+"00m";
		
		printRouteInstructions(distancia,275,340,'D');
		
		
		string duracionStr="";
		//Asumiendo una velocidad de 65m/min 		
		double duracion=dist[destinationNode.id]/0.65;
		stringstream ss;
		ss <<fixed << setprecision(2) << duracion;
		duracionStr = ss.str();

	
		
		string duracionEstimada="Duracion estimada: "+duracionStr+"min"; 
		printRouteInstructions(duracionEstimada,275,315,'D');
		
		cout<<destinationNode.id;
		int parnode= parent[destinationNode.id];
		int ruta[20];
		ruta[0]=destinationNode.id;
		int i=1;
		
		//streetA, streetB, streetName
		
		while(parnode!=sourceNode.id){
			ruta[i]=parnode;
			parnode=parent[parnode];
			i++;
		}
		cout<<endl;
		
		int puntoARuta=sourceNode.id;
		int puntoBRuta;
		int distanciaNodos;
		string instruccion;
		int ubicacionYInstruccion=290;
		string distanciaInstruccion;
		string direccion="";
		for(int j=i-1;j>=0;j--){
			puntoBRuta=ruta[j];
		
			
			for(int k=0;k<50;k++){
				if((street[k].A==puntoARuta && street[k].B==puntoBRuta) || (street[k].B==puntoARuta && street[k].A==puntoBRuta)){
					distanciaNodos=(dist[puntoBRuta]-dist[puntoARuta])*100;
					distanciaInstruccion=to_string(distanciaNodos);
					
					direccion=getDirection(puntoARuta,puntoBRuta);
					drawLine(puntoARuta,puntoBRuta,distanciaNodos/100,'R');
					instruccion="Camine " +distanciaInstruccion+"m al "+direccion+" hasta "+ building[puntoBRuta].name+" sobre " + street[k].name;
					
						printRouteInstructions(instruccion,275,ubicacionYInstruccion,'I');
						ubicacionYInstruccion=ubicacionYInstruccion-25;
						usleep(100000);
				}
			}
		
			puntoARuta=ruta[j];
			
		}
		
		redrawNodes();
		cout<<"distancia:"<<dist[destinationNode.id]<<endl;
		
}


void adjustMatrix(){
//Hace ajustes a la matriz de adyacencia generada para asegurar el buen funcionamiento de dijkstra 
	for(int i=0;i<20;i++){
		for(int j=0;j<20;j++){
			if(adjMatrix[i][j]==0 && i!=j){
				adjMatrix[i][j]=999;
			}
		}
			
	}

}


//Imprime la matriz de adyacencia(SOLO PARA USOS DE DEBUG)
void verAdjMatrix(){
	for(int i=0;i<20;i++){
		for(int j=0;j<20;j++){
			cout << adjMatrix[i][j] << " ";
		}
			cout << endl;
	}
}


//Devuelve las variables globales a sus valores originales 
//Es ejecutado al presionar el botón reset 
void resetToDefault(){
	
	if(darkMode){
		glClearColor(0.098f,0.145f,0.208f,1.0f);
	}else{
		glClearColor(0.965f, 0.973f, 0.976f, 1.0f );
	
	}
	//
	//glClearColor(0.098f,0.102f,0.11f,1.0f);

//
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
drawInfoBackground();
drawSearchIcon();
drawMataRedondaButton();
drawToggleDarkButton();
drawEscazuButton();
drawRotateButton();
drawDivider(0,400,1200,400);
drawDivider(250,400,250,0);
	V=0;

	for(int j=0;j<100;j++){
		dist[j]=0;
		visited[j]=false;
		parent[j]=0;
	}
	
	for(int i=0;i<50;i++){
		street[i].name="";
		building[i].name="";
		street[i].A=0;
		street[i].B=0;
		building[i].x=0;
		building[i].y=0;
	}
	

	nodeCount=-1;

	for(int i=0;i<20;i++){
		for(int j=0;j<20;j++){
		adjMatrix[i][j]=0;
		}
	}
	
	
	for(int i=0;i<20;i++){
		nodes[i].id=0;
		nodes[i].xCoordinate=0;
		nodes[i].yCoordinate=0;
	}
	
	lineNodes[0].id=0;
	lineNodes[0].xCoordinate=0;
	lineNodes[0].yCoordinate=0;
	
	lineNodes[1].id=0;
	lineNodes[1].xCoordinate=0;
	lineNodes[1].yCoordinate=0;
	
	sourceNode.id=0;
	sourceNode.xCoordinate=0;
	sourceNode.yCoordinate=0;
	
	destinationNode.id=0;
	destinationNode.xCoordinate=0;
	destinationNode.yCoordinate=0;

	
	lineNodesCount = 0;
	
	sourceNodeCreated = 0;
	
	destinationNodeCreated = 0;



  	drawNodes();
  	
	drawNodesLines();

	redrawNodes();

	adjustMatrix();
	
	//verAdjMatrix();
}


void toggleDarkMode(){
	darkMode=!darkMode;
	resetToDefault();
}

void toggleRotate(){
	rotated=!rotated;
	resetToDefault();
}

void checkIfDarkTogglePressed(int x,int y){
	
	if(x<=(darkToggleX1+darkToggleRadius) && x>=(darkToggleX1-darkToggleRadius) && y<=(darkToggleY1+darkToggleRadius) && y>=(darkToggleY1-darkToggleRadius)){
			toggleDarkMode();
		}	

}

void checkIfRotatePressed(int x,int y){
	
	if(x<=(rotateX1+rotateRadius) && x>=(rotateX1-rotateRadius) && y<=(rotateY1+rotateRadius) && y>=(rotateY1-rotateRadius)){
			toggleRotate();
		}
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



//revisa si el click realizado fue en una coordenada que se encuentra dentro del área del botón reset 
void checkIfResetPressed(int x,int y)
{
	if(x>resetX1 && x<resetX2 && y>resetY1 && y<resetY2){
		selectedMap="mataRedonda";
		resetToDefault();
	}

}

void checkIfEscazuPressed(int x,int y){
	
	if(x>escazuX1 && x<escazuX2 && y>escazuY1 && y<escazuY2){
		selectedMap="escazu";
		resetToDefault();
	}
}

//Seleccionar Inicio y Destino con el Mouse
void clickMouse(int btn, int state, int x, int y)
{
	
	//correccion en el sistema de coordenadas para poder verificar la seleccion de nodos
	y = height-y;
	
	
	if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
			checkIfResetPressed(x,y);
			checkIfDarkTogglePressed(x,y);
			checkIfRotatePressed(x,y);
			checkIfEscazuPressed(x,y);
	}
	
	
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
						init();
   						dijkstra();
   						drawInstructions();
   					
					} 
				}

			}
		}
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

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Tico Maps");
    myInit();
    glutMouseFunc(clickMouse);
	resetToDefault();

    glutMainLoop();

    return 0;
}

