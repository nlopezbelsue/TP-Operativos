#include <manejarInterfaz.h>





void manejarInterfazGenerica(uint32_t unidadesDeTrabajo) {    

    //sleep(cfg_entradaSalida->TIEMPO_UNIDAD_TRABAJO+4);               //DESPUES VER SI ES USLEEP O SLEEP
    usleep(cfg_entradaSalida->TIEMPO_UNIDAD_TRABAJO * unidadesDeTrabajo * 5000);
    printf("SLEEP TERMINADO\n");
    
}

void manejarInterfazStdin(t_list* listaInts) {

    t_datos* datos = malloc(sizeof(t_datos));
    datos->datos = (void*)readline("INSERTE TEXTO>");
    uint32_t tamanio = *(uint32_t*)list_get(listaInts, 2);
    datos->tamanio = tamanio;

    log_trace(trace_logger, "El texto ingresado fue: %s", datos->datos);

    enviarListaIntsYDatos(listaInts, datos, memoria_fd, info_logger, ACCESO_PEDIDO_ESCRITURA);

    //free(datosLeidos);

}


// FileSystem
void crearArchivo(char* nombreArchivo) {

    char* base = string_new();
    string_append(&base, cfg_entradaSalida->PATH_BASE_DIALFS);
    string_append(&base, "/");
    string_append(&base, nombreArchivo); // se da por hecho que esta el .txt
    //string_append(base, ".txt");

    log_info(info_logger, "%s", base);
    FILE* f = fopen(base, "rb");
    if(f != NULL) {
        log_info(info_logger, "El archivo ya existe");
        fclose(f);
        return;
    }

    f = fopen(base, "wb");
    if(f == NULL) {
        log_info(info_logger, "No se creo el archivo");
    }

  
    t_archivo_metadata* archivoCreado = malloc(sizeof(t_archivo_metadata));
    
    int tamanio_nombre_archivo = strlen(nombreArchivo);

    archivoCreado->nombreArchivo = malloc(sizeof(tamanio_nombre_archivo + 1));
    archivoCreado->tamArchivo = 0;
    strcpy(archivoCreado->nombreArchivo, nombreArchivo);

    t_config* configAux = config_create(base);

    
    archivoCreado->bloqueInicial = obtener_bloque_libre(bitmap);
    if(archivoCreado->bloqueInicial == -1) {
        return;
    }

    char* stringAuxiliar = string_itoa(archivoCreado->bloqueInicial);
    
    config_set_value(configAux, "NOMBRE_ARCHIVO", nombreArchivo);
    config_set_value(configAux, "TAMANIO_ARCHIVO", "0");
    config_set_value(configAux, "BLOQUE_INICIAL", stringAuxiliar);
	config_save(configAux);

    archivoCreado->configArchivo = configAux;
    fclose(f);
}

uint32_t obtener_bloque_libre (t_bitarray* bitmap) {

    for (uint32_t i = 0 ; i < bitarray_get_max_bit(bitmap); i++) {
            //accesoABitmap(i, bitarray_test_bit(bitmap, i));

        if (!bitarray_test_bit(bitmap, i)) {

            bitarray_set_bit(bitmap, i);
            //accesoABitmap(i, bitarray_test_bit(bitmap, i));

            return i;
        }
    }

    msync(bitmap, cfg_entradaSalida->BLOCK_COUNT/8, MS_SYNC);
	log_info(info_logger, "No se obtuvo un bloque libre");
    return -1;
}

void eliminarArchivo(char* nombreArchivo) {
    char* base = string_new();
    string_append(&base, cfg_entradaSalida->PATH_BASE_DIALFS);
    string_append(&base, "/");
    string_append(&base, nombreArchivo); // se da por hecho que esta el .txt
    //string_append(base, ".txt");

    t_config* configAux = config_create(base);

    uint32_t bloqueInicial = config_get_int_value(configAux, "BLOQUE_INICIAL");
    uint32_t tamanioArchivo = config_get_int_value(configAux, "TAMANIO_ARCHIVO");

    uint32_t tamanioEnBloques = (uint32_t)ceil((double)tamanioArchivo / (double)cfg_entradaSalida->BLOCK_SIZE);

    for(int i=bloqueInicial; i<bloqueInicial + tamanioEnBloques; i++) {
        
        bitarray_clean_bit(bitmap, i);
    }
    

    if (remove(base) == 0) {
        log_info(info_logger, "El archivo %s ha sido borrado exitosamente.\n", base);
    } else {
        log_info(info_logger, "Error al borrar el archivo");
    }

}

