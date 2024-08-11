#include <clean.h>

void cerrarPrograma(){

    if(logsCreados){
        destruirLoggers();
    }
}

void destruirLoggers(){
    log_destroy(info_logger);
    log_destroy(error_logger);
    log_destroy(trace_logger);
    

}