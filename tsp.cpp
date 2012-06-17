#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <memory.h>
#include "mpi.h"
#include "list.cpp"
#include "tsp.h"

// Representa el número asignado a cada proceso.
int processRank;
// Representa el total de procesos usados.
int NumberOfProcesses;
// Representa el número de ciudades en la ruta.
int NumberOfCities;

// Permite almacenar la matriz de adyacencia como un arreglo de enteros
int *Distance;

// Constructor de la clase Path
Path::Path()
{ 
  length = 0; 
  visited = 1;
  for(int i = 0; i < NumberOfCities; i++)
     city[i] = i;
}

// Permite establecer una ruta consistente de un largo, las ciudades y un indicador para saber si ha sido visitada.
void Path::Set(int Length, int *City, int Visited)
{
  length = Length;
  memcpy(city, City, NumberOfCities * sizeof(int));
  visited = Visited;
}

// Permite imprimir por pantalla las ciudades de la ruta y el largo de dicha ruta.
void Path::Print()
{
  for (int i = 0; i < visited; i++) 
     printf("  %d", city[i]);
  printf("; longitud = %d\n", length);
}

// Permite leer la matriz de adyacencia como un arreglo de enteros
void FillDistance(char *argv[])
{
  // El archivo de entrada de la matriz de adyacencia.	
  FILE *inputFile = fopen(argv[2],"r");  
  // Si el proceso es el coordinador, entonces almacena el número de ciudades en la ruta
  if (processRank == 0)    
     NumberOfCities = atoi(argv[1]);
  // Se envia a todos los demas procesos el número de ciudades. 
  MPI_Bcast(&NumberOfCities, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Revisar si el número de ciudades es menor que el máximo permitido
  assert(NumberOfCities <= MAXCITIES);
  
  // Se crea la matriz de adyacencia, como un arreglo de enteros.
  Distance = new int[NumberOfCities * NumberOfCities];
  
  // Si es el proceso administrador, se lee la matriz de adyacencia
  if (processRank == 0)
     for(int i = 0; i < NumberOfCities; i++)
        for(int j = 0 ; j < NumberOfCities ; j++ )
           //scanf("%d", &Distance[i * NumberOfCities + j]);
           fscanf(inputFile,"%d", &Distance[i * NumberOfCities + j]);
  
  fclose(inputFile);
  // Envia la matriz de adyancencia, numero de elementos, tipo de elementos, la raiz del broadcast, el communicator
  MPI_Bcast(Distance, NumberOfCities * NumberOfCities, MPI_INT, 0, MPI_COMM_WORLD);        
  // Si es el proceso administrador, imprimo la información
  if (processRank == 0)        
  {
     // El número de ciudades	  
     printf("Numero de Ciudades: %d\n", NumberOfCities);
     // La matriz de adyancencia
     for(int i = 0; i < NumberOfCities; i++)
     {
        for(int j=0; j < NumberOfCities; j++)
           printf("%5d", Distance[i * NumberOfCities + j]);
        printf("\n");
     }
  }
}

// Implementacion el proceso administrador
void Coordinator()
{
  // Permite guardar el estado.	
  MPI_Status status;
  // Permite almacenar el mensaje a ser intercambiado.
  Message message;

  // Permite mantener registro de todos los procesos esperando por una ruta.
  int* waiting = new int[NumberOfProcesses];
  // Permite almacenar la cantidad de procesos esperando.  
  int numberOfWaitingProcesses = 0;	         
  // Cuenta el número de mejores rutas recibidas.
  int numberOfBestPath = 0;                        
                                     
  // Permite almacenar la ruta más corta encontrada hasta ahora.
  Path Shortest;
  // Permite almacenar las tareas a ser asignadas en una cola.
  List queue;
  // Instanciamos una nueva ruta.
  Path *P = new Path;    
  // Inserta la primera ruta, la de largo 0.
  queue.Insert(P, 0);	       
                               
  // La ruta inicial debe ser mala.
  Shortest.length = INT_MAX;   
  // Mientras los procesos que esperan por asignacion de algun ruta sean menores a la cantidad total menos 1
  while (numberOfWaitingProcesses < NumberOfProcesses - 1) 
  {
    // Se reciben los mensajes enviados por los trabajadores	  
    MPI_Recv (&message, MessageSize, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 

    // Se revisa el status
    switch (status.MPI_TAG) 
    {
      // Si es una ruta mejor la que se envia	    
      case BestPathTag:
	// Si el largo de la ruta es menor que el de la menor encontrada hasta ahora      
	if (message.length < Shortest.length)
	{
           numberOfBestPath++;
           printf("Mejor Ruta encontrada %d, origen = %d, largo = %d\n", 
                numberOfBestPath, status.MPI_SOURCE, message.length);
           
	   // actualizar la ruta mas corta
           Shortest.Set(message.length, message.city, NumberOfCities);
	   // Le envio la nueva ruta a los trabajadores
           for(int i = 1; i < NumberOfProcesses; i++)
              MPI_Send(&(Shortest.length), 1, MPI_INT, i, UpdateBestPathTag, MPI_COMM_WORLD );
        }
        break;
      // Si hay que insertar una ruta en la cola de tareas
      case PutPathTag:
	// Si hay procesos esperando por tareas
	if (numberOfWaitingProcesses > 0) 
	{
	  // no se pone la ruta en la cola, se envia al proceso en espera
	  MPI_Send (&message, MessageSize, MPI_INT, waiting[--numberOfWaitingProcesses], ReplyPathTag, MPI_COMM_WORLD);
	} 
	else // si no hay procesos en espera de tareas
	{
	  // sea crea una nueva ruta
	  P = new Path();
	  // se configuran sus valores
	  P->Set(message.length, message.city, message.visited);
	  // se inserta en la cola de tareas
	  queue.Insert(P, message.length);
	}
	break;
      // Si hay que asignar una tarea.
      case GetPathTag:
	// si aun hay rutas en la cola de tareas.
	if (!queue.IsEmpty()) 
	{
	  // se obtiene la ruta y se la envia con la mejor longitud
	  P = (Path *)queue.Remove(NULL); 
	  message.length = P->length;
	  memcpy(message.city, P->city, MAXCITIES*sizeof(int));
	  message.visited = P->visited;
	  MPI_Send (&message, MessageSize, MPI_INT, status.MPI_SOURCE, ReplyPathTag, MPI_COMM_WORLD);
	  delete P;
	}
	else 
	{
	  // El proceso que pidio la ruta debe esperar
	  waiting[numberOfWaitingProcesses++] = status.MPI_SOURCE;
	  if (numberOfWaitingProcesses == NumberOfProcesses - 1)
	    // se les comunica a los demás procesos que se terminó 
	    for (int i = 1;i < NumberOfProcesses; i++)
	      MPI_Send (NULL, 0, MPI_INT, i, DoneTag, MPI_COMM_WORLD);
	}
 	break;
    }
  }
  // Se imprime la ruta más corta.
  printf("La Ruta Mas Corta Es : \n");
  Shortest.Print();
}


// Implementacion del procesor trabajador.
void Worker ()
{ // Permite almacenar al estado.
  MPI_Status status;
  // Permite almacenar el mensaje a ser intercambiado.
  Message message;
  // Permite almacenar el valor de la ruta mas corta encontrada hasta ahora.
  int shortestLength = INT_MAX;

  
  MPI_Send (NULL, 0, MPI_INT, 0, GetPathTag, MPI_COMM_WORLD);

  while (1) 
  {
    MPI_Recv (&message, MessageSize, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // El estado es de terminar la ejecución.
    if (status.MPI_TAG == DoneTag) 
    {
       break; // salimos de la ejecución.
    }
    // El estado corresponde a una actualizacion de la mejor ruta encontrada.
    if (status.MPI_TAG == UpdateBestPathTag)
    {
       // Se actualiza el valor de la mejor ruta	    
       shortestLength = message.length;    
       continue;
    }

    message.visited++;
    if (message.visited == NumberOfCities) 
    {
      int d1 = Distance[(message.city[NumberOfCities - 2]) * NumberOfCities + message.city[NumberOfCities - 1]];
      int d2 = Distance[(message.city[NumberOfCities - 1]) * NumberOfCities];
      if (d1 * d2)    // ambas aristas existen
      {
	 // Actualizamos el valor de la ruta hasta ahora.     
         message.length += d1 + d2;
      
         // si la ruta es mas corta que la encontrada hasta ahora, se la enviamos el administrador
         if (message.length < shortestLength)
	    MPI_Send(&message, MessageSize, MPI_INT, 0, BestPathTag, MPI_COMM_WORLD);
      }
      // no es valida, pedimos otra ruta parcial
    }
    else 
    {
      // Para cada ciudad no visitada aun, extendemos la ruta:
      int length = message.length;
      for (int i = message.visited - 1; i < NumberOfCities; i++) 
      {

	// intercambiamos las ciudades identificadas por i y visited-1.
	if (i > message.visited - 1)
	{
	   int tmp = message.city[message.visited - 1];
	   message.city[message.visited - 1] = message.city[i];
	   message.city[i] = tmp;
	}
      
	// visitamos la ciudad identificada por visited-1
	if (int d = Distance[(message.city[message.visited - 2]) * NumberOfCities + message.city[message.visited - 1]])
	  {
	    message.length = length + d;
	    if (message.length < shortestLength)
	      MPI_Send(&message, MessageSize, MPI_INT, 0, PutPathTag, MPI_COMM_WORLD);
	  }
      }
    }
    MPI_Send(NULL, 0, MPI_INT, 0, GetPathTag, MPI_COMM_WORLD);
  }
}

// Programa principal
int main(int argc, char *argv[])
{
  MPI_Init (&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
  MPI_Comm_size(MPI_COMM_WORLD, &NumberOfProcesses);

  if (NumberOfProcesses < 4) {
    printf("Al menos 4 procesos son necesarios\n");
    exit(-1);
  }  

  FillDistance(argv);

  if (processRank == 0) 
    Coordinator();
  else
    Worker();
  
  MPI_Finalize();
  return 0;
}

