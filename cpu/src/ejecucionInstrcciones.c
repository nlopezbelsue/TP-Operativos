#include "ejecucionInstrucciones.h"

void SET(char* registro, char* valor){
    PCB_Actual->program_counter++;
	cambiar_valor_del_registroCPU(registro,valor);
}


void SUM(char* value1, char* value2) {
    int suma=0;
	suma+=obtener_valor_registroCPU(value1);
	suma+=obtener_valor_registroCPU(value2);

	char buffer[20];
	sprintf(buffer, "%d", suma);

    SET(value1, buffer);
}

void SUB(char* value1, char* value2) {
	int resta=0;
	resta+=obtener_valor_registroCPU(value1);
	resta-=obtener_valor_registroCPU(value2);

	char buffer[20];
	sprintf(buffer, "%d", resta);

	SET(value1, buffer);
}

void JNZ(char* registro, char* numeroInstruccion){
	if(obtener_valor_registroCPU(registro))
		PCB_Actual->program_counter=atoi(numeroInstruccion);
	else
		PCB_Actual->program_counter++;
}

void resize(char* tamanio){
    uint32_t tam = atoi(tamanio);
    t_list* listaInts = list_create();

    list_add(listaInts, &PCB_Actual->id);
	list_add(listaInts, &tam);

    enviarListaUint32_t(listaInts,memoria_fd, info_logger, RESIZE);
    PCB_Actual->program_counter++;

    int cod_op = recibir_operacion(memoria_fd);

		switch (cod_op) {
		case RESIZE:{
            uint32_t resultado = recibirValor_uint32(memoria_fd);
            if(resultado == 1){
                log_info(info_logger, "Se agrandó correctamente el tamaño a %d", tam);
            }
            if(resultado == -1){
                copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
                t_paquete* paquete = crear_paquete(OUT_OF_MEMORY, info_logger);
                agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
                enviar_paquete(paquete, kernel_fd);
                eliminar_paquete(paquete);
                cicloInstrucciones = false;
            }

            break;
        }
        default:
                log_error(error_logger, "No se recibio el valor resize correctamente, cerrando el programa");
                exit(1);  //TODO: Hay que cerrar como se debe
        }

    list_clean(listaInts);
	list_destroy(listaInts);
}

void COPY_STRING(char* tamanio) {
    uint32_t tam = atoi(tamanio);
    uint32_t direccionLogicaOrigen = registroCPU_SI;
    uint32_t direccionLogicaDestino = registroCPU_DI;
    uint32_t direccionFisicaOrigen = traducir_direccion_logica(direccionLogicaOrigen);
    uint32_t direccionFisicaDestino = traducir_direccion_logica(direccionLogicaDestino);

    // t_list* listaInts = list_create();
    // list_add(listaInts, &PCB_Actual->id);
    // list_add(listaInts, &direccionFisicaOrigen);
    // list_add(listaInts, &tam);
    // list_add(listaInts, &direccionFisicaDestino);

    //enviarListaUint32_t(listaInts, memoria_fd, info_logger, ACCESO_PEDIDO_LECTURA); //pid, dirLeer, tamanio, dirEscribir
   // log_warning(warning_logger, "MANDO A LEER CPS");
    char* valor = leer_char_de_memoria(direccionFisicaOrigen, tam);
   // log_warning(warning_logger, "MANDO A ESC CPS");
    escribir_char_en_memoria(direccionFisicaDestino, tam, valor);
    //free(valor);
    
    PCB_Actual->program_counter++;
}

