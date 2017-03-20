/*
Programme pour réparer des parebrises
Matos :
2 Electrovannes
1 capteur de pression
1 pompe à air comprimé
*/

#include <SoftwareSerial.h>
#include <Nextion.h>
#include <Wire.h>
SoftwareSerial nextionSerial(10, 11); // RX, TX

Nextion nex(nextionSerial, 9600);

/*
Exemple output nextion
"65 0 2 0 ffff ffff ffff"
    65: Correspond à un événement touché
    0:  Correspont au numero de la pge
    2: Est le numero du boutton
    0: Le type d'évènement "0" pour modifié, "1" pour pressé
    FFFF FFFF FFFF: Pour dire que le message est terminé
*/    

String ReparationRapide = "65 1 1 1 ffff ffff ffff"; // Page 1 - Réparation rapide "b0"
String ReparationA = "65 1 2 1 ffff ffff ffff"; // Page 1 - Réparation A "b1"
String ReparationB = "65 1 3 1 ffff ffff ffff"; // Page 1 - Réparation B "b2"
String ReparationManu = "65 1 4 1 ffff ffff ffff"; // Page 1 - Réparation C "b3"

String Depart = "65 2 6 1 ffff ffff ffff"; // Page 2 - Départ réparation "b1"
String Arret = "65 2 7 1 ffff ffff ffff"; // Page 2  - Arrêt réparation "b2"

// Pression 0 bar = 35, -0.8 bar = 7, 3 bar = 120
float Pression_1 = 0;
int Cycle_1 = 0;
float Pression_2 = 0;
int Cycle_2 = 0;
float Pression_3 = 0;
int Cycle_3 = 0;
float Pression_4 = 0;
int Cycle_4 = 0;
int CycleTotal = 0;

bool EventChoix = false;
bool EventReparation = false;

const int EncodeurPression = 5;    //  sortie Encodeur Pression
const int EncodeurDepression = 6;  //  sortie Encodeur Depression
const int PressionDeBase = 7;  //  sortie Encodeur Depression

static long EtatPression = 0; 
int EncodeurPressionLast = LOW;
int EtatEncodeur = LOW;

const int Electrovanne1 = 2; // Led verte Pression
const int Electrovanne2 = 3; // Led bleu Depression
const int Pompe = 4; // LOW Pompe coupé, HIGH en marche // Led rouge
const int CapteurPression1 = A0;
int LecturePression1 = 0;
int atm = 160;


int AvancementEtape = 0;
int AvancementTotal = 0;

long Temps = 0;
int i = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(Electrovanne1, OUTPUT);
  pinMode(Electrovanne2, OUTPUT);

  nextionSerial.begin(9600);
  nex.init();

  // Electrovanne 1
  pinMode(Electrovanne1, OUTPUT);
  digitalWrite(Electrovanne1, LOW);
  // Electrovanne 2
  pinMode(Electrovanne1, OUTPUT);
  digitalWrite(Electrovanne1, LOW);
  // Pompe
  pinMode(Pompe, OUTPUT);
  digitalWrite(Pompe, LOW);
  Temps = millis();
  
  pinMode(EncodeurPression,INPUT);
  pinMode(EncodeurDepression,INPUT); 
  pinMode(PressionDeBase,INPUT);

}

