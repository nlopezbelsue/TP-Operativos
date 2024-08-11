#include <comunicacion.h>

int cpuDispatch_fd;
int cpuInterrupt_fd;
int memoria_fd;
int kernel_fd;
int stdin_fd;
int vectorIO[10];

int recibirConexion (char* puerto) {
	int entradasalida_fd;
	t_log* logger;
	pthread_t tid[10];
	logger = log_create("modulo.log", "-", 1, LOG_LEVEL_INFO);
	int32_t tipoInterfaz;
	
	kernel_fd = iniciar_servidor(logger, "Server Kernel", puerto);

	while (1) {
		entradasalida_fd = esperar_cliente(kernel_fd);
		recv(entradasalida_fd, &tipoInterfaz, sizeof(int32_t), MSG_WAITALL);
		vectorIO[tipoInterfaz] = entradasalida_fd;
		cualInterfaz(tipoInterfaz);		
		void* fdDeIo = &vectorIO[tipoInterfaz];
		pthread_create(&tid[tipoInterfaz], NULL, recibirIO, fdDeIo);
		pthread_detach(tid[tipoInterfaz]);
	}

	return EXIT_SUCCESS;
}

void cualInterfaz (int tipoInterfaz) {
	t_log* logger;
	logger = log_create("modulo.log", "-", 1, LOG_LEVEL_INFO);

	switch (tipoInterfaz) {
	case 0:
		log_info(logger, "Interfaz STDOUT conectada");
		break;
	case 1:
		log_info(logger, "Interfaz STDIN conectada");
		break;
	case 2:
		log_info(logger, "Interfaz DIALFS conectada");
		break;
	case 3:
		log_info(logger, "Interfaz ESPERA conectada");
		break;
	case 4:
		log_info(logger, "Interfaz SLP1 conectada");
		break;
	case 5:
		log_info(logger, "Interfaz GENERICA conectada");
		break;
	case 6:
		log_info(logger, "Interfaz IO_GEN_SLEEP conectada");
		break;
	default:
		break;
	}

	log_destroy(logger);
}