void truncarArchivo (char* nombreArchivo, uint32_t tamanio) {
    log_info(info_logger, "TAMAÑO RECIBIDO <%d>", tamanio);

    log_trace(trace_logger, "Entre a truncar");
    char* base = string_new();
    string_append(&base, cfg_entradaSalida->PATH_BASE_DIALFS);
    string_append(&base, "/");
    string_append(&base, nombreArchivo);
    
    FILE* f = fopen(base, "ab");

    t_archivo_metadata* archivoATruncar = malloc(sizeof(t_archivo_metadata));
    archivoATruncar->configArchivo = config_create(base);
    archivoATruncar->bloqueInicial = config_get_int_value(archivoATruncar->configArchivo, "BLOQUE_INICIAL");
    archivoATruncar->tamArchivo = config_get_int_value(archivoATruncar->configArchivo, "TAMANIO_ARCHIVO");
    archivoATruncar->nombreArchivo = malloc(strlen(nombreArchivo) + 1);
    archivoATruncar->nombreArchivo = config_get_string_value(archivoATruncar->configArchivo, "NOMBRE_ARCHIVO");

    if(archivoATruncar->tamArchivo == tamanio) {
        return;
    }


    if(archivoATruncar->tamArchivo > tamanio){

        if(tamanio < 0) {
            log_info(info_logger, "No puede haber tamanio negativo");
            fclose(f);
            return;
        }

		achicarArchivo(nombreArchivo, tamanio, archivoATruncar);
        log_trace(trace_logger, "Nuevo tamanio del archivo %s achicado: %d", nombreArchivo, tamanio);

	}else{
        log_trace(trace_logger, "Voy a agrandar el archivo");
        agrandarArchivo(nombreArchivo, tamanio, archivoATruncar);
        log_trace(trace_logger, "Nuevo tamanio del archivo %s agrandado: %d", nombreArchivo, tamanio);
	}

    //msync(bitmap, cfg_entradaSalida->BLOCK_COUNT/8, MS_SYNC);
    char* stringAuxiliar = string_itoa(tamanio);
    config_set_value(archivoATruncar->configArchivo, "TAMANIO_ARCHIVO", stringAuxiliar);
    config_save(archivoATruncar->configArchivo);

    fclose(f);
}

