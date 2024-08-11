#include "consola.h"

pthread_mutex_t semaforo;
pthread_mutex_t semaforo2;

char* path;

int planificadorLargoAvance;
int planificadorCortoAvance;

void* iniciarConsola () {
	pthread_mutex_init(&semaforo, NULL);

	while (1) {
	    char* linea = readline(">");
	    log_info(info_logger,"linea: %s",linea);

		if (!strncmp(linea, ":q", 2)) {
			free(linea);
			break;
		}

	    if (!strncmp(linea,"EJECUTAR_SCRIPT", strlen("EJECUTAR_SCRIPT")))
	    	ejecutar_script(linea);
		else
			funcionesDeLaConsola(linea);

	    free(linea);
	}
}

void funcionesDeLaConsola(char* linea) {
	if (!strncmp(linea,"INICIAR_PROCESO", strlen("INICIAR_PROCESO"))){
		//log_info(info_logger, "lei iniciar proceso");
		iniciar_proceso(linea);
	}

	if (!strncmp(linea, "FINALIZAR_PROCESO", strlen("FINALIZAR_PROCESO"))) {
		//log_info(info_logger, "lei finalizar proceso");
		finalizar_proceso(linea);
	}

	if (!strncmp(linea,"DETENER_PLANIFICACION", strlen("DETENER_PLANIFICACION"))) {
		//log_info(info_logger, "lei detener plani");
		DETENER_PLANIFICACION(linea);
	}

	if (!strncmp(linea,"INICIAR_PLANIFICACION", strlen("INICIAR_PLANIFICACION"))) {
		//log_info(info_logger, "lei iniciar plani");
		INICIAR_PLANIFICACION(linea);
	}

	if (!strncmp(linea,"MULTIPROGRAMACION", strlen("MULTIPROGRAMACION"))) {
		//log_info(info_logger, "lei cambiar grado multiprog");
		MULTIPROGRAMACION(linea);
		log_info(info_logger, "Grado multiprog actualizado: %d", GRADO_MAX_MULTIPROGRAMACION);
	}

	if (!strncmp(linea, "PROCESO_ESTADO", strlen("PROCESO_ESTADO"))) {
		//log_info(info_logger, "lei mostrar procesos");
		PROCESO_ESTADO(linea);
	}

	return;
}

void ejecutar_script (char* linea) {
	log_info(info_logger, "RECONOCI LA LINEA %s", linea);

	path = malloc(strlen(linea) + 1);
	char* saveptr = malloc(strlen(linea) + 1);

	saveptr[0] =' \0';
	path[0] = '\0';
	sscanf(linea,"%s %s",saveptr, path);

	pthread_mutex_lock(&mutex_iniciarProceso);
    pthread_t tid;
    pthread_create(&(tid), NULL, ejecutar_script_operaciones, path);
    pthread_join(tid, NULL);
    free(saveptr);
	pthread_mutex_unlock(&mutex_iniciarProceso);

    return;
}

void* ejecutar_script_operaciones (void* parametros) {
	char* directory = "/home/utnso/scripts-pruebas";
    char* file = (char*) parametros;
	
	log_info(info_logger, "Ruta a buscar: %s", directory);

	size_t total_length = strlen(directory) + strlen(file) + 1;
    char pathInstrucciones[total_length];

	strcpy(pathInstrucciones, directory);
    strcat(pathInstrucciones, file);

	log_info(info_logger, "Archivo a buscar: %s", pathInstrucciones);

	FILE* fptr;

	fptr = fopen(pathInstrucciones, "r");
	char instruccion[200];
	
	if (fptr == NULL)
		log_info(info_logger, "NO ENCONTRE EL ARCHIVO");

	if (fptr != NULL)
		log_info(info_logger, "ENCONTRE EL ARCHIVO");

	while (fgets(instruccion, sizeof(instruccion), fptr)) {
		log_info(info_logger, "ESTOY LEYENDO UN ARCHIVO. Instruccion <%s>", instruccion);

		funcionesDeLaConsola(instruccion);
	}
	
	fclose(fptr);

	return;
}

