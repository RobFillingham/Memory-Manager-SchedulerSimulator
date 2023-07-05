#include <iostream>
#include<ctime>
#include<cstdlib>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stack>
#define KEY_LEFT 75
#define KEY_RIGHT 77

void gotoxy(int x,int y){  //Debemos agregar esta funcion
  HANDLE hcon;  
  hcon = GetStdHandle(STD_OUTPUT_HANDLE);  
  COORD dwPos;  
  dwPos.X = x;  
  dwPos.Y= y;  
  SetConsoleCursorPosition(hcon,dwPos);  
}

using namespace std;

const int TAM=8192; //Tama?o de la memoria


int auxid=4500; //variable que asigna ID's
int NOP=0; //Cuenta de procesos
float velocidad=2000;
int Mminimo=0; //minimo nodo memoria
int nProcesosTerminados=0; //cuenta de procesos terminados
long long cantidadCiclos=0; //cuenta de ciclos 

int cantidadNodos=0;   //cantidad de nodos a los que se les evauo su fragmentacion interna
long long sumaFragmentaciones=0;  //suma de la fragmentacion interna en cada nodo evaluado
double fragmentacionInterna=0;
int inserciones=0;


class Proceso{
    private:
        int id;
        int size;
        int cuanto;
        int vuelta;
    	friend class Arbol;
        friend void ejecucionProceso();
    public:
        Proceso(int cuantoMax, int maxPsize)
        {
            id=auxid;
            this->size=1+rand()%maxPsize;
            this->cuanto=4+rand()%cuantoMax;
            this->vuelta=0;

        }

        int getid()
        {
            return id;
        }
        int getsize()
        {
            return size;
        }
        int getCuanto(){
            return cuanto;
        }
        int getVuelta(){
            return vuelta;
        }

        void setVuelta(int vuelta){
            this->vuelta=vuelta;
        }

        void setCuanto(int cuanto){
            this->cuanto=cuanto;
        }
        void incrementarVuelta(){
            vuelta++;
        }
};

class ListaDoble
{
    public:
        class Nodo
        {
            public:
                Nodo *izq;
                Nodo *der;
                Proceso *proc1;
                Nodo (Proceso *proc1)
                {
                    //cout << "Entre al constructor con parametros de Nodo con " << proc1.id << endl;
                    this->proc1=proc1;
                    this->izq=NULL;
                    this->der=NULL;
                }
        };
    private:
        Nodo *inicio;
        Nodo *fin;
    public:
		ListaDoble(){inicio=fin=NULL;}

        void insertaFinal(Proceso *proc)
        {
            Nodo *nuevo;
            nuevo=new Nodo (proc);
            if (inicio==NULL)
                inicio=fin=nuevo;
            else
            {
                nuevo->izq=fin;
                fin->der=nuevo;
                fin=nuevo;
            }
        }


        void imprimir (Nodo *aux, int y)
        {
            HANDLE color=GetStdHandle(STD_OUTPUT_HANDLE);
            if (aux!=NULL)
            {	
            	
            	if(y!=17 && aux->der==NULL){
            		SetConsoleTextAttribute(color,7);
            		gotoxy(4,y);
            		cout<<"FinCola->";
            		SetConsoleTextAttribute(color,14);
				}
            	gotoxy(21, y);
                cout << aux->proc1->getid()     << "\t"
                    << aux->proc1->getsize()   << "\t  "
                    << aux->proc1->getCuanto() << "\t    "
                    << aux->proc1->getVuelta() << "\n";
                SetConsoleTextAttribute(color,14);
                imprimir(aux->der, y+1);
            }

        }

        void imprimir2 (Nodo *aux, int y)
        {
            if (aux!=NULL)
            {	
            	gotoxy(71, y);
                cout << aux->proc1->getid()     << "\t"
                    << aux->proc1->getsize()   << "\t  "
                    << aux->proc1->getCuanto() << "\t  "
                    << aux->proc1->getVuelta() << "\n";
                    
            	imprimir2(aux->der, y+1);
            }

        }

        void eliminarInicio(){
            Nodo *aux = inicio;
            inicio=inicio->der;
            aux->izq=NULL;
            delete aux;
        }


        Proceso* getFront(){
            return inicio->proc1;
        }
        Nodo *getInicio(){ return this->inicio; }

};

class Arbol{
    public:
        class Nodo{
            public:
                Nodo *izq;
                Nodo *der;
                Nodo *padre;
                Proceso *procesoEnMemoria;
                bool    validacion;
                int memorySize;

                Nodo (int memorySize){
                    this->memorySize=memorySize;
                    this->izq=NULL;
                    this->der=NULL;
                    this->padre=NULL;
                    this->procesoEnMemoria=NULL;
                    this->validacion=false;
                }
        };
    private:
        Nodo *raiz;
    public:
        Arbol(int memorySize){
            this->raiz=new Nodo(memorySize);
        }