void agrandarArchivo(char* nombreArchivo, uint32_t tamanio, t_archivo_metadata* archivoATruncar) {
    double bloquesSinRedondear = (double) tamanio / (double)cfg_entradaSalida->BLOCK_SIZE;
    uint32_t bloquesASumar = (uint32_t) ceil(bloquesSinRedondear);  
    (tamanio - archivoATruncar->tamArchivo) / cfg_entradaSalida->BLOCK_SIZE;
    uint32_t tamanioActualEnBloques = archivoATruncar->tamArchivo / cfg_entradaSalida->BLOCK_SIZE;  
    uint32_t posicionFinalActual;
     
    if(!tamanioActualEnBloques) {
        posicionFinalActual = archivoATruncar->bloqueInicial;
    }else {
        posicionFinalActual = archivoATruncar->bloqueInicial + tamanioActualEnBloques - 1;
    }                                                                                   
    

    if (tengoEspacioAMiLado(archivoATruncar, tamanio)) {              // AGRANDAMOS NORMAL
        log_info(info_logger, "Tengo espacio a mi lado, bloques a sumar: %d, bloques sin redondear %f", bloquesASumar, bloquesSinRedondear);
        while(bloquesASumar) {

            bitarray_set_bit(bitmap, posicionFinalActual + bloquesASumar - 1);
            //log_info(info_logger, "Asigno bit: %d", posicionFinalActual + bloquesASumar - 1);
            bloquesASumar--;
        }
        return;
    }

    uint32_t posInicialNueva = hayEspacioContiguo(bloquesASumar);

    if(posInicialNueva != -1){

        log_info(info_logger, "Hay espacio sin compactar, posicion inicial nueva del achivo %s: %d", nombreArchivo, posInicialNueva);

        uint32_t dirASacarDatos = archivoATruncar->bloqueInicial * cfg_entradaSalida->BLOCK_SIZE;
        uint32_t direccionAPonerDatos = posInicialNueva * cfg_entradaSalida->BLOCK_SIZE;

        void* datos = leerArchivo(nombreArchivo, dirASacarDatos, archivoATruncar->tamArchivo);

        //log_info(info_logger, "Estoy pasando cositas: %s", datos);  //TODO ver si cambiar a %p

        escribirArchivo(nombreArchivo, datos, direccionAPonerDatos, archivoATruncar->tamArchivo);

        for(int i=archivoATruncar->bloqueInicial; i<=posicionFinalActual; i++){     //ese menor o igual 
            //log_info(info_logger, "entre al for con inicial %d y final %d", archivoATruncar->bloqueInicial, posicionFinalActual);
            bitarray_clean_bit(bitmap, i);
        }

        char* stringAuxiliar = string_itoa(posInicialNueva);
        config_set_value(archivoATruncar->configArchivo, "BLOQUE_INICIAL", stringAuxiliar);
        config_save(archivoATruncar->configArchivo);

        uint32_t posicionFinal = posInicialNueva + bloquesASumar;

        for(int i=posInicialNueva; i<posicionFinal; i++){
            //log_info(info_logger, "Asigno bit: %d", i);
            bitarray_set_bit(bitmap, i);
        }
        return;
    }


    compactarV2(nombreArchivo, tamanio, archivoATruncar);
    
    return;

}

int es_archivo_txt(const char *filename) {
    const char *ext = strrchr(filename, '.');
    return ext && strcmp(ext, ".txt") == 0;
}


