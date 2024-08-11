#include "cicloInstruccion.h"

bool cicloInstrucciones=true;
char* nombre_instruccion_actual;
Instruccion* instruccion;
int interrupciones;
int desalojo;


uint8_t registroCPU_AX;
uint8_t registroCPU_BX;
uint8_t registroCPU_CX;
uint8_t registroCPU_DX;
uint32_t registroCPU_EAX;
uint32_t registroCPU_EBX;
uint32_t registroCPU_ECX;
uint32_t registroCPU_EDX;
uint32_t registroCPU_SI;
uint32_t registroCPU_DI;


void ciclo_de_instruccion(){
    
    cicloInstrucciones=true;
    interrupciones=0;
	desalojo=0;

    while(cicloInstrucciones){

        fetch();

        decode();

        execute();

        checkInsterrupt();

    }
}

void fetch(){
	t_list* listaInts = list_create();

	log_info(info_logger, "PID: <%d> - FETCH - Program Counter: <%d>", PCB_Actual->id, PCB_Actual->program_counter);
	
	list_add(listaInts, &PCB_Actual->id);
	list_add(listaInts, &PCB_Actual->program_counter);

	enviarListaUint32_t(listaInts,memoria_fd, info_logger, SOLICITUDINSTRUCCION);

	list_clean(listaInts);
	list_destroy(listaInts);
}

void decode(){
	op_code_cliente cod;
    if(cicloInstrucciones){
		cod = recibir_operacion(memoria_fd);
	}
	else{
		cod = -1;
	}
	if(cod == SOLICITUDINSTRUCCION){        //en memoria mandar enviar instruccion
		instruccion = recibirInstruccion(memoria_fd);

		nombre_instruccion_actual=instruccion->id;
		log_info(info_logger,"instruccion recibida: %s", nombre_instruccion_actual);

	}
	else{
		if(cicloInstrucciones){
			log_error(error_logger,"FALLO en el recibo de la instruccion");
		}
		cicloInstrucciones=0;
	}
}

