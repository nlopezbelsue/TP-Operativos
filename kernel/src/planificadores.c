#include "planificadores.h"

int proteccion = 1;

void planificadorLargoPlazo() {
    log_info(info_logger, "Kernel - PLANIFICADOR LARGO PLAZO INICIADO.\n");
    while (planificacionFlag) {
        pthread_mutex_lock(&planificacionLargo);

        sem_wait(&sem_procesosEnNew);
	
        if (!planificacionFlag)
        	planificadorLargoAvance = 1;

        if (procesosTotales_MP < GRADO_MAX_MULTIPROGRAMACION && queue_size(colaNew) > 0 && planificacionFlag) {
            pthread_mutex_lock(&mutex_colaNew);
            PCB* pcbAReady = queue_peek(colaNew);
            pthread_mutex_unlock(&mutex_colaNew);
            aumentarGradoMP();

            
            enviar_uint32_y_uint32_y_char(pcbAReady->nombreRecurso,pcbAReady->id, pcbAReady->size, memoria_fd, INICIALIZAR_PROCESO_MEMORIA, info_logger);
            int cod_op = recibir_operacion(memoria_fd);

            if (cod_op == INICIALIZAR_PROCESO_MEMORIA) {
            	recibirOrden(memoria_fd);
            	moverProceso_NewReady();
            	if (queue_size(colaNew) == 0 || GRADO_MAX_MULTIPROGRAMACION == procesosTotales_MP)
            		sem_post(&sem_procesosReady);
            	else
            		sem_post(&sem_procesosEnNew);
            }
            log_info(info_logger,"PID <%d> Enviado a memoria para ser cargado", pcbAReady->id);
        }

        pthread_mutex_unlock(&planificacionLargo);
    }
}

void planificadorCortoPlazo(){
    log_info(info_logger, "Kernel - PLANIFICADOR CORTO PLAZO INICIADO.");
    while (planificacionFlag) {
    	if (proteccion) {
			pthread_mutex_lock(&planificacionCorto);
			sem_wait(&sem_cpuLibre);
			sem_wait(&sem_procesosReady);
			mostrarEstadoColasAux("READY", colaReady);
            mostrarEstadoColasAux("READY VRR", colaReadyVRR);
			if (!planificacionFlag)
				planificadorCortoAvance = 1;
			if (planificacionFlag && (list_size(colaReady) || list_size(colaReadyVRR)))
				moverProceso_readyExec();
			if (!list_size(colaReady) && !list_size(colaExec) && list_size(colaReadyVRR))
				proteccion = 0;
			pthread_mutex_unlock(&planificacionCorto);
    	} else {
    		if (list_size(colaReady) || list_size(colaReadyVRR)) {
    			proteccion = 1;
    			sem_post(&sem_cpuLibre);
    			sem_post(&sem_procesosReady);
    		}
    	}
    }
}

void liberar_procesos() {
    while (1) {
        sem_wait(&sem_procesosExit);
        pthread_mutex_lock(&mutex_colaExit);
        PCB* pcbALiberar = queue_pop(colaExit);
        pthread_mutex_unlock(&mutex_colaExit);

        enviarValor_uint32(pcbALiberar->id, memoria_fd, FINALIZAR_PROCESO_MEMORIA, info_logger);

        liberarPcb(pcbALiberar);
        int cod_op = recibir_operacion(memoria_fd);
		if (cod_op == FINALIZAR_PROCESO_MEMORIA) {
			recibirOrden(memoria_fd);
			disminuirGradoMP();
			if (queue_size(colaNew))
				sem_post(&sem_procesosEnNew);
		}
    }
}

void liberarPcb (PCB* pcb) {
    free(pcb->registros);//Estaba comentado cuando andaba
    list_destroy(pcb->listaInstrucciones);
    list_destroy(pcb->tablaPaginas);
    list_destroy(pcb->recursosTomados);
    free(pcb);//Estaba comentado cuando andaba
}

void mandarPaquetePCB(PCB *pcb){
	t_paquete* paquete = crear_paquete(CONTEXTOEJECUCION, info_logger);
	agregar_ContextoEjecucion_a_paquete(paquete, pcb);
	enviar_paquete(paquete, cpuDispatch_fd);
	eliminar_paquete(paquete);
}

