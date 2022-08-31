////////////////////////////  CONTROL REMOTO /////////////////////////////////////////

#include <PS4Controller.h>

#define DEBUG true

#define PRIMERA_MARCHA 1
#define SEGUNDA_MARCHA 2

#define P_MOTOR_R_IZQ 15
#define P_MOTOR_L_IZQ 2
#define P_MOTOR_R_DER 4
#define P_MOTOR_L_DER 16

#define PIN_BUZZER 1

#define PIN_PULSADOR 1

#define DEAD_ZONE_IZQUIERDA -30
#define DEAD_ZONE_DERECHA 30

#define VELOCIDAD_ESTANDAR 220
#define VELOCIDAD_ENBESTIDA 255

enum
{
    CAMBIARMARCHA,
    FRENODEMANO,
    AVANZAR,
    RETROCESO,
    DOBLARDERECHA,
    DOBLARIZQUIERDA
};

int estadoControl = FRENODEMANO;

int marcha;

bool esperandoPulsador = true;

bool doblarDerecha;
bool doblarizquierda;
bool avanzar;
bool Retroceso;
bool cambioDeMarcha;
bool frenoDeMano;

int velocidad;

////////////pwm motor
int canal_l_derecho = 0;
int canal_r_derecho = 1;
int canal_l_izquierdo = 2;
int canal_r_izquierdo = 3;

int frecuencia = 5000;

int resolucion = 8;

class Buzzer
{
private:
    int pin;

public:
    Buzzer(int pin)
    {
        this->pin = pin;
        pinMode(pin, OUTPUT);
    }
    void Encender()
    {
        digitalWrite(pin, HIGH);
    }
    void Apagar()
    {
        digitalWrite(pin, LOW);
    }
};

class Pulsador
{
private:
    int pin;

public:
    Pulsador(int pin)
    {
        this->pin = pin;
        pinMode(pin, INPUT);
    }

    bool getEstado()
    {
        bool estado = digitalRead(pin);
        return estado;
    }
};

class MotoresPwm
{
private:
    int _p_r_izq, _p_l_izq, _p_r_der, _p_l_der, _canalDerL, _canalDerR, _canalIzql, _canalIzqR;
    int _frecuencia, _resolucion;

public:
    MotoresPwm(int pr_izq, int pl_izq, int pr_der, int pl_der, int canalDerL, int canalDerR, int canalIzql, int canalIzqR, int frecuencia, int resolucion)
    {
        _p_r_izq = pr_izq;
        _p_l_izq = pl_izq;
        _p_r_der = pr_der;
        _p_l_der = pl_der;
        _canalDerL = canalDerL;
        _canalDerR = canalDerR;
        _canalIzql = canalIzql;
        _canalIzqR = canalIzqR;
        _frecuencia = frecuencia;
        _resolucion = resolucion;

        ledcSetup(_canalDerL, _frecuencia, _resolucion);
        ledcSetup(_canalDerR, _frecuencia, _resolucion);
        ledcSetup(_canalIzql, _frecuencia, _resolucion);
        ledcSetup(_canalIzqR, _frecuencia, _resolucion);

        ledcAttachPin(_p_r_izq, _canalIzqR);
        ledcAttachPin(_p_l_izq, _canalIzql);
        ledcAttachPin(_p_r_der, _canalDerR);
        ledcAttachPin(_p_l_der, _canalDerL);
    }
    void Avanzar(int velocidad)
    {

        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, velocidad);

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, velocidad);
    }
    void Retroceder(int velocidad)
    {
        ledcWrite(_canalIzqR, velocidad);
        ledcWrite(_canalIzql, LOW);

        ledcWrite(_canalDerR, velocidad);
        ledcWrite(_canalDerL, LOW);
    }
    void Freno()
    {
        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, LOW);

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, LOW);
    }
    void DoblarIzquierda(int velocidad)
    {
        ledcWrite(_canalIzqR, velocidad);
        ledcWrite(_canalIzql, LOW);

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, velocidad);
    }
    void DoblarDerecha(int velocidad)
    {
        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, velocidad);

        ledcWrite(_canalDerR, velocidad);
        ledcWrite(_canalDerL, LOW);
    }
    void RotarIzq(int velocidad, int decremento)
    {

        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, velocidad - decremento);

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, velocidad);
    }
    void RotarDer(int velocidad, int decremento)
    {

        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, velocidad);

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, velocidad - decremento);
    }
};

MotoresPwm *movimientos = new MotoresPwm(P_MOTOR_R_IZQ, P_MOTOR_L_IZQ, P_MOTOR_R_DER, P_MOTOR_L_DER, canal_l_derecho, canal_r_derecho, canal_l_izquierdo, canal_r_izquierdo, frecuencia, resolucion);

Buzzer *buzzer = new Buzzer(PIN_BUZZER);

Pulsador *pulsador = new Pulsador(PIN_PULSADOR);

void Inicio()
{

    while (esperandoPulsador)
    {
        bool inicio = pulsador->getEstado();
        if (inicio)
        {
            buzzer->Encender();
            delay(5000);
            esperandoPulsador = false;
            buzzer->Apagar();
        }
    }
}

void CambiarMarchas()
{
    marcha++;
    if (marcha == PRIMERA_MARCHA)
        velocidad = VELOCIDAD_ESTANDAR;
    if (marcha == SEGUNDA_MARCHA)
        velocidad = VELOCIDAD_ENBESTIDA;
    if (marcha > SEGUNDA_MARCHA)
        marcha = PRIMERA_MARCHA;
}
void Controles()
{
    switch (estadoControl)
    {
    case CAMBIARMARCHA:
    {
        CambiarMarchas();
        ControlRemoto();
        break;
    }
    case FRENODEMANO:
    {
        movimientos->Freno();
        ControlRemoto();

        break;
    }
    case AVANZAR:
    {
        movimientos->Avanzar(velocidad);
        ControlRemoto();

        break;
    }
    case RETROCESO:
    {
        movimientos->Retroceder(velocidad);
        ControlRemoto();

        break;
    }
    case DOBLARDERECHA:
    {
        movimientos->DoblarDerecha(velocidad);
        ControlRemoto();

        break;
    }
    case DOBLARIZQUIERDA:
    {
        movimientos->DoblarIzquierda(velocidad);
        ControlRemoto();

        break;
    }
    }
}

void ControlRemoto()
{
    doblarDerecha = PS4.LStickX() > DEAD_ZONE_DERECHA;
    doblarizquierda = PS4.LStickX() < DEAD_ZONE_IZQUIERDA;
    avanzar = PS4.R2();
    Retroceso = PS4.L2();
    cambioDeMarcha = PS4.Circle();
    frenoDeMano = PS4.L1();

    if (cambioDeMarcha)
        estadoControl = CAMBIARMARCHA;
    if (frenoDeMano)
        estadoControl = FRENODEMANO;
    if (avanzar)
        estadoControl = AVANZAR;
    if (Retroceso)
        estadoControl = RETROCESO;
    if (doblarDerecha)
        estadoControl = DOBLARDERECHA;
    if (doblarizquierda)
        estadoControl = DOBLARIZQUIERDA;
    else
        estadoControl = FRENODEMANO;
    Controles();
}
void notify()
{
    ControlRemoto();
}
void onConnect()
{
  Serial.println("Connected!.");
}

void onDisConnect()
{
  Serial.println("Disconnected!.");    
}

void setup()
{
    
    CambiarMarchas();
    PS4.attach(notify);
    PS4.attachOnConnect(onConnect);
    PS4.attachOnDisconnect(onDisConnect);
    PS4.begin();
    Inicio();
    
}

void loop()
{
}
