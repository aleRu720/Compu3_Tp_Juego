# Compu3_Tp_Juego
## Ayuda

- Expansión de la maquina de estado para poder manejar cualquier número de pulsadores
- Estructura para agrupar la información de los pulsadores
- Las funciones BusIn y BusOut deben inicializarse con el pulsador y el led que se quiere asociar en la misma posición en cada función.
```c++

BusIn pulsadores(PB_6,PB_7,PB_8,PB_9);

BusOut leds(PB_12,PB_13,PB_14,PB_15);

```
## Generación de Tiempos y leds aleatorios

Se agrega la generación de tiempos aleatorios y leds aleatorios. Para que la función rand() genere números en un determinado intervalo se utiliza la siguiente formula:
```c
int numeroAleatorio = rand() % (valorMaximo+1)+valorBase;
```
Si quiero generar números ente 100 y 1000 entonces:
- valorMAximo = 1000
- valorBase = 100
- Dentro del programa, la parte que genera los tiempos y los leds de forma aleatoria es:
```c
    randomLed =  ((rand()%MAXLED+1)-1);
    randomInterval = (rand()% (MAXTIME+1)+BASETIME);
```
## IMPORTANTE
Para generar la secuencia aleatoria, se debe presionar el pulsador 1 y 4 simultaneamente. En caso contrario, el programa responde a la detección de flanco en los pulsadores encendiendo el led asociado a cada pulsador en el cambio de flanco.