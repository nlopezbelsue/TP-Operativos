#include "esquema.h"

bool flagComunicacion = false;
uint32_t *marcosAsignados;
int RETARDO_RESPUESTA;
pthread_mutex_t mutexFS;

espacioContiguoMemoria espacioUsuario;

uint32_t buscarMarcoLibre() {
    for (uint32_t i = 0; i < CANT_MARCOS; i++) {
        if (marcosAsignados[i] == 0)
            return i;
    }
    return -1;//indica hay un Page Fault
}

void marcarMarcoOcupado(int numeroMarco) {
    if (numeroMarco >= 0 && numeroMarco < CANT_MARCOS) {
        marcosAsignados[numeroMarco] = 1;
    } else {
        log_error(error_logger, "Número de marco inválido");
    }
}

void marcarMarcoLibre(int numeroMarco) {
    if (numeroMarco >= 0 && numeroMarco < CANT_MARCOS) {
        marcosAsignados[numeroMarco] = 0;
    } else {
        log_error(error_logger, "Número de marco inválido");
    }
}

void crearTablaPaginasProceso(uint32_t pid, uint32_t size){
	TablaDePaginas* nuevaTabla = malloc(sizeof (TablaDePaginas));
	  if (nuevaTabla == NULL) {
	        log_error(error_logger, "Error: No se pudo asignar memoria para nuevaPagina");
	        exit(EXIT_FAILURE);
	    }

	  int cantPaginas = size / TAM_PAGINA;
	  nuevaTabla->pid = pid;
     
	    nuevaTabla->paginas = list_create();
	for(int i = 0; i < cantPaginas ; i++){
		Pagina* nuevaPagina = malloc(sizeof(Pagina));
	    if (nuevaPagina == NULL) {
	        log_error(error_logger, "Error: No se pudo asignar memoria para nuevaPagina");
	        exit(EXIT_FAILURE);
	    }
		nuevaPagina->marco = buscarMarcoLibre();
        log_trace(trace_logger, "MARCO ELEGIDO PARA LA PAGINA %d: %d", i, nuevaPagina->marco);
        log_trace(trace_logger, "LA CANT DE PAG ES %d", cantPaginas);
        list_add(nuevaTabla->paginas, nuevaPagina);
		marcarMarcoOcupado(nuevaPagina->marco);
	}
	pthread_mutex_lock(&mutex_tablasPaginas);
    list_add(tablaGeneral, nuevaTabla);
    pthread_mutex_unlock(&mutex_tablasPaginas);
    log_trace(trace_logger,"Tamaño de tablaGeneral después de iniciar proceso: %d", list_size(tablaGeneral));
    log_info(info_logger, "PID: <%d> - Tamaño: <%d>", pid, cantPaginas); //log obligatorio
}

void finalizacionDeProceso(uint32_t pid){
	TablaDePaginas* tabla = obtenerTablaPorPID(pid);
    log_trace(trace_logger, "TAMAÑO TABLA: %d", list_size(tabla->paginas));
    for (int j = 0; j < list_size(tabla->paginas); j++) {
        Pagina* pagina = list_get(tabla->paginas, j);
        log_trace(trace_logger, "MARCO DE LA PAGINA %d: %d",j, pagina->marco);
        marcarMarcoLibre(pagina->marco);
    }
	liberarTablaDePaginas(pid);
	log_trace(trace_logger,"Tamaño de tablaGeneral después de finalizar proceso: %d", list_size(tablaGeneral));
	return;
};

TablaDePaginas* obtenerTablaPorPID(uint32_t pid) {
    for (int i = 0; i < list_size(tablaGeneral); i++) {
        TablaDePaginas* tabla = list_get(tablaGeneral, i);
        if (tabla->pid == pid)
            return tabla;
    }
    return NULL;
}

uint32_t obtenerMarcoDePagina(uint32_t pid, uint32_t numeroPagina){
	TablaDePaginas* tabla = obtenerTablaPorPID(pid);
	Pagina* pagina = list_get(tabla->paginas, numeroPagina);
	if (pagina->marco >= 0 && pagina->marco < CANT_MARCOS){
		log_info(info_logger,"PID: <%d> - Pagina: <%d> - Marco: <%d>", pid, numeroPagina, pagina->marco); //log obligatorio
		return pagina->marco;
	}
	else{
		log_trace(trace_logger,"No encontre frame de la pag %d", numeroPagina); //log obligatorio
		return -1;//HAY QUE VER QUE PASA SI HAY OUT OF MEMORY
    }
}

