// Representa el numero maximo de ciudades en la ruta.
const unsigned int MAXCITIES = 1000;

// Clase que permite representar una ruta.
// Atributos:
// int length: el largo de la ruta.
// int city[] : el arreglo de ciudades.
// int visited : las ciudades visitadas.
//
// Metodos:
// Path(): Constructor de la clase.
// void Set(int Lenght, int *city, int Visited): Permite establecer el valor de una ruta.
// void Print(): Permite imprimir la ruta.
class Path {
 public:
  int length;
  int city[MAXCITIES];
  int visited;
  Path();
  void Set(int Length, int *City, int Visited); 
  void Print();
};

// Enumeration para los tags
enum
{
    // Tag para poner una ruta.	
    PutPathTag,
    // Tag para la mejor ruta.
    BestPathTag, 
    // Tag para obtener una ruta.
    GetPathTag, 
    // Tag para actualizar la mejor ruta.
    UpdateBestPathTag, 
    // Tag para indicar que el proceso terminó
    DoneTag, 
    // Tag para enviar una tarea.
    ReplyPathTag
};

// Permite representar un mensaje.
// Cada mensaje está compuesto por:
// length: el largo de la ruta.
// city : el arreglo de ciudades.
// visited: el número de ciudades visitadas.
struct Message{
  int length;
  int city[MAXCITIES];
  int visited;
};

// Permite calcular el tamaño del mensaje.
const unsigned int MessageSize = sizeof(Message)/sizeof(int);
