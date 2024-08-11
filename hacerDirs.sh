#!/bin/bash

cd ..
mkdir -p dialfs
mkdir -p scripts-pruebas
cd ./scripts-pruebas
mkdir -p scripts_kernel
mkdir -p scripts_memoria
git clone https://github.com/sisoputnfrba/c-comenta-pruebas.git
cd ./c-comenta-pruebas
cp -r scripts_kernel/ /home/utnso/scripts-pruebas/
cp -r scripts_memoria/ /home/utnso/scripts-pruebas/