void compactar(char* nombreArchivo, uint32_t tamanio, t_archivo_metadata* archivoATruncar) {
    log_trace(trace_logger, "Voy a compactar :)"); //CAMBIAR
    log_info(info_logger, "Inicio compactacion");
    
    uint32_t tamanioActual = archivoATruncar->tamArchivo / cfg_entradaSalida->BLOCK_SIZE;
    uint32_t posicionFinalActual;
     
    if(!tamanioActual) {
        posicionFinalActual = archivoATruncar->bloqueInicial;
    }else {
        posicionFinalActual = archivoATruncar->bloqueInicial + tamanioActual - 1;
    }

    if(posicionFinalActual == 0){
        for(uint i=0; i<tamanioActual; i++) {
            bitarray_clean_bit(bitmap, posicionFinalActual);
            posicionFinalActual++;
        }
    }else{
        for(uint i=0; i<tamanioActual; i++) {
            bitarray_clean_bit(bitmap, posicionFinalActual);
            posicionFinalActual--;
        }
    }

    uint32_t dondeLeer = archivoATruncar->bloqueInicial * cfg_entradaSalida->BLOCK_SIZE;
    void* datosAux = leerArchivo(nombreArchivo, dondeLeer, archivoATruncar->tamArchivo);                    //NO OLVIDAR DE ESCRIBIR
    
    //Hasta acá bien


    uint32_t bitDisponible = primerBitDisponible();
    uint32_t bitOcupado = primerBitOcupado();
    uint32_t tamArchivoAMover;
    uint32_t bloquesArchivoAMover;

    if(bitOcupado != 0) {

            
        DIR *dir = opendir(cfg_entradaSalida->PATH_BASE_DIALFS);
        if (dir == NULL) {
            perror("No se puede abrir el directorio");
            return;
        }

        struct dirent *entry;
        struct stat statbuf;

        // Leer cada entrada en el directorio
        while ((entry = readdir(dir)) != NULL) {
            // Construir la ruta completa del archivo
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", cfg_entradaSalida->PATH_BASE_DIALFS, entry->d_name);

            // Obtener información sobre el archivo
            if (stat(filepath, &statbuf) == -1) {
                perror("No se puede obtener información del archivo");
                continue;
            }

            // Verificar si es un archivo regular y si es un archivo .txt
            if (S_ISREG(statbuf.st_mode) && es_archivo_txt(entry->d_name)) {
                // Construir el nuevo path basado en PATH_BASE_DIALFS
                log_info(info_logger, "Archivo en directorio: %s", entry->d_name);
                char *path = string_new();
                string_append(&path, cfg_entradaSalida->PATH_BASE_DIALFS);
                string_append(&path, "/");
                string_append(&path, entry->d_name);

                // Crear la configuración del archivo
                t_config *config = config_create(path);
                if (config == NULL) {
                    fprintf(stderr, "No se pudo crear la configuración para el archivo: %s\n", path);
                    free(path);
                    continue;
                }

                // Obtener el valor de BLOQUE_INICIAL
                uint32_t posicion = config_get_int_value(config, "BLOQUE_INICIAL");
                printf("Archivo: %s, BLOQUE_INICIAL: %u\n", entry->d_name, posicion);

                if(posicion == bitOcupado)  {
                
                tamArchivoAMover = config_get_int_value(config, "TAMANIO_ARCHIVO");
                double bloquesArchivoAMoverSinRedondear = (double) tamArchivoAMover / (double)cfg_entradaSalida->BLOCK_SIZE;
                bloquesArchivoAMover = (uint32_t) ceil(bloquesArchivoAMoverSinRedondear);  //a chequear
                
                uint32_t posFinalActual = bitOcupado + bloquesArchivoAMover - 1;
                
                uint32_t nuevoBit = bitDisponible;
                for(uint32_t i=bitOcupado; i<=posFinalActual; i++) {
                    
                    bitarray_clean_bit(bitmap, i);
                    bitarray_set_bit(bitmap, nuevoBit);
                    nuevoBit ++;
                }

                msync(bitmap, cfg_entradaSalida->BLOCK_COUNT/8, MS_SYNC);

			

                char* bitAux= string_itoa(bitDisponible);
                config_set_value(config, "BLOQUE_INICIAL", bitAux);
                config_save(config);


                uint32_t dirASacarDatos = bitOcupado * cfg_entradaSalida->BLOCK_SIZE;
                uint32_t direccionAPonerDatos = bitDisponible * cfg_entradaSalida->BLOCK_SIZE;

                void* datosAMover = leerArchivo(nombreArchivo, dirASacarDatos, tamArchivoAMover);
                log_info(info_logger, "Estoy pasando cositas: %s", datosAMover);  //TODO ver si cambiar a %p
                escribirArchivo(nombreArchivo, datosAMover, direccionAPonerDatos, tamArchivoAMover);

                }



            }
        }

        // Cerrar el directorio
        closedir(dir);
    
    }

    uint32_t tamanioArchivoAuxiliar;
    if(!archivoATruncar->tamArchivo)
        tamanioArchivoAuxiliar = archivoATruncar->tamArchivo / cfg_entradaSalida->BLOCK_SIZE;
    else
        tamanioArchivoAuxiliar = 1;

    bool esElPrimero = true;
    uint32_t primerBit;
    for(int i=0; i<bitarray_get_max_bit(bitmap); i++) {
        
        if(!bitarray_test_bit(bitmap, i)){

            for(int j=0; j<tamanioArchivoAuxiliar; j++){

                if(esElPrimero) {
                    primerBit = i;
                    esElPrimero = false;
                }
                bitarray_set_bit(bitmap, i);
                i++;
            }
            break;
        }
    }

    char* stringAuxiliar = string_itoa(primerBit);
    config_set_value(archivoATruncar->configArchivo, "BLOQUE_INICIAL", stringAuxiliar);
    config_save(archivoATruncar->configArchivo);

    uint32_t dirAEscribir = primerBit * cfg_entradaSalida->BLOCK_SIZE;
    
    escribirArchivo(archivoATruncar->nombreArchivo, datosAux, dirAEscribir, archivoATruncar->tamArchivo);

    msync(bitmap, cfg_entradaSalida->BLOCK_COUNT/8, MS_SYNC);

    log_info(info_logger, "Fin compactacion");


    usleep(cfg_entradaSalida->RETRASO_COMPACTACION);


}



