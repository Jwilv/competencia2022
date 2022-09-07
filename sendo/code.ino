#define DEBUG true
//colocar pines
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
int VELOCIDAD_DE_GIRO = 130;
int VELOCIDAD_RECONOCER = 130;
#define MINIMO_PISO 200
#define CNYDER A5
#define CNYIZQ A4
int VELOCIDAD_ATAQUE = 200;
int VELOCIDAD_SEGUIMIENTO = 145;

//////////////////////////////////
bool giroIzquierda;
bool giroDerecha ;
bool atras;
////////////////////////////

int distanciaDer;
int distanciaIzq;
int pisoDer;
int pisoIzq;
unsigned long tiempo;
int periodo;
bool ultraLogicDer = false;
bool ultraLogicIzq = false;

bool sumoIniciar = true;

class Cny70 {
private:
  int _pin, _receptor;

public:
  Cny70(int pin) {
    _pin = pin;
  }

  int GetValor() {
    return _receptor = analogRead(_pin);
  }
};
class Motores {
private:
  int _p_r_izq, _p_l_izq, _p_r_der, _p_l_der;

public:
  Motores(int pr_izq, int pl_izq, int pr_der, int pl_der) {
    _p_r_izq = pr_izq;
    _p_l_izq = pl_izq;
    _p_r_der = pr_der;
    _p_l_der = pl_der;

    pinMode(_p_r_izq, OUTPUT);
    pinMode(_p_l_izq, OUTPUT);
    pinMode(_p_r_der, OUTPUT);
    pinMode(_p_l_der, OUTPUT);
  }
  void avanzar(int velocidad) {
    digitalWrite(_p_r_izq, LOW);
    analogWrite(_p_l_izq, velocidad);

    digitalWrite(_p_r_der, LOW);
    analogWrite(_p_l_der, velocidad);
  }
  void retroceder(int velocidad) {
    digitalWrite(_p_l_izq, LOW);
    analogWrite(_p_r_izq, velocidad);

    digitalWrite(_p_l_der, LOW);
    analogWrite(_p_r_der, velocidad);
  }
  void frenar() {
    digitalWrite(_p_r_izq, LOW);
    digitalWrite(_p_l_izq, LOW);

    digitalWrite(_p_r_der, LOW);
    digitalWrite(_p_l_der, LOW);
  }
  void giro_izquierda(int velocidad) {
    digitalWrite(_p_l_izq, LOW);
    analogWrite(_p_r_izq, velocidad);

    digitalWrite(_p_r_der, LOW);
    analogWrite(_p_l_der, velocidad);
  }
  void giro_derecha(int velocidad) {
    digitalWrite(_p_r_izq, LOW);
    analogWrite(_p_l_izq, velocidad);

    digitalWrite(_p_l_der, LOW);
    analogWrite(_p_r_der, velocidad);
  }
};

class UltraSonido {
private:
  int _trigger;
  int _echo;
  int _distancia;
  unsigned long _tiempo;

public:
  UltraSonido(int trigger, int echo) {
    _trigger = trigger;
    _echo = echo;
    pinMode(_trigger, OUTPUT);
    pinMode(_echo, INPUT);
    digitalWrite(_trigger, LOW);
  }
  int GetDistancia() {
    digitalWrite(_trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigger, LOW);
    _tiempo = pulseIn(_echo, HIGH);
    _distancia = _tiempo / 59;
    return _distancia;
  }
};

Motores *motor = new Motores(P_MOTOR_R_IZQ, P_MOTOR_L_IZQ, P_MOTOR_R_DER, P_MOTOR_L_DER);  // poner pines de los motores como estan en la libreria
UltraSonido *ultraSDer = new UltraSonido(P_ULTRA_DER_TRIGGER, P_ULTRA_DER_ECHO);
UltraSonido *ultraSIzq = new UltraSonido(P_ULTRA_IZQ_TRIGGER, P_ULTRA_IZQ_ECHO);
Cny70 *cnyDer = new Cny70(CNYDER);
Cny70 *cnyIzq = new Cny70(CNYIZQ);

//avanzamos solo si no estamos sobre la linea del borde en ese caso doblamos
// poner parametros en limite
//la velocidad de reintegro es la velocidad de giro para volver a entrar en la pista
void Adelante(int velocidadAdelante, int velocidadReintegro) {
  bool giroIzquierda = (pisoDer < MINIMO_PISO);
  bool giroDerecha = (pisoIzq < MINIMO_PISO);
  bool atras = (giroDerecha && giroIzquierda);

  if (atras) motor->retroceder(velocidadReintegro);
  else if (giroDerecha)
    motor->giro_derecha(velocidadReintegro);
  else if (giroIzquierda)
    motor->giro_izquierda(velocidadReintegro);
  else
    motor->avanzar(velocidadAdelante);
}

