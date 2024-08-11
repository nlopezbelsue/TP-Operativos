#include <comunicacion.h>
#include "memoriaInstrucciones.h"

int memoria_fd;
int cpu_fd;
int interfazIO_fd[4];
int stdout_fd;
int stdin_fd;
int dialfs_fd;
int generica_fd;
int kernel_fd;
t_log* logger;

uint32_t pidGlobal;
uint32_t sizeGlobal;

int recibirConexion(char *puerto) {
	logger = log_create("modulo.log", "-", 1, LOG_LEVEL_INFO);
	pthread_t tid[5];
//	pthread_t hilosIO[4];
//	int contadorIO=0;

	memoria_fd = iniciar_servidor(logger, "Server Memoria" , puerto);
	log_info(logger, "Servidor listo para recibir a los clientes");

	cpu_fd = esperar_cliente(memoria_fd);
	pthread_create(&tid[3], NULL, recibirCPU, NULL);
	kernel_fd = esperar_cliente(memoria_fd);
	pthread_create(&tid[4], NULL, recibirKernel, NULL);
	int32_t tipoInterfaz;
	int entradasalida_fd;
	while (1) {
		entradasalida_fd = esperar_cliente(memoria_fd);
		recv(entradasalida_fd, &tipoInterfaz, sizeof(int32_t), MSG_WAITALL);
		interfazIO_fd[tipoInterfaz] = entradasalida_fd;
		cualInterfaz(tipoInterfaz);
		void* fdDeIo = &interfazIO_fd[tipoInterfaz];
		pthread_create(&tid[tipoInterfaz], NULL, recibirIO, fdDeIo);
		pthread_detach(tid[tipoInterfaz]);
	}
	//pthread_join(tid[0], NULL);
	//pthread_join(tid[1], NULL);
	// pthread_join(tid[2], NULL);

	return EXIT_SUCCESS;
}


void cualInterfaz(int tipoInterfaz){

	t_log* logger;
	logger = log_create("modulo.log", "-", 1, LOG_LEVEL_INFO);

	switch (tipoInterfaz)
	{
	case 0: //STDOUT
		log_info(logger, "Interfaz STDOUT conectada");
		break;
	case 1: //STDIN
		log_info(logger, "Interfaz STDIN conectada");
		break;
	case 2: //DIAL_FS
		log_info(logger, "Interfaz DIALFS conectada");
		break;
	default:
		break;
	}

	log_destroy(logger);
}

 void *recibirCPU(void) {
	while(1) {
		int cod_op = recibir_operacion(cpu_fd);
		t_list *lista;
		switch (cod_op) {
			case HANDSHAKE_CPU:
				recibirOrden(cpu_fd);
				log_info(logger,"HANDSHAKE con CPU acontecido");
				PaqueteHand(cpu_fd, logger);
				break;
			case SOLICITUDMARCO:
				lista = recibirListaUint32_t(cpu_fd);
				uint32_t marco = obtenerMarcoDePagina(*(uint32_t*)list_get(lista,0), *(uint32_t*)list_get(lista,1));
				enviarValor_uint32(marco, cpu_fd, SOLICITUDMARCO, info_logger);
//					list_clean(lista);
				// list_destroy_and_destroy_elements(lista, free); //LINEA MODIFICADA
				break;
			case ACCESO_PEDIDO_LECTURA:
				realizarPedidoLectura(cpu_fd);
				break;

			case ACCESO_PEDIDO_LECTURA_UINT:
				//realizarPedidoLectura(cpu_fd);
				realizarPedidoLecturaCpu();
				break;

			case ACCESO_PEDIDO_ESCRITURA:
				realizarPedidoEscritura(cpu_fd);
				break;

			case RESIZE:
				lista = recibirListaUint32_t(cpu_fd);
				uint32_t resultado = resizeProceso(*(uint32_t*)list_get(lista,0), *(uint32_t*)list_get(lista,1)); //pid tamanio
				enviarValor_uint32(resultado, cpu_fd, RESIZE, info_logger);

				break;

			case SOLICITUDINSTRUCCION:
				lista = recibirListaUint32_t(cpu_fd);
				t_paquete* paquete = crear_paquete(SOLICITUDINSTRUCCION, info_logger);
				Instruccion* instruccion;
				usleep(RETARDO_RESPUESTA*1000); //ver si cambiar a sleep

				instruccion = retornarInstruccionACPU(*(uint32_t*)list_get(lista,0),*(uint32_t*)list_get(lista,1)); // pid y pc
				log_trace(trace_logger, "PID: %d PC: %d", *(uint32_t*)list_get(lista,0),*(uint32_t*)list_get(lista,1));
				//log_trace(trace_logger, "Instruccion: %s %s %s", instruccion->id, instruccion->param1, instruccion->param2);
				// log_info(info_logger, "instruccion: %s %s %s %s %s %s\n", instruccion->id, instruccion->param1, instruccion->param2
				// 														, instruccion->param3, instruccion->param4, instruccion->param5);
				agregar_instruccion_a_paquete(paquete, instruccion);
				enviar_paquete(paquete, cpu_fd);
				eliminar_paquete(paquete);
//					list_clean(lista);
//					list_destroy_and_destroy_elements(lista, free); // ESTO ERA SOLO LIST_DESTRO
				break;
			case -1:
				log_error(logger, "el cliente se desconecto.");
				log_error(logger, "Terminando servidor.CPU");
				return NULL;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
		}
	}
}

void *recibirIO(void* interfaz_fd) {

	int conexion = *((int*) interfaz_fd);
	log_info(info_logger, "Conecto con una interfaz");
  	while(1) {
  		int cod_op = recibir_operacion(conexion); //seguro se necesita un mutex;
  		switch (cod_op) {
 			case ACCESO_PEDIDO_ESCRITURA:{
				realizarPedidoEscritura(conexion);
 				break;
			} //ACCESO_PEDIDO_ESCRITURA
 			case ACCESO_PEDIDO_LECTURA:{
				realizarPedidoLectura(conexion);
 				break;
			} //ACCESO_PEDIDO_LECTURA
  			case -1:{
  				log_error(logger, "el cliente se desconecto.");
  				log_error(logger, "Terminando servidor. I/O");
  				return NULL;
			}
  			default:{
  				log_warning(logger,"Operacion desconocida. No quieras meter la pata %d ", cod_op);
  				break;
			}
  		}
  	}
}


void *recibirKernel() {
	while(1) {
		int cod_op = recibir_operacion(kernel_fd);
 		switch (cod_op) {
        	case INICIALIZAR_PROCESO_MEMORIA: 
        		inicializarProceso(kernel_fd);
        		break;

        	case FINALIZAR_PROCESO_MEMORIA:
        		finalizarProceso(kernel_fd);
        		break;

 			case -1:
 				log_error(logger, "el cliente se desconecto.");
 				log_error(logger, "Terminando servidor KERNEL");
 				return NULL;
				
 			default:
 				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
 				break;
 		}
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

void PaqueteHand(int conexion, t_log* logger){
	t_paquete* paquete = crear_paquete(HANDSHAKE_CPU,logger);

	char buffer[20];
	sprintf(buffer, "%d", TAM_PAGINA);

	agregar_a_paquete(paquete, buffer, strlen(buffer)+1);

	enviar_paquete(paquete, conexion);
	free(paquete);
}

void realizarPedidoLectura(int cliente_socket){			//envia LECTURA_REALIZADA
    t_list* listaInts = recibirListaUint32_t(cliente_socket);	// 0 pid, 1 dirfisica, 2 tamanio
    uint32_t pid = *(uint32_t*)list_get(listaInts,0);
    uint32_t posicion = *(uint32_t*)list_get(listaInts,1);
    uint32_t tamanio = *(uint32_t*)list_get(listaInts,2);

    void* datos = malloc(tamanio);
	datos = manejarLectura(posicion, tamanio, pid);
    t_datos* unosDatos = malloc(sizeof (t_datos));
    unosDatos->datos = datos;
    unosDatos->tamanio = tamanio;
    enviarListaIntsYDatos(listaInts, unosDatos, cliente_socket, info_logger, LECTURA_REALIZADA);
    free(datos);
    free(unosDatos);
    list_clean_and_destroy_elements(listaInts,free);
    list_destroy(listaInts);
}

void* manejarLectura(uint32_t posInicial, uint32_t tamanio, uint32_t pid) {
	void* datos = malloc(tamanio);
	uint32_t* lei = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* frameQueCorresponde = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* tamPrimerFrame = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* pagActual = (uint32_t*)malloc(sizeof(uint32_t));

	*lei = 0;
	*frameQueCorresponde = posInicial / TAM_PAGINA;
	*tamPrimerFrame = TAM_PAGINA * (*frameQueCorresponde + 1) - posInicial;
	
	while(tamanio > *tamPrimerFrame) {
		pthread_mutex_lock(&mutex_espacioContiguo);
		memcpy(datos+*lei, leerMemoria(posInicial, *tamPrimerFrame, pid), *tamPrimerFrame);
		pthread_mutex_unlock(&mutex_espacioContiguo);

		*frameQueCorresponde = posInicial / TAM_PAGINA;
		//*tamPrimerFrame = TAM_PAGINA * (*frameQueCorresponde + 1) - posInicial; //cuanto puedo leer en el frame actual
		*lei += *tamPrimerFrame;
		*pagActual = obtenerPaginaConMarco(*frameQueCorresponde);
		*frameQueCorresponde = obtenerMarcoDePagina(pid, *pagActual+1);
		posInicial = *frameQueCorresponde * TAM_PAGINA;
		tamanio = tamanio - *tamPrimerFrame;
		*tamPrimerFrame = TAM_PAGINA;
		log_trace(trace_logger, "LEO en la siguiente pagina");
	}
	pthread_mutex_lock(&mutex_espacioContiguo);
	memcpy(datos+*lei, leerMemoria(posInicial, tamanio, pid), tamanio);
	pthread_mutex_unlock(&mutex_espacioContiguo);

	// free(lei);
	// free(frameQueCorresponde);
	// free(tamPrimerFrame);
	// free(pagActual);

	return datos;
}

void manejarEscritura(uint32_t posInicial, void* datos, uint32_t tamanio, uint32_t pid) {
	uint32_t* offset = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* frameQueCorresponde = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* tamPrimerFrame = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* pagActual = (uint32_t*)malloc(sizeof(uint32_t));

	*offset = 0;
	*frameQueCorresponde = posInicial / TAM_PAGINA;
	*tamPrimerFrame = TAM_PAGINA * (*frameQueCorresponde + 1) - posInicial;

	//log_trace(trace_logger, "ME MANDARON A ESCRIBIR: %s", datos);
	
	while(tamanio > *tamPrimerFrame) {
		log_trace(trace_logger, "tamanio a leer %d, tamanio que puedo leer en esta pag %d", tamanio, *tamPrimerFrame);
		pthread_mutex_lock(&mutex_espacioContiguo);
		escribirMemoria(posInicial, datos+*offset, *tamPrimerFrame, pid);
		pthread_mutex_unlock(&mutex_espacioContiguo);
		*frameQueCorresponde = posInicial / TAM_PAGINA;
		*tamPrimerFrame = TAM_PAGINA * (*frameQueCorresponde + 1) - posInicial; //cuanto puedo leer en el frame actual 
		*offset += *tamPrimerFrame;
		*pagActual = obtenerPaginaConMarco(*frameQueCorresponde);
		*frameQueCorresponde = obtenerMarcoDePagina(pid, *pagActual+1);
		posInicial = *frameQueCorresponde * TAM_PAGINA;
		tamanio = tamanio - *tamPrimerFrame;
		*tamPrimerFrame = TAM_PAGINA;
		log_trace(trace_logger, "escribo en la siguiente pagina");
	}
	pthread_mutex_lock(&mutex_espacioContiguo);
	escribirMemoria(posInicial, datos+*offset, tamanio, pid);
	pthread_mutex_unlock(&mutex_espacioContiguo);

	// free(offset);
	// free(frameQueCorresponde);
	// free(tamPrimerFrame);
	// free(pagActual);

	return;
}

void realizarPedidoEscritura(int cliente_socket){		//Vale para io y cpu. Les manda ESCRITURA_REALIZADA
    t_datos* unosDatos = malloc(sizeof(t_datos));
    t_list* listaInts = recibirListaIntsYDatos(cliente_socket, unosDatos);
    uint32_t* posicion = list_get(listaInts,1);
    uint32_t* pid = list_get(listaInts,0);
	uint32_t* tamanio = list_get(listaInts, 2);
    log_trace(trace_logger,"Accediendo a Espacio de Usuario para Escritura en la Direccion: <%d> para el Proceso con PID: <%d>", *posicion, *pid);
   // log_trace(trace_logger,"me llego pid %d, pos %d y tamanio %d y valor %d", *pid, *posicion, *tamanio, unosDatos->datos);
	manejarEscritura(*posicion, unosDatos->datos, *tamanio, *pid);
    log_trace(trace_logger,"Se accedio a Espacio de Usuario correctamente");
    free(unosDatos->datos);
    free(unosDatos);

	enviarValor_uint32(*pid, cliente_socket, ESCRITURA_REALIZADA, info_logger);
    //enviarOrden(ESCRITURA_REALIZADA, cliente_socket, info_logger);	// Memoria tiene que darle el pid a IO
    list_clean_and_destroy_elements(listaInts, free);
    list_destroy(listaInts);
}

void realizarPedidoLecturaCpu(){
    t_list* listaInts = recibirListaUint32_t(cpu_fd);	// 0 pid, 1 dirfisica, 2 tamanio
    uint32_t pid = *(uint32_t*)list_get(listaInts,0);
    uint32_t posicion = *(uint32_t*)list_get(listaInts,1);
    uint32_t tamanio = *(uint32_t*)list_get(listaInts,2);

    log_trace(trace_logger,"Accediendo a Espacio de Usuario para Lectura en la Dirección: <%d> de Tamanio: <%d> para el Proceso con PID: <%d>", posicion, tamanio, pid);

	uint32_t datos = manejarLecturaCpu(posicion, tamanio, pid);
    log_trace(trace_logger,"Se accedió a Espacio de Usuario correctamente");
  //  log_trace(trace_logger,"MANDO A CPU: %d", datos);

	list_add(listaInts,&datos);
    enviarListaUint32_t(listaInts, cpu_fd, info_logger, LECTURA_REALIZADA_UINT);
}


uint32_t manejarLecturaCpu(uint32_t posInicial, uint32_t tamanio, uint32_t pid) {
	uint32_t datos = 0;
	void* datosPuntero = &datos;
	uint32_t* lei = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* frameQueCorresponde = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* tamPrimerFrame = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t* pagActual = (uint32_t*)malloc(sizeof(uint32_t));

	*lei = 0;
	*frameQueCorresponde = posInicial / TAM_PAGINA;
	*tamPrimerFrame = TAM_PAGINA * (*frameQueCorresponde + 1) - posInicial;

	while(tamanio > *tamPrimerFrame) {
	    pthread_mutex_lock(&mutex_espacioContiguo);
		memcpy(datosPuntero+*lei, leerMemoria(posInicial, *tamPrimerFrame, pid), *tamPrimerFrame);
	    pthread_mutex_unlock(&mutex_espacioContiguo);

		*frameQueCorresponde = posInicial / TAM_PAGINA;

		*lei += *tamPrimerFrame;										

		*pagActual = obtenerPaginaConMarco(*frameQueCorresponde);
		*frameQueCorresponde = obtenerMarcoDePagina(pid, *pagActual+1);
		posInicial = *frameQueCorresponde * TAM_PAGINA;
		tamanio = tamanio - *tamPrimerFrame;
		*tamPrimerFrame = TAM_PAGINA;
		log_trace(trace_logger, "LEO en la siguiente pagina");
	}
	pthread_mutex_lock(&mutex_espacioContiguo);
	memcpy(datosPuntero+*lei, leerMemoria(posInicial, tamanio, pid), tamanio);
	pthread_mutex_unlock(&mutex_espacioContiguo);

	free(lei);
	free(frameQueCorresponde);
	free(tamPrimerFrame);
	free(pagActual);

	return datos;
}

void inicializarProceso(int cliente_socket){

    char* file_name = recibirEnteroEnteroChar(cliente_socket, &pidGlobal, &sizeGlobal); 
    log_trace(trace_logger, "%s %d %d", file_name, pidGlobal, sizeGlobal);

	GuardarInstrucsDeProceso(pidGlobal, file_name);

    crearTablaPaginasProceso(pidGlobal, sizeGlobal);
	sleep(1);
	log_trace(trace_logger,"Tamaño de tablaGeneral antes de terminar inicializarProceso: %d", list_size(tablaGeneral));

	enviarOrden(INICIALIZAR_PROCESO_MEMORIA,cliente_socket, trace_logger);
}

void finalizarProceso(int cliente_socket){
	//log_trace(trace_logger,"Tamaño de tabla General al llegar a finalizar Proceso: %d", list_size(tablaGeneral));
    pthread_mutex_lock(&mutex_espacioContiguo);
    uint32_t pid = recibirValor_uint32(cliente_socket);
	TablaDePaginas* tabla = obtenerTablaPorPID(pid);
	if (tabla!=NULL){
		for (int j = 0; j < list_size(tabla->paginas); j++) {
			Pagina* pagina = list_get(tabla->paginas, j);
			marcarMarcoLibre(pagina->marco);
		}
	}

	bool buscarPorPID(void* programa){
		return ((Proceso*)programa)->pid == pid;
	}

	Proceso* programa = list_find(instruccionesDeProcesos, buscarPorPID);
	for(int i=0;i<list_size(programa->instrucciones);i++){
		Instruccion* instruccion = list_get(programa->instrucciones,i);
		free(instruccion);
	}

	list_remove_by_condition(instruccionesDeProcesos, buscarPorPID);
	//free(programa);


    if(tabla!=NULL)
    	liberarTablaDePaginas(pid);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    enviarOrden(FINALIZAR_PROCESO_MEMORIA, cliente_socket, info_logger);
    log_trace(trace_logger, "PID: <%d> - Proceso finalizado con éxito", pid);
    //log_trace(trace_logger,"Tamaño de tablaGeneral al terminar finalizarProceso: %d", list_size(tablaGeneral));
}