int conectarModuloCPU(char* modulo){
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	t_log *logger;

	strcpy(charAux, modulo);
	logger= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(config,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_DISPATCH");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(config,parametro);

	log_info(logger, "IP=%s\n", ip);
	log_info(logger, "PUERTO=%s\n", puerto);

	cpuDispatch_fd= crear_conexion(logger, "Conecto Kernel a CPU Dispatch",ip, puerto);

	log_destroy(logger);
//	terminar_programa(conexion, logger);

	return cpuDispatch_fd;
}

int conectarModuloCPUInterrupt(char *modulo){
	
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	t_log *logger;

	strcpy(charAux, modulo);
	logger= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(config,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_INTERRUPT");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(config,parametro);

	log_info(logger, "IP=%s\n", ip);
	log_info(logger, "PUERTO=%s\n", puerto);

	cpuInterrupt_fd= crear_conexion(logger, "Conecto kernel a CPU Interrupt",ip, puerto);

	log_destroy(logger);

	return cpuInterrupt_fd;
}

int conectarModuloMemoria(char *modulo){
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	t_log *logger;

	strcpy(charAux, modulo);
	logger= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(config,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_DISPATCH");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(config,parametro);

	log_info(logger, "IP=%s\n", ip);
	log_info(logger, "PUERTO=%s\n", puerto);

	memoria_fd = crear_conexion(logger, "Conecto Kernel a memoria",ip, puerto);

	log_destroy(logger);

	return memoria_fd;
}

void* recibirIO (void* interfaz_fd) {
	int conexion = *((int*) interfaz_fd);

	while (1) {
		t_log* logger;
		logger = iniciar_logger("recibirIO.log");
		
		int cod_op = recibir_operacion(conexion); //seguro se necesita un mutex
		// pthread_mutex_lock(&mutexFS);

	    switch (cod_op) {
			case SOLICITUD_IO_CUMPLIDA: {
				t_list* listaEnteros = list_create();
				listaEnteros = recibirListaUint32_t(conexion);
				uint32_t pid = *(uint32_t*)list_get(listaEnteros, 0);
	
				PCB* pcbBuscado = buscarProcesoBloq(pid);
				moverProceso_BloqReady(pcbBuscado);
				break;
			}
			case -1:{
				log_error(logger, "el cliente se desconecto.");
				log_error(logger, "Terminando servidor ENTRADASALIDA");
				return NULL;
			}
			default:{
				log_warning(logger,"Operacion desconocida. No quieras meter la pata %d ", cod_op);
				break;
			}
		}
		//pthread_mutex_unlock(&mutexFS);
	}
}

t_log* iniciar_logger(char *nombre) {
	t_log* nuevo_logger;
	nuevo_logger = log_create(nombre, "tp", 1, LOG_LEVEL_INFO);
	if(nuevo_logger == NULL){
		printf("No se genero de forma correcta el logger");
		exit(1);
	}

	return nuevo_logger;
}

void terminar_programa(int conexion, t_log* logger) {
	(logger);
	liberar_conexion(conexion);
}

PCB* obtenerPcbExec() {
    pthread_mutex_lock(&mutex_colaExec);
    PCB* unaPcb = list_get(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);
    return unaPcb;
}

void moverProceso_BloqrecursoReady(Recurso* recurso) {
    pthread_mutex_lock(&semaforos_io[recurso->indiceSemaforo]);
    PCB* pcbLiberada = list_remove(recurso->cola,0);
    pthread_mutex_unlock(&semaforos_io[recurso->indiceSemaforo]);
    uint32_t num;
    PCB* pcbEncontrado = encontrarProceso(pcbLiberada->id, &num);
    if(pcbEncontrado != NULL) {
		pthread_mutex_lock(&mutex_colaBloq);
		eliminarElementoLista(pcbLiberada, colaBloq);
		pthread_mutex_unlock(&mutex_colaBloq);
		
    if (pcbLiberada->quantum) {
        pthread_mutex_lock(&mutex_colaVRR);
        list_add(colaReadyVRR, pcbLiberada);
        pthread_mutex_unlock(&mutex_colaVRR);
    }
    else {
        pthread_mutex_lock(&mutex_ColaReady);
        list_add(colaReady, pcbLiberada);
        pthread_mutex_unlock(&mutex_ColaReady);
    }
	
		sem_post(&sem_procesosReady);
		log_info(info_logger,"PID: <%d> - Estado Anterior: <BLOCKED_RECURSO[%s]> - Estado Actual: <READY>",pcbLiberada->id,recurso->nombreRecurso);
    }
}

void escucharCPU (void) {
	while (cpuDispatch_fd != -1) {
		int cod_op = recibir_operacion(cpuDispatch_fd);

		switch(cod_op){
		    case WAIT: {
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char(cpuDispatch_fd);
				char* nombreRecurso = pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				manejoDeRecursos("WAIT", nombreRecurso);
				break;
			}

			case SIGNAL: {
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char(cpuDispatch_fd);
				char* nombreRecurso = pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				manejoDeRecursos("SIGNAL", nombreRecurso);
				break;
			}

			case INTERRUPCIONCPU: {
				PCB* pcbRecibida = recibir_contextoEjecucion(cpuDispatch_fd);
				if (list_size(colaExec)) {
					actualizarPcbEjecucion(pcbRecibida);
					PCB* pcbActualizada = obtenerPcbExec();
					moverProceso_ExecReady(pcbActualizada);
				}
				break;
			}

			case EXIT: {
				PCB* pcbRecibida = recibir_contextoEjecucion(cpuDispatch_fd);
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <SUCCESS>",pcbActualizada->id);
				moverProceso_ExecExit(pcbActualizada);
				break;
			}

			case IO_GEN_SLEEP: {
				uint32_t tiempoSleep;
				uint32_t interfaz;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_uint32_y_uint32(cpuDispatch_fd, &interfaz, &tiempoSleep);
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				
				t_list* listaInts = list_create();

				list_add(listaInts, &pcbActualizada->id);
				list_add(listaInts, &tiempoSleep);

				enviarListaUint32_t(listaInts,vectorIO[interfaz], info_logger, IO_GEN_SLEEP);

				moverProceso_ExecBloq(pcbActualizada);
				/*SleepCpu *aux = malloc(sizeof(SleepCpu));
				aux->pcb = pcbActualizada;
				aux->segundosSleep = tiempoSleep;
				*/log_info(info_logger,"PID: <%d> - Bloqueado por <SLEEP>", pcbActualizada->id);/*
				pthread_t atenderSleep;
				pthread_create(&atenderSleep,NULL,esperaTiempo,(void*)aux);
				pthread_detach(atenderSleep);*/

				list_clean(listaInts);
				list_destroy(listaInts);

				break;
			}
			case IO_STDIN_READ: {	
				uint32_t direccion_fisica;
				uint32_t interfaz;
				uint32_t tamanio;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_uint32_y_uint32_y_uint32(cpuDispatch_fd, &interfaz, &direccion_fisica, &tamanio);
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();

				moverProceso_ExecBloq(pcbActualizada); //RECIBIR CONFIRMACION DE BAUTI

				t_list* listaInts = list_create();
				log_info(info_logger, "PID %d - dir fisica %d - tamanio %d", pcbActualizada->id, direccion_fisica, tamanio);
				list_add(listaInts, &pcbActualizada->id);
				list_add(listaInts, &direccion_fisica);
				list_add(listaInts, &tamanio);

				enviarListaUint32_t(listaInts, vectorIO[interfaz], info_logger, IO_STDIN_READ);
				list_clean(listaInts);
				list_destroy(listaInts);

				break;
			}
			case IO_STDOUT_WRITE: {	//FALTA VER QUE PASA CON EL TAMAÑO
				uint32_t direccion_fisica;
				uint32_t interfaz;
				uint32_t tamanio;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_uint32_y_uint32_y_uint32(cpuDispatch_fd, &interfaz, &direccion_fisica, &tamanio);
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				moverProceso_ExecBloq(pcbActualizada); //RECIBIR CONFIRMACION DE BAUTI

				t_list* listaInts = list_create();

				list_add(listaInts, &pcbActualizada->id);
				list_add(listaInts, &direccion_fisica);
				list_add(listaInts, &tamanio);

				enviarListaUint32_t(listaInts,vectorIO[interfaz], info_logger, IO_STDOUT_WRITE);
				list_clean(listaInts);
				list_destroy(listaInts);

				break;
			}
			case IO_FS_CREATE: {
				uint32_t interfaz;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char_y_uint32(cpuDispatch_fd, &interfaz);
				char* nombreArchivo=pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				moverProceso_ExecBloq(pcbActualizada); //RECIBIR CONFIRMACION DE BAUTI

				log_info(info_logger, "PCB->id: <%d>", pcbActualizada->id);
				enviarEnteroYString(pcbActualizada->id, nombreArchivo, vectorIO[interfaz], info_logger, IO_FS_CREATE);

				break;
			}
			case IO_FS_DELETE: {
				uint32_t interfaz;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char_y_uint32(cpuDispatch_fd, &interfaz);
				char* nombreArchivo=pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				moverProceso_ExecBloq(pcbActualizada); //RECIBIR CONFIRMACION DE BAUTI

				enviarEnteroYString(pcbActualizada->id, nombreArchivo, vectorIO[interfaz], info_logger, IO_FS_DELETE);

				break;
			}
			case IO_FS_TRUNCATE: {
				uint32_t interfaz;
				uint32_t tamanio;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char_y_uint32_y_uint32(cpuDispatch_fd, &interfaz, &tamanio);
				char* nombreArchivo = pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				moverProceso_ExecBloq(pcbActualizada); //RECIBIR CONFIRMACION DE BAUTI

				enviar_uint32_y_uint32_y_char(nombreArchivo, pcbActualizada->id, tamanio, vectorIO[interfaz], IO_FS_TRUNCATE, info_logger); //NO ESTOY SEGURO SI VAN LOS "&"

				break;
			}
			case IO_FS_WRITE: {
				uint32_t interfaz;
				uint32_t tamanio;
				uint32_t punteroArchivo;
				uint32_t direccion_fisica;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char_y_uint32_y_uint32_y_uint32_y_uint32(cpuDispatch_fd, &interfaz, &tamanio, &punteroArchivo, &direccion_fisica);
				char* nombreArchivo = pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				moverProceso_ExecBloq(pcbActualizada); //RECIBIR CONFIRMACION DE BAUTI

				enviar_uint32_y_uint32_y_uint32_y_uint32_y_char(nombreArchivo, pcbActualizada->id, tamanio, punteroArchivo, direccion_fisica, vectorIO[interfaz], IO_FS_WRITE, info_logger);

				break;
			}
			case IO_FS_READ: {
				uint32_t interfaz;
				uint32_t tamanio;
				uint32_t punteroArchivo;
				uint32_t direccion_fisica;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char_y_uint32_y_uint32_y_uint32_y_uint32(cpuDispatch_fd, &interfaz, &tamanio, &punteroArchivo, &direccion_fisica);
				char* nombreArchivo = pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				moverProceso_ExecBloq(pcbActualizada); //RECIBIR CONFIRMACION DE BAUTI

				enviar_uint32_y_uint32_y_uint32_y_uint32_y_char(nombreArchivo, pcbActualizada->id, tamanio, punteroArchivo, direccion_fisica, vectorIO[interfaz], IO_FS_READ, info_logger);

				break;
			}
			case OUT_OF_MEMORY: {
				PCB* pcbRecibida = recibir_contextoEjecucion(cpuDispatch_fd);
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				moverProceso_ExecExit(pcbActualizada);//Antes exec ready
				log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <OUT_OF_MEMORY>",pcbActualizada->id);
				break;
			}
			case -1:
				log_error(info_logger, "Cliente desconectado de %s...", "CPU");
				return;
			default:
				log_error(info_logger, "Algo anduvo mal en el server de %s", "CPU");
				log_info(info_logger, "Cop: %d", cod_op);
				return;
		}
	}
}

void actualizarPcbEjecucion(PCB* pcbRecibida) {
    pthread_mutex_lock(&mutex_colaExec);
    PCB* pcbExec = list_get(colaExec, 0);

    RegistrosCPU* registrosAux = pcbExec->registros;
    pcbExec->program_counter = pcbRecibida->program_counter;
    pcbExec->registros = pcbRecibida->registros;
    pcbRecibida->registros = registrosAux;
    liberarPcbCpu(pcbRecibida);
    pthread_mutex_unlock(&mutex_colaExec);
}

void manejoDeRecursos(char* orden, char* recursoSolicitado) {
    PCB* unaPcb = obtenerPcbExec();

    bool coincideConSolicitado (Recurso* unRecurso) {
        return strncmp(unRecurso->nombreRecurso, recursoSolicitado, strlen(unRecurso->nombreRecurso)) == 0;
    }

    Recurso* recursoEncontrado = list_find(estadoBlockRecursos, coincideConSolicitado);
    if (recursoEncontrado == NULL) {
        log_info(info_logger,"Recurso <%s> solicitado INEXISTENTE", recursoSolicitado);
        log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>",unaPcb->id);
        moverProceso_ExecExit(unaPcb);
    } else {
        if (strcmp(orden,"WAIT") == 0) {
            waitRecursoPcb(recursoEncontrado, unaPcb);
        } else {
            signalRecursoPcb(recursoEncontrado, unaPcb);
        }
    }
    free(recursoSolicitado);
}

void waitRecursoPcb (Recurso* recurso, PCB* unaPcb) {
    recurso->instanciasRecurso--;
    list_add(unaPcb->recursosTomados,recurso);
    log_info(info_logger,"PID: <%d> - Wait: <%s> - Instancias: <%d>", unaPcb->id, recurso->nombreRecurso, recurso->instanciasRecurso);
    if (recurso->instanciasRecurso < 0) {
        log_info(info_logger,"PID: <%d> - Bloqueado por: <%s>",unaPcb->id,recurso->nombreRecurso);
        bloquearProcesoPorRecurso(recurso);
    } else {
    	t_paquete* paquete= crear_paquete(CONTEXTOEJECUCION, info_logger);
		agregar_ContextoEjecucion_a_paquete(paquete, unaPcb);
		enviar_paquete(paquete, cpuDispatch_fd);
		eliminar_paquete(paquete);
    }
}

void signalRecursoPcb (Recurso* recurso, PCB* unaPcb) {
    recurso->instanciasRecurso++;
    list_remove_element(unaPcb->recursosTomados,recurso);
    log_info(info_logger,"PID: <%d> - Signal: <%s> - Instancias: <%d>", unaPcb->id, recurso->nombreRecurso, recurso->instanciasRecurso);
    if (!list_is_empty(recurso->cola))
        moverProceso_BloqrecursoReady(recurso);
    t_paquete* paquete = crear_paquete(CONTEXTOEJECUCION, info_logger);
	agregar_ContextoEjecucion_a_paquete(paquete, unaPcb);
	enviar_paquete(paquete, cpuDispatch_fd);
	eliminar_paquete(paquete);
}

void bloquearProcesoPorRecurso(Recurso* recurso) {
    pthread_mutex_lock(&mutex_colaExec);
    PCB* pcbABlockedRecurso = list_remove(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);

    sem_post(&sem_cpuLibre);

    pthread_mutex_lock(&mutex_colaBloq);
	list_add(colaBloq,pcbABlockedRecurso);
	pthread_mutex_unlock(&mutex_colaBloq);

	pthread_mutex_lock(&semaforos_io[recurso->indiceSemaforo]);
	list_add(recurso->cola, pcbABlockedRecurso);
	pthread_mutex_unlock(&semaforos_io[recurso->indiceSemaforo]);

    log_info(info_logger,"PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED_RECURSO[%s]>", pcbABlockedRecurso->id, recurso->nombreRecurso);
}

void* esperaTiempo(void* aux1) {
	SleepCpu* aux2 = (SleepCpu *) aux1;

    sleep(aux2->segundosSleep);
    moverProceso_BloqReady(aux2->pcb);
    free(aux2);
}

PCB* buscarProcesoBloq(uint32_t pid) {
	for(int i=0 ; i<list_size(colaBloq) ; i++){
		PCB* pcbBuscado = list_get(colaBloq, i);
		if(pcbBuscado->id == pid)
			return pcbBuscado;
	}
	log_error(info_logger, "No se encontro el proceso bloqueado");
}