// giramos y en caso de no encontrar nada avanzamos
void Reconocer() {
  motor->giro_izquierda(VELOCIDAD_DE_GIRO);  // meter velocidad de giro
  if (millis() < tiempo + periodo) {
    tiempo = millis();
    Adelante(VELOCIDAD_RECONOCER, VELOCIDAD_DE_GIRO);
  }
}


void sensores() {
  distanciaDer = ultraSDer->GetDistancia();
  distanciaIzq = ultraSIzq->GetDistancia();
  pisoDer = cnyDer->GetValor();
  pisoIzq = cnyIzq->GetValor();
  ultraLogicIzq = (distanciaIzq < DISTANCIA_MAX);
  ultraLogicDer = (distanciaDer < DISTANCIA_MAX); 
}


void Busquedad() {
  //bool atacar = (ultraLogicIzq && ultraLogicDer);
  //bool izquierda = (ultraLogicIzq);
  //bool derecha = (ultraLogicDer);

  if (ultraLogicIzq && ultraLogicDer) Adelante(VELOCIDAD_ATAQUE, VELOCIDAD_DE_GIRO);
  else if (ultraLogicIzq)
    motor->giro_izquierda(VELOCIDAD_SEGUIMIENTO);
  else if (ultraLogicDer)
    motor->giro_derecha(VELOCIDAD_SEGUIMIENTO);
  else
    Reconocer();
}

void Pulsador() {
  
  int estadoPulsador = 0;
  while (sumoIniciar) {
    estadoPulsador = analogRead(PULSADOR);
    if (estadoPulsador > 0) {
      delay(5000);
      sumoIniciar = false;
    }
  }
}

void setup() {
  Serial.begin(9600);
  Pulsador();
}

void loop() {
  Busquedad();
} 
////////////////////////////////////////////////////////////////////////////////////////////
enum MOVIMIENTO {
    ADELANTE,
    RETROCEDER,
    ATACAR,
    GIRAR,
    DOBLARIZQUIERDA,
    DOBLARDERECHA
}; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
switch() {


case ADELANTE: {
  Adelante(VELOCIDAD_ATAQUE, VELOCIDAD_DE_GIRO);
  //LLAMAR A LOS SENSORES  
  break;
}

case RETROCEDER: {
  motor->retroceder(velocidadReintegro);
  //LLAMAR A LOS SENSORES
  break;
}

case ATACAR: {
  motor->avanzar(velocidadAdelante);
    //LLAMAR A LOS SENSORES
  break;
}

case GIRAR: {
  motor->giro_izquierda(VELOCIDAD_DE_GIRO);
   //LLAMAR A LOS SENSORES
  break;
}

case DOBLARIZQUIERDA: {
  motor->giro_izquierda(VELOCIDAD_SEGUIMIENTO);
  //LLAMAR A LOS SENSORES  
  break;
}

case DOBLARDERECHA: {
  motor->giro_derecha(VELOCIDAD_SEGUIMIENTO);
    //LLAMAR A LOS SENSORES 
  break;
}

} 

void Reconocimiento(){
 distanciaDer = ultraSDer->GetDistancia();
  distanciaIzq = ultraSIzq->GetDistancia();
  pisoDer = cnyDer->GetValor();
  pisoIzq = cnyIzq->GetValor();
  ultraLogicIzq = (distanciaIzq < DISTANCIA_MAX);
  ultraLogicDer = (distanciaDer < DISTANCIA_MAX);
//////////////////////////////////////////////////
bool giroIzquierda = (pisoDer < MINIMO_PISO);
bool giroDerecha = (pisoIzq < MINIMO_PISO);
bool atras = (giroDerecha && giroIzquierda);
//////////////////////////////////////////////////
if(atras)
else if(giroIzquierda)
else if(giroDerecha)

  

////////////////////////////////////////////////////
 if (ultraLogicIzq && ultraLogicDer) Adelante(VELOCIDAD_ATAQUE, VELOCIDAD_DE_GIRO);
  else if (ultraLogicIzq)
    motor->giro_izquierda(VELOCIDAD_SEGUIMIENTO);
  else if (ultraLogicDer)
    motor->giro_derecha(VELOCIDAD_SEGUIMIENTO);


  break;
}
