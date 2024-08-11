#include <comunicacion.h>

int contadorDispositivosIO = 0;
int memoria_fd;
int kernel_fd;

char* archivoAEscribir;


void* iniciarMemoria () {
	conectarMemoria("MEMORIA");
	return NULL;
}

int conectarMemoria(char *modulo) {
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	strcpy(charAux, modulo);
    t_log* loggerIOMem;
	loggerIOMem= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(configuracionEntradasalida,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_DISPATCH");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(configuracionEntradasalida,parametro);

	log_info(loggerIOMem, "IP=%s\n", ip);
	log_info(loggerIOMem, "PUERTO=%s\n", puerto);

	memoria_fd= crear_conexion(loggerIOMem, "Conecto dispositivo IO a Memoria",ip, puerto);
	

	int32_t handshakeEntradasalida;

	handshakeEntradasalida = cfg_entradaSalida->TIPO_INTERFAZ_INT;

	send(memoria_fd, &handshakeEntradasalida, sizeof(int32_t), 0);


	pthread_t tid;


	pthread_create(&tid, NULL, recibirMemoria, NULL);

	pthread_join(tid, NULL);

	log_destroy(loggerIOMem);

	return memoria_fd;
}

void* iniciarKernel () {
	conectarKernel("KERNEL");
	return NULL;
}

int conectarKernel(char *modulo) {
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	strcpy(charAux, modulo);
    t_log* loggerIOKernel;
	loggerIOKernel= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(configuracionEntradasalida,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_DISPATCH");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(configuracionEntradasalida,parametro);

	log_info(loggerIOKernel, "IP=%s\n", ip);
	log_info(loggerIOKernel, "PUERTO=%s\n", puerto);

	kernel_fd = crear_conexion(loggerIOKernel, "Conecto dispositivo IO a Kernel",ip, puerto);

	int32_t handshakeEntradasalida;

	handshakeEntradasalida = cfg_entradaSalida->TIPO_INTERFAZ_INT;

	send(kernel_fd, &handshakeEntradasalida, sizeof(int32_t), 0);

	log_info(info_logger, "ANTES DE cualInterfaz");

	cualInterfaz();

	log_destroy(loggerIOKernel);

	return kernel_fd;
}

t_log* iniciar_logger(char *nombre) {
	t_log* nuevo_logger;
	nuevo_logger = log_create(nombre, "tp", 1, LOG_LEVEL_INFO);
	if(nuevo_logger == NULL)
	{
		printf("No se genero de forma correcta el logger");
		exit(1);
	}

	return nuevo_logger;
}

void paquete(int conexion, t_log* logger) {	
	char *leido;
	t_paquete* paquete = crear_paquete(PAQUETECLIENTE,logger);

	leido = readline("> ");
	while(*leido != '\0'){
		agregar_a_paquete(paquete, leido, strlen(leido)+1);
		free(leido);
		leido = readline("> ");
	}

	enviar_paquete(paquete, conexion);
	free(paquete);
	free(leido);

	return;
}

void cualInterfaz() {
	pthread_t tid;

    switch (cfg_entradaSalida->TIPO_INTERFAZ_INT) {
    case 0:{
		pthread_create(&tid, NULL, recibirKernelStdout, NULL);
		pthread_join(tid, NULL);
        break;
	}
    case 1:{
		pthread_create(&tid, NULL, recibirKernelStdin, NULL);
		pthread_join(tid, NULL);
        break;
	}
    case 2:{
		pthread_create(&tid, NULL, recibirKernelDialfs, NULL);
		pthread_join(tid, NULL);

        break;
	}
    case 3:{
		pthread_create(&tid, NULL, recibirKernelGenerica, NULL);
		pthread_join(tid, NULL);
        break;
	} case 4:{
		pthread_create(&tid, NULL, recibirKernelGenerica, NULL);
		pthread_join(tid, NULL);
		break;
	} case 5:{
		pthread_create(&tid, NULL, recibirKernelGenerica, NULL);
		pthread_join(tid, NULL);
		break;
	} case 6:{
		pthread_create(&tid, NULL, recibirKernelGenerica, NULL);
		pthread_join(tid, NULL);
		break;
	}
    default:
        printf("Esa interfaz no existe :/");
        break;
    }

    return;
}

void terminar_programa(int conexion, t_log* logger) {
	log_destroy(logger);
	liberar_conexion(conexion);

	return;
}

void *recibirMemoria() {
	log_info(info_logger, "Entro a recibir memoria");

	while(1) {
		int cod_op = recibir_operacion(memoria_fd);
		if(!strcmp(cfg_entradaSalida->TIPO_INTERFAZ, "DIALFS")) {
				switch (cod_op) {
				case LECTURA_REALIZADA:{
					log_info(info_logger, "Solicitud IO DIALFS Cumplida");
					t_datos* datos = malloc(sizeof(t_datos));
					t_list* listaInts = list_create();
					listaInts = recibirListaIntsYDatos(memoria_fd, datos);
					uint32_t pid = *(uint32_t*) list_get(listaInts, 0);
					uint32_t tamanio = *(uint32_t*) list_get(listaInts, 2);
					uint32_t puntero = *(uint32_t*) list_get(listaInts, 3);
					logEscribirArchivo(pid, archivoAEscribir, tamanio, puntero);
					escribirArchivo(archivoAEscribir, datos->datos, puntero, tamanio);
					enviarListaUint32_t(listaInts, kernel_fd, info_logger, SOLICITUD_IO_CUMPLIDA);
					break;
				}
				case ESCRITURA_REALIZADA:{
					log_info(info_logger, "Solicitud IO DIALFS Cumplida");
					uint32_t valor = recibirValor_uint32(memoria_fd);
					t_list* listaInts = list_create();
					list_add(listaInts, &valor);
					enviarListaUint32_t(listaInts, kernel_fd, info_logger, SOLICITUD_IO_CUMPLIDA);
					break;
				}
				case -1:{
					log_error(info_logger, "El cliente se desconecto");
					return NULL;
					break;
				}
				default:{
					log_warning(info_logger, "Operacion desconocida, cuidado: %d", cod_op);//DialFS cae siempre aca
					break;

				}		
			}
		} else {
				switch (cod_op) {
				case LECTURA_REALIZADA:{
					devolucionIO_STDOUT_WRITE(&memoria_fd);
					break;
				}
				case ESCRITURA_REALIZADA:{
					log_info(info_logger, "Solicitud IO STDIN Cumplida");
					uint32_t valor = recibirValor_uint32(memoria_fd);
					t_list* listaInts = list_create();
					list_add(listaInts, &valor);
					enviarListaUint32_t(listaInts, kernel_fd, info_logger, SOLICITUD_IO_CUMPLIDA);
					break;
				}
				case -1:{
					log_error(info_logger, "El cliente se desconecto");
					return NULL;
					break;
				}
				default:{
					log_warning(info_logger, "Operacion desconocida, cuidado: %d", cod_op);//DialFS cae siempre aca
					break;

				}		
			}
		}
	}

	return NULL;
}


void* recibirKernelStdin() {
	while(1) {

		int cod_op = recibir_operacion(kernel_fd);

		switch (cod_op)
		{
		case IO_STDIN_READ:
			solicitudIO_STDIN_READ(&kernel_fd);
			break;
		
		case -1:
			log_error(info_logger, "El cliente se desconecto");
			return NULL;
			break;
		
		default:
			log_warning(info_logger, "Operacion desconocida, cuidado: %d", cod_op);
			break;
		}
	}

	return NULL;
}

void* recibirKernelStdout() {
	while(1) {		

		int cod_op = recibir_operacion(kernel_fd);

		switch (cod_op)
		{
		case IO_STDOUT_WRITE:
			solicitudIO_STDOUT_WRITE(&kernel_fd);
			break;

		case -1:
			log_error(info_logger, "El cliente se desconecto");
			return NULL;
			break;
		
		default:
			log_warning(info_logger, "Operacion desconocida, cuidado: %d", cod_op);
			break;
		}
	}
	
	return NULL;
}

void* recibirKernelGenerica() {
	while(1) {
	
		int cod_op = recibir_operacion(kernel_fd);

		switch (cod_op)
		{
		case IO_GEN_SLEEP: 	
			solicitudIO_GEN_SLEEP(&kernel_fd);
			break;
		
		case -1:
			log_error(info_logger, "El cliente se desconecto");
			return NULL;
			break;
		
		default:
			log_warning(info_logger, "Operacion desconocida, cuidado: %d", cod_op);
				break;
		}
	}

	return NULL;
}

void* recibirKernelDialfs() {
	while(1) {
		int cod_op = recibir_operacion(kernel_fd);
		//log_trace(trace_logger, "ENTRE A RECIBIR KERNEL");

		switch (cod_op) {
		case IO_FS_CREATE:{
			uint32_t pid;
			//log_info(info_logger, "PID ANTES DE RECIBIR: %d", pid);
			char* nombreArchivo = recibirEnteroYString(kernel_fd, &pid);
			//log_info(info_logger, "PID DSP DE RECIBIR: %d", pid);
			crearArchivo(nombreArchivo);
			logCrearArchivo(pid, nombreArchivo);
			t_list* listaInts = list_create();
			list_add(listaInts, &pid);
			enviarListaUint32_t(listaInts, kernel_fd, info_logger, SOLICITUD_IO_CUMPLIDA);
			break;
		}
		case IO_FS_DELETE:{
			uint32_t pid;
			char* nombreArchivo = recibirEnteroYString(kernel_fd, &pid);
			eliminarArchivo(nombreArchivo);
			logEliminarArchivo(pid, nombreArchivo);
			t_list* listaInts = list_create();
			list_add(listaInts, &pid);
			enviarListaUint32_t(listaInts, kernel_fd, info_logger, SOLICITUD_IO_CUMPLIDA);
			break;
		}
		case IO_FS_READ:{
			t_list* listaInts = list_create();
			t_datos* datos = malloc(sizeof(t_datos));
			uint32_t pid;
			uint32_t dirFisica;
			uint32_t tamanio;
			uint32_t puntero;
			//log_info(info_logger, "ANTES DE RECIBIR");
			char* nombreArchivo = recibirEnteroEnteroEnteroEnteroChar(kernel_fd, &pid, &tamanio, &puntero, &dirFisica);
			//log_info(info_logger, "nombreArchivo: %s, tamanio: %d, puntero: %d", nombreArchivo, tamanio, puntero);
			datos->datos = leerArchivo(nombreArchivo, puntero, tamanio);
			datos->tamanio = tamanio;
			logLeerArchivo(pid, nombreArchivo, tamanio, puntero);
			list_add(listaInts, &pid);
			list_add(listaInts, &dirFisica);
			list_add(listaInts, &tamanio);
			enviarListaIntsYDatos(listaInts, datos, memoria_fd, info_logger, ACCESO_PEDIDO_ESCRITURA);
			break;
		}
		case IO_FS_TRUNCATE:{
			uint32_t pid;
			uint32_t tamanio;
			char* nombreArchivo = recibirEnteroEnteroChar(kernel_fd, &pid, &tamanio);
			logTruncarArchivo(pid, nombreArchivo, tamanio);
			truncarArchivo(nombreArchivo, tamanio);
			t_list* listaInts = list_create();
			list_add(listaInts, &pid);
			enviarListaUint32_t(listaInts, kernel_fd, info_logger, SOLICITUD_IO_CUMPLIDA);
			break;
		}
		case IO_FS_WRITE:{
			uint32_t pid;
			uint32_t tamanio;
			uint32_t punteroArchivo;
			uint32_t direccionFisica;
			archivoAEscribir = recibirEnteroEnteroEnteroEnteroChar(kernel_fd, &pid, &tamanio, &punteroArchivo, &direccionFisica);
			t_list* listaInts = list_create();
			list_add(listaInts, &pid);
			list_add(listaInts, &direccionFisica);
			list_add(listaInts, &tamanio);
			list_add(listaInts, &punteroArchivo);
			enviarListaUint32_t(listaInts, memoria_fd, info_logger, ACCESO_PEDIDO_LECTURA);
			break;
		}
		case -1:
			log_error(info_logger, "El cliente se desconecto");
			return NULL;
			break;
		
		default:
			log_warning(info_logger, "Operacion desconocida, cuidado: %d", cod_op);
			break;
		}
	}

	return NULL;
}


void* devolucionIO_STDOUT_WRITE(void* cliente_socket) {  //Esta funcion puede causar problemas. Estar al tanto.
	int conexion = *((int*) cliente_socket);

	t_datos* datitos = malloc(sizeof(t_datos));

	t_list* listaInts;

	listaInts = list_create();

	listaInts = recibirListaIntsYDatos(conexion, datitos);

	uint32_t pid = *(uint32_t*)list_get(listaInts, 0);
	uint32_t tamanio = *(uint32_t*)list_get(listaInts,2);
	
    char buffer[tamanio + 1]; 
    memcpy(buffer, datitos->datos, tamanio);
    buffer[tamanio] = '\0'; 

    printf("\n\n PID <%d> - <%s>\n\n", pid, buffer);

	enviarListaUint32_t(listaInts, kernel_fd, info_logger, SOLICITUD_IO_CUMPLIDA);
	return NULL;
}


void* solicitudIO_STDOUT_WRITE(void* cliente_socket) {
	
	int conexion = *((int*) cliente_socket);

	t_list* listaEnteros = list_create();
	listaEnteros = recibirListaUint32_t(conexion); // 0 pid, 1 dirfisica, 2 tamanio
	uint32_t pid = *(uint32_t*)list_get(listaEnteros, 0);
	enviarListaUint32_t(listaEnteros, memoria_fd, info_logger, ACCESO_PEDIDO_LECTURA);

	log_info(info_logger, "PID: <%d> Direccion fisica enviada a memoria", pid);
	logOperacion(pid, "IO_STDOUT_WRITE");

	return NULL;
}

void *solicitudIO_STDIN_READ(void* cliente_socket) {

	int conexion = *((int*) cliente_socket);

	t_list* listaEnteros = list_create();
	listaEnteros = recibirListaUint32_t(conexion);	// 0 pid, 1 dirfisica, 2 tamanio
	uint32_t pid = *(uint32_t*)list_get(listaEnteros, 0); 
	//enviarListaUint32_t(listaEnteros, memoria_fd, info_logger, ACCESO_PEDIDO_LECTURA);

	logOperacion(pid, "IO_STDIN_READ");
	manejarInterfazStdin(listaEnteros);
	
	return NULL;
}

void* solicitudIO_GEN_SLEEP (void* cliente_socket) {

	int conexion = *((int*) cliente_socket);
	
	t_list* listaEnteros = list_create();
	listaEnteros = recibirListaUint32_t(conexion);

	uint32_t pid = *(uint32_t*)list_get(listaEnteros, 0);
	uint32_t unidadesDeTrabajo = *(uint32_t*)list_get(listaEnteros, 1);

	logOperacion(pid, "IO_GEN_SLEEP");
	manejarInterfazGenerica(unidadesDeTrabajo);

	enviarListaUint32_t(listaEnteros, kernel_fd, info_logger, SOLICITUD_IO_CUMPLIDA);

	return NULL;
}

