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
int Pression_1 = 0;
int Cycle_1 = 0;
int Pression_2 = 0;
int Cycle_2 = 0;
int Pression_3 = 0;
int Cycle_3 = 0;
int Pression_4 = 0;
int Cycle_4 = 0;
int CycleTotal = 0;

bool EventChoix = false;
bool EventReparation = false;

const int EncodeurPression = 5;    //  sortie Encodeur Pression
const int EncodeurDepression = 6;  //  sortie Encodeur Depression
const int PressionDeBase = 7;  //  sortie Encodeur Depression

static long EtatPression = 35;  
int EncodeurPressionLast = LOW;
int EtatEncodeur = LOW;

const int Electrovanne1 = 2; // Led verte Pression
const int Electrovanne2 = 3; // Led bleu Depression
const int Pompe = 4; // LOW Pompe coupé, HIGH en marche // Led rouge
int CapteurPression1 = A0;
int LecturePression1 = 0;
float atm = 1.56;
int millibar = 0;


int AvancementEtape = 0;
int AvancementTotal = 0;

long Temps = 0;
int i = 0;
bool Phase = 1; // Phase test avec servo => 0, Phase prod avec capteur pression => 1

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
        while ( message == Depart )
        {
          if ( nex.listen() == Arret ) { message = Arret; }
          EtatEncodeur = digitalRead(EncodeurPression);
          // Condition pour jouer avec l'encodeur
          if ((EncodeurPressionLast == LOW) && (EtatEncodeur == HIGH)) 
          {
            if ( nex.listen() == Arret ) { message = Arret; }
            if (digitalRead(EncodeurDepression) == LOW) 
            {
              if ( EtatPression >= 0 )
              {
                EtatPression--;
                digitalWrite(Pompe, HIGH);
                digitalWrite(Electrovanne1, LOW);
                digitalWrite(Electrovanne2, HIGH);
              }
              else { }
            }
            else
            {
              if ( EtatPression <= 120 )
              {
                EtatPression++;
                digitalWrite(Pompe, HIGH);
                digitalWrite(Electrovanne1, HIGH);
                digitalWrite(Electrovanne2, LOW);
              }
              else { }
            }
            // Mise à jour de l'aiguille de pression de l'encodeur
            delay(500);
            digitalWrite(Pompe, LOW);
            nextionSerial.print("z0.val=");
            nextionSerial.print(EtatPression);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            if ( nex.listen() == Arret ) { message = Arret; }
          }
          else 
          {
            if ( nex.listen() == Arret ) { message = Arret; }
            // Cette partie sert à afficher les millibar du capteur, mais bizarrement ça fait planter l'encodeur, à creuser
            /*
            nextionSerial.print("z1.val=");
            nextionSerial.print(CheckPression ( CapteurPression1, atm, Phase ));
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            //*/
          }
          EncodeurPressionLast = EtatEncodeur;
          if ( nex.listen() == Arret ) { message = Arret; }
        }
        digitalWrite(Pompe, LOW);
        //digitalWrite(Electrovanne1, LOW);
        //digitalWrite(Electrovanne2, LOW);
      }
}