void compactarV2(char* nombreArchivo, uint32_t tamanio, t_archivo_metadata* archivoATruncar) {
    
   
    //tengo la info del archivo a truncar
    uint32_t cantBloques = (uint32_t)ceil((double)archivoATruncar->tamArchivo / (double)cfg_entradaSalida->BLOCK_SIZE);

    if(!archivoATruncar->tamArchivo) {
        cantBloques++;
    }

    log_trace(trace_logger, "Voy a compactar :)"); //CAMBIAR
    log_info(info_logger, "Inicio compactacion");

    uint32_t dondeLeer = archivoATruncar->bloqueInicial * cfg_entradaSalida->BLOCK_SIZE;
    void* datosAux = leerArchivo(nombreArchivo, 0, archivoATruncar->tamArchivo);                    //NO OLVIDAR DE ESCRIBIR
    
    //Limpiar bitmap
    for(int i=archivoATruncar->bloqueInicial; i<cantBloques + archivoATruncar->bloqueInicial; i++) {
        //log_error(error_logger, "Limpio el bit %d", i);
        bitarray_clean_bit(bitmap,  i);
    }

    uint32_t ultimoBloqueLibre = aplicar_algoritmo_compactacion(archivoATruncar);
   

    usleep(cfg_entradaSalida->RETRASO_COMPACTACION * 1000);

    // meto al final al archivo q saque
    config_set_value(archivoATruncar->configArchivo, "BLOQUE_INICIAL", string_itoa((int)ultimoBloqueLibre));
    uint32_t punteroEnDisco = (ultimoBloqueLibre * cfg_entradaSalida->BLOCK_SIZE);
    config_set_value(archivoATruncar->configArchivo, "TAMANIO_ARCHIVO", string_itoa((int)tamanio));
    cantBloques = (uint32_t)ceil((double)tamanio / (double)cfg_entradaSalida->BLOCK_SIZE);
    for(int i=ultimoBloqueLibre; i<=cantBloques + ultimoBloqueLibre; i++) {
        //log_error(error_logger, "compactacion Asigno bit: %d", i);
        bitarray_set_bit(bitmap, i);
    }
    escribirArchivo(nombreArchivo, datosAux, punteroEnDisco, archivoATruncar->tamArchivo);
    //log_debug(debug_logger, "Se re-escribio el archivo a partir del bloque %u, con size %u", ultimoBloqueLibre, tamanio);
    config_save(archivoATruncar->configArchivo);
	 resetearBitmap();

}

uint32_t aplicar_algoritmo_compactacion(t_archivo_metadata* archivoATruncar) {
    t_config *config_archivo_a_mover;
    uint32_t primer_bloque_libre = 0;
    uint8_t busco_bloque_libre = true;

    for (int i = 0; i < cfg_entradaSalida->BLOCK_COUNT; i++)
    {
        //log_debug(debug_logger, "Bloque %d: %d", i, bitarray_test_bit(bitmap, i));

        if (!bitarray_test_bit(bitmap, i) && busco_bloque_libre)
        {
            //log_debug(debug_logger, "Bloque libre en %d", i);
            primer_bloque_libre = i;
            busco_bloque_libre = false;
        }
        if (bitarray_test_bit(bitmap, i) && !busco_bloque_libre)
        {
            
            config_archivo_a_mover = conseguirConfigArchivoPorInicio(i);
            moverArchivo(primer_bloque_libre, config_archivo_a_mover);
            uint32_t bloqueInicial = (uint32_t)config_get_int_value(config_archivo_a_mover, "BLOQUE_INICIAL");
            uint32_t tamanioArchivo = (uint32_t)config_get_int_value(config_archivo_a_mover, "TAMANIO_ARCHIVO");
            uint32_t cant_bloques = (uint32_t)ceil((double)tamanioArchivo / (double)cfg_entradaSalida->BLOCK_SIZE); // ceil redondea para arriba
            if (tamanioArchivo == 0)
            {
                cant_bloques++;
            }
            primer_bloque_libre = bloqueInicial + cant_bloques;
            i = primer_bloque_libre - 1; // primer_bloque_libre = primer_bloque_libre-1
        }
    }
    config_destroy(config_archivo_a_mover);
    return primer_bloque_libre;
}