void loop()
{
  
  while ( EventChoix != true )
  {
    String message = nex.listen(); //check message nextion
    if ( message == ReparationRapide )
    {
    
      Pression_1 = -800;
      Cycle_1 = 30;
      Pression_2 = 2500;
      Cycle_2 = 30;
      Pression_3 = -800;
      Cycle_3 = 30;
      Pression_4 = 3000;
      Cycle_4 = 60;
      CycleTotal = Cycle_1 + Cycle_2 + Cycle_3 + Cycle_4;
      EventChoix = true;
      AvancementAzero(); // Met à 0 les barres d'avancement
      
    }
    else if ( message == ReparationA )
    {
      Pression_1 = -800;
      Cycle_1 = 3;
      Pression_2 = 2500;
      Cycle_2 = 3;
      Pression_3 = -800;
      Cycle_3 = 3;
      Pression_4 = 3000;
      Cycle_4 = 6;
      CycleTotal = Cycle_1 + Cycle_2 + Cycle_3 + Cycle_4;
      EventChoix = true;
      AvancementAzero(); // Met à 0 les barres d'avancement
    }
    else if ( message == ReparationB )
    {
      Pression_1 = -800;
      Cycle_1 = 30;
      Pression_2 = 2500;
      Cycle_2 = 30;
      Pression_3 = -800;
      Cycle_3 = 30;
      Pression_4 = 3000;
      Cycle_4 = 60;
      CycleTotal = Cycle_1 + Cycle_2 + Cycle_3 + Cycle_4;
      EventChoix = true;
      AvancementAzero(); // Met à 0 les barres d'avancement
    }
    else if ( message == ReparationManu )
    {
      AvancementAzero(); // Met à 0 les barres d'avancement
      EtatPression = 35 - 1; // Affichage
      nextionSerial.print("z0.val=");
      nextionSerial.print(EtatPression);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);

      while ( EventChoix != true )
      {
        String message = nex.listen(); //check message nextion
        while ( message == Depart )
        {

          EtatEncodeur = digitalRead(EncodeurPression);
   
          if ((EncodeurPressionLast == LOW) && (EtatEncodeur == HIGH)) 
          {
            if (digitalRead(EncodeurDepression) == LOW) 
            {
              if ( EtatPression >= 0 )
              {
                EtatPression--;
              }
              else { }
              //EtatDeLaPression ( EtatPression, CheckPression ( CapteurPression1, atm ) );
              EtatDeLaPression ( EtatPression, CheckPressionSimulation ( CapteurPression1 ) );
            }
            else
            {
              if ( EtatPression <= 120 )
              {
                EtatPression++;
              }
              else { }
              //EtatDeLaPression ( EtatPression, CheckPression ( CapteurPression1, atm ) );
              EtatDeLaPression ( EtatPression, CheckPressionSimulation ( CapteurPression1 ) );
          
            }
            nextionSerial.print("z0.val=");
            nextionSerial.print(EtatPression);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);      
          }
          EncodeurPressionLast = EtatEncodeur;
          /*
          EtatDeLaPression ( EtatPression, CheckPressionSimulation ( CapteurPression1 ) );
          nextionSerial.print("z1.val=");
          nextionSerial.print(CheckPressionSimulation ( CapteurPression1 ));
          nextionSerial.write(0xff);
          nextionSerial.write(0xff);
          nextionSerial.write(0xff); 
          */
        }
        digitalWrite(Pompe, LOW);
        digitalWrite(Electrovanne1, LOW);
        digitalWrite(Electrovanne2, LOW);
        
      }
    }
    else { }
  }


  String message = nex.listen(); //check message nextion
  if ( message == Depart )
  {
    delay(1000);
    
    // ------------------- Partie 1/4 ------------------- 
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Fermé Led verte Pression
    digitalWrite(Electrovanne2, HIGH); // Electrovanne 2 Ouverte Led bleu Depression
    MiseEnPression ( Pression_1, Cycle_1, 7, 0, 0, Cycle_1); // -0.8

    // ------------------- Partie 2/4 ------------------- 
    digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Ouverte Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Fermé Led bleu Depression
    MiseEnPression ( Pression_2, Cycle_2, 105, -800, Cycle_1, Cycle_1 + Cycle_2); // 2.5

    // ------------------- Partie 3/4 ------------------- 
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Fermé Led verte Pression
    digitalWrite(Electrovanne2, HIGH); // Electrovanne 2 Ouverte Led bleu Depression
    MiseEnPression ( Pression_3, Cycle_3, 7, 2500, Cycle_1 + Cycle_2, Cycle_1 + Cycle_2 + Cycle_3 ); // -0.8
    
    // ------------------- Partie 4/4 ------------------- 
    digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Ouverte Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Fermé Led bleu Depression
    MiseEnPression ( Pression_4, Cycle_4, 120, -800, Cycle_1 + Cycle_2 + Cycle_3, CycleTotal ); // 3

    EventReparation = true;
  }
  else { }

}


