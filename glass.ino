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
float atm = 1.56;


int AvancementEtape = 0;
int AvancementTotal = 0;

long Temps = 0;
int i = 0;
bool Phase = 0; // Phase test avec servo => 0, Phase prod avec capteur pression => 1

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
      
      // Boucle du programme de réparation manuel
      while ( EventChoix != true )
      {
        message = nex.listen(); //check message nextion
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
              }
              else { }
            }
            else
            {
              if ( EtatPression <= 120 )
              {
                EtatPression++;
              }
              else { }
            }
            // Mise à jour de l'aiguille de pression de l'encodeur
            nextionSerial.print("z0.val=");
            nextionSerial.print(EtatPression);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            if ( nex.listen() == Arret ) { message = Arret; }
          }
          else 
          {
            EtatDeLaPression ( map( EtatPression, 0, 120, -1000, 3000 ), CheckPression ( CapteurPression1, atm, Phase ) );
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
    MiseEnPression ( Pression_1, Cycle_1, 7, CapteurPression1, 0, Cycle_1, atm, Phase ); // -0.8

    // ------------------- Partie 2/4 ------------------- 
    digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Ouverte Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Fermé Led bleu Depression
    MiseEnPression ( Pression_2, Cycle_2, 105, CapteurPression1, Cycle_1, Cycle_1 + Cycle_2, atm, Phase); // 2.5

    // ------------------- Partie 3/4 ------------------- 
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Fermé Led verte Pression
    digitalWrite(Electrovanne2, HIGH); // Electrovanne 2 Ouverte Led bleu Depression
    MiseEnPression ( Pression_3, Cycle_3, 7, CapteurPression1, Cycle_1 + Cycle_2, Cycle_1 + Cycle_2 + Cycle_3, atm, Phase ); // -0.8
    
    // ------------------- Partie 4/4 ------------------- 
    digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Ouverte Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Fermé Led bleu Depression
    MiseEnPression ( Pression_4, Cycle_4, 120, CapteurPression1, Cycle_1 + Cycle_2 + Cycle_3, CycleTotal, atm, Phase ); // 3

    EventChoix = false;
  }
  else { }
}


void MiseEnPression ( int Pression, int Cycle, int ForcerPression, int CapteurPression1, int Etape, int EtapeSuivante, float atm, bool Phase )
{ 
    nextionSerial.print("z0.val=");
    nextionSerial.print(map(CheckPression ( CapteurPression1, atm, Phase ), -1000, 3000, 0, 120));
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    
    // Tant que la pression n'est pas atteinte
    bool VerifPression = 0;
    while ( VerifPression != 1 )
    {
      VerifPression = EtatDeLaPression ( Pression, CheckPression ( CapteurPression1, atm, Phase ) );
      nextionSerial.print("z0.val=");
      nextionSerial.print(map( CheckPression ( CapteurPression1, atm, Phase ), -1000, 3000, 0, 120 ) );
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
    }

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
        EtatDeLaPression ( Pression, CheckPression ( CapteurPression1, atm, Phase ) );
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

bool EtatDeLaPression ( int EtatPression, int CheckPression )
{
    if ( CheckPression <= EtatPression - 50 )
    {
      digitalWrite(Pompe, HIGH);
      digitalWrite(Electrovanne1, HIGH);
      digitalWrite(Electrovanne2, LOW);
      return 0;
    }
    else if ( CheckPression >= EtatPression + 50  )
    {
      digitalWrite(Pompe, HIGH);
      digitalWrite(Electrovanne1, LOW);
      digitalWrite(Electrovanne2, HIGH);
      return 0;
    }
    else if ( ( CheckPression > 0 ) && ( EtatPression - 50 < CheckPression < EtatPression + 50 )  )
    {
      digitalWrite(Pompe, LOW);
      digitalWrite(Electrovanne1, LOW);
      digitalWrite(Electrovanne2, LOW);
      return 1;
    }
    else if ( ( CheckPression < 0 ) && ( EtatPression + 50 > CheckPression > EtatPression - 50 )  )
    {
      digitalWrite(Pompe, LOW);
      digitalWrite(Electrovanne1, LOW);
      digitalWrite(Electrovanne2, LOW);
      return 1;
    }
}

// Fonction récupérant la valeur du capteur de pression
int CheckPression ( const int CapteurPression1, float atm, bool Phase )
{
  if ( Phase == 0 )
  {
    int pressureSensorRaw = analogRead(CapteurPression1); // Lis la valeur analoogique sur le pin A0
    int millibar = map(pressureSensorRaw, 17, 645, -1000, 3000); // 0 = -1000 , 120 = 3000
    return millibar;
    //return pressureSensorRaw;
  }
  else
  {
    int pressureSensorRaw = analogRead(CapteurPression1); //Reads the sensor raw value on analog port 0
    float pressureSensorVoltage = pressureSensorRaw * (5.0 / 1023.0);  // convert the raw reading to voltage
    float bar = (pressureSensorVoltage + 0.2) * 7/4.5;
    bar = bar - atm;
    int millibar = (int) ( bar * 1000 );
    return millibar;
  }
}