void iniciar_proceso (char* linea) {
	log_info(info_logger, "funcion iniciar");
	path = malloc(strlen(linea) + 1);
	char* saveptr = malloc(strlen(linea) + 1);

	saveptr[0] =' \0';
	path[0] = '\0';
	sscanf(linea,"%s %s",saveptr, path);

    pthread_t tid;
    pthread_create(&(tid), NULL, inicializarProceso, path);
    pthread_join(tid, NULL);
	free(saveptr);

    return;
}

void* inicializarProceso (void* parametros) {
	log_info(info_logger, "voy a iniciar proceso");
	pthread_mutex_lock(&semaforo);
    char* pathptr = (char*) parametros;
	PCB* pcb = crearPcb(pathptr);

    queue_push(colaNew,pcb);

    sem_post(&sem_procesosEnNew);
    log_info(info_logger, "Se crea el proceso <%d> en NEW", pcb->id);
    pthread_mutex_unlock(&semaforo);

	return;
}

void finalizar_proceso (char* linea) {
    pthread_t tid;

	char* saveptr = malloc(sizeof(linea));
	char* pid = malloc(sizeof(linea));

	saveptr[0] = '\0';
	pid[0] = '\0';

	sscanf(linea,"%s %s",saveptr, pid);

    pthread_create(&(tid), NULL, finalizarProceso, atoi(pid));

    pthread_join(tid, NULL);
    free(saveptr);
    free(pid);

    return;
}

