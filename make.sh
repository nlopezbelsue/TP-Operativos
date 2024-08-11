#!/bin/bash

cd ./memoria
make
cd ..

cd ./entradasalida
make
cd ..

cd ./cpu
make
cd ..

cd ./kernel
make
cd ..


cd ..