void ejecutar_WAIT(char* nombre_recurso) {
	copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(WAIT, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
    agregar_a_paquete2(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
}

void ejecutar_SIGNAL(char* nombre_recurso) {
	copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(SIGNAL, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
    agregar_a_paquete2(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
}

void ejecutar_MOV_IN(char* registro, int direccion_logica) {
    int cantidad_bytes = calcular_bytes_segun_registro(registro);
	int direccion_fisica = traducir_direccion_logica(direccion_logica);

    if (!(direccion_fisica < 0)) {

        uint32_t valor = leer_valor_de_memoria(direccion_fisica, cantidad_bytes);
        char buffer[20];
        sprintf(buffer, "%d", valor);
        //log_info(info_logger, "LEI: %s", buffer);
        log_info(info_logger, "Pongo valor %s en el registro %s", buffer, registro);
        cambiar_valor_del_registroCPU(registro,buffer);
        PCB_Actual->program_counter++;
    }
}


void ejecutar_MOV_OUT(int direccion_logica, char* registro) {
    int cantidad_bytes = calcular_bytes_segun_registro(registro);
    log_info(info_logger, "TAMANIO A LEER: %d", cantidad_bytes);
	int valorDelRegistro = obtener_valor_registroCPU(registro);
    int direccion_fisica = traducir_direccion_logica(direccion_logica);

    // char buffer[20];
	// sprintf(buffer, "%d", valorDelRegistro);
    if (!(direccion_fisica < 0)) {
        escribir_valor_en_memoria(direccion_fisica,cantidad_bytes, valorDelRegistro);
        PCB_Actual->program_counter++;
    }
    //free(valorDelRegistro);
}

void ioGenSleep(char* interfaz, char* unidadesDeTrabajo){
    uint32_t numeroInterfaz = obtenerInterfaz(interfaz);
    uint32_t tiempoEspera = atoi(unidadesDeTrabajo);

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(IO_GEN_SLEEP, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    agregar_a_paquete2(paquete, &numeroInterfaz, sizeof(uint32_t)); //Diseñar para Kernel recibir contexto ejecución y uint y uint
    agregar_a_paquete2(paquete, &tiempoEspera, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
}

void ioStdinRead(char* interfaz, char* registroDireccion, char* registroTamanio){
    uint32_t numeroInterfaz = obtenerInterfaz(interfaz); 
    uint32_t tamanio = obtener_valor_registroCPU(registroTamanio); //COMO LO MANDO CON TODO LO DEMAS, TENER EN CUENTA QUE BAUTI NO ESTA USANDO EL TAMAÑO
    uint32_t direccionLogica = obtener_valor_registroCPU(registroDireccion);
    uint32_t direccion_fisica = traducir_direccion_logica(direccionLogica); //ENVIAR A KERNEL

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(IO_STDIN_READ, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    agregar_a_paquete2(paquete, &numeroInterfaz, sizeof(uint32_t)); 
    agregar_a_paquete2(paquete, &direccion_fisica, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &tamanio, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
}

void ioStdOutWrite(char* interfaz, char* registroDireccion, char* registroTamanio){
    uint32_t numeroInterfaz = obtenerInterfaz(interfaz);
    uint32_t tamanio = obtener_valor_registroCPU(registroTamanio); //COMO LO MANDO CON TODO LO DEMAS, TENER EN CUENTA QUE BAUTI NO ESTA USANDO EL TAMAÑO
    uint32_t direccionLogica = obtener_valor_registroCPU(registroDireccion);
    uint32_t direccion_fisica = traducir_direccion_logica(direccionLogica); //ENVIAR A KERNEL

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(IO_STDOUT_WRITE, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    agregar_a_paquete2(paquete, &numeroInterfaz, sizeof(uint32_t)); 
    agregar_a_paquete2(paquete, &direccion_fisica, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &tamanio, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
}

void ioFsCreate(char* interfaz, char* nombreArchivo){
    uint32_t numeroInterfaz = obtenerInterfaz(interfaz);

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(IO_FS_CREATE, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombreArchivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombreArchivo, largo_nombre);
    agregar_a_paquete2(paquete, &numeroInterfaz, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);

    cicloInstrucciones = false;
}

void ioFsDelete(char* interfaz, char* nombreArchivo){
    uint32_t numeroInterfaz = obtenerInterfaz(interfaz);

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(IO_FS_DELETE, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombreArchivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombreArchivo, largo_nombre);
    agregar_a_paquete2(paquete, &numeroInterfaz, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);

    cicloInstrucciones = false;
}

void ioFsTruncate(char* interfaz, char* nombreArchivo, char* registroTamanio){
    uint32_t numeroInterfaz = obtenerInterfaz(interfaz);
    uint32_t tamanio = obtener_valor_registroCPU(registroTamanio);

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(IO_FS_TRUNCATE, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombreArchivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombreArchivo, largo_nombre);
    agregar_a_paquete2(paquete, &numeroInterfaz, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &tamanio, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);

    cicloInstrucciones = false;
}

void ioFsWrite(char* interfaz, char* nombreArchivo, char* registroDireccion, char* registroTamanio, char* registroPunteroArchivo){
    uint32_t numeroInterfaz = obtenerInterfaz(interfaz);
    uint32_t tamanio = obtener_valor_registroCPU(registroTamanio);
    uint32_t punteroArchivo = obtener_valor_registroCPU(registroPunteroArchivo);
    uint32_t direccionLogica = obtener_valor_registroCPU(registroDireccion);
    uint32_t direccion_fisica = traducir_direccion_logica(direccionLogica);

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(IO_FS_WRITE, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombreArchivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombreArchivo, largo_nombre);
    agregar_a_paquete2(paquete, &numeroInterfaz, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &tamanio, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &punteroArchivo, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &direccion_fisica, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);

    cicloInstrucciones = false;
}

void ioFsRead(char* interfaz, char* nombreArchivo, char* registroDireccion, char* registroTamanio, char* registroPunteroArchivo){
    uint32_t numeroInterfaz = obtenerInterfaz(interfaz);
    uint32_t tamanio = obtener_valor_registroCPU(registroTamanio);
    uint32_t punteroArchivo = obtener_valor_registroCPU(registroPunteroArchivo);
    uint32_t direccionLogica = obtener_valor_registroCPU(registroDireccion);
    uint32_t direccion_fisica = traducir_direccion_logica(direccionLogica);

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(IO_FS_READ, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombreArchivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombreArchivo, largo_nombre);
    agregar_a_paquete2(paquete, &numeroInterfaz, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &tamanio, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &punteroArchivo, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &direccion_fisica, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);

    cicloInstrucciones = false;
}

void ejecutar_EXIT(){
	copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    t_paquete* paquete = crear_paquete(EXIT, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
}

uint32_t leer_valor_de_memoria(int direccion_fisica, int bytesRegistro) {
    t_list* listaInts = list_create();
    uint32_t uint32t_dir_fis = direccion_fisica;
    uint32_t uint32t_tamanio = bytesRegistro; 

    list_add(listaInts, &PCB_Actual->id);
    list_add(listaInts, &uint32t_dir_fis);
    list_add(listaInts, &uint32t_tamanio);

    enviarListaUint32_t(listaInts,memoria_fd, info_logger, ACCESO_PEDIDO_LECTURA_UINT); //pid, direccion, tamanio
    void* leido = recibir_valor_de_memoria();
    uint32_t valor = 0;
    if(bytesRegistro == 4)
        memcpy(&valor, leido, 4);
    else   
        memcpy(&valor, leido, 1);

 //   log_warning(warning_logger, "VALOR LEIDO DE MEMORIA: %d", valor);

    list_clean(listaInts);
    list_destroy(listaInts);
    log_info(info_logger, "PID: <%d> - Acción: <LEER> - Dirección Fisica: <%d> - Valor: <%d", PCB_Actual->id, direccion_fisica, valor);

    return valor;
}

char* leer_char_de_memoria(int direccion_fisica, int bytesRegistro) {
    t_list* listaInts = list_create();
    uint32_t uint32t_dir_fis = direccion_fisica;
    uint32_t uint32t_tamanio = bytesRegistro; 

    list_add(listaInts, &PCB_Actual->id);
    list_add(listaInts, &uint32t_dir_fis);
    list_add(listaInts, &uint32t_tamanio);

    //log_warning(warning_logger, "PEDIDO LECTURA");
    enviarListaUint32_t(listaInts,memoria_fd, info_logger, ACCESO_PEDIDO_LECTURA); //pid, direccion, tamanio
    //char* valor = recibir_char_de_memoria();
    char* valor = (char*)recibir_valor_de_memoria();

   // log_warning(warning_logger, "VALOR LEIDO DE MEMORIA: %d", valor);

    list_clean(listaInts);
    list_destroy(listaInts);
    log_info(info_logger, "PID: <%d> - Acción: <LEER> - Dirección Fisica: <%d> - Valor: <%d", PCB_Actual->id, direccion_fisica, valor);

    return valor;
}

void* recibir_valor_de_memoria(){

        void* valor;
        int cod_op = recibir_operacion(memoria_fd);

		switch (cod_op) {
        case LECTURA_REALIZADA : {
            t_datos* datitos = malloc(sizeof(t_datos));
            t_list* listaInts = recibirListaIntsYDatos(memoria_fd, datitos); //pid, dirLeer, tamanio, dirEscribir
            uint32_t tamanio = *(uint32_t*)list_get(listaInts,2);

            valor = datitos->datos;
            //log_info(info_logger, "VALOR LEIDO DE MEMORIA: %s", datitos->datos);
            list_clean_and_destroy_elements(listaInts,free);
            list_destroy(listaInts);

            break;
        }
        
		case LECTURA_REALIZADA_UINT: {
            t_list* listaInts = recibirListaUint32_t(memoria_fd); //pid, direcicon, tamanio, valor
            uint32_t tamanio = *(uint32_t*)list_get(listaInts,2);
            uint32_t puntero = *(uint32_t*)list_get(listaInts,3);
            //valor = (void*)puntero;
            valor = &puntero;
            //log_info(info_logger, "VALOR LEIDO DE MEMORIA ABER?: %d", puntero);

            list_clean_and_destroy_elements(listaInts,free);
            list_destroy(listaInts);

            break;
        }
        default:
                log_error(error_logger, "No se recibio el valor correctamente, cerrando el programa");
                exit(1);  //TODO: Hay que cerrar como se debe
        }

    return valor;
}

char* recibir_char_de_memoria(){

        char* valor;
        int cod_op = recibir_operacion(memoria_fd);

		switch (cod_op) {
        case LECTURA_REALIZADA : {
            t_datos* datitos = malloc(sizeof(t_datos));
            t_list* listaInts = recibirListaIntsYDatos(memoria_fd, datitos); //pid, dirLeer, tamanio, dirEscribir
            uint32_t tamanio = *(uint32_t*)list_get(listaInts,2);
            valor = (char*)datitos->datos;
            //log_info(info_logger, "VALOR LEIDO DE MEMORIA: %s", datitos->datos);
            list_clean_and_destroy_elements(listaInts,free);
            list_destroy(listaInts);

            break;
        }
		
        default:
                log_error(error_logger, "No se recibio el valor correctamente, cerrando el programa");
                exit(1);  //TODO: Hay que cerrar como se debe
        }
    return valor;
}

void escribir_valor_en_memoria(int direccion_fisica, int cantidad_bytes, uint32_t valor) {
    t_list* listaInts = list_create();
    t_datos* unosDatos = malloc(sizeof(t_datos));
    unosDatos->tamanio = cantidad_bytes;
    unosDatos->datos = &valor;
    list_add(listaInts, &PCB_Actual->id);
    list_add(listaInts, &direccion_fisica);
    list_add(listaInts, &cantidad_bytes);
    //log_warning(warning_logger, "MANDO A ESCRIBIR: %d", valor);

    enviarListaIntsYDatos(listaInts, unosDatos, memoria_fd, info_logger, ACCESO_PEDIDO_ESCRITURA);

    list_clean(listaInts);
    list_destroy(listaInts);

    char* valor2 = recibir_confirmacion_de_escritura();
    if (strcmp(valor2, "OK") == 0) 
        log_info(info_logger, "PID: <%d> - Accion: <ESCRIBIR> - Dirección Fisica: <%d> - Valor: <%d>", PCB_Actual->id, direccion_fisica, valor);
    //free(unosDatos);
}

void escribir_char_en_memoria(int direccion_fisica, int cantidad_bytes, void* valor) {
    t_list* listaInts = list_create();
    t_datos* unosDatos = malloc(sizeof(t_datos));
    unosDatos->tamanio = cantidad_bytes;
    unosDatos->datos = valor;
    list_add(listaInts, &PCB_Actual->id);
    list_add(listaInts, &direccion_fisica);
    list_add(listaInts, &cantidad_bytes);
 //   log_warning(warning_logger, "MANDO A ESCRIBIR: %s", valor);

    enviarListaIntsYDatos(listaInts, unosDatos, memoria_fd, info_logger, ACCESO_PEDIDO_ESCRITURA);

    list_clean(listaInts);
    list_destroy(listaInts);

    char* valor2 = recibir_confirmacion_de_escritura();
    if (strcmp(valor2, "OK") == 0) 
        log_info(info_logger, "PID: <%d> - Accion: <ESCRIBIR> - Dirección Fisica: <%d> - Valor: <%d>", PCB_Actual->id, direccion_fisica, valor);
    //free(unosDatos);
}

char* recibir_confirmacion_de_escritura() {
    char* valor; 
    int cod_op = recibir_operacion(memoria_fd);

    switch (cod_op) {
        case ESCRITURA_REALIZADA:
            recibirOrden(memoria_fd);
            valor= "OK";
            break;
    }
    return valor;
}

int calcular_bytes_segun_registro(char* registro){
    int bytes;

    if ((strcmp(registro, "AX") == 0) || (strcmp(registro, "BX") == 0) || (strcmp(registro, "CX") == 0) || (strcmp(registro, "DX") == 0))
        bytes = 1;
    if ((strcmp(registro, "EAX") == 0) || (strcmp(registro, "EBX") == 0) || (strcmp(registro, "ECX") == 0) || (strcmp(registro, "EDX") == 0) || (strcmp(registro, "SI") == 0) || (strcmp(registro, "DI") == 0))
        bytes = 4;

    return bytes;
}

void cambiar_valor_del_registroCPU(char* registro, char* valor) {
   // log_warning(warning_logger, "CAMBIO EL VALOR DE %s A %s", registro, valor);
    if (strcmp(registro, "PC") == 0)
        PCB_Actual->program_counter = atoi(valor);

    if (strcmp(registro, "AX") == 0)
        registroCPU_AX = atoi(valor);

    if (strcmp(registro, "BX") == 0)
        registroCPU_BX = atoi(valor);

    if (strcmp(registro, "CX") == 0)
        registroCPU_CX = atoi(valor);

    if (strcmp(registro, "DX") == 0)
        registroCPU_DX = atoi(valor);

    if (strcmp(registro, "EAX") == 0)
        registroCPU_EAX = atoi(valor);

    if (strcmp(registro, "EBX") == 0)
        registroCPU_EBX = atoi(valor);

    if (strcmp(registro, "ECX") == 0)
        registroCPU_ECX = atoi(valor);

    if (strcmp(registro, "EDX") == 0)
        registroCPU_EDX = atoi(valor);

    if (strcmp(registro, "SI") == 0)
        registroCPU_SI = atoi(valor);
    
    if (strcmp(registro, "DI") == 0)
        registroCPU_DI = atoi(valor);
}

int obtener_valor_registroCPU(char* registro) {

    if (!strncmp(registro, "AX", strlen("AX"))){
        return registroCPU_AX;
    }
    if (!strncmp(registro, "BX", strlen("BX"))) {
        return registroCPU_BX;
     }
    if (!strncmp(registro, "CX", strlen("CX"))) {
        return registroCPU_CX;
     }
    if (!strncmp(registro, "DX", strlen("DX"))) {
        return registroCPU_DX;
     }
    if (!strncmp(registro, "EAX", strlen("EAX"))) {
        return registroCPU_EAX;
     }
    if (!strncmp(registro, "EBX", strlen("EBX"))) {
        return registroCPU_EBX;
     }
    if (!strncmp(registro, "ECX", strlen("ECX"))) {
        return registroCPU_ECX;
     }
    if (!strncmp(registro, "EDX", strlen("EDX"))) {
        return registroCPU_EDX;
     }
    if (!strncmp(registro, "SI", strlen("SI"))) {
        return registroCPU_SI;
     }
    if (!strncmp(registro, "DI", strlen("DI"))) {
        return registroCPU_DI;
     }

}

uint32_t obtenerInterfaz(char* interfaz){
    uint32_t numeroInterfaz = 0;

    if(!strcmp(interfaz,"STDOUT") || !strcmp(interfaz,"MONITOR"))
        numeroInterfaz=0;
    if(!strcmp(interfaz,"STDIN") || !strcmp(interfaz,"TECLADO"))
        numeroInterfaz=1;
    if(!strcmp(interfaz,"DIAL_FS") || !strcmp(interfaz,"FS") || !strcmp(interfaz,"DIALFS"))
        numeroInterfaz=2;
    if(!strcmp(interfaz,"ESPERA"))
        numeroInterfaz=3;
    if (!strcmp(interfaz,"SLP1")){
        numeroInterfaz = 4;
    }
    if (!strcmp(interfaz,"GENERICA")) {
       numeroInterfaz = 5;
    }
    if (!strcmp(interfaz,"IO_GEN_SLEEP")) {
        numeroInterfaz = 6;
    }
    
    

    return numeroInterfaz;
}