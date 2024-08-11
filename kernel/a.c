uint8_t truncar_archivo(t_interfaz_dialfs *idial, char *nombre_archivo, uint32_t nuevo_size, uint32_t pid)
{
    uint8_t resultado = 0;
    // consigo la info del archivo
    char *path_metadata = armar_path_metadata(nombre_archivo, idial->path_base_dialfs);
    t_config *config = config_create(path_metadata);
    uint32_t bloque_inicial = (uint32_t)config_get_int_value(config, "BLOQUE_INICIAL");
    uint32_t size_archivo = (uint32_t)config_get_int_value(config, "TAMANIO_ARCHIVO");

    if (nuevo_size > size_archivo)
    { // en este caso me tengo q fijar que exista el espacio
        double bloque_a_agregar_sin_redondear = (double)(nuevo_size) / (double)(idial->block_size);
        uint32_t bloques_a_agregar = (uint32_t)ceil(bloque_a_agregar_sin_redondear)
        // 0 bq 1 0 = pos arra 0 
        uint32_t pos_arranque = bloque_inicial + size_archivo; // esto es el primer bloque que le sigue al ultimo que ya tiene asignado el archivo
        uint8_t puedo_truncar = 1;

        // caso especial SOLAMENTE si el el truncate es para un archivo recien creado (size 0)
        if (size_archivo == 0)
        { // esto es xq aunque yo en el bitmap le asigno un bloque al archivo cuando lo creo, en la metadata le tengo que poner tamanio 0
            pos_arranque++;
            bloques_a_agregar--;
        }
        for (uint32_t i = pos_arranque; i < (pos_arranque + bloques_a_agregar); i++)
        { // se pueden dar 2 situacion por las cuales no pueda truncar:
            // 1- pido mas de lo que le queda al filesystem de espacio (desde mi pos actual, ej: hay 1024, estoy en la 700 y pido 500)
            // 2- hay bloques ocupados que necesitaria tomar para poder hacer el trunque (ej: estoy en 200, pido 50 y desde la 230 esta ocupado por otro archivo)
            if (i >= idial->block_count || esta_bloque_ocupado(idial->bitmap, i))
            {
                log_error(logger, "No se puede truncar porque el bloque %u esta ocupado por otro proceso (me faltaron %u bloques).", i, bloques_a_agregar - i);
                puedo_truncar = 0;
                break;
            }
        }

        // trunco si se puede
        if (puedo_truncar)
        {
            // seteo el bitmap
            for (uint32_t i = pos_arranque; i < pos_arranque + bloques_a_agregar; i++)
            {
                if (!ocupar_bloque(idial->bitmap, i))
                    log_warning(logger, "Se intento ocupar el bloque %u pero ya se encontraba ocupado.");
                else
                    log_debug(logger, "Se pudo ocupar el bloque %u correctamente, faltan %u.", i, pos_arranque + bloques_a_agregar - i);
                // limpio el bloque del archivo asi no tiene basura
                limpiar_bloque(idial->bloques, i);
            }
            log_debug(logger, "Termine de truncar. Voy a Modificar el archivo de metadata.");

            // seteo la metadata (solamente varia el size)
            config_set_value(config, "TAMANIO_ARCHIVO", string_itoa(nuevo_size));
            config_save(config);
            resultado = 1;
        }
        else if (hay_espacio_suficiente(idial, bloques_a_agregar))
        {
            log_info(logger, "PID: <%u> - Inicio Compactación.", pid); // log obligatorio
            compactar(idial, nombre_archivo, nuevo_size);

            log_info(logger, "PID: <%u> - Fin Compactación.", pid); // log obligatorio
            resultado = 1;
        }
        else
        {
            log_error(logger, "Es imposible truncar el archivo %s para el proceso %u, ya que no hay espacio suficiente.", nombre_archivo, pid);
        }

    } // si es menor no valido nada, interpreto que es valido perder informacion al truncar
    else
    {
        uint32_t cant_bloques_actual = (uint32_t)ceil((double)size_archivo / (double)idial->block_size);
        uint32_t cant_bloques_nueva = (uint32_t)ceil((double)nuevo_size / (double)idial->block_size);
        uint32_t bloques_a_sacar = cant_bloques_actual - cant_bloques_nueva;
        uint32_t pos_arranque = bloque_inicial + size_archivo;

        // siempre voy a tener espacio para truncar ya que estoy borrando
        for (uint32_t i = pos_arranque; i > pos_arranque - bloques_a_sacar; i--)
        {
            if (!liberar_bloque(idial->bitmap, i))
                log_warning(logger, "Se intento liberar el bloque %u pero ya se encontraba libre.");
            else
                log_debug(logger, "Se pudo liberar el bloque %u correctamente, faltan %u.", i, i - (pos_arranque - bloques_a_sacar));
        }

        // seteo la metadata (solamente varia el size)
        config_set_value(config, "TAMANIO_ARCHIVO", nuevo_size);
        config_save(config);
        resultado = 1;
    }

    // cierro todo
    config_destroy(config);
    free(path_metadata);

    return resultado;
}

