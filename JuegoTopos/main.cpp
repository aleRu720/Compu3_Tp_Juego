#include "mbed.h"

#define TRUE    1

/**
 * @brief Defino el intervalo entre lecturas para "filtrar" el ruido del Botón
 * 
 */
#define INTERVAL    40
/**
 * @brief Defino la cantidad de botones que voy a usar
 * 
 */
#define NUMBUTT     4
/**
 * @brief Definiciones varias 
 * 
 */
#define ON          1
#define OFF         0
#define INVALIDTIME -1
#define HEARBEATIME 200
#define NUMBEAT     4



/**
 * @brief Enumeración que contiene los estados de la máquina de estados(MEF) que se implementa para 
 * el "debounce" 
 * El botón está configurado como PullUp, establece un valor lógico 1 cuando no se presiona
 * y cambia a valor lógico 0 cuando es presionado.
 */
typedef enum{
    BUTTON_DOWN,
    BUTTON_UP,
    BUTTON_FALLING,
    BUTTON_RISING
}_eButtonState;

/**
 * @brief Estructura para los botones 
 * 
 */
typedef struct{
    uint16_t mask;          //!< MAscara para filtrar el valor de la entrada deseada
    uint8_t buttonState;    //!< Estado del boton según la MEF
    uint8_t buttonValue;    //!< Valor leído de la entrada
    uint8_t ledState;       //!< Estado del led asociado al botón ON/OFF
    uint8_t index;          //!< Indice, posición el botón/led en los arreglos BusIn/BusOut
    int32_t timeDown;       //!< Se utilizará para almacenar el valor del timer cuando se presionó el botón
    int32_t timeDif;        //!< Se utiliza para almacenar el tiempo que estuvo presionado el botón
} _sButton;


/**
 * @brief Dato del tipo Enum para asignar los estados de la MEF
 * 
 */
_eButtonState myButton;




/**
 * @brief  Máquina de Estados Finitos(MEF)
 * Actualiza el estado del botón cada vez que se invoca.
 * 
 * @param[in] buttons Puntero a estructura del tipo _sButton, contiene todos los datos del botón asociado
 */
void actuallizaMef(_sButton * buttons );

/**
 * @brief Cambia el estado de led asociado a un botón
 * 
 * @param[in] buttons Puntero a estructura del tipo _sButton, contiene todos los datos del botón asociado así 
 *                    como también el valor del led correspondiente.
 */
void togleLed(_sButton * buttons);


/**
 * @brief Inicializa los pulsadores 
 * Inicializa los valores del arreglo de estructura _sButton, con los valores de arranque. 
 * También se inicializa la MEF con el correspondiente valor para cada Botón
 * 
 * @param[in] buttons Puntero a estructura del tipo _sButton, contiene todos los datos del botón asociado
 */
void inicializaPulsadores( _sButton * buttons);

/**
 * @brief Latido!
 * Destella el led de la placa para comprobar que el programa funciona
 * 
 */
void hearbeat(void);

/**
 * @brief Arreglo de entradas
 * Permite manejar todas las entradas como una sola variable.
 */
BusIn pulsadores(PB_6,PB_7,PB_8,PB_9);

/**
 * @brief Arreglo de salidas
 * Permite manejar un grupo de salidas como una sola variable. 
 */
BusOut leds(PB_12,PB_13,PB_14,PB_15);

/**
 * @brief Hearbeat (Latido de corazón) salida utilizada para ver que el programa está funcionando
 *  
 */
DigitalOut hearBeat(PC_13);

Timer miTimer; //!< Timer para hacer la espera de 40 milisegundos

int tiempoMs=0; //!< variable donde voy a almacenar el tiempo del timmer una vez cumplido


int main()
{
    uint16_t buttonValueAux;
    int beatTime=1;
    _sButton pulsador[NUMBUTT];

    miTimer.start();    
    inicializaPulsadores(pulsador);
    hearBeat=0;
 
    while(TRUE)
    {
       if ((miTimer.read_ms()-beatTime)>HEARBEATIME)
       {
           beatTime=miTimer.read_ms();
           hearbeat();
       }
       
       if ((miTimer.read_ms()-tiempoMs)>INTERVAL)
       {
        tiempoMs=miTimer.read_ms();
         buttonValueAux = pulsadores;
            for (uint8_t index=0; index < NUMBUTT; index++){
                pulsador[index].buttonValue = pulsador[index].mask & buttonValueAux;
                actuallizaMef(&pulsador[index]);
            }
        }

    }
    return 0;
}



void actuallizaMef(_sButton * buttons)
{
    switch ((buttons->buttonState))
    {
        case BUTTON_DOWN:
            
            if( buttons->buttonValue)
                buttons->buttonState = BUTTON_RISING;
        break;
        case BUTTON_UP:
            if(!buttons->buttonValue)
                buttons->buttonState = BUTTON_FALLING;
        
        break;
        case BUTTON_FALLING:
            if(!buttons->buttonValue)
            {
            buttons->buttonState = BUTTON_DOWN;
                //Flanco de bajada
            }
            else
                buttons->buttonState = BUTTON_UP;    

        break;
        case BUTTON_RISING:
            if(buttons->buttonValue)
            {
                buttons->buttonState = BUTTON_UP;
                //Flanco de Subida
                buttons->ledState = !buttons->ledState;
                togleLed(buttons);
            }
            else
                buttons->buttonState = BUTTON_DOWN;
        break;
        
        default:
            buttons->buttonState = BUTTON_UP;
            break;
    }
}

void togleLed(_sButton * buttons){
    uint16_t ledsAux=leds;
    if(buttons->ledState)
        ledsAux |= 1 << (buttons->index) ;
    else
       ledsAux &= ~(1 << (buttons->index)) ; 
    leds = ledsAux ;
}


void inicializaPulsadores( _sButton * buttons){

    for(uint8_t index=0; index<NUMBUTT;index++){
         buttons[index].mask=0;
        buttons[index].mask |= 1<<index ;
        buttons[index].index=index;
        buttons[index].ledState = OFF;
        buttons[index].timeDif=INVALIDTIME;
        buttons[index].timeDown=INVALIDTIME;
        buttons[index].buttonState=BUTTON_UP;
        buttons[index].buttonValue = buttons[index].mask & pulsadores.read();
    }

}

void hearbeat(void)
{
    static uint8_t index=0;
    if(index<NUMBEAT){
        hearBeat=!hearBeat;
        index++;
    }else{
        hearBeat=1;
        index = (index>=25) ? (0) : (index+1);
    
    }

}