        Nodo *crearNodo (Nodo*nodo)
        {
            Nodo *aux1 = new Nodo (nodo->memorySize/2);
            Nodo *aux2 = new Nodo (nodo->memorySize/2);
            aux1->padre = nodo;
            aux2->padre = nodo;
            nodo->izq=aux1;
            nodo->der=aux2;
            return nodo->izq;
        }

        Nodo *buscar (Nodo *a, int mem)
		{
		    Nodo *aux;
		    if (a!=NULL)
            {
                stack <Nodo*> pila;
                pila.push(a);
                stack <Nodo*> pila1;
                pila1.push(a);

                while (!pila.empty())
                {
                    aux=pila.top();
                    pila.pop();
                    pila1.push(aux);

                    if (aux->izq!=NULL) pila.push(aux->izq);
                    if (aux->der!=NULL) pila.push(aux->der);
                }

                 while (!pila1.empty())
                {
                    aux=pila1.top();
                    if (aux->izq==NULL && aux->der==NULL && aux->memorySize==mem&&aux->validacion==false)
                        return aux;
                    pila1.pop();
                }
                return NULL;
            }
		}

		Nodo *buscarEliminar (Nodo *a, int id)
		{
		    Nodo *aux;
		    int id2,id3;
		    if (a!=NULL)
            {
                stack <Nodo*> pila;
                pila.push(a);
                stack <Nodo*> pila1;
                pila1.push(a);

                while (!pila.empty())
                {
                    aux=pila.top();
                    pila.pop();
                    pila1.push(aux);
                    if (aux->validacion==true)
                    {
                        id3=aux->procesoEnMemoria->getid();
                    }
                    if (aux->izq!=NULL) pila.push(aux->izq);
                    if (aux->der!=NULL) pila.push(aux->der);
                }
                 while (!pila1.empty())
                {
                    aux=pila1.top();

                    if (aux->validacion==true)
                    {
                        id2=aux->procesoEnMemoria->getid();
                        if (id2==id)
                        {
                            return aux;
                        }
                    }
                    pila1.pop();
                }
                return NULL;
            }
		}

        void minimo (Nodo *nodo, int memPro)
		{
			if(nodo==NULL)
				return;
			else
			{
				if(nodo->izq==NULL && nodo->der==NULL)
                {
                    if (nodo->memorySize<=Mminimo && nodo->memorySize>=memPro && nodo->validacion==false)
                        Mminimo = nodo->memorySize;
                }
				else
                {
                    minimo(nodo->izq,memPro);
					minimo(nodo->der,memPro);
                }
			}
		}

        Nodo *nodoInsertar (Nodo* raiz, int memPro)
        {
            Mminimo = TAM;
            minimo(raiz,memPro);
            Nodo *aux=buscar(raiz, Mminimo);
            if (aux==NULL)
            {
                return NULL;
            }
            else
            {
                while (aux->memorySize/2>memPro)
                {
                    aux = crearNodo (aux);
                    if (aux==NULL)
                        return NULL;
                }
                return aux;
            }
        }

        bool InsertarProceso(Nodo *nodo,Proceso *procesoAInsertar){
            int v=0;
            Nodo *aux=nodoInsertar(nodo,procesoAInsertar->getsize());
            if (aux!=NULL)
            {
                aux->procesoEnMemoria=procesoAInsertar;
                aux->validacion=true;
                return true;
            }
            return false;
        }

        void memoriaBuddy (Nodo *nodo) //cuenta las hojas del arbol
		{
			HANDLE color=GetStdHandle(STD_OUTPUT_HANDLE);
			if(nodo==NULL)
				return;
			else
			{
				if(nodo->izq==NULL && nodo->der==NULL)
                {

                    if (nodo->validacion==false)
                        cout<< "[ " << nodo ->memorySize << " ]";
                    else
                    {
                        cout<< "[ ";
                        SetConsoleTextAttribute(color,4);
                        cout << nodo ->memorySize<<", "<<nodo->procesoEnMemoria->id;
                        SetConsoleTextAttribute(color,15);
                        cout << " ]";
                    }
                }
				else
                {
                    memoriaBuddy(nodo->izq);
					memoriaBuddy(nodo->der);
                }
			}
		}

        bool eliminar2 (Nodo *nodo, int id)
		{
		    bool seguir = true;
		    Nodo *aux=buscarEliminar (nodo,id);
		    if (aux==NULL)
                return false;
            else
            {
                aux->validacion=false;
                while (seguir)
                    seguir = checar (nodo);
                return true;
            }
		}

