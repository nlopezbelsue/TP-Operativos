#ifndef EJECUCIONINSTRUCCIONES_H_
#define EJECUCIONINSTRUCCIONES_H_

#include "main.h"

void SET(char*, char*);
void SUM(char*, char*);
void SUB(char*, char*);
void JNZ(char*, char*);
void resize(char*);
void COPY_STRING(char*);

void ejecutar_WAIT(char* );
void ejecutar_SIGNAL(char*);

void ejecutar_MOV_IN(char*, int);
void ejecutar_MOV_OUT(int, char*);

void ioGenSleep(char* interfaz, char* unidadesDeTrabajo);
void ioStdinRead(char* interfaz, char* registroDireccion, char* registroTamanio);
void ioStdOutWrite(char* interfaz, char* registroDireccion, char* registroTamanio);
void ioFsCreate(char* interfaz, char* nombreArchivo);
void ioFsDelete(char* interfaz, char* nombreArchivo);
void ioFsTruncate(char* interfaz, char* nombreArchivo, char* registroTamanio);
void ioFsWrite(char* interfaz, char* nombreArchivo, char* registroDireccion, char* registroTamanio, char* registroPunteroArchivo);
void ioFsRead(char* interfaz, char* nombreArchivo, char* registroDireccion, char* registroTamanio, char* registroPunteroArchivo);

void ejecutar_EXIT();

uint32_t leer_valor_de_memoria(int direccion_fisica, int bytesRegistro);
char* leer_char_de_memoria(int direccion_fisica, int bytesRegistro);
void*  recibir_valor_de_memoria();
char* recibir_char_de_memoria();
void escribir_valor_en_memoria(int direccion_fisica, int cantidad_bytes, uint32_t valor);
void escribir_char_en_memoria(int direccion_fisica, int cantidad_bytes, void* valor);
char*  recibir_confirmacion_de_escritura();
int calcular_bytes_segun_registro(char* registro);

void cambiar_valor_del_registroCPU(char* registro, char* valor);
int obtener_valor_registroCPU(char* registro);
uint32_t obtenerInterfaz(char* interfaz);

#endif