
/////////////////////////////     seguidor
#define PIN_PULSADOR 1

#define PIN_BUZZER 1

#define PROMEDIO 2 

#define DIFERENCIAL 20

#define MARGEN 20

bool esperandoPulsador = true;

int velocidadSeguidor = 200; //si se pasa baja la velocidad 

#define CNYDER 1
#define CNYIZQ 1 //poner pin

#define P_MOTOR_R_IZQ 1 //poner pin
#define P_MOTOR_L_IZQ 1 //poner pin
#define P_MOTOR_R_DER 1 //poner pin
#define P_MOTOR_L_DER 1 //poner pin


#define P_ULTRA_DER_TRIGGER 1 //poner pin
#define P_ULTRA_DER_ECHO 1 //poner pin

#define P_ULTRA_IZQ_TRIGGER 1 //poner pin
#define P_ULTRA_IZQ_ECHO 1 //poner pin

#define DISTANCIA_MAX 30

int VELOCIDAD_DE_GIRO = 130;

int VELOCIDAD_RECONOCER = 130;

#define MINIMO_PISO 200

#define NEGROMAX 400



int VELOCIDAD_ATAQUE = 100; // regular
int VELOCIDAD_SEGUIMIENTO = 145;
////////////////////////////////////
bool giroIzquierda;
bool giroDerecha;
///////////////////////////


int estadoActualDerecho;
int estadoActualIzquierdo;

int promedioIdeal;
int promedioActual;

int valorSensorIzquierdo;
int valorSensorDerecho;

bool sensorNegroIzq = false;
bool sensorNegroDer = false;

bool doblarIzquierda = false;

bool buscandoPromedio = true;

////////////pwm motor
int canalDerL = 0;
int canalDerR = 1;
int canalIzql = 2;
int canalIzqR = 3;

int frecuencia = 5000;

int resolucion = 8;
/////////////////////////////////////////////
class Pulsador{
private:
int pin;
public:
Pulsador(int pin){
this->pin = pin;
pinMode(pin,INPUT);
}
 bool getEstado()
  {
    bool estado = digitalRead(pin);
    return estado;
  }
};

/////////////////////////////////////////////////
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
    void avanzar(int velocidad)
    {

        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, velocidad);

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, velocidad);
    }
    void retroceder(int velocidad)
    {
        ledcWrite(_canalIzqR, velocidad);
        ledcWrite(_canalIzql, LOW);

        ledcWrite(_canalDerR, velocidad);
        ledcWrite(_canalDerL, LOW);
    }
    void frenar()
    {
        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, LOW);

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, LOW);
    }
    void giro_izquierda(int velocidad)
    {
        ledcWrite(_canalIzqR, velocidad);
        ledcWrite(_canalIzql, LOW);

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, velocidad);
    }
    void giro_derecha(int velocidad)
    {
        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, velocidad);

        ledcWrite(_canalDerR, velocidad);
        ledcWrite(_canalDerL, LOW);
    }
    void RotarIzq(int velocidad, int decremento){
        
        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, velocidad - decremento );

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, velocidad );
    }
    void RotarDer(int velocidad, int decremento){
        
        ledcWrite(_canalIzqR, LOW);
        ledcWrite(_canalIzql, velocidad  );

        ledcWrite(_canalDerR, LOW);
        ledcWrite(_canalDerL, velocidad - decremento);
    }
};
////////////////////////////////////////////////

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

/////////////////////////////////////
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
        return _receptor = map(analogRead(_pin),0,4095,0,1000);;
    }
};
/////////////////////////////////////////////////////////////////
class Pulsador1{
private:
int pin;
public:
Pulsador1(int pin){
this->pin = pin;
pinMode(pin,INPUT);
}
 bool getEstado()
  {
    bool estado = digitalRead(pin);
    return estado;
  }
};
/////////////////////////////////////////////////////////////////

Pulsador1 *pulsador = new Pulsador1(PIN_PULSADOR);

Buzzer *buzzer = new Buzzer(PIN_BUZZER);

////////////////////////////////////////////////////////

Cny70 *cnyDer = new Cny70(CNYDER);
Cny70 *cnyIzq = new Cny70(CNYIZQ);
///////////////////////////////////////////////
MotoresPwm *motor = new MotoresPwm(P_MOTOR_R_IZQ, P_MOTOR_L_IZQ, P_MOTOR_R_DER, P_MOTOR_L_DER, canalDerL, canalDerR, canalIzql, canalIzqR, frecuencia, resolucion);
////////////////////////////////////////////////////seguidor/////////////////////////////////////////////////////////////
void TomarValores(int sensorIzquierdo,int sensorDerecho){
    sensorIzquierdo = cnyIzq->GetValor();
    sensorDerecho = cnyDer->GetValor();
}

int PromedioIdeal(){
    TomarValores(valorSensorIzquierdo,valorSensorDerecho);
    if(valorSensorIzquierdo > valorSensorDerecho) sensorNegroIzq = true, sensorNegroDer = false;
    else if(valorSensorDerecho > valorSensorIzquierdo) sensorNegroDer = true, sensorNegroIzq = false;
    return (valorSensorIzquierdo + valorSensorDerecho) / PROMEDIO;
}

int PromedioActual(){
    TomarValores(estadoActualIzquierdo,estadoActualDerecho);
    return (estadoActualIzquierdo + estadoActualDerecho) / PROMEDIO;
}

void Seguir(){
    promedioActual = PromedioActual();
    if(promedioActual > promedioIdeal + MARGEN){
        if(!sensorNegroDer) motor->RotarDer(velocidadSeguidor,20);
        else if(!sensorNegroIzq) motor->RotarIzq(velocidadSeguidor,20);
    } //significa que nos estamos llendo a lo negro
    else if(promedioActual < promedioIdeal - MARGEN){
        if(sensorNegroDer) motor->RotarDer(velocidadSeguidor,20);
        else if(sensorNegroIzq)  motor->RotarIzq(velocidadSeguidor,20);
    } 
    else motor->avanzar(velocidadSeguidor);
}//significa que nos estamos llendo a lo blanco 

void BuscarPromedio(){
    if(doblarIzquierda)motor->RotarIzq(velocidadSeguidor,20);
    //else //dobalr derecha
    TomarValores(estadoActualIzquierdo,estadoActualDerecho);
    bool promediar = (estadoActualIzquierdo - estadoActualDerecho) > DIFERENCIAL;
    if(promediar){
        promedioIdeal = PromedioIdeal();
        buscandoPromedio = false;
    }
}



void seguidor(){

    if(buscandoPromedio)BuscarPromedio();
    Seguir();
}

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



void setup()
{
    Inicio();
}
void loop()
{
    seguidor();
}
