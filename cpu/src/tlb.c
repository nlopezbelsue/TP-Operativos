#include "tlb.h"

int buscarMarcoEnTLB(uint32_t pidBuscar, uint32_t numPagBuscar)
{
    bool coincidePidYPag(entradaTLB* unaEntrada){
        return unaEntrada->pid == pidBuscar && unaEntrada->numPag==numPagBuscar;
    }

    entradaTLB* entradaEncontrada = list_find(TLB, coincidePidYPag);
    if(entradaEncontrada != NULL){
        if(!strcmp(ALGORITMO_TLB, "LRU"))
        {
            list_remove_element(TLB, entradaEncontrada);
            list_add(TLB, entradaEncontrada);
        }
        log_info(info_logger, "PID: <%d> - TLB HIT - Pagina: <%d>", pidBuscar, numPagBuscar);
    	return entradaEncontrada->marco;
	}else
    {
        log_info(info_logger, "PID: <%d> - TLB MISS - Pagina: <%d>", pidBuscar, numPagBuscar);
        return -1;
    }
}

void agregarEntradaTLB(uint32_t pidAgregar, uint32_t numPagAgregar, uint32_t marcoAgregar)
{
    entradaTLB* entradaAgregar = malloc(sizeof(entradaTLB));
    entradaAgregar->pid = pidAgregar;
    entradaAgregar->numPag = numPagAgregar;
    entradaAgregar->marco = marcoAgregar;
    
    if(list_size(TLB) <  CANTIDAD_ENTRADAS_TLB)
    {
        list_add(TLB, entradaAgregar);
    }
    else{
        if(CANTIDAD_ENTRADAS_TLB){
            entradaTLB* entradaRemovida = list_remove(TLB, 0);
            free(entradaRemovida);
            list_add(TLB, entradaAgregar);
        }
    }

}