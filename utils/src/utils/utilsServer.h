#ifndef UTILSSERVER_H_
#define UTILSSERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>

typedef enum{
	MENSAJESERVIDOR,
	PAQUETESERVIDOR
}op_code_server;

void* recibir_buffer(int*, int);
void* recibir_buffer2(int* size, int socket_cliente);


int iniciar_servidor(t_log* logger, const char* name, char* puerto);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);


void simularRetardoSinMensaje(int); //REVISAR
#endif /* UTILSSERVER_H_ */