void* esperarRR (void* pcbReady) {
	PCB* pcb = (PCB*) pcbReady;
	pcb->tiempoEjecutando = 0;
	while (1) {
		/*if (QUANTUM > 1000) {
			usleep(1000);
		} else {
			usleep(10000);
		}*/

        usleep((QUANTUM/2 -600));

		pcb->tiempoEjecutando++;
		if (pcb->tiempoEjecutando >= QUANTUM) {
            if(list_size(colaExec)){
                PCB* pcbExec = obtenerPcbExec();
                if (pcb->id == pcbExec->id) {
                    enviarValor_uint32(pcbExec->id, cpuInterrupt_fd, INTERRUPCIONCPU, info_logger);
                    log_info(info_logger, "PID: <%d> - Desalojado por fin de Quantum", pcbExec->id);
                }
            }
            pcb->tiempoEjecutando = 0;
            return NULL;
		}
	}
}

void* esperarVRR(void* pcbReady) {
	PCB* pcb = (PCB*) pcbReady;
	pcb->tiempoEjecutando = 0;
    uint32_t numCola = 0;
	while (1) {
        usleep((QUANTUM/2 -600));

		pcb->tiempoEjecutando++;

        if (pcb->tiempoEjecutando >= QUANTUM || (pcb->quantum && (pcb->tiempoEjecutando > pcb->quantum))) {//DESALOJO
            if (list_size(colaExec)) {
                PCB* pcbExec = obtenerPcbExec();
                if (pcb->id == pcbExec->id) {
                    pcb->quantum = 0;
                    enviarValor_uint32(pcbExec->id, cpuInterrupt_fd, INTERRUPCIONCPU, info_logger);
                    log_info(info_logger, "PID: <%d> - Desalojado por fin de Quantum", pcbExec->id);
                } 
            }
            pcb->tiempoEjecutando = 0;
			return NULL;
		}

        if (pcb->tiempoEjecutando < QUANTUM) {
            if (list_size(colaExec)) {
                PCB* pcbExec = obtenerPcbExec();
                if (pcb->id != pcbExec->id) {
                    pcb->quantum = QUANTUM - pcb->tiempoEjecutando;
                    log_info(info_logger, "PID: <%d> - Proceso con <%d> de quantum sobrante", pcbExec->id, pcbExec->quantum);
                    pcb->tiempoEjecutando = 0;
			        return NULL;
                }
            }
        }
	}
}

void moverProceso_readyExec(){
        pthread_mutex_lock(&mutex_colaExec);
        PCB* pcbReady;

        if (list_size(colaReadyVRR)) {
            pthread_mutex_lock(&mutex_colaVRR);
            pcbReady = list_remove(colaReadyVRR,0);
            list_add(colaExec,pcbReady);
            pthread_mutex_unlock(&mutex_colaVRR);
        }
        else if (list_size(colaReady)) {
            pthread_mutex_lock(&mutex_ColaReady);
            pcbReady = list_remove(colaReady,0);
            list_add(colaExec,pcbReady);
            pthread_mutex_unlock(&mutex_ColaReady);
        } 

        if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0) {
            log_info(info_logger, "Usando VRR");
            pthread_t atenderVRR;
            pthread_create(&atenderVRR, NULL, esperarVRR, (void*) pcbReady);
            pthread_detach(atenderVRR);
            mostrarEstadoColas();
        } 

        if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0) {
            log_info(info_logger, "Usando RR");
            pthread_t atenderRR;
            pthread_create(&atenderRR, NULL, esperarRR, (void*) pcbReady);
            pthread_detach(atenderRR);
        }

        pthread_mutex_unlock(&mutex_colaExec);
        mandarPaquetePCB(pcbReady);
        log_info(info_logger, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcbReady->id);
}


void moverProceso_NewReady(){
    pthread_mutex_lock(&mutex_colaNew);
    PCB* pcbAReady = queue_pop(colaNew);
    pthread_mutex_unlock(&mutex_colaNew);

    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbAReady);
    pthread_mutex_unlock(&mutex_ColaReady);
}

void moverProceso_ExecBloq (PCB* pcbBuscado) {
    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_unlock(&mutex_colaExec);

    pthread_mutex_lock(&mutex_colaBloq);
    list_add(colaBloq, pcbBuscado);
    pthread_mutex_unlock(&mutex_colaBloq);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: BLOQ.", pcbBuscado->id);

    sem_post(&sem_cpuLibre);
    if (list_size(colaReady) || list_size(colaReadyVRR))
    	sem_post(&sem_procesosReady);
}

void moverProceso_BloqReady (PCB* pcbBuscado) {
    pthread_mutex_lock(&mutex_colaBloq);
    eliminarElementoLista(pcbBuscado, colaBloq);
    pthread_mutex_unlock(&mutex_colaBloq);

    if (pcbBuscado->quantum) {
        pthread_mutex_lock(&mutex_colaVRR);
        list_add(colaReadyVRR, pcbBuscado);
        pthread_mutex_unlock(&mutex_colaVRR);
    }
    else {
        pthread_mutex_lock(&mutex_ColaReady);
        list_add(colaReady, pcbBuscado);
        pthread_mutex_unlock(&mutex_ColaReady);
    }
        
    log_info(info_logger, "PID: [%d] - Estado Anterior: BLOQ - Estado Actual: READY.", pcbBuscado->id);
    sem_post(&sem_procesosReady);

    if (!list_size(colaExec))
        sem_post(&sem_cpuLibre);

}

