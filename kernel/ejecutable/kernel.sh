# Obtener la ubicación del script
script_dir=$(dirname "$0")

# Directorio relativo al script
directorio=$(realpath "$0")
directorioEjecutable=$(dirname "$directorio")
directorioModulo=$(dirname "$directorioEjecutable")
directorioTp=$(dirname "$directorioModulo")

numPrueba="$1"

directorioConfig="$directorioTp/configs/$test"

directorioBin="$directorioModulo/bin/"


# Cambiar al directorio
cd "$directorioBin"
# Ejecutar el programa con parámetros determinados
./kernel "$directorioConfig/kernel$numPrueba.config" 