## Autores
<hr>

- [Alberto Abancens - 62581](https://github.com/tataabancens/)

- [Gonzalo Rossin - 60135](https://github.com/GonzaloRossin/)
 
- [Uriel Mihura - 59039](https://github.com/uri-99/)

<hr>

# Manual de Usuario
Sistema operativo basado en Barebonesx64 por RowDaBoat. Primero se debe tener instalado qemu-system para visualizar nuestro SO y Docker (con el contenedor `agodio/itba-so:1.0` otorgrado por la cátedra) para el compilado del mismo. Luego se debe compilar desde Docker y ejecutarlo:
1. Solamente la primera vez que se accede se debe hacer ```cd Toolchain``` y luego ```make all```.
2. Luego para compilar el resto del proyecto (primero volviendo al directorio anterior) se puede hacer de dos formas distintas; cada una determinará el Memory Manager que se utilizará:
- `make all MM=BUDDY_MM ` (para utilizar el buddy system).
- `make all MM=FREELIST_MM ` (para utilizar free list).
- De no funcionar la asignación de la variable MM por consola, se puede cambiar la variable `MM` por la deseada dentro del `makefile` de `kernel`. 
- Importante: es necesario hacer `make clean` entre la compilación con un sistema y el otro.
3. Finalmente solo queda ejecutar el sistema, ya sea con el comando ```run.sh``` desde Linux, o ```./run.bat``` desde Windows.

## Shell
Es entéprete de comandos del usuario, es el primer proceso lanzado por el sisyema, y dispone de las siguientes funcionalidades:
- **help**: Despliega al usuario los comandos disponibles. 
- **get_time**: Muestra la hora actual (UTC).
- **inforeg**: Imprime en pantalla el valor de todos los registros (con ctrl se guardan los registros).
- **test_divisionby0**: Ejemplo de excepcion de dividir por 0.
- **test_invalidop**: Ejemplo de excepcion por operacion invalida.
- **printmem**: realiza en memoria un volcado de memoria de 32 bytes a partir de la direccion recibida.
- **clean**: Limpia la pantalla.
- **test_mem**: Testeo de memoria.
- ***ps:*** Imprime el estado de los procesos vivos.
- ***kill:*** Mata a un proceso dado su ID.
- ***nice:*** Cambia la prioridad de un proceso dado su ID y la nueva prioridad.
- ***block:*** Cambia el estado de un proceso entre bloqueado y listo dado su ID.
- ***mem:*** muestra el estado de la memoria heap (bytes libres respecto del total).
- ***loop:*** Crea un proceso el cual itera sobre si mismo.

IPC:
- ***sem:*** Imprime la lista de todos los semáforos con sus propiedades.
- ***pipe:*** Imprime la lista de todos los pipes con sus propiedades.
- ***cat:*** Imprime el stdin tal como lo recibe. (con TAB se termina su ejecución)
- ***wc:*** Cuenta la cantidad de líneas del input. (con TAB se termina su ejecución)
- ***filter:*** Filtra las vocales recibidas como argumento de entrada. (con TAB se termina su ejecución)

- ***phylo:*** Dilema de los filosofos comiendo, inicia con 5 filosofos, más indiaciones son dadas al ejecutarlo.

- Para ```pipear``` la salida de un comando hacia la entrada de otro se utiliza el "***.***" , por ejemplo ***wc . filter***, al finalizar la ejecución del comando ***wc*** se mostrará su salida sin las vocales.


----------------------------------
## Testeos de la cátedra
- ***test_processes:*** Prueba la creacion de procesos con el scheduler (archivo dado por la cátedra).
- ***test_prio:*** Prueba la prioridad del scheduler (archivo dado por la cátedra).
- ***test_sync:*** Prueba la sincronizacion de semaforos (archivo dado por la cátedra).
- ***test_no_sync:*** Prueba la sincronizacion sin el uso de semaforos (archivo dado por la cátedra).
- ***test_mm:*** Prueba el manejo de memoria del memory manager (archivo dado por la cátedra).


---------------------------------

## Testeos PVS y CPP

Para el testeo con **PVS-Studio** se debe primero instalar **PVS-Studio** de no tenerlo, de la siguiente manera en su contenedor de *docker*.
```bash
apt-get update
apt-get install pvs-studio
pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"
```

Luego es muy recomendable instalar y utilizar la herramienta **Bear** para correctamente configurar las instrucciones de compilación que utilizará el **PVS-Studio**

  ```bash
  apt-get install bear
  bear make
  ```

  Y finalmente podrá ejecutar el **PVS-Studio**. Este anda mejor si realiza un `make clean` antes de ejecutarse:

  ``pvs-studio-analizer analyze -o project.log``

  Este generará un informe con el nombre **project.log**, pero para que sea legible por el usuario cosidere:

  ``plog-converter -a GA:1,2 -t tasklist -o project.tasks project.log``

  Ahora con un simple ``cat project.tasks`` podrá visualizar los resultados del testeo con **PVS-Studio**


Y para **Cppcheck**

``apt-get update``

``apt-get install cppcheck``

y luego ``cppcheck ./`` parado en el directorio base.