void moverProceso_ExecExit (PCB *pcbBuscado) {

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_unlock(&mutex_colaExec);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: EXIT", pcbBuscado->id);

    sem_post(&sem_cpuLibre);

    pthread_mutex_lock(&mutex_colaExit);
    queue_push(colaExit, pcbBuscado);
    pthread_mutex_unlock(&mutex_colaExit);

    if (!list_is_empty(pcbBuscado->recursosTomados))
        liberarRecursosTomados(pcbBuscado);

    sem_post(&sem_procesosExit);
    if (list_size(colaReady) || list_size(colaReadyVRR))
     	sem_post(&sem_procesosReady);
}

void moverProceso_ExecReady (PCB* pcbBuscado) {
    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_unlock(&mutex_colaExec);

    if (pcbBuscado->quantum) {
        pthread_mutex_lock(&mutex_colaVRR);
        list_add(colaReadyVRR, pcbBuscado);
        pthread_mutex_unlock(&mutex_colaVRR);
    }
    else {
        pthread_mutex_lock(&mutex_ColaReady);
        list_add(colaReady, pcbBuscado);
        pthread_mutex_unlock(&mutex_ColaReady);
    }

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: READY", pcbBuscado->id);
    sem_post(&sem_procesosReady);
    sem_post(&sem_cpuLibre);
}

void liberarRecursosTomados (PCB* pcb) {
    for (int i = 0; i < list_size(pcb->recursosTomados); i++) {
        Recurso* recursoTomado = list_get(pcb->recursosTomados, i);
        pthread_mutex_lock(&semaforos_io[recursoTomado->indiceSemaforo]);
        recursoTomado->instanciasRecurso++;
        pthread_mutex_unlock(&semaforos_io[recursoTomado->indiceSemaforo]);
        log_info(info_logger,"PID:<%d> - libera Recurso:<%s> - Instancias <%d>", pcb->id, recursoTomado->nombreRecurso, recursoTomado->instanciasRecurso);
        if (!list_is_empty(recursoTomado->cola))
        	moverProceso_BloqrecursoReady(recursoTomado);
    }
    list_clean(pcb->recursosTomados);
}

void aumentarGradoMP () {
    pthread_mutex_lock(&mutex_MP);
    procesosTotales_MP++;
    pthread_mutex_unlock(&mutex_MP);
    log_info(info_logger, "Kernel - GRADO DE MULTIPROGRAMACION: %d.\n", procesosTotales_MP);
}

void disminuirGradoMP () {
    pthread_mutex_lock(&mutex_MP);
    procesosTotales_MP--;
    pthread_mutex_unlock(&mutex_MP);
    log_info(info_logger, "Kernel - GRADO DE MULTIPROGRAMACION: %d.\n", procesosTotales_MP);
}

void mostrarEstadoColas () {
    pthread_mutex_lock(&mutex_debug_logger);
    mostrarEstadoColasAux("Nuevos", colaNew->elements);
    mostrarEstadoColasAux("Bloqueados", colaBloq);
    mostrarEstadoColasAux("Ejecutando", colaExec);
    mostrarEstadoColasAux("Ready", colaReady);
    mostrarEstadoColasAux("Ready VRR", colaReadyVRR);
    pthread_mutex_unlock(&mutex_debug_logger);
}

void mostrarEstadoColasAux (char* colaMsg, t_list* cola) {
    char* idsPcb = string_new();
    void guardarIds(void* pcbVoid) {
    	PCB* unaPcb = (PCB*) pcbVoid;
        char* unId = string_itoa(unaPcb->id);
        string_append(&idsPcb, unId);
        string_append(&idsPcb, ", ");
    }

    string_append(&idsPcb, "[");
    if (!list_is_empty(cola))
    	list_iterate(cola, guardarIds);

    string_append(&idsPcb, "]");
    log_info(info_logger,"Cola de procesos %s: %s", colaMsg, idsPcb);
    free(idsPcb);
}

void eliminarElementoLista (PCB* pcbBuscado, t_list* lista) {
  PCB* pcbAux;
  for (int i = 0; i < list_size(lista); i++) {
    pcbAux = list_get(lista, i);
    if(pcbBuscado->id == pcbAux->id)
    	list_remove(lista, i);
  }
}
