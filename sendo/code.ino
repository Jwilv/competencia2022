#define DEBUG true
// colocar pines
#define PULSADOR A3
#define P_MOTOR_R_IZQ 9
#define P_MOTOR_L_IZQ 10
#define P_MOTOR_R_DER 11
#define P_MOTOR_L_DER 12
#define P_ULTRA_DER_TRIGGER 4
#define P_ULTRA_DER_ECHO 5
#define P_ULTRA_IZQ_TRIGGER 3
#define P_ULTRA_IZQ_ECHO 2
#define DISTANCIA_MAX 40

/// /////////////////////// VELOCIDADES //////////
int VELOCIDAD_DE_GIRO = 130;
int VELOCIDAD_RECONOCER = 150;
int VELOCIDAD_ATACAR = 200;
int VELOCIDAD_SEGUIMIENTO = 130;
int VELOCIDAD_REINTEGRO = 150;

////////////////////TIEMPO Y PERIODO PARA BUSQUEDA 

int tiempoBusqueda;
int periodoDebusquedad = 2000;
int tiempoAdelante = 200;


////////////////////////////////
#define MINIMO_PISO 200
#define CNYDER A5
#define CNYIZQ A4


//////////////////////////////////
bool giroIzquierda;
bool giroDerecha;
bool atras;
////////////////////////////
bool estadoPulsador;
//////////////////7

int distanciaDer;
int distanciaIzq;
int pisoDer;
int pisoIzq;
unsigned long tiempo;
int periodo;
bool ultraLogicDer = false;
bool ultraLogicIzq = false;

bool sumoIniciar = true;

class Cny70
{
private:
    int _pin, _receptor;

public:
    Cny70(int pin)
    {
        _pin = pin;
    }

    int GetValor()
    {
        return _receptor = analogRead(_pin);
    }
};
class Motores
{
private:
    int _p_r_izq, _p_l_izq, _p_r_der, _p_l_der;

public:
    Motores(int pr_izq, int pl_izq, int pr_der, int pl_der)
    {
        _p_r_izq = pr_izq;
        _p_l_izq = pl_izq;
        _p_r_der = pr_der;
        _p_l_der = pl_der;

        pinMode(_p_r_izq, OUTPUT);
        pinMode(_p_l_izq, OUTPUT);
        pinMode(_p_r_der, OUTPUT);
        pinMode(_p_l_der, OUTPUT);
    }
    void avanzar(int velocidad)
    {
        digitalWrite(_p_r_izq, LOW);
        analogWrite(_p_l_izq, velocidad);

        digitalWrite(_p_r_der, LOW);
        analogWrite(_p_l_der, velocidad);
    }
    void retroceder(int velocidad)
    {
        digitalWrite(_p_l_izq, LOW);
        analogWrite(_p_r_izq, velocidad);

        digitalWrite(_p_l_der, LOW);
        analogWrite(_p_r_der, velocidad);
    }
    void frenar()
    {
        digitalWrite(_p_r_izq, LOW);
        digitalWrite(_p_l_izq, LOW);

        digitalWrite(_p_r_der, LOW);
        digitalWrite(_p_l_der, LOW);
    }
    void giro_izquierda(int velocidad)
    {
        digitalWrite(_p_l_izq, LOW);
        analogWrite(_p_r_izq, velocidad);

        digitalWrite(_p_r_der, LOW);
        analogWrite(_p_l_der, velocidad);
    }
    void giro_derecha(int velocidad)
    {
        digitalWrite(_p_r_izq, LOW);
        analogWrite(_p_l_izq, velocidad);

        digitalWrite(_p_l_der, LOW);
        analogWrite(_p_r_der, velocidad);
    }
};

class UltraSonido
{
private:
    int _trigger;
    int _echo;
    int _distancia;
    unsigned long _tiempo;

public:
    UltraSonido(int trigger, int echo)
    {
        _trigger = trigger;
        _echo = echo;
        pinMode(_trigger, OUTPUT);
        pinMode(_echo, INPUT);
        digitalWrite(_trigger, LOW);
    }
    int GetDistancia()
    {
        digitalWrite(_trigger, HIGH);
        delayMicroseconds(10);
        digitalWrite(_trigger, LOW);
        _tiempo = pulseIn(_echo, HIGH);
        _distancia = _tiempo / 59;
        return _distancia;
    }
};