void moverArchivo(int nuevoOrigen, t_config* configArchivo) {
   
    uint32_t bloqueInicial = (uint32_t)config_get_int_value(configArchivo, "BLOQUE_INICIAL");
    uint32_t sizeArchivo = (uint32_t)config_get_int_value(configArchivo, "TAMANIO_ARCHIVO");
    uint32_t punteroEnDisco = 0;
    uint32_t cantBloques = (uint32_t)ceil((double)sizeArchivo / (double)cfg_entradaSalida->BLOCK_SIZE);
    char* nombreArchivo = config_get_string_value(configArchivo, "NOMBRE_ARCHIVO");
    if(sizeArchivo == 0) {
        cantBloques++;
    }

    uint32_t actualUltimoBloque = bloqueInicial + cantBloques - 1;
    uint32_t nuevoUltimoBloque = nuevoOrigen + cantBloques - 1;
    uint32_t desplazamiento = bloqueInicial - nuevoOrigen;
    void* informacionArchivo = leerArchivo(nombreArchivo, punteroEnDisco, sizeArchivo);


   
    config_set_value(configArchivo, "BLOQUE_INICIAL", string_itoa(nuevoOrigen));
    config_save(configArchivo);
    punteroEnDisco = (nuevoOrigen * cfg_entradaSalida->BLOCK_SIZE);
    for(int i=nuevoOrigen; i<=nuevoUltimoBloque; i++) {
        //log_error(error_logger, "en mover Asigno bit: %d", i);
        bitarray_set_bit(bitmap, i);
    }
    escribirArchivo(nombreArchivo, informacionArchivo, 0, sizeArchivo);
    for(int i=nuevoUltimoBloque + 1; i<=actualUltimoBloque; i++) {
        //log_error(error_logger, "en mover Limpio bit: %d", i);
        bitarray_clean_bit(bitmap, i);
    }
}

t_config* conseguirConfigArchivoPorInicio(int inicioArchivo) {

    DIR *dir = opendir(cfg_entradaSalida->PATH_BASE_DIALFS);
        if (dir == NULL) {
            perror("No se puede abrir el directorio");
            return;
        }

        struct dirent *entry;
        struct stat statbuf;

        // Leer cada entrada en el directorio
        while ((entry = readdir(dir)) != NULL) {
            // Construir la ruta completa del archivo
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", cfg_entradaSalida->PATH_BASE_DIALFS, entry->d_name);

            // Obtener información sobre el archivo
            if (stat(filepath, &statbuf) == -1) {
                perror("No se puede obtener información del archivo");
                continue;
            }

            // Verificar si es un archivo regular y si es un archivo .txt
            if (S_ISREG(statbuf.st_mode) && es_archivo_txt(entry->d_name)) {
                // Construir el nuevo path basado en PATH_BASE_DIALFS
                log_info(info_logger, "Archivo en directorio: %s", entry->d_name);
                char *path = string_new();
                string_append(&path, cfg_entradaSalida->PATH_BASE_DIALFS);
                string_append(&path, "/");
                string_append(&path, entry->d_name);

                // Crear la configuración del archivo
                t_config *config = config_create(path);
                if (config == NULL) {
                    fprintf(stderr, "No se pudo crear la configuración para el archivo: %s\n", path);
                    free(path);
                    continue;
                }

                // Obtener el valor de BLOQUE_INICIAL
                uint32_t posicion = config_get_int_value(config, "BLOQUE_INICIAL");
                printf("Archivo: %s, BLOQUE_INICIAL: %u\n", entry->d_name, posicion);

               if(posicion == inicioArchivo)  {
                   return config;
               }
                
            }
        }

    log_info(info_logger, "no se encontro el archivo");

    return NULL;

}

