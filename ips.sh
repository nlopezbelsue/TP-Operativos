#!/bin/bash

validate_ip() {
  
  local ip_pattern='^([0-9]{1,3}\.){3}[0-9]{1,3}$'

  if [[ $1 =~ $ip_pattern ]]; then
    return 0  
  else
    return 1  
  fi
}



read -p "Ingresa la IP de Kernel: " ipKernel
read -p "Ingresa la IP de Memoria: " ipMemoria
read -p "Ingresa la IP de CPU: " ipCPU

if [[ $ipKernel == "" || $ipMemoria == "" || $ipCPU == "" ]]; then
  echo "Error: No ingresaste nada :0"
  exit 0
fi

if validate_ip "$ipKernel" && validate_ip "$ipMemoria" && validate_ip "$ipCPU" ; then
  echo "Todas las IPs ingresadas son validas para actualizar los configs"
else
  echo "Error: Pusiste cualquier cosa :/"
  exit 0
fi

# Regexs
sKer="IP_KERNEL=[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}"
sMem="IP_MEMORIA=[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}"
sFs="IP_FILESYSTEM=[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}"
sCpu="IP_CPU=[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}"

# Kernel

kc1="./configs/kernelplani.config"
kc2="./configs/kerneldeadlock.config"
kc3="./configs/kernelmem.config"
kc4="./configs/kernelio.config"
kc5="./configs/kernelfs.config"
kc6="./configs/kernelsalvation.config"


sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $kc1
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $kc2
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $kc3
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $kc4
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $kc5
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $kc6


sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc1
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc2
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc3
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc4
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc5
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc6


sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc1
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc2
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc3
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc4
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc5
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc6



# IO
ioc0="./configs/GENERICA.config"
ioc1="./configs/ESPERA.config"
ioc2="./configs/SLP1.config"
ioc3="./configs/IO_GEN_SLEEP.config"
ioc4="./configs/MONITOR.config"
ioc5="./configs/TECLADO.config"
ioc6="./configs/FS.config"

sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $ioc0
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $ioc1
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $ioc2
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $ioc3
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $ioc4
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $ioc5
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $ioc6

sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $ioc0
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $ioc1
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $ioc2
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $ioc3
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $ioc4
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $ioc5
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $ioc6


# CPU

cpc1="./configs/cpuplani.config"
cpc2="./configs/cpudeadlock.config"
cpc3="./configs/cpumem.config"
cpc4="./configs/cpuio.config"
cpc5="./configs/cpufs.config"
cpc6="./configs/cpusalvation.config"


sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc1
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc2
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc3
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc4
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc5
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc6

sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $cpc0
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $cpc1
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $cpc2
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $cpc3
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $cpc4
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $cpc5
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $cpc6


# Memoria
mc1="./configs/memoriaplani.config"
mc2="./configs/memoriadeadlock.config"
mc3="./configs/memoriamem.config"
mc4="./configs/memoriaio.config"
mc5="./configs/memoriafs.config"
mc6="./configs/memoriasalvation.config"



sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $mc1
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $mc2
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $mc3
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $mc4
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $mc5
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $mc6