Motores *motor = new Motores(P_MOTOR_R_IZQ, P_MOTOR_L_IZQ, P_MOTOR_R_DER, P_MOTOR_L_DER); // poner pines de los motores como estan en la libreria
UltraSonido *ultraSDer = new UltraSonido(P_ULTRA_DER_TRIGGER, P_ULTRA_DER_ECHO);
UltraSonido *ultraSIzq = new UltraSonido(P_ULTRA_IZQ_TRIGGER, P_ULTRA_IZQ_ECHO);
Cny70 *cnyDer = new Cny70(CNYDER);
Cny70 *cnyIzq = new Cny70(CNYIZQ);

void Pulsador()
{

    estadoPulsador = false;
    while (sumoIniciar)
    {
        estadoPulsador = analogRead(PULSADOR);
        if (estadoPulsador)
        {
            delay(5000);
            sumoIniciar = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
    enum MOVIMIENTO {
        ADELANTE,
        RETROCEDER,
        ATACAR,
        DOBLARIZQUIERDA,
        DOBLARDERECHA,
        BUSCAR
    };
int movimientos = BUSCAR;


/////////////////////////////////////////////////////

void Reconocimiento()
{
    distanciaDer = ultraSDer->GetDistancia();
    distanciaIzq = ultraSIzq->GetDistancia();
    pisoDer = cnyDer->GetValor();
    pisoIzq = cnyIzq->GetValor();
    ultraLogicIzq = (distanciaIzq < DISTANCIA_MAX);
    ultraLogicDer = (distanciaDer < DISTANCIA_MAX);
    //////////////////////////////////////////////////
    giroIzquierda = (pisoDer < MINIMO_PISO);
    giroDerecha = (pisoIzq < MINIMO_PISO);
    atras = (giroDerecha && giroIzquierda);
    //////////////////////////////////////////////////
    if (atras) movimientos = RETROCEDER;
    else if (giroIzquierda) movimientos = DOBLARIZQUIERDA;
    else if (giroDerecha) movimientos = DOBLARDERECHA;
    ////////////////////////////////////////////////////
    else if (ultraLogicIzq && ultraLogicDer) movimientos = ATACAR;
    else if (ultraLogicIzq) movimientos = DOBLARIZQUIERDA;
    else if (ultraLogicDer) movimientos = DOBLARDERECHA;
    else movimientos = BUSCAR;

    if(DEBUG){
        Serial.println("distancia ultra derecha");
        Serial.println(distanciaDer);
        Serial.println("distancia ultra izquierda");
        Serial.println(distanciaIzq);
        Serial.println("valor cny derecho");
        Serial.println(pisoDer);
        Serial.println("valor cny izquierdo");
        Serial.println(pisoIzq);

    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
void movimiento(int estado){
switch (estado)
{

case ADELANTE:
{
    motor->avanzar(VELOCIDAD_RECONOCER);
    delay(tiempoAdelante);
    Reconocimiento();
    break;
}

case RETROCEDER:
{
    motor->retroceder(VELOCIDAD_REINTEGRO);
    delay(100);
    Reconocimiento();
    break;
}

case ATACAR:
{
    motor->avanzar(VELOCIDAD_ATACAR);
    Reconocimiento();
    break;
}

case DOBLARIZQUIERDA:
{
    motor->giro_izquierda(VELOCIDAD_SEGUIMIENTO);
    Reconocimiento();
    break;
}

case DOBLARDERECHA:
{
    motor->giro_derecha(VELOCIDAD_SEGUIMIENTO);
    Reconocimiento();
    break;
}
case BUSCAR:
{
    motor->giro_derecha(VELOCIDAD_SEGUIMIENTO);
    if (tiempoBusqueda + millis() > periodoDebusquedad)
    {
        tiempoBusqueda = millis();
        movimientos = ADELANTE;
    }
    
    Reconocimiento();
    break;
}
}
    
}



void setup()
{
    Serial.begin(9600);
    Pulsador();
}
    
//////////////////////////////

void loop(){
    Reconocimiento();
    movimiento(movimientos);
}