		bool eliminar (Nodo *nodo, Proceso *procesoEliminar)
		{
		    Nodo *aux=buscarEliminar (nodo,procesoEliminar->getid());
            if (aux==NULL)
                return false;
            else
            {
                aux->validacion=false;
                checar (nodo);
                return true;
            }
		}

		bool checar (Nodo*nodo)
		{
		    if(nodo==NULL)
				return false;
			else
			{
                if (nodo->izq==NULL && nodo->der==NULL)
                {
                    if ((nodo->validacion==false))
                    {
                        //cout << "\nPadre es: " << nodo->padre->memorySize;
                        if (nodo->padre->der == nodo)
                        {
                            if (nodo->padre->izq->validacion==false && nodo->padre->izq->izq==NULL && nodo->padre->izq->der==NULL)
                            {
                                limpiar (nodo->padre);
                                return true;
                            }
                        }
                        else
                        {
                            if (nodo->padre->der->validacion==false && nodo->padre->der->izq==NULL && nodo->padre->der->der==NULL)
                            {
                                limpiar (nodo->padre);
                                return true;
                            }
                        }
                    }
                }
                else
                {
                    checar(nodo->izq);
					checar(nodo->der);
                }

			}
			return false;
		}

		void limpiar (Nodo *nodo)
		{
		    //cout << "\nLimpios-> " << nodo->der->memorySize << " & " << nodo->izq->memorySize;
		    nodo->der=NULL;
		    nodo->izq=NULL;
		}
		
		void evaluacionFragmentacion(Nodo*nodo ){
			if(nodo!=NULL){
				evaluacionFragmentacion(nodo->izq);
				if(nodo->procesoEnMemoria!=NULL && nodo->validacion){
					sumaFragmentaciones+=(nodo->memorySize-nodo->procesoEnMemoria->size);
					cantidadNodos++;
				}
				evaluacionFragmentacion(nodo->der);
				
			}
		}
		

        Nodo * getRaiz(){ return this->raiz;}
};

void ModuloPar(int *cuantoMax, int *maxPsize);
void ejecucionProceso(ListaDoble&, int, Arbol&, ListaDoble &);
void moduloEstadisticas();

int main()
{
    HANDLE color=GetStdHandle(STD_OUTPUT_HANDLE);
    Proceso *aux;
    ListaDoble ColaRR, listaProcesosTerminados;
    Arbol memoria(TAM);

    bool entroMemoria=true, program=true;
    int cuantoMax, maxPsize;

    srand(time(NULL));

    //modulo parametros
    ModuloPar(&cuantoMax, &maxPsize);
	while(program)
	{
	    while (!kbhit()){
	       system ("cls");
	       SetConsoleTextAttribute(color,9);
			
			gotoxy(45, 2);
			cout<<"REPRESENTACION DE LA MEMORIA";
			 SetConsoleTextAttribute(color,7);
			gotoxy(10, 3);
			cout<<"//Cada par de corchetes representa un bloque de la memoria, el primer numero indica el size del bloque, ";
			gotoxy(28, 4);
			SetConsoleTextAttribute(color, 12);
			cout<<"El segundo numero es el ID del proceso en el bloque!!!//";
			gotoxy(20, 5);
			SetConsoleTextAttribute(color, 7);
			cout<<"//Los bloques rojos estan ocupados por un proceso, los blancos estan libres.//";
			gotoxy(17, 8);
			SetConsoleTextAttribute(color,9);
			cout<<"|(P)Pausar (F)Finalizar |";
			gotoxy(78, 8);  
			SetConsoleTextAttribute(color,9); 
			cout<<"| (<-)-velocidad+(->) |";
			SetConsoleTextAttribute(color,7); 
			gotoxy(0, 10);
	       memoria.memoriaBuddy(memoria.getRaiz()); //imprime memoria
	
	       if(entroMemoria){
	            aux=new Proceso(cuantoMax, maxPsize);
	            auxid++;
	       }
	
	        entroMemoria=memoria.InsertarProceso(memoria.getRaiz(), aux);
	       if(entroMemoria){
		   
	            ColaRR.insertaFinal(aux);
				memoria.evaluacionFragmentacion(memoria.getRaiz());
				fragmentacionInterna+=sumaFragmentaciones/cantidadNodos;
            	inserciones++;
			}
	        gotoxy(32, 13);
	       	SetConsoleTextAttribute(color,9);       
	        cout << "COLA DE PROCESOS";
            gotoxy(30, 14);
            SetConsoleTextAttribute(color,11);
            cout<<"Cuanto del Sistema: "<<cuantoMax;
	        SetConsoleTextAttribute(color,7);
	        gotoxy(21, 16);
			cout << " ID    Size(KB)  Cuanto  Ciclos-En-CPU";
			gotoxy(0, 17);
			cout<<"ProcesoEnEjecucion->";
	        SetConsoleTextAttribute(color,10);
	        ColaRR.imprimir(ColaRR.getInicio(), 17);
	        SetConsoleTextAttribute(color,15);
	        ejecucionProceso(ColaRR, cuantoMax, memoria, listaProcesosTerminados);
			
			
	        gotoxy(79, 13);
	        SetConsoleTextAttribute(color,9);
	        cout << "COLA PROCESOS TERMINADOS";
	               	SetConsoleTextAttribute(color,7);  
			gotoxy(72, 16);
	        cout << "ID    Size(KB)  Cuanto  CuantoOriginal ";
	        SetConsoleTextAttribute(color,2);
	        listaProcesosTerminados.imprimir2(listaProcesosTerminados.getInicio(), 17);
	        gotoxy(100, 25);
	        
	        gotoxy(0,0);
	        SetConsoleTextAttribute(color,5);
	        cout<<"Velocidad: "<<velocidad/1000;
	        Sleep (velocidad);
	    }
		
		char tecla=getch();
		if(tecla=='p' or tecla=='P')
		{
			gotoxy(55,8);
			SetConsoleTextAttribute(color,4);
			cout<<" |PAUSADO| ";
			getch();
		}
			
		if(tecla==KEY_LEFT)
		{
			velocidad=velocidad-500;
			if(velocidad<=0)
				velocidad=500;
			
		}
		if(tecla==KEY_RIGHT)
		{
			velocidad=velocidad+500;
			if(velocidad>=5000)
			    velocidad=5000;
			    
		}
		if(tecla=='f' or tecla=='F')
			program=false;
			
	}
	
	moduloEstadisticas();
	
}

