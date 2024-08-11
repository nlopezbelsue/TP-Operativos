#ifndef ESQUEMA_H_
#define ESQUEMA_H_
#include "comunicacion.h"
typedef struct {
    TablaDePaginas* procesos;
    size_t numProcesos;
} TablaGeneral;

typedef struct{
	int tamanioMemoria;
	void *espacioDisponible;
	int memoriaIniciada;
} espacioContiguoMemoria;

uint32_t buscarMarcoLibre(); //Devuelve el primer marco libre que encuentra
void marcarMarcoOcupado(int numeroMarco);
void marcarMarcoLibre(int numeroMarco);
void crearTablaPaginasProceso(uint32_t pid, uint32_t size); //Crea la tabla de paginas de un proceso y la agrega a la TablaGeneral
void finalizacionDeProceso(uint32_t pid); //Marca como libres los marcos del proceso
TablaDePaginas* obtenerTablaPorPID(uint32_t pid);
void liberarTablaDePaginas(uint32_t); //Elimina la tabla del proceso de la TablaGeneral
uint32_t obtenerMarcoDePagina(uint32_t, uint32_t); //Devuelve el numero de marco de la pagina solicitada
uint32_t obtenerPaginaConMarco(uint32_t marco);

uint32_t resizeProceso(uint32_t pid, uint32_t tamanio);
uint32_t agrandar(TablaDePaginas* tabla, uint32_t tamanio);
void achicar(TablaDePaginas* tabla, uint32_t tamanio);
uint32_t cantidadMarcosVacios();

extern espacioContiguoMemoria espacioUsuario;
extern TablaDePaginas tablaDePaginas;
extern int tam_pagina;
extern bool flagComunicacion;

//Devuelve el valor de la direccion fisica pedida
void* leerMemoria(uint32_t direccionFisica, uint32_t tamanio, uint32_t pid);
void escribirMemoria(uint32_t direccionFisica, void* datos, uint32_t tamanio, uint32_t pid);

#endif /* ESQUEMA_H_ */