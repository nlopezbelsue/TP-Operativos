#ifndef MEMORIAINSTRUCCIONES_H_
#define MEMORIAINSTRUCCIONES_H_
#include "comunicacion.h"

//#define TAM_PAGINA config_get_int_value(config,"TAM_PAGINA")

//extern void* espacio_contiguo;
//extern uint32_t espacioDisponible;
//extern bool memoriaInicializada;
//extern bool tablaPaginasCreada;
//extern bool pagina0Creada;
//extern bool semaforosCreados;
//extern t_list* tablasPaginas;
//
//extern pthread_mutex_t mutex_espacioContiguo;
//extern pthread_mutex_t mutex_tablasPaginas;
//extern pthread_mutex_t mutex_idPagina;
//extern pthread_mutex_t mutex_espacioDisponible;
//
//extern t_log* trace_logger;
//extern t_log* debug_logger;
//extern t_log* info_logger;
//extern t_log* warning_logger;
//extern t_log* error_logger;

bool inicializarBitmap(); //Crea un bitmap que indica que marcos están libres
void liberarMemory(); 
void liberarPagina(Pagina*);
bool crearSemaforos();
bool crearEspacioContiguoDeMemoria();
bool crearEstructurasAdministrativas();
bool iniciarMemoria();


void GuardarInstrucsDeProceso(uint32_t pid, char* file_name); // Guarda en una lista de procesos pid y lista de instrucciones
Instruccion* FormatearInstruccion(char* linea); //Pasa una linea de pseudocodigo a nuestro formato Instruccion
Instruccion* retornarInstruccionACPU(uint32_t pid, uint32_t pc);


//REVISAR
void liberarTablaDePaginas(uint32_t pid);      //REVISAR


/*Estructuras de Memoria de instrucciones*/
typedef struct {
    uint32_t pid;
    t_list* instrucciones;
} Proceso;

/*Esquema de memoria*/

#endif