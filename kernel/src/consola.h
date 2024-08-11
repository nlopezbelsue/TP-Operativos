#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include "main.h"

extern pthread_mutex_t semaforo2;

extern int planificadorLargoAvance;
extern int planificadorCortoAvance;

extern char* path;
extern pthread_mutex_t planificacionLargo;
extern pthread_mutex_t planificacionCorto;

PCB* encontrarProceso(uint32_t, uint32_t*);

void* inicializarProceso(void*);
void* finalizarProceso(void*);
void iniciar_proceso(char*);
void pedirProceso(char*);
void* mostrarColasPuntero();
void* detenerPlanificacion();
void* iniciarPlanificacion();
void* multiprogramacion(void*);
void* ejecutar_script_operaciones (void* parametros);
void iniciar_proceso (char* linea);
void* iniciarConsola ();
void funcionesDeLaConsola(char* linea);


#endif /* SRC_CONSOLA_H_ */