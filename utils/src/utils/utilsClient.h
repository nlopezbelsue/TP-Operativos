#ifndef UTILSCLIENT_H_
#define UTILSCLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <string.h>
#include "pthread.h"

typedef enum
{	MENSAJECLIENTE,
	PAQUETECLIENTE,
	CONTEXTOEJECUCION,
	SOLICITUDINSTRUCCION,
	IO_GEN_SLEEP,
	WAIT,
	SIGNAL,
	F_OPEN,
	F_TRUNCATE,
	F_SEEK,
	F_WRITE,
	F_READ,
	F_CLOSE,
	EXIT,
	SOLICITUDMARCO,
	ACCESO_PEDIDO_LECTURA,
	ACCESO_PEDIDO_LECTURA_UINT,
	LECTURA_REALIZADA,
	LECTURA_REALIZADA_UINT,
	ACCESO_PEDIDO_ESCRITURA,
	ESCRITURA_REALIZADA,
	RESIZE,
	HANDSHAKE_CPU,
	INTERRUPCIONCPU,
	DESALOJOCPU,
    APERTURA_ARCHIVO_EXITOSA,
    APERTURA_ARCHIVO_FALLIDA,
	ESCRITURA_ARCHIVO_EXITOSA,
	TRUNCACION_ARCHIVO_EXITOSA,
	LECTURA_ARCHIVO_EXITOSA,
    CREACION_ARCHIVO_EXITOSA,
	SLEEPCPU,
	CARGA_PAGINA,
	INICIALIZAR_PROCESO_MEMORIA,
	FINALIZAR_PROCESO_MEMORIA,
	APERTURA_ARCHIVO,
	TRUNCACION_ARCHIVO,
	LECTURA_ARCHIVO,
	ESCRITURA_ARCHIVO,
	ESTRUCTURAS_INICIALIZADAS,
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	CREACION_ARCHIVO,
	LECTURA_ARCHIVO_POSIBLE,
	ESCRITURA_ARCHIVO_POSIBLE,
	PEDIR_SWAP,
	DEVOLVER_SWAP,
	ESCRITURA_BLOQUE_SWAP,
	LECTURA_BLOQUE_SWAP,
	IO_GEN_SLEEP_OPC,
	IO_STDIN_READ,
	IO_STDOUT_WRITE,
	IO_FS_CREATE,
	IO_FS_DELETE,
	IO_FS_TRUNCATE,
	IO_FS_WRITE,
	IO_FS_READ,
	IO_INTERFAZ_ERRONEA,
	IO_STDIN_READ_DONE,
	IO_STDOUT_WRITE_LEER_DIRECCION_EN_MEMORIA,
	IO_STDOUT_WRITE_LECTURA_EXITOSA,
	SOLICITUD_IO_CUMPLIDA,
	OUT_OF_MEMORY,
	LISTA_DE_ARCHIVOS
}op_code_cliente;

typedef struct
{	int size;
	void* stream;
} t_buffer;

typedef struct
{	op_code_cliente codigo_operacion;
	t_buffer* buffer;
} t_paquete;

int crear_conexion(t_log* logger, const char* server_name, char* ip, char* puerto);
void enviar_mensaje(char*, int);
t_paquete* crear_paquete(op_code_cliente, t_log*);
void agregar_a_paquete(t_paquete*, void*, int);
void agregar_a_paquete2(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete*, int);
void liberar_conexion(int);
void eliminar_paquete(t_paquete*);

#endif /* UTILSCLIENT_H_ */
