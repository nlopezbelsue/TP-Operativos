#ifndef TLB_H_
#define TLB_H_

#include "main.h"

typedef struct{

	uint32_t pid;
	uint32_t numPag;
    uint32_t marco;

}entradaTLB;

int buscarMarcoEnTLB(uint32_t pidBuscar, uint32_t numPagBuscar);
bool coincidePidYPag(entradaTLB* unaEntrada);
void agregarEntradaTLB(uint32_t pidAgregar, uint32_t numPagAgregar, uint32_t marcoAgregar);

#endif 