void moduloEstadisticas(){
    
    system("cls");
    for(int x=1; x<=75; x++)
	{
		gotoxy (x,0);
		putchar ('*');
		gotoxy (x,25);
		putchar ('*');
	}
	
	gotoxy(25,2);
    cout<<"ESTADISICAS"<<endl;
    gotoxy(6,5);
    cout<<"Numero Total de Proceos Atendidos: "<<nProcesosTerminados;
    gotoxy(6,7);
    cout<<"Cantidad de ciclos que tomo para ejecutar un proceso en promedio: "<<cantidadCiclos/nProcesosTerminados;
    gotoxy(6,9);
    cout<<"Fragmentacion Interna Promedio: "<<fragmentacionInterna/inserciones;
    
    for(int y=1; y<=25; y++)
	{
		gotoxy (0,y);
		putchar ('*');
		gotoxy (75,y);
		putchar ('*');
	}
    cout<<endl;
    system("pause");
}

void ModuloPar(int *cuantoMax, int *maxPsize)
{
	bool aux=false;

	do{
		system("cls");
		
		for(int i=0; i<300; i++){
			gotoxy(rand()%200, rand()%30);
			cout<<".";
		}
		
		gotoxy(32, 5);
		cout<<"MODULO DE CONFIGURACION DE PARAMTEROS";
		
		gotoxy(25, 8);
		cout<<"Ingresa el cuanto del sistema: ";
		cin>>*cuantoMax;
		
		gotoxy(25, 10);
		cout<<"Ingresa el tamano maximo de memoria de cada proceso (En KB y menor a 4096): ";
		cin>>*maxPsize;
		
		
	
		

		if(*maxPsize<TAM/2)
			aux=true;
		else
		{
			gotoxy(30, 11);
			cout<<endl<<"\tEscribe un tamano correcto..."; system("pause");
		}

	}while(aux==false);
	gotoxy(30, 12);
	system("pause");
	system("cls");
}

void ejecucionProceso(ListaDoble &ColaRR, int cuantoMax, Arbol &memoria, ListaDoble &listaProcesosTerminados){
    Proceso *aux=ColaRR.getFront();
    Arbol *auxmem;
    int cuantoProceso=aux->getCuanto();
    aux->incrementarVuelta();
    int vuelta = aux->getVuelta();
    cantidadCiclos++;

    if (vuelta<cuantoMax && cuantoProceso>0)
    {
        aux->setCuanto (cuantoProceso-1);
    }
    else if (cuantoProceso == 0)
    {
        ColaRR.eliminarInicio();
        memoria.eliminar2(memoria.getRaiz(), aux->getid());
        listaProcesosTerminados.insertaFinal(aux);
        nProcesosTerminados++;
    }
    else if (vuelta > cuantoMax)
    {
        aux->setVuelta(0);
        ColaRR.insertaFinal(aux);
        ColaRR.eliminarInicio();
    }
}



