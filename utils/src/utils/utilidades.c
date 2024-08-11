#include "utilidades.h"

Instruccion* recibirInstruccion(int conexion){
	uint32_t tamanioBuffer;
	uint32_t desplazamiento = 0;
	void *buffer = recibir_buffer(&tamanioBuffer, conexion);
	Instruccion* instruccion = malloc(sizeof(Instruccion));

	memcpy(&(instruccion->cantidadParametros), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	memcpy(&(instruccion->idLength), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	instruccion->id = malloc(instruccion->idLength + 1);
	memcpy(instruccion->id, buffer + desplazamiento, instruccion->idLength + 1);
	desplazamiento += instruccion->idLength + 1;

	if (instruccion->cantidadParametros == 1) {
		memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param1 = malloc(instruccion->param1Length + 1);
		memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
		desplazamiento += instruccion->param1Length + 1;
	}

	if (instruccion->cantidadParametros == 2) {
		memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param1 = malloc(instruccion->param1Length + 1);
		memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
		desplazamiento += instruccion->param1Length + 1;

		memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param2 = malloc(instruccion->param2Length + 1);
		memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length + 1);
		desplazamiento += instruccion->param2Length + 1;
	}

    if (instruccion->cantidadParametros == 3) {
		memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param1 = malloc(instruccion->param1Length + 1);
		memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
		desplazamiento += instruccion->param1Length + 1;

		memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param2 = malloc(instruccion->param2Length + 1);
		memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length + 1);
		desplazamiento += instruccion->param2Length + 1;

        memcpy(&(instruccion->param3Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param3 = malloc(instruccion->param3Length + 1);
		memcpy(instruccion->param3, buffer + desplazamiento, instruccion->param3Length + 1);
		desplazamiento += instruccion->param3Length + 1;
	}

    if (instruccion->cantidadParametros == 4) {
		memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param1 = malloc(instruccion->param1Length + 1);
		memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
		desplazamiento += instruccion->param1Length + 1;

		memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param2 = malloc(instruccion->param2Length + 1);
		memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length + 1);
		desplazamiento += instruccion->param2Length + 1;

        memcpy(&(instruccion->param3Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param3 = malloc(instruccion->param3Length + 1);
		memcpy(instruccion->param3, buffer + desplazamiento, instruccion->param3Length + 1);
		desplazamiento += instruccion->param3Length + 1;

        memcpy(&(instruccion->param4Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param4 = malloc(instruccion->param4Length + 1);
		memcpy(instruccion->param4, buffer + desplazamiento, instruccion->param4Length + 1);
		desplazamiento += instruccion->param4Length + 1;
	}

	if (instruccion->cantidadParametros == 5) {
		memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param1 = malloc(instruccion->param1Length + 1);
		memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
		desplazamiento += instruccion->param1Length + 1;

		memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param2 = malloc(instruccion->param2Length + 1);
		memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length + 1);
		desplazamiento += instruccion->param2Length + 1;

        memcpy(&(instruccion->param3Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param3 = malloc(instruccion->param3Length + 1);
		memcpy(instruccion->param3, buffer + desplazamiento, instruccion->param3Length + 1);
		desplazamiento += instruccion->param3Length + 1;

        memcpy(&(instruccion->param4Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param4 = malloc(instruccion->param4Length + 1);
		memcpy(instruccion->param4, buffer + desplazamiento, instruccion->param4Length + 1);
		desplazamiento += instruccion->param4Length + 1;

		memcpy(&(instruccion->param5Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param5 = malloc(instruccion->param5Length + 1);
		memcpy(instruccion->param5, buffer + desplazamiento, instruccion->param5Length + 1);
		desplazamiento += instruccion->param5Length + 1;
	}

	return instruccion;
}

void liberarInstruccion(Instruccion * instruccion) {
    if(instruccion->cantidadParametros==1)
        free(instruccion->param1);

    if(instruccion->cantidadParametros==2){
        free(instruccion->param1);
        free(instruccion->param2);
    }
	if(instruccion->cantidadParametros==3){
        free(instruccion->param1);
        free(instruccion->param2);
		free(instruccion->param3);
    }
	if(instruccion->cantidadParametros==4){
        free(instruccion->param1);
        free(instruccion->param2);
		free(instruccion->param3);
		free(instruccion->param4);
    }
	if(instruccion->cantidadParametros==5){
        free(instruccion->param1);
        free(instruccion->param2);
		free(instruccion->param3);
		free(instruccion->param4);
		free(instruccion->param5);
    }

    free(instruccion->id);
    free(instruccion);
}
 
bool enviarListaUint32_t(t_list* listaInts, int socket_cliente, t_log* logger, op_code_cliente codigo) {
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarUint32_tsAPaquete(listaInts, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete);
    return true;
}

bool enviarListaString(t_list* listaStrings, int socket_cliente, t_log* logger, op_code_cliente codigo) {//POR FAVOR CHEQUEAR
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarStringsAPaquete(listaStrings, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete);
    return true;
}

bool agregarStringsAPaquete(t_list* listaStrings, t_paquete* paquete) {//Creo que el error esta aca
    paquete->buffer->size += 100*list_size(listaStrings);//HARDCODEADO EL 100
    paquete->buffer->size += sizeof(uint8_t);

    void* stream = malloc(paquete->buffer->size);
    int offset=0;

    void copiarElementos(char* unString){//Seguramente aca
        memcpy(stream + offset, unString, strlen(unString + 1));
        offset += strlen(unString + 3);
    }

    int cantidad_strings = list_size(listaStrings);

    memcpy(stream + offset, &cantidad_strings, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    list_iterate(listaStrings,copiarElementos);
    paquete->buffer->stream = stream;
    return true;
}

bool agregarUint32_tsAPaquete(t_list* listaInts, t_paquete* paquete) {
    paquete->buffer->size+= sizeof(uint32_t)*list_size(listaInts);
    paquete->buffer->size += sizeof(uint8_t);

    void* stream = malloc(paquete->buffer->size);
    int offset=0;

    void copiarElementos(uint32_t* unEntero){
        memcpy(stream + offset, unEntero, sizeof(uint32_t));
        offset+= sizeof(uint32_t);
    }
    int cantidad_ints = list_size(listaInts);
    memcpy(stream + offset, &cantidad_ints, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    list_iterate(listaInts,copiarElementos);
    paquete->buffer->stream = stream;
    return true;
}

void agregar_registros_a_paquete(t_paquete* paquete, RegistrosCPU* registro) {

    //Los valores a almacenar en los registros siempre tendrán la misma longitud que el registro,
    // es decir que si el registro es de 16 bytes siempre se escribirán 16 caracteres.


	agregar_a_paquete2(paquete, &(registro->registro_AX), sizeof(uint8_t));
	agregar_a_paquete2(paquete, &(registro->registro_BX), sizeof(uint8_t));
	agregar_a_paquete2(paquete, &(registro->registro_CX), sizeof(uint8_t));
	agregar_a_paquete2(paquete, &(registro->registro_DX), sizeof(uint8_t));


	agregar_a_paquete2(paquete, &(registro->registro_EAX), sizeof(uint32_t));
	agregar_a_paquete2(paquete, &(registro->registro_EBX), sizeof(uint32_t));
	agregar_a_paquete2(paquete, &(registro->registro_ECX), sizeof(uint32_t));
	agregar_a_paquete2(paquete, &(registro->registro_EDX), sizeof(uint32_t));
	agregar_a_paquete2(paquete, &(registro->registro_SI), sizeof(uint32_t));
	agregar_a_paquete2(paquete, &(registro->registro_DI), sizeof(uint32_t));

}

void agregar_ContextoEjecucion_a_paquete(t_paquete *paquete, PCB* pcb) {
    //PCB(UINTS_32T): ID
    agregar_a_paquete2(paquete, &(pcb->id), sizeof(uint32_t));
    agregar_a_paquete2(paquete, &(pcb->program_counter), sizeof(uint32_t));
    //la parte de la PCB que no son uint32_t
    //PCB: REGISTROS CPU
    agregar_registros_a_paquete(paquete, pcb->registros);
}

PCB* recibir_contextoEjecucion(int conexion) {
    //Pido el stream del buffer en el que viene serilizada la pcb
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    //Inicializo todas las estructuras que necesito
    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));


    //ID
    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Program counter
    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //la parte de la PCB que no son uint32_t
    //PCB: REGISTROS CPU

    //REGISTROS CPU 


    //Una vez obtenidos los tamaños, obtenemos los registros
    memcpy(&(registros->registro_AX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_BX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_CX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_DX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

	//Una vez obtenidos los tamaños, obtenemos los registros
    memcpy(&(registros->registro_EAX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	memcpy(&(registros->registro_SI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_DI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    PcbRecv->registros = registros;

    free(buffer);

    return PcbRecv;
}

void liberarPcbCpu(PCB* pcb) {
    free(pcb->registros);
    free(pcb);
}

t_list* recibirListaUint32_t(int socket_cliente){
    int tamanio;
    int desplazamiento = 0;//Raro este 0
    void *buffer = recibir_stream(&tamanio, socket_cliente);

    t_list* listaInts = list_create();
    int cantidad_ints = 0;

    memcpy(&cantidad_ints, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    for (int i = 0; i < cantidad_ints; ++i) {
        uint32_t* nuevoEntero = malloc(sizeof(uint32_t));
        memcpy(nuevoEntero, buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento+=sizeof(uint32_t);
        list_add(listaInts, nuevoEntero);
    }
    free(buffer);
    return listaInts;
}

t_list* recibirListaString(int socket_cliente) {
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);

    t_list* listaStrings = list_create();
    int cantidad_strings = 0;

    memcpy(&cantidad_strings, buffer + desplazamiento, sizeof(uint8_t));
    //memcpy(&tamanioString, buffer + desplazamiento, sizeof(uint8_t));

    desplazamiento += sizeof(uint8_t);

    for (int i = 0; i < cantidad_strings; ++i) {
        char* nuevoString = malloc(100 + 1);//Revisar en nombre de jesucristo
        memcpy(nuevoString, buffer + desplazamiento, sizeof (uint32_t));
        desplazamiento += sizeof(uint32_t);
        list_add(listaStrings, nuevoString);
    }

    free(buffer);
    return listaStrings;
}

char* recibirString(int socket_cliente){
    //int tamanio;
    /*int desplazamiento = 0;
    void *buffer = recibir_stream(&desplazamiento, socket_cliente);

    uint8_t tamanioString = 0;
    char* string;

    memcpy(&tamanioString, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    string= malloc(tamanioString);
    memcpy(string, buffer + desplazamiento, tamanioString);
    free(buffer);
    return string;*/

    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = recibir_stream(&paquete->buffer->size, socket);
    char* valor;
    valor = malloc(100);//HARDCODEADO el 100
    memcpy(&valor, paquete->buffer->stream, sizeof(uint32_t));
    eliminar_paquete(paquete);

    return valor;
}


uint32_t recibirValor_uint32(int socket) {
    t_paquete *paquete = malloc(sizeof (t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = recibir_stream(&paquete->buffer->size, socket);
    uint32_t valor = -1;
    memcpy(&valor, paquete->buffer->stream, sizeof(uint32_t));
    eliminar_paquete(paquete);

    return valor;
}

void enviar_uint32_y_uint32_y_char(char* path, uint32_t valor1, uint32_t valor2, int socket, op_code_cliente orden, t_log *logger){
    t_paquete * paquete= crear_paquete(orden, logger);
    agregar_a_paquete2(paquete, &valor1, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &valor2, sizeof(uint32_t));

    uint32_t largo_nombre = strlen(path) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, path, largo_nombre);

    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete);
}

void recibirOrden(int socket){
    int tamanio;
    void* stream = recibir_stream(&tamanio, socket);
    free (stream);
}

void simularRetardoSinMensaje(int retardo){
    usleep(retardo*1000);
}

//VER porque esta fea... es nueva
void agregar_instruccion_a_paquete(t_paquete *paquete, Instruccion* instruccion) {

        agregar_a_paquete2(paquete, &(instruccion->cantidadParametros), sizeof(uint8_t));
        agregar_a_paquete2(paquete, &(instruccion->idLength), sizeof(uint8_t));
        agregar_a_paquete2(paquete, instruccion->id, instruccion->idLength + 1);


        if(instruccion->cantidadParametros == 1){

            agregar_a_paquete2(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param1, instruccion->param1Length+1);          }

        if(instruccion->cantidadParametros == 2){

            agregar_a_paquete2(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param1, instruccion->param1Length+1);
            agregar_a_paquete2(paquete,  &(instruccion->param2Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param2, instruccion->param2Length+1);        }
        
        if(instruccion->cantidadParametros == 3){

            agregar_a_paquete2(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param1, instruccion->param1Length+1);
            agregar_a_paquete2(paquete,  &(instruccion->param2Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param2, instruccion->param2Length+1); 
            agregar_a_paquete2(paquete,  &(instruccion->param3Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param3, instruccion->param3Length+1);        }
        
        if(instruccion->cantidadParametros == 5){

            agregar_a_paquete2(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param1, instruccion->param1Length+1);
            agregar_a_paquete2(paquete,  &(instruccion->param2Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param2, instruccion->param2Length+1); 
            agregar_a_paquete2(paquete,  &(instruccion->param3Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param3, instruccion->param3Length+1);
            agregar_a_paquete2(paquete,  &(instruccion->param4Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param4, instruccion->param4Length+1);
            agregar_a_paquete2(paquete,  &(instruccion->param5Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param5, instruccion->param5Length+1);         }

}

bool agregarIntsYDatosAPaquete(t_list* listaInts, t_datos* datos, t_paquete* paquete){

    paquete->buffer->size+= sizeof(uint32_t)*list_size(listaInts);
    paquete->buffer->size+= datos->tamanio + sizeof(uint32_t);

    paquete->buffer->size += sizeof(uint8_t);
    void* stream = malloc(paquete->buffer->size);
    int offset=0;

    void copiarElementos(uint32_t* unEntero){
        memcpy(stream + offset, unEntero, sizeof(uint32_t));
        offset+= sizeof(uint32_t);
    }
    uint8_t cantidad_ints = list_size(listaInts);
    memcpy(stream + offset, &cantidad_ints, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    list_iterate(listaInts,copiarElementos);
    memcpy(stream + offset, &datos->tamanio, sizeof(uint32_t));
    offset+= sizeof(uint32_t);
    memcpy(stream + offset, datos->datos, datos->tamanio);
    paquete->buffer->stream = stream;
    return true;

}

bool enviarListaIntsYDatos(t_list* listaInts,t_datos* datos, int socket_cliente, t_log* logger, op_code_cliente codigo)
{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarIntsYDatosAPaquete(listaInts, datos, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete);
    return true;
}

t_list* recibirListaIntsYDatos(int socket_cliente,t_datos* datos){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    t_list* listaInts = list_create();
    uint8_t cantidad_ints = 0;
    memcpy(&cantidad_ints, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    for (int i = 0; i < cantidad_ints; ++i) {
        uint32_t* nuevoEntero = malloc(sizeof(uint32_t));//Nunca se libera
        memcpy(nuevoEntero, buffer + desplazamiento, sizeof (uint32_t));
        desplazamiento+=sizeof(uint32_t);
        list_add(listaInts, nuevoEntero);
    }

    memcpy(&datos->tamanio, buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento+=sizeof(uint32_t);
    datos->datos = malloc(datos->tamanio);//Se libera?
    memcpy(datos->datos, buffer + desplazamiento, datos->tamanio);

    free(buffer);
    return listaInts;

}

void enviarOrden(op_code_cliente orden, int socket, t_log *logger) {
    t_paquete* paquete = crear_paquete(orden, logger);
    paquete->buffer->size += sizeof(uint32_t);
    void* stream = malloc(paquete->buffer->size);
    uint32_t valor = 0;
    int offset = 0;
    memcpy(stream + offset, &valor, sizeof(uint32_t));
    paquete->buffer->stream = stream;

    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete);
    //free(valor);//Este creo que tambien rompe
    //free(offset);//Esta bien este free?
    //Habria que liberar el stream
}

void enviarValor_uint32(uint32_t valor, int socket, op_code_cliente orden, t_log *logger) {
    t_paquete * paquete = crear_paquete(orden, logger);
    paquete->buffer->size = sizeof(uint32_t);
    void* stream = malloc(paquete->buffer->size);
    int offset = 0;

    memcpy(stream + offset, &valor, sizeof(uint32_t));
    paquete->buffer->stream = stream;

    enviar_paquete(paquete,socket);
    log_info(logger, "se envio el paquete");

    eliminar_paquete(paquete);
    //free(stream);
}

void* recibir_stream(int* size, uint32_t cliente_socket) {
    recv(cliente_socket, size, sizeof(int), MSG_WAITALL);
    void* stream = malloc(*size);
    recv(cliente_socket, stream, *size, MSG_WAITALL);
    return stream;
}

PCB* recibir_contextoEjecucion_y_char (int conexion) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_BX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_CX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_DX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

	//Una vez obtenidos los tamaños, obtenemos los registros
    memcpy(&(registros->registro_EAX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	memcpy(&(registros->registro_SI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_DI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    PcbRecv->registros = registros;

    uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	PcbRecv->nombreRecurso = malloc(tamanioPalabra + 1);
	memcpy(PcbRecv->nombreRecurso, buffer + desplazamiento, tamanioPalabra + 1);
	desplazamiento += tamanioPalabra + 1;

    free(buffer);

    return PcbRecv;
}

PCB* recibir_contextoEjecucion_y_uint32(int conexion, uint32_t* direccion) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_BX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_CX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_DX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

	//Una vez obtenidos los tamaños, obtenemos los registros
    memcpy(&(registros->registro_EAX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	memcpy(&(registros->registro_SI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_DI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    PcbRecv->registros = registros;

	memcpy(direccion, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    free(buffer);

    return PcbRecv;
}

PCB* recibir_contextoEjecucion_y_uint32_y_uint32(int conexion, uint32_t* direccion, uint32_t* direccion2) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_BX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_CX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_DX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

	//Una vez obtenidos los tamaños, obtenemos los registros
    memcpy(&(registros->registro_EAX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	memcpy(&(registros->registro_SI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_DI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    PcbRecv->registros = registros;

	memcpy(direccion, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(direccion2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    free(buffer);

    return PcbRecv;
}

PCB* recibir_contextoEjecucion_y_uint32_y_uint32_y_uint32(int conexion, uint32_t* direccion, uint32_t* direccion2, uint32_t* direccion3) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_BX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_CX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_DX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

	//Una vez obtenidos los tamaños, obtenemos los registros
    memcpy(&(registros->registro_EAX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	memcpy(&(registros->registro_SI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_DI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    PcbRecv->registros = registros;

	memcpy(direccion, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(direccion2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(direccion3, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    free(buffer);

    return PcbRecv;
}

bool enviarEnteroYString(uint32_t entero,char* string, int socket_cliente, t_log* logger, op_code_cliente codigo)

{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarEnteroYStringAPaquete(entero,string, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete);
    return true;
}

bool agregarEnteroYStringAPaquete(uint32_t entero, char* string, t_paquete* paquete)
{

    paquete->buffer->size+= sizeof(uint32_t);
    uint32_t tamanioString = strlen(string) +1;
    paquete->buffer->size+= tamanioString + sizeof(uint32_t);


    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento



    memcpy(stream + offset, &entero, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &tamanioString, sizeof(uint32_t));
    offset+= sizeof(uint32_t);
    memcpy(stream + offset, string, tamanioString);
    paquete->buffer->stream = stream;

    return true;

}



char* recibirEnteroYString(int socket_cliente,uint32_t* entero) {
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    memcpy(entero, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);
    uint32_t tamanioString =0;
    memcpy(&tamanioString, buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento+=sizeof(uint32_t);
    char* string = malloc(tamanioString);
    memcpy(string, buffer + desplazamiento, tamanioString);

    free(buffer);
    return string;
}

char* recibirEnteroEnteroChar(int socket_cliente, uint32_t* entero1, uint32_t* entero2){
	uint32_t tamanioBuffer;
	uint32_t desplazamiento = 0;
	void *buffer = recibir_buffer(&tamanioBuffer, socket_cliente);

	memcpy(entero1, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(entero2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* palabraRecibida = malloc(tamanioPalabra + 1);
	memcpy(palabraRecibida, buffer + desplazamiento, tamanioPalabra + 1);
	desplazamiento += tamanioPalabra + 1;

	return palabraRecibida;
}

PCB* recibir_contextoEjecucion_y_char_y_uint32(int conexion, uint32_t* numero) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_BX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_CX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_DX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&(registros->registro_EAX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	memcpy(&(registros->registro_SI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_DI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    PcbRecv->registros = registros;

    uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	PcbRecv->nombreRecurso= malloc(tamanioPalabra);
	memcpy(PcbRecv->nombreRecurso, buffer + desplazamiento, tamanioPalabra);
	desplazamiento += tamanioPalabra;

	memcpy(numero, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    free(buffer);

    return PcbRecv;
}

PCB* recibir_contextoEjecucion_y_char_y_uint32_y_uint32(int conexion, uint32_t* numero1, uint32_t* numero2) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_BX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_CX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_DX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&(registros->registro_EAX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	memcpy(&(registros->registro_SI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_DI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    PcbRecv->registros = registros;

    uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	PcbRecv->nombreRecurso= malloc(tamanioPalabra);
	memcpy(PcbRecv->nombreRecurso, buffer + desplazamiento, tamanioPalabra);
	desplazamiento += tamanioPalabra;

	memcpy(numero1, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(numero2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    free(buffer);

    return PcbRecv;
}

PCB* recibir_contextoEjecucion_y_char_y_uint32_y_uint32_y_uint32_y_uint32(int conexion, uint32_t* numero1, uint32_t* numero2, uint32_t* numero3, uint32_t* numero4) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_BX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_CX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(&(registros->registro_DX), buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&(registros->registro_EAX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	memcpy(&(registros->registro_SI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->registro_DI), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    PcbRecv->registros = registros;

    uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	PcbRecv->nombreRecurso= malloc(tamanioPalabra);
	memcpy(PcbRecv->nombreRecurso, buffer + desplazamiento, tamanioPalabra);
	desplazamiento += tamanioPalabra;

	memcpy(numero1, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(numero2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(numero3, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(numero4, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    free(buffer);

    return PcbRecv;
}

void enviar_uint32_y_uint32_y_uint32_y_uint32_y_char(char* path, uint32_t valor1, uint32_t valor2, uint32_t valor3, uint32_t valor4, int socket, op_code_cliente orden, t_log *logger){
    t_paquete * paquete= crear_paquete(orden, logger);
    agregar_a_paquete2(paquete, &valor1, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &valor2, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &valor3, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &valor4, sizeof(uint32_t));

    uint32_t largo_nombre = strlen(path) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, path, largo_nombre);

    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete);
}

char* recibirEnteroEnteroEnteroEnteroChar(int socket_cliente, uint32_t* entero1, uint32_t* entero2, uint32_t* entero3, uint32_t* entero4){
	uint32_t tamanioBuffer;
	uint32_t desplazamiento = 0;
	void *buffer = recibir_buffer(&tamanioBuffer, socket_cliente);

	memcpy(entero1, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(entero2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(entero3, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(entero4, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* palabraRecibida = malloc(tamanioPalabra + 1);
	memcpy(palabraRecibida, buffer + desplazamiento, tamanioPalabra + 1);
	desplazamiento += tamanioPalabra + 1;

	return palabraRecibida;
}

char* recibirEnteroEnteroEnteroChar(int socket_cliente, uint32_t* entero1, uint32_t* entero2, uint32_t* entero3){
	uint32_t tamanioBuffer;
	uint32_t desplazamiento = 0;
	void *buffer = recibir_buffer(&tamanioBuffer, socket_cliente);

	memcpy(entero1, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(entero2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    memcpy(entero3, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* palabraRecibida = malloc(tamanioPalabra + 1);
	memcpy(palabraRecibida, buffer + desplazamiento, tamanioPalabra + 1);
	desplazamiento += tamanioPalabra + 1;

	return palabraRecibida;
}


void enviar_uint32_y_uint32_y_uint32_y_char(char* path, uint32_t valor1, uint32_t valor2, uint32_t valor3, int socket, op_code_cliente orden, t_log *logger){
    t_paquete * paquete= crear_paquete(orden, logger);
    agregar_a_paquete2(paquete, &valor1, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &valor2, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &valor3, sizeof(uint32_t));

    uint32_t largo_nombre = strlen(path) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, path, largo_nombre);

    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete);
}