void compactar(t_interfaz_dialfs *idialfs, char *archivo_agrandar, uint32_t nuevo_size)
{
    // consigo la info del archivo
    char *path_metadata = armar_path_metadata(archivo_agrandar, idialfs->path_base_dialfs);
    t_config *config = config_create(path_metadata);
    uint32_t bloque_inicial = (uint32_t)config_get_int_value(config, "BLOQUE_INICIAL");
    uint32_t size_archivo = (uint32_t)config_get_int_value(config, "TAMANIO_ARCHIVO");
    uint32_t cant_bloques = (uint32_t)ceil((double)size_archivo / (double)idialfs->block_size); // ceil redondea para arriba

    // caso especial SOLAMENTE si el el truncate es para un archivo recien creado (size 0)
    if (size_archivo == 0)
    { // esto es xq aunque yo en el bitmap le asigno un bloque al archivo cuando lo creo, en la metadata le tengo que poner tamanio 0
        cant_bloques++;
    }

    log_debug(logger, "Bloque inicial archivo_agrandar: %u", bloque_inicial);
    // primero voy a sacar el archivo q quiere agrandarse
    void *buffer = leer_en_archivo(idialfs, archivo_agrandar, size_archivo, 0);
    mem_hexdump(buffer, size_archivo);
    limpiar_bitmap(idialfs, bloque_inicial, cant_bloques);

    // desp ejecuto el algoritmo para mover el resto de archivos hacia la izquierda
    uint32_t ultimo_bloque_libre = aplicar_algoritmo_compactacion(idialfs);
    log_debug(logger, "Mi archivo ahora va a arrancar en %u", ultimo_bloque_libre);

    usleep(idialfs->retraso_compactacion * 1000);

    // meto al final al archivo q saque
    config_set_value(config, "BLOQUE_INICIAL", string_itoa((int)ultimo_bloque_libre));
    uint32_t puntero_en_disco = (ultimo_bloque_libre * idialfs->block_size);
    config_set_value(config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_size));
    ocupar_bitmap(idialfs, ultimo_bloque_libre, (uint32_t)ceil((double)nuevo_size / (double)idialfs->block_size));
    escribir_bloque(idialfs->bloques, puntero_en_disco, size_archivo, buffer);
    //log_debug(logger, "Se re-escribio el archivo a partir del bloque %u, con size %u", puntero_en_disco, size_archivo);
    config_save(config);
    // lloro :´( 😭
}

uint32_t aplicar_algoritmo_compactacion(t_interfaz_dialfs *idial)
{
    t_config *config_archivo_a_mover;
    uint32_t primer_bloque_libre = 0;
    uint8_t busco_bloque_libre = true;

    for (int i = 0; i < idial->block_count; i++)
    {
        if (!esta_bloque_ocupado(idial->bitmap, i) && busco_bloque_libre)
        {
            log_debug(logger, "Bloque libre en %d", i);
            primer_bloque_libre = i;
            busco_bloque_libre = false;
        }
        if (esta_bloque_ocupado(idial->bitmap, i) && !busco_bloque_libre)
        {
            log_debug(logger, "Bloque ocupado en %d", i);
            config_archivo_a_mover = conseguir_config_archivo_por_inicio(idial, i);
            mover_archivo(idial, primer_bloque_libre, config_archivo_a_mover);
            uint32_t bloque_inicial = (uint32_t)config_get_int_value(config_archivo_a_mover, "BLOQUE_INICIAL");
            uint32_t tamanio_archivo = (uint32_t)config_get_int_value(config_archivo_a_mover, "TAMANIO_ARCHIVO");
            uint32_t cant_bloques = (uint32_t)ceil((double)tamanio_archivo / (double)idial->block_size); // ceil redondea para arriba
            if (tamanio_archivo == 0)
            {
                cant_bloques++;
            }
            primer_bloque_libre = bloque_inicial + cant_bloques;
            i = primer_bloque_libre - 1; // primer_bloque_libre = primer_bloque_libre-1
        }
    }
    config_destroy(config_archivo_a_mover);
    return primer_bloque_libre;
}

void mover_archivo(t_interfaz_dialfs *idialfs, int nuevo_origen, t_config *config_archivo)
{
    log_debug(logger, "Voy a mover a un archivo su bloque inicial a %d", nuevo_origen);
    uint32_t bloque_inicial = (uint32_t)config_get_int_value(config_archivo, "BLOQUE_INICIAL");
    uint32_t size_archivo = (uint32_t)config_get_int_value(config_archivo, "TAMANIO_ARCHIVO");
    uint32_t puntero_en_disco = (bloque_inicial * idialfs->block_size);
    uint32_t cant_bloques = (uint32_t)ceil((double)size_archivo / (double)idialfs->block_size); // ceil redondea para arriba

    // caso especial SOLAMENTE si el el truncate es para un archivo recien creado (size 0)
    if (size_archivo == 0)
    {
        cant_bloques++;
    }
    uint32_t actual_ultimo_bloque = bloque_inicial + cant_bloques - 1; // 1
    uint32_t nuevo_ultimo_bloque = nuevo_origen + cant_bloques - 1;    // 0
    uint32_t desplazamiento = bloque_inicial - nuevo_origen;           // 1 - 0 = 1
    void *informacion_archivo = leer_bloque(idialfs->bloques, puntero_en_disco, size_archivo);

    config_set_value(config_archivo, "BLOQUE_INICIAL", string_itoa(nuevo_origen));
    config_save(config_archivo);
    puntero_en_disco = (nuevo_origen * idialfs->block_size);
    ocupar_bitmap(idialfs, nuevo_origen, desplazamiento);
    escribir_bloque(idialfs->bloques, puntero_en_disco, size_archivo, informacion_archivo);
    limpiar_bitmap(idialfs, nuevo_ultimo_bloque + 1, desplazamiento);
}