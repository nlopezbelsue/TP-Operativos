#include "comunicacion.h"
#include "pthread.h"
#include <commons/collections/queue.h>
#include<commons/config.h>

extern t_config* config;

extern int TAM_PAGINA;
extern int TAM_MEMORIA;
extern int CANT_MARCOS;
extern int RETARDO_RESPUESTA;

extern uint32_t *marcosAsignados;

extern void* espacio_contiguo;

extern bool memoriaInicializada;
extern bool tablaPaginasCreada;
extern bool pagina0Creada;
extern bool semaforosCreados;

extern t_list* tablasPaginas;
extern t_list* tablaGeneral;

extern pthread_mutex_t mutex_espacioContiguo;
extern pthread_mutex_t mutex_tablasPaginas;
extern pthread_mutex_t mutex_idPagina;
extern pthread_mutex_t mutex_espacioDisponible;
extern pthread_mutex_t mutexFS;

extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_list* instruccionesDeProcesos;

extern char* PATH_INSTRUCCIONES;

void *recibir();
void *conectarFS();
void *recibir();
t_config *crearConfig(char* configPath);
void sigint_handler(int sig);