void* finalizarProceso (void* pid) {
	pthread_mutex_lock(&semaforo);
	uint32_t idProceso = (uint32_t) pid;
	uint32_t numCola = 0;
	PCB* pcbBuscado = encontrarProceso(idProceso, &numCola);

	switch(numCola){
		case 1:
			pthread_mutex_lock(&mutex_colaNew);
			eliminarElementoLista(pcbBuscado, colaNew->elements);
			pthread_mutex_unlock(&mutex_colaNew);
			log_info(info_logger, "PID: [%d] - Estado Anterior: NEW - Estado Actual: EXIT", pcbBuscado->id);
			break;
		case 2:
			pthread_mutex_lock(&mutex_ColaReady);
			eliminarElementoLista(pcbBuscado, colaReady);
			pthread_mutex_unlock(&mutex_ColaReady);
			log_info(info_logger, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXIT", pcbBuscado->id);
			break;
		case 4:
			pthread_mutex_lock(&mutex_colaBloq);
			eliminarElementoLista(pcbBuscado, colaBloq);
			pthread_mutex_unlock(&mutex_colaBloq);
			log_info(info_logger, "PID: [%d] - Estado Anterior: BLOQUEADO - Estado Actual: EXIT", pcbBuscado->id);
			break;
		case 5:
			pthread_mutex_lock(&mutex_colaVRR);
			eliminarElementoLista(pcbBuscado, colaReadyVRR);
			pthread_mutex_unlock(&mutex_colaVRR);
			log_info(info_logger, "PID: [%d] - Estado Anterior: READYVRR - Estado Actual: EXIT", pcbBuscado->id);
			break;
		default:
			if(numCola != 3){
				log_info(info_logger,"No se encontro el proceso %d en ninguna lista", idProceso);
				return NULL;
			}
			break;
	}

	if (numCola != 3) {
		log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <INTERRUPTED_BY_USER>",pcbBuscado->id);
		sem_post(&sem_cpuLibre);

		pthread_mutex_lock(&mutex_colaExit);
		queue_push(colaExit,pcbBuscado);
		pthread_mutex_unlock(&mutex_colaExit);

		if (!list_is_empty(pcbBuscado->recursosTomados))
			liberarRecursosTomados(pcbBuscado);

		sem_post(&sem_procesosExit);

		pthread_mutex_unlock(&semaforo);
	} else {
		enviarValor_uint32(pcbBuscado->id, cpuInterrupt_fd, DESALOJOCPU, info_logger);
		pthread_mutex_unlock(&semaforo);
	}

	return NULL;
}

PCB* encontrarProceso (uint32_t pid, uint32_t* cola) {
    bool coincideProceso(PCB* unPCB) {
        return unPCB->id == pid;
    }

    PCB* pcbEncontrado = list_find(colaNew->elements, coincideProceso);
    if (pcbEncontrado != NULL) {
    	*cola = 1;
    	return pcbEncontrado;
	}

    pcbEncontrado = list_find(colaReady, coincideProceso);
    if(pcbEncontrado != NULL) {
		*cola = 2;
		return pcbEncontrado;
	}

    pcbEncontrado = list_find(colaExec, coincideProceso);
    if(pcbEncontrado != NULL) {
		*cola = 3;
		return pcbEncontrado;
	}

    pcbEncontrado = list_find(colaBloq, coincideProceso);
    if(pcbEncontrado != NULL) {
		*cola = 4;
		return pcbEncontrado;
	}

	pcbEncontrado = list_find(colaReadyVRR, coincideProceso);
    if(pcbEncontrado != NULL) {
		*cola = 5;
		return pcbEncontrado;
	}

    *cola = 0;
    
    return pcbEncontrado;
}

void DETENER_PLANIFICACION (char* linea) {
    pthread_t tid;

    planificadorLargoAvance = 0;
    planificadorCortoAvance = 0;

    pthread_create(&(tid), NULL, detenerPlanificacion, linea);
    pthread_join(tid, NULL);

    return;
}

void* detenerPlanificacion () {
	log_info(info_logger, "PAUSA PLANIFICACION");
	planificacionFlag = false;

	return NULL;
}

void INICIAR_PLANIFICACION (char* linea) {
    pthread_t tid;

    pthread_create(&(tid), NULL, iniciarPlanificacion, linea);
    pthread_join(tid, NULL);

    return;
}

void* iniciarPlanificacion () {
	log_info(info_logger, "INICIO PLANIFICACION");

	if (!planificacionFlag) {
		planificacionFlag = true;

		if (planificadorLargoAvance == 1)
			sem_post(&sem_procesosEnNew);

		if (planificadorCortoAvance == 1){
			if(queue_size(colaNew) == 0)
				sem_post(&sem_procesosReady);

			sem_post(&sem_cpuLibre);
		}

		pthread_create(&hilo_planificador_LP, NULL, (void*) planificadorLargoPlazo, NULL);
		pthread_create(&hilo_planificador_corto, NULL, (void*) planificadorCortoPlazo, NULL);
	}

	return NULL;
}

void MULTIPROGRAMACION (char* linea) {
	char* saveptr = malloc(sizeof(linea));
	char* mp = malloc(sizeof(linea));

	saveptr[0] = '\0';
	mp[0] = '\0';

	sscanf(linea, "%s %s", saveptr, mp);

    pthread_t tid;
    pthread_create(&(tid), NULL, multiprogramacion, atoi(mp));
    pthread_join(tid, NULL);
    free(saveptr);
    free(mp);

    return;
}

void* multiprogramacion (void* linea) {
	pthread_mutex_lock(&semaforo);
	uint32_t nuevoGradoMP = (uint32_t) linea;

	log_info(info_logger, "Grado Anterior: <%d> - Grado Actual: <%d>", GRADO_MAX_MULTIPROGRAMACION, nuevoGradoMP);
	GRADO_MAX_MULTIPROGRAMACION=nuevoGradoMP;

	uint32_t tamLista = queue_size(colaNew);

	if(tamLista){
		for(int i=0 ; i<tamLista ; i++){
			sem_post(&sem_procesosEnNew);
		}
	}

	pthread_mutex_unlock(&semaforo);

    return NULL;
}

void PROCESO_ESTADO (char* linea) {
	pthread_t tid;
	pthread_create(&(tid), NULL, mostrarColasPuntero, NULL);
	pthread_join(tid, NULL);
    return;
}

void* mostrarColasPuntero () {
	mostrarEstadoColas();
    return NULL;
}