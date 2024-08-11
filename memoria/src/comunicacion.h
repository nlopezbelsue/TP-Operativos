#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/utilidades.h>
#include "pthread.h"

#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "main.h"
#include "esquema.h"
#include "memoriaInstrucciones.h"

extern t_config *config;

extern int memoria_fd;
extern int cpu_fd;
extern int interfazIO_fd[4];
extern int kernel_fd;
extern t_log* info_logger;
extern int RETARDO_RESPUESTA;
extern pthread_mutex_t mutexFS;

int recibirConexion(char *);
void *recibirCPU(void);
void* recibirIO (void* interfaz_fd);
void *recibirKernel();
void cualInterfaz(int tipoInterfaz);

void inicializarProceso(int);
void GuardarNombreArchiv(uint32_t pid, char* file_name);
void* manejarLectura(uint32_t posInicial, uint32_t tamanio, uint32_t pid);
void manejarEscritura(uint32_t posInicial, void* datos, uint32_t pid,  uint32_t tamanio);
void realizarPedidoLectura(int cliente_socket);     //Vale para io. Le manda LECTURA_REALIZADA
void realizarPedidoEscritura(int cliente_socket);   //Vale para io y cpu. Les manda ESCRITURA_REALIZADA
void realizarPedidoLecturaCpu();
uint32_t manejarLecturaCpu(uint32_t posInicial, uint32_t tamanio, uint32_t pid);

void finalizarProceso(int);
t_log* iniciar_logger(char*);
// void Paquete(int, t_log*);


// int conectarModulo(char *);
// void leer_consola(t_log*);
void PaqueteHand(int, t_log*);
// void terminar_programa(int, t_log*);


#endif