void resetearBitmap() {

    

    for(int i=0; i<cfg_entradaSalida->BLOCK_COUNT; i++) {
        bitarray_clean_bit(bitmap, i);
    }

      DIR *dir = opendir(cfg_entradaSalida->PATH_BASE_DIALFS);
        if (dir == NULL) {
            perror("No se puede abrir el directorio");
            return;
        }

        struct dirent *entry;
        struct stat statbuf;

        // Leer cada entrada en el directorio
        while ((entry = readdir(dir)) != NULL) {
            // Construir la ruta completa del archivo
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", cfg_entradaSalida->PATH_BASE_DIALFS, entry->d_name);

            // Obtener información sobre el archivo
            if (stat(filepath, &statbuf) == -1) {
                perror("No se puede obtener información del archivo");
                continue;
            }

            // Verificar si es un archivo regular y si es un archivo .txt
            if (S_ISREG(statbuf.st_mode) && es_archivo_txt(entry->d_name)) {
                // Construir el nuevo path basado en PATH_BASE_DIALFS
               // log_info(info_logger, "Archivo en directorio: %s", entry->d_name);
                char *path = string_new();
                string_append(&path, cfg_entradaSalida->PATH_BASE_DIALFS);
                string_append(&path, "/");
                string_append(&path, entry->d_name);

                // Crear la configuración del archivo
                t_config *config = config_create(path);
                if (config == NULL) {
                    fprintf(stderr, "No se pudo crear la configuración para el archivo: %s\n", path);
                    free(path);
                    continue;
                }

                // Obtener el valor de BLOQUE_INICIAL
                uint32_t posicion = config_get_int_value(config, "BLOQUE_INICIAL");
                uint32_t tamanio = config_get_int_value(config, "TAMANIO_ARCHIVO");
                uint32_t tamanioEnBloques = (uint32_t) ceil((double) tamanio / (double) cfg_entradaSalida->BLOCK_SIZE);
                char* nombreArchivo = config_get_string_value(config, "NOMBRE_ARCHIVO");

                for(int i=posicion; i<posicion + tamanioEnBloques; i++) {
                    //log_info(info_logger, "Asigno bit: %d a archivo %s", i, nombreArchivo);
                    bitarray_set_bit(bitmap, i);
                }
                //printf("Archivo: %s, BLOQUE_INICIAL: %u\n", entry->d_name, posicion);
            }
        }

}


//Asumimos que siempre hay espacio disponible y ocupado
uint32_t primerBitDisponible() {

    for(uint32_t i=0; i<bitarray_get_max_bit(bitmap); i++) {
        if(!bitarray_test_bit(bitmap, i))
            return i;
    }
    return -1;
}

uint32_t primerBitOcupado() {

    for(uint32_t i=0; i<bitarray_get_max_bit(bitmap); i++) {
        if(bitarray_test_bit(bitmap, i))
            return i;
    }

    return -1;
}

bool tengoEspacioAMiLado(t_archivo_metadata* archivoATruncar, uint32_t tamanioNuevo) {

    uint32_t tamanioNuevoEnBloques = tamanioNuevo / cfg_entradaSalida->BLOCK_SIZE;

    uint32_t tamanioActual = archivoATruncar->tamArchivo / cfg_entradaSalida->BLOCK_SIZE;

    uint32_t bloquesASumar;

    uint32_t posicionFinalActual;
     
    if(!tamanioActual) {
        posicionFinalActual = archivoATruncar->bloqueInicial;
    }else {
        posicionFinalActual = archivoATruncar->bloqueInicial + tamanioActual - 1;
    }
    
    if(!tamanioActual) {
        bloquesASumar = tamanioNuevoEnBloques - 1;
    }else {
        bloquesASumar = tamanioNuevoEnBloques - tamanioActual;
    }

    uint32_t posicionFinal = posicionFinalActual + bloquesASumar;

    for(uint32_t i = posicionFinalActual + 1; i <= posicionFinal; i++) {

        if(bitarray_test_bit(bitmap, i)) {
            return false;
        }
    }

    return true;
}

void achicarArchivo(char* nombreArchivo, uint32_t tamanio, t_archivo_metadata* archivoATruncar) {    
    uint32_t bloquesActuales = archivoATruncar->tamArchivo / cfg_entradaSalida->BLOCK_SIZE;

    uint32_t bloquesARestar = (archivoATruncar->tamArchivo - tamanio) / cfg_entradaSalida->BLOCK_SIZE;
    
    log_info(info_logger, "Bloque restado: %d", bloquesActuales);

    while(bloquesARestar) {

        bloquesARestar--;
        if(bloquesARestar == 0)
            break;

        bitarray_clean_bit(bitmap, bloquesActuales - 1);
        bloquesActuales--;
    }

    return; 
}

