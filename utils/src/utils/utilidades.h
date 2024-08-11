#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include "estructurasCompartidas.h"
#include "utilsServer.h"

extern int idProcesoGlobal;

Instruccion* recibirInstruccion(int conexion);
void liberarInstruccion(Instruccion * instruccion);
bool agregarUint32_tsAPaquete(t_list*, t_paquete*);
bool enviarListaUint32_t(t_list*, int, t_log*, op_code_cliente);

void agregar_registros_a_paquete(t_paquete* paquete, RegistrosCPU* registro);
void agregar_ContextoEjecucion_a_paquete(t_paquete *paquete, PCB* pcb);
PCB* recibir_contextoEjecucion(int conexion);
void liberarPcbCpu(PCB* pcb);
void enviar_uint32_y_uint32_y_char(char* path, uint32_t valor1, uint32_t valor2, int socket, op_code_cliente orden, t_log *logger);
void recibirOrden(int socket);
void enviarOrden(op_code_cliente orden, int socket, t_log *logger);

void enviarValor_uint32(uint32_t valor, int socket, op_code_cliente orden, t_log *logger);
void* recibir_stream(int* size, uint32_t cliente_socket);
PCB* recibir_contextoEjecucion_y_char(int conexion);
PCB* recibir_contextoEjecucion_y_uint32(int conexion, uint32_t* direccion);
PCB* recibir_contextoEjecucion_y_uint32_y_uint32(int conexion, uint32_t* direccion, uint32_t* direccion2);
PCB* recibir_contextoEjecucion_y_uint32_y_uint32_y_uint32(int conexion, uint32_t* direccion, uint32_t* direccion2, uint32_t* direccion3);

void* recibir_stream(int* size, uint32_t cliente_socket);

t_list* recibirListaUint32_t(int socket_cliente);
uint32_t recibirValor_uint32(int socket);
void* recibir_stream(int* size, uint32_t cliente_socket);
bool enviarEnteroYString(uint32_t entero,char* string, int socket_cliente, t_log* logger, op_code_cliente codigo);
bool agregarEnteroYStringAPaquete(uint32_t entero, char* string, t_paquete* paquete);
char* recibirEnteroYString(int socket_cliente,uint32_t* entero);

void simularRetardoSinMensaje(int);
void agregar_instruccion_a_paquete(t_paquete *paquete, Instruccion* instruccion); //VER porque esta fea... es nueva
bool agregarIntsYDatosAPaquete(t_list* listaInts, t_datos* datos, t_paquete* paquete);
bool enviarListaIntsYDatos(t_list* listaInts,t_datos* datos, int socket_cliente, t_log* logger, op_code_cliente codigo);
t_list* recibirListaIntsYDatos(int socket_cliente,t_datos* datos);
char* recibirEnteroEnteroChar(int socket_cliente, uint32_t* entero1, uint32_t* entero2);
PCB* recibir_contextoEjecucion_y_char_y_uint32(int conexion, uint32_t* numero);
PCB* recibir_contextoEjecucion_y_char_y_uint32_y_uint32(int conexion, uint32_t* numero1, uint32_t* numero2);
PCB* recibir_contextoEjecucion_y_char_y_uint32_y_uint32_y_uint32_y_uint32(int conexion, uint32_t* numero1, uint32_t* numero2, uint32_t* numero3, uint32_t* numero4);
void enviar_uint32_y_uint32_y_uint32_y_uint32_y_char(char* path, uint32_t valor1, uint32_t valor2, uint32_t valor3, uint32_t valor4, int socket, op_code_cliente orden, t_log *logger);
char* recibirEnteroEnteroEnteroEnteroChar(int socket_cliente, uint32_t* entero1, uint32_t* entero2, uint32_t* entero3, uint32_t* entero4);
void enviar_uint32_y_uint32_y_uint32_y_char(char* path, uint32_t valor1, uint32_t valor2, uint32_t valor3, int socket, op_code_cliente orden, t_log *logger);
char* recibirEnteroEnteroEnteroChar(int socket_cliente, uint32_t* entero1, uint32_t* entero2, uint32_t* entero3);

bool enviarListaString(t_list* listaStrings, int socket_cliente, t_log* logger, op_code_cliente codigo);
bool agregarStringsAPaquete(t_list* listaStrings, t_paquete* paquete);
char* recibirString(int socket_cliente);
t_list* recibirListaString(int socket_cliente);

#endif