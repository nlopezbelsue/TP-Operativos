#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/estructurasCompartidas.h>
#include "pthread.h"
#include "main.h"

extern PCB* PCB_Actual;
extern int kernel_fd;
extern int kernel_interrupt_fd;
extern int memoria_fd;
extern int tam_pagina;

int conectarModulo(char *);
t_log* iniciar_logger(char*);
void paquete(int, t_log*);
void terminar_programa(int, t_log*);
void recibirConexion(char *);
void recibirConexionInterrupt(char *);
void iterator(char* value);

#endif