void execute(){
	if (strcmp(nombre_instruccion_actual , "SET") == 0) {
        char* registro = instruccion->param1;
        char* valor = instruccion->param2;

        log_info(info_logger, "PID: <%d> - Ejecutando: <SET> - <%s> - <%s>", PCB_Actual->id, registro, valor);

        SET(registro, valor);

    }
    else if (strcmp(nombre_instruccion_actual , "SUM") == 0) {
		char* registro = instruccion->param1;
		char* valor = instruccion->param2;

		log_info(info_logger, "PID: <%d> - Ejecutando: <SUM> - <%s> - <%s>", PCB_Actual->id, registro, valor);

		SUM(registro, valor);

	}
    else if (strcmp(nombre_instruccion_actual , "SUB") == 0) {
		char* registro = instruccion->param1;
		char* valor = instruccion->param2;

		log_info(info_logger, "PID: <%d> - Ejecutando: <SUB> - <%s> - <%s>", PCB_Actual->id, registro, valor);

		SUB(registro, valor);

	}
    else if (strcmp(nombre_instruccion_actual , "JNZ") == 0) {
    		char* registro = instruccion->param1;
    		char* valor = instruccion->param2;

    		log_info(info_logger, "PID: <%d> - Ejecutando: <JNZ> - <%s> - <%s>", PCB_Actual->id, registro, valor);

    		JNZ(registro, valor);

    	}
    else if (strcmp(nombre_instruccion_actual , "RESIZE") == 0) {
		char* tamanio = instruccion->param1;

		log_info(info_logger, "PID: <%d> - Ejecutando: <RESIZE> - <%s>", PCB_Actual->id, tamanio);

		resize(tamanio);

	}
	else if (strcmp(nombre_instruccion_actual , "COPY_STRING") == 0) {
		char* tamanio = instruccion->param1;

		log_info(info_logger, "PID: <%d> - Ejecutando: <COPY_STRING> - <%s>", PCB_Actual->id, tamanio);

		COPY_STRING(tamanio);

	}
    else if (strcmp(nombre_instruccion_actual, "WAIT") == 0) {
		char* recurso = instruccion->param1;
		log_info(info_logger, "PID: <%d> - Ejecutando <WAIT> - <%s>", PCB_Actual->id, recurso);

		ejecutar_WAIT(recurso);
	}
	else if (strcmp(nombre_instruccion_actual, "SIGNAL") == 0) {
		char* recurso = instruccion->param1;
		log_info(info_logger, "PID: <%d> - Ejecutando <SIGNAL> - <%s>", PCB_Actual->id, recurso);

		ejecutar_SIGNAL(recurso);
	}
    else if (strcmp(nombre_instruccion_actual, "MOV_IN") == 0) {
        char* registro = instruccion->param1;
        int direccion_logica = obtener_valor_registroCPU(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <MOV_IN> - <%s> - <%d>", PCB_Actual->id, registro, direccion_logica);

        ejecutar_MOV_IN(registro, direccion_logica);
    }

    else if (strcmp(nombre_instruccion_actual, "MOV_OUT") == 0) {

        int direccion_logica = obtener_valor_registroCPU(instruccion->param1);
        char* registro = instruccion->param2;

        log_info(info_logger, "PID: <%d> - Ejecutando: <MOV_OUT> - <%d> - <%s>", PCB_Actual->id, direccion_logica, registro);

        ejecutar_MOV_OUT(direccion_logica, registro);
    }

	else if (strcmp(nombre_instruccion_actual, "IO_GEN_SLEEP") == 0) {

        char* interfaz = instruccion->param1;
        char* unidadesDeTrabajo = instruccion->param2;

        log_info(info_logger, "PID: <%d> - Ejecutando: <IO_GEN_SLEEP> - <%s> - <%s>", PCB_Actual->id, interfaz, unidadesDeTrabajo);

		//Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.
        ioGenSleep(interfaz, unidadesDeTrabajo);
    }

	else if (strcmp(nombre_instruccion_actual, "IO_STDIN_READ") == 0) {

        char* interfaz = instruccion->param1;
        char* registroDireccion = instruccion->param2;
		char* registroTamanio = instruccion->param3;

        log_info(info_logger, "PID: <%d> - Ejecutando: <IO_STDIN_READ> - <%s> - <%s> - <%s>", PCB_Actual->id, interfaz, registroDireccion, registroTamanio);

		//Esta instrucción solicita al Kernel que mediante la interfaz ingresada se lea desde el STDIN (Teclado) un valor cuyo tamaño está delimitado por el valor del Registro Tamaño y el mismo se guarde a partir de la Dirección Lógica almacenada en el Registro Dirección.
        ioStdinRead(interfaz, registroDireccion, registroTamanio);
    }

	else if (strcmp(nombre_instruccion_actual, "IO_STDOUT_WRITE") == 0) {

        char* interfaz = instruccion->param1;
        char* registroDireccion = instruccion->param2;
		char* registroTamanio = instruccion->param3;

        log_info(info_logger, "PID: <%d> - Ejecutando: <IO_STDOUT_WRITE> - <%s> - <%s> - <%s>", PCB_Actual->id, interfaz, registroDireccion, registroTamanio);

		//Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde la posición de memoria indicada por la Dirección Lógica almacenada en el Registro Dirección, un tamaño indicado por el Registro Tamaño y se imprima por pantalla.
        ioStdOutWrite(interfaz, registroDireccion, registroTamanio);
    }

	else if (strcmp(nombre_instruccion_actual, "IO_FS_CREATE") == 0) {

        char* interfaz = instruccion->param1;
        char* nombreArchivo = instruccion->param2;

        log_info(info_logger, "PID: <%d> - Ejecutando: <IO_FS_CREATE> - <%s> - <%s>", PCB_Actual->id, interfaz, nombreArchivo);

		//Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se cree un archivo en el FS montado en dicha interfaz.
        ioFsCreate(interfaz, nombreArchivo);
    }

	else if (strcmp(nombre_instruccion_actual, "IO_FS_DELETE") == 0) {

        char* interfaz = instruccion->param1;
        char* nombreArchivo = instruccion->param2;

        log_info(info_logger, "PID: <%d> - Ejecutando: <IO_FS_DELETE> - <%s> - <%s>", PCB_Actual->id, interfaz, nombreArchivo);

		//Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se elimine un archivo en el FS montado en dicha interfaz
        ioFsDelete(interfaz, nombreArchivo);
    }

	else if (strcmp(nombre_instruccion_actual, "IO_FS_TRUNCATE") == 0) {

        char* interfaz = instruccion->param1;
        char* nombreArchivo = instruccion->param2;
		char* registroTamanio = instruccion->param3;

        log_info(info_logger, "PID: <%d> - Ejecutando: <IO_FS_TRUNCATE> - <%s> - <%s> - <%s>", PCB_Actual->id, interfaz, nombreArchivo, registroTamanio);

		//Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se modifique el tamaño del archivo en el FS montado en dicha interfaz, actualizando al valor que se encuentra en el registro indicado por Registro Tamaño.
        ioFsTruncate(interfaz, nombreArchivo, registroTamanio);
    }

	else if (strcmp(nombre_instruccion_actual, "IO_FS_WRITE") == 0) {

        char* interfaz = instruccion->param1;
        char* nombreArchivo = instruccion->param2;
		char* registroDireccion = instruccion->param3;
		char* registroTamanio = instruccion->param4;
		char* registroPunteroArchivo = instruccion->param5;

        log_info(info_logger, "PID: <%d> - Ejecutando: <IO_FS_WRITE> - <%s> - <%s> - <%s> - <%s> - <%s>", PCB_Actual->id, interfaz, nombreArchivo, registroDireccion, registroTamanio, registroPunteroArchivo);

		//Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde Memoria la cantidad de bytes indicadas por el Registro Tamaño a partir de la dirección lógica que se encuentra en el Registro Dirección y se escriban en el archivo a partir del valor del Registro Puntero Archivo.
        ioFsWrite(interfaz, nombreArchivo, registroDireccion, registroTamanio, registroPunteroArchivo);
    }

	else if (strcmp(nombre_instruccion_actual, "IO_FS_READ") == 0) {

        char* interfaz = instruccion->param1;
        char* nombreArchivo = instruccion->param2;
		char* registroDireccion = instruccion->param3;
		char* registroTamanio = instruccion->param4;
		char* registroPunteroArchivo = instruccion->param5;

        log_info(info_logger, "PID: <%d> - Ejecutando: <IO_FS_READ> - <%s> - <%s> - <%s> - <%s> - <%s>", PCB_Actual->id, interfaz, nombreArchivo, registroDireccion, registroTamanio, registroPunteroArchivo);

		//Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se lea desde el archivo a partir del valor del Registro Puntero Archivo la cantidad de bytes indicada por Registro Tamaño y se escriban en la Memoria a partir de la dirección lógica indicada en el Registro Dirección.
        ioFsRead(interfaz, nombreArchivo, registroDireccion, registroTamanio, registroPunteroArchivo);
    }

    else if (!strncmp(nombre_instruccion_actual, "EXIT", strlen("EXIT"))) {
        log_info(info_logger, "PID: <%d> - Ejecutando: <EXIT>", PCB_Actual->id);

        ejecutar_EXIT();
    } else {
		if(cicloInstrucciones){
        	log_info(info_logger, "PID: <%d> - NO EXISTE LA INSTRUCCION %s", PCB_Actual->id, nombre_instruccion_actual); // log_error() ?
		}

    }
	if(cicloInstrucciones)
		liberarInstruccion(instruccion);
}

void checkInsterrupt(){
    if(interrupciones && cicloInstrucciones && !desalojo)
	{
		copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);

		t_paquete* paquete= crear_paquete(INTERRUPCIONCPU, info_logger);
		agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
		enviar_paquete(paquete, kernel_fd);
		eliminar_paquete(paquete);
		interrupciones=0;


		cicloInstrucciones = false;
		log_info(info_logger, "PID: <%d> - Error Interrupcion", PCB_Actual->id);
	}

	if(desalojo)
		ejecutar_EXIT();

	if(!cicloInstrucciones)
		sem_post(&bin_ciclo);
}

void copiar_registroPCB_a_los_registrosCPU (RegistrosCPU* registro) {
	registroCPU_AX = registro->registro_AX;
	registroCPU_BX = registro->registro_BX;
	registroCPU_CX = registro->registro_CX;
	registroCPU_DX = registro->registro_DX;
    registroCPU_EAX = registro->registro_EAX;
	registroCPU_EBX = registro->registro_EBX;
	registroCPU_ECX = registro->registro_ECX;
	registroCPU_EDX = registro->registro_EDX;
    registroCPU_SI = registro->registro_SI;
	registroCPU_DI = registro->registro_DI;
}

void copiar_registrosCPU_a_los_registroPCB(RegistrosCPU* registro) {
    registro->registro_AX = registroCPU_AX;
    registro->registro_BX = registroCPU_BX;
    registro->registro_CX = registroCPU_CX;
    registro->registro_DX = registroCPU_DX;
    registro->registro_EAX = registroCPU_EAX;
    registro->registro_EBX = registroCPU_EBX;
    registro->registro_ECX = registroCPU_ECX;
    registro->registro_EDX = registroCPU_EDX;
    registro->registro_SI = registroCPU_SI;
    registro->registro_DI = registroCPU_DI;
}