uint32_t obtenerPaginaConMarco(uint32_t marco){
    log_trace(trace_logger, "busco marco %d", marco);

    for (int i = 0; i < list_size(tablaGeneral); i++) {
        TablaDePaginas* tabla = list_get(tablaGeneral, i);
        for (int j = 0; j < list_size(tabla->paginas); j++) {
            Pagina* pagina = list_get(tabla->paginas, j);
            if (pagina->marco == marco){
                log_trace(trace_logger, "Encontre pag %d", j);
                return j;
            }
        }
    }
    log_trace(trace_logger, "El marco %d solicitado está vacío", marco);
    return NULL;
}


void* leerMemoria(uint32_t direccionFisica, uint32_t tamanio, uint32_t pid){	//Devuelve el valor de la direccion fisica pedida
    void* datosPuntero = malloc(tamanio);

    memcpy(datosPuntero, espacio_contiguo + direccionFisica, tamanio);
	log_info(info_logger,"PID: <%d> - Accion: <LEER> - Direccion fisica: <%d> - Tamanio: <%d>", pid, direccionFisica, tamanio); //log obligatorio
    simularRetardoSinMensaje(RETARDO_RESPUESTA);

    return datosPuntero;
}

void escribirMemoria(uint32_t direccionFisica, void* datos, uint32_t tamanio, uint32_t pid){	//Escribir lo indicado a partir de la dirección física pedida
    
    memcpy(espacio_contiguo + direccionFisica, datos, tamanio);
    //log_trace(trace_logger, "ESCRIBI %s", datos);
	log_info(info_logger,"PID: <%d> - Accion: <ESCRIBIR> - Direccion fisica: <%d> - Tamanio: <%d>", pid, direccionFisica, tamanio); //log obligatorio
    simularRetardoSinMensaje(RETARDO_RESPUESTA);
}

uint32_t resizeProceso(uint32_t pid, uint32_t tamanio) {
	uint32_t respuesta = 1;
    TablaDePaginas* tabla = obtenerTablaPorPID(pid);
    
    int tamOriginal = list_size(tabla->paginas) * TAM_PAGINA;

	if(list_size(tabla->paginas)*TAM_PAGINA >= tamanio){
		achicar(tabla, tamanio);
        if(tamanio != 0) 
            log_info(info_logger, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d>", pid, tamOriginal, tamOriginal-tamanio); //log obligatorio
        else
            log_info(info_logger, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d", pid, tamOriginal, tamanio);
	}else{
		respuesta = agrandar(tabla, tamanio);
        log_info(info_logger, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>", pid, tamOriginal, tamanio-tamOriginal); //log obligatorio
	}
	return respuesta;
}

void achicar(TablaDePaginas* tabla, uint32_t tamanio){
    int pagsFinales = tamanio / TAM_PAGINA;
    if(tamanio % TAM_PAGINA){
        pagsFinales += 1;
    }
    for(int i=list_size(tabla->paginas); i>pagsFinales; i--){
        Pagina* pag = list_get(tabla->paginas, i-1);
        marcarMarcoLibre(pag->marco);
        list_remove(tabla->paginas, i-1);
        liberarPagina(pag);
    }
}

uint32_t cantidadMarcosVacios() {
    uint32_t marcosVacios = 0;

    for (uint32_t i = 0; i < CANT_MARCOS; ++i) {
        if (marcosAsignados[i] == 0)
            marcosVacios ++;
    }
    return marcosVacios;
}

uint32_t agrandar(TablaDePaginas* tabla, uint32_t tamanio){
    int pagsIniciales = list_size(tabla->paginas);
    int pagsFinales = tamanio / TAM_PAGINA;
    if(tamanio % TAM_PAGINA){
        pagsFinales += 1;
    }

    if(cantidadMarcosVacios() >= pagsFinales-pagsIniciales){
        for(int i=list_size(tabla->paginas); i<pagsFinales; i++){
            Pagina* nuevaPagina = malloc(sizeof(Pagina));
            if (nuevaPagina == NULL) {
                log_trace(trace_logger, "Error: No se pudo asignar memoria para nuevaPagina");
                exit(EXIT_FAILURE);
            }
            nuevaPagina->marco = buscarMarcoLibre();
            log_trace(trace_logger, "MARCO ELEGIDO PARA LA PAGINA %d: %d", i, nuevaPagina->marco);
            list_add(tabla->paginas, nuevaPagina);
            marcarMarcoOcupado(nuevaPagina->marco);
        }
    }else return -1; //VER retorna Out Of Memory.
    return 1;
}