#ifndef CICLOINSTRUCCION_H_
#define CICLOINSTRUCCION_H_

#include "main.h"

extern bool cicloInstrucciones;
extern int interrupciones;
extern int desalojo;

extern uint8_t registroCPU_AX;
extern uint8_t registroCPU_BX;
extern uint8_t registroCPU_CX;
extern uint8_t registroCPU_DX;
extern uint32_t registroCPU_EAX;
extern uint32_t registroCPU_EBX;
extern uint32_t registroCPU_ECX;
extern uint32_t registroCPU_EDX;
extern uint32_t registroCPU_SI;
extern uint32_t registroCPU_DI;

void ciclo_de_instruccion();
void fetch();
void decode();
void execute();
void checkInsterrupt();

void copiar_registroPCB_a_los_registrosCPU (RegistrosCPU* registro);
void copiar_registrosCPU_a_los_registroPCB(RegistrosCPU* registro);

#endif