uint32_t hayEspacioContiguo(uint32_t blocksRequested) {
    bool esElPrimero = true;
    uint32_t bloquesLibres = 0;
    uint32_t bloqueInicialNuevo = 0;
    log_info(info_logger, "blocks requested: %d", blocksRequested);
    for(int i=0; i<bitarray_get_max_bit(bitmap); i++) {

        if(!bitarray_test_bit(bitmap, i)){
            if(esElPrimero) {
                bloqueInicialNuevo = i;
            }
            esElPrimero = false;
            bloquesLibres ++;
            log_info(info_logger, "bloques pasados: %d", i);
            if(bloquesLibres == blocksRequested)
                return bloqueInicialNuevo;

        }else{
            esElPrimero = true;
            bloquesLibres = 0;
        }
    }

    return -1;
}

void escribirArchivo(char* nombreArchivo, void* datos, uint32_t direccionAEscribir, uint32_t tamanioAEscribir) {

    char* pathArchivoAEscribir = string_new();
    string_append(&pathArchivoAEscribir, cfg_entradaSalida->PATH_BASE_DIALFS);
    string_append(&pathArchivoAEscribir, "/");
    string_append(&pathArchivoAEscribir, nombreArchivo); 
    t_config* configArchivo = config_create(pathArchivoAEscribir);
    uint32_t bloqueInicialArchivo = config_get_int_value(configArchivo, "BLOQUE_INICIAL");

    char* pathArchivoBloques = string_new();
    string_append(&pathArchivoBloques, cfg_entradaSalida->PATH_BASE_DIALFS);
    string_append(&pathArchivoBloques, "/bloques.dat");

    uint32_t bloqueAEscribir = bloqueInicialArchivo * cfg_entradaSalida->BLOCK_SIZE;

    archivoBloques->fd = open(pathArchivoBloques, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);

    archivoBloques->direccionArchivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

   

    uint32_t offset = bloqueInicialArchivo * cfg_entradaSalida->BLOCK_SIZE + direccionAEscribir;

  

    memcpy(archivoBloques->direccionArchivo + offset, datos, tamanioAEscribir);
    
    msync(archivoBloques->direccionArchivo, archivoBloques->tamanio, MS_SYNC);

    close(archivoBloques->fd);
}

//no se usa porque damos por hecho que la dir es pasada en bytes
uint32_t encontrarNumeroBloque(uint32_t direccion) {
    return direccion / cfg_entradaSalida->BLOCK_SIZE;
}

void* leerArchivo(char* nombreArchivo, uint32_t direccionALeer, uint32_t tamanioALeer) {

    void* datos = malloc(tamanioALeer);

     char* pathArchivoALeer = string_new();
    string_append(&pathArchivoALeer, cfg_entradaSalida->PATH_BASE_DIALFS);
    string_append(&pathArchivoALeer, "/");
    string_append(&pathArchivoALeer, nombreArchivo); 
    t_config* configArchivo = config_create(pathArchivoALeer);
    uint32_t bloqueInicialArchivo = config_get_int_value(configArchivo, "BLOQUE_INICIAL");

    uint32_t bloqueALeer = bloqueInicialArchivo * cfg_entradaSalida->BLOCK_SIZE;

    char* pathArchivoBloques = string_new();
    string_append(&pathArchivoBloques, cfg_entradaSalida->PATH_BASE_DIALFS);
    string_append(&pathArchivoBloques, "/bloques.dat");

    archivoBloques->fd = open(pathArchivoBloques, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);

    archivoBloques->direccionArchivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

    //msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);
    memcpy(datos, archivoBloques->direccionArchivo + direccionALeer + bloqueALeer, tamanioALeer);

    //log_info(info_logger, "La data que lei es: %s", datos);

    close(archivoBloques->fd);
    return datos;
}
