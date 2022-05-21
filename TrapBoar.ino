#include <Servo.h>  // add servo library
 
Servo myservo;  // create servo object to control a servo

//Constastes
const int potPin = A0;  // analog pin used to connect the potentiometer
const int Led_Auto = 13;
const int Led_Trava = 4;
const int trigPin = 11; 
const int echoPin = 10;
const int Servo_Motor = 9;  //PWM
const int Rele_Servo = 5; //para cortar a alimentacao do Servo Motor

//Graus para o Servo Motor
const int Porta_Travada = 110;
const int Porta_Destravada = 157;

//Botão Interrupt
const int BotaoTrava = 3;
bool JavCapt = true; //Javali Capturado

//Variaveis
int Graus_Servo;    // variable to read the value from the analog pin
int Aux_Graus_Servo = 0;  //para comparar se o valor dos passos ssrvo motor mudou
float duration, distance;
bool TrapBoarAtivo =  false;
//bool TravaAtiva =  false;
bool stateTrava = false;

bool Print_Serial = false;  //variavel para evitar erros no Bluetooh

void setup() {
  myservo.attach(Servo_Motor);  // attaches the servo on pin 9 to the servo object
  pinMode(Led_Auto, OUTPUT);
  pinMode(Led_Trava, OUTPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  pinMode(Rele_Servo, OUTPUT);

  // Define os botoes como entrada e ativa os resistores de pull-up
  pinMode(BotaoTrava, INPUT_PULLUP);
  // Define as interrupções, ambos como detecção na borda de descida (Ativa e desativa a trava da porta)
  attachInterrupt(digitalPinToInterrupt(BotaoTrava), ativaTravaPorta, FALLING);
  // Inicia a comunicação serial
  Serial.begin(9600);
  Serial.println("TrapBoar");
  
}
void ativaTravaPorta() {
  // Comuta o estado da variavel
  stateTrava = !stateTrava;
  Serial.print("stateTrava: ");
  Serial.println(stateTrava);
  
  
  
} 
void loop() {

  // Criando uma variável do tipo caractere 
  char RecebeBluetooth;

  // Variável 'RecebeBluetooth' recebe o valor da porta Serial
  RecebeBluetooth = Serial.read();
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  
  //if para evitar erros no Bluetooth
  if (Print_Serial){
    Serial.print("|Distance|");
    Serial.print(distance);
    Serial.print("|TrapBoarAtivo| ");
    Serial.print(TrapBoarAtivo);
    Serial.print("|stateTrava| ");
    Serial.print(stateTrava);
    Serial.print("|RecebeBluetooth| ");
    Serial.println(RecebeBluetooth);
  }
  
  //delay(100);
  
  //valorPot = analogRead(potPin);            // reads the value of the potentiometer (value between 0 and 1023)
  //Serial.print("Potenciometro: ");
  //Serial.println(valorPot);
  //valorPot = map(valorPot, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  //Serial.print("Servo Motor: ");
  //Serial.println(valorPot);
  
  //Ativa e desativa modo automatico do TrapBoar
  if (RecebeBluetooth == 'A'){
    TrapBoarAtivo = true;
    //digitalWrite(Led_Auto, HIGH);
    //TravaAtiva = false;    
  }
  if (RecebeBluetooth == 'a'){
    TrapBoarAtivo = false;
    //digitalWrite(Led_Auto, LOW);
    //TravaAtiva = false;    
  }
  //Ativa e desativa trava do TrapBoar
  if (RecebeBluetooth == 'T'){
    stateTrava = true;
    //digitalWrite(Led_Trava, HIGH);
  }
  if (RecebeBluetooth == 't'){
    stateTrava = false;
    //digitalWrite(Led_Trava, LOW);
       
  }
    
  if (TrapBoarAtivo){
    digitalWrite(Led_Auto, HIGH);
    if (distance < 6.5){
      Graus_Servo = Porta_Destravada; //porta destravada
      
      if (Graus_Servo != Aux_Graus_Servo){
        digitalWrite(Rele_Servo, !HIGH);
        myservo.write(Graus_Servo); // sets the servo position according to the scaled value
        delay(1000);
        digitalWrite(Rele_Servo, !LOW);        
        Aux_Graus_Servo = Graus_Servo;
      }
      
      
      
      delay(2000);
      Graus_Servo = Porta_Travada; //porta travada
      TrapBoarAtivo =  false;
      
      //envia mensagem por Bluetooth
      Serial.print("Jc|");
      Serial.println(distance);
    }
  }
  else{
    digitalWrite(Led_Auto, LOW); 
    if (stateTrava){
      Graus_Servo = Porta_Destravada; //porta destravada
      digitalWrite(Led_Trava, HIGH);
      // Comuta o estado do LED
      //digitalWrite(Led_Trava, !digitalRead(Led_Trava));
        
      //ativar a abertura da Porta novamente 
      //TrapBoarAtivo =  true;
    }
    else{
      digitalWrite(Led_Trava, LOW);
      Graus_Servo = Porta_Travada; //porta travada
    }
  }
  
  
  if (Graus_Servo != Aux_Graus_Servo){
    digitalWrite(Rele_Servo, !HIGH);
    myservo.write(Graus_Servo); // sets the servo position according to the scaled value
    delay(1000); // waits for the servo to get there
    digitalWrite(Rele_Servo, !LOW);        
    Aux_Graus_Servo = Graus_Servo;;
  }
 delay(1000); 
}