int MiseEnPression ( int Pression, int Cycle, int ForcerPression, int LecturePression, int Etape, int EtapeSuivante )
{ 
    // Tant que la pression n'est pas atteinte
    if ( LecturePression1 >= 0 )
    {
      while ( LecturePression1 > Pression )
      {
        LecturePression1 = analogRead(CapteurPression1);
        delay(10);
        digitalWrite(Pompe, HIGH);
        delay(1000); // Simule le temps de mise en pression
        LecturePression1 = Pression; // Force la pression pour tester
      }
    }
    else
    {
      while ( LecturePression1 < Pression )
      {
        LecturePression1 = analogRead(CapteurPression1);
        delay(10);
        digitalWrite(Pompe, HIGH);
        delay(1000); // Simule le temps de mise en pression
        LecturePression1 = Pression; // Force la pression pour tester
      }
    }
    digitalWrite(Pompe, LOW);


    nextionSerial.print("z0.val=");
    nextionSerial.print(ForcerPression);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    
    // Effet visuel de la progression sur XX secondes
    unsigned long currentMillis = millis();
    unsigned long previousMillis = millis();
    while ( (currentMillis - previousMillis) < (Cycle * 1000) )
    {

        // Ajouter condition pour envoyer la pression en cas de manque
        
        currentMillis = millis();
        AvancementEtape = map(currentMillis - previousMillis, 0, Cycle * 1000, 0, 100);
        nextionSerial.print("j0.val=");
        nextionSerial.print(AvancementEtape);
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);

        int EtapeTemporaire = map(Etape, 0, CycleTotal, 0, 100);
        int EtapeSuivanteTemporaire = map(EtapeSuivante, 0, CycleTotal, 0, 100);
        AvancementTotal = map(currentMillis - previousMillis, 0, Cycle * 1000, EtapeTemporaire, EtapeSuivanteTemporaire );
        nextionSerial.print("j1.val=");
        nextionSerial.print(AvancementTotal);
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);

    }


    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Led bleu Depression
    digitalWrite(Pompe, LOW);
}

void AvancementAzero ( void )
{
        delay(100);
        nextionSerial.print("j0.val=0");
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);

        nextionSerial.print("j1.val=0");
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
}

void EtatDeLaPression ( int EtatPression, int CheckPression )
{
  if ( EtatPression == 35 )
  {
    digitalWrite(Pompe, LOW);
    digitalWrite(Electrovanne1, LOW);
    digitalWrite(Electrovanne2, LOW);
  }
  else if ( EtatPression > 35 )
  {
    if ( map(EtatPression, 0, 120, -1000, 3000) > CheckPression )
    {
      digitalWrite(Pompe, LOW);
    }
    else
    {
      digitalWrite(Pompe, HIGH);
    }
    digitalWrite(Pompe, HIGH);
    digitalWrite(Electrovanne1, HIGH);
    digitalWrite(Electrovanne2, LOW);
  }
  else if ( EtatPression < 35 )
  {
    if ( map(EtatPression, 0, 120, -1000, 3000) < CheckPression *(-1) )
    {
      digitalWrite(Pompe, LOW);
    }
    else
    {
      digitalWrite(Pompe, HIGH);
    }
    digitalWrite(Pompe, HIGH);
    digitalWrite(Electrovanne1, LOW);
    digitalWrite(Electrovanne2, HIGH);
  }
  else { }
}

int CheckPression ( const int CapteurPression1, int atm )
{
  int pressureSensorRaw = analogRead(CapteurPression1); // Lis la valeur analoogique sur le pin A0
  int pressureSensorVoltage = pressureSensorRaw * (5.0 / 1023.0);  // Convertis les volts en valeur de pression
  int millibar = ( (pressureSensorVoltage + 0.2) * 700.0/4.5 ) * 10;  // Convertis en millibar
  millibar = millibar - atm * 10;
  return millibar;
}

int CheckPressionSimulation ( const int CapteurPression1 )
{
  int pressureSensorRaw = analogRead(CapteurPression1); // Lis la valeur analoogique sur le pin A0
  int millibar = map(pressureSensorRaw, 23, 503, 0, 120); // 0 = -1000 , 120 = 3000
  return millibar;
}
