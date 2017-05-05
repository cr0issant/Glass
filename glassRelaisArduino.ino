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
/*
String ReparationRapide = "65 1 1 1 ffff ffff ffff"; // Page 1 - Réparation rapide "b0"
String ReparationA = "65 1 2 1 ffff ffff ffff"; // Page 1 - Réparation A "b1"
String ReparationB = "65 1 3 1 ffff ffff ffff"; // Page 1 - Réparation B "b2"
String ReparationManu = "65 1 4 1 ffff ffff ffff"; // Page 1 - Réparation C "b3"
String Depart = "65 2 6 1 ffff ffff ffff"; // Page 2 - Départ réparation "b1"
String Arret = "65 2 7 1 ffff ffff ffff"; // Page 2  - Arrêt réparation "b2"
*/

String ReparationRapide = "65 1 2 0 ffff ffff ffff";      
String ReparationA      = "65 1 3 0 ffff ffff ffff";     
String ReparationB      = "65 1 4 0 ffff ffff ffff";      
String ReparationManu   = "65 1 5 0 ffff ffff ffff";    

String ultraViolet      = "65 1 6 0 ffff ffff ffff";   
String info             = "65 1 7 0 ffff ffff ffff";    

String departAuto       = "65 2 2 0 ffff ffff ffff";     
//String arretAuto        = "65 2 3 0 ffff ffff ffff";    
String depressionAuto   = "65 2 9 0 ffff ffff ffff";   

String departManu       = "65 5 2 0 ffff ffff ffff";   
String arretManu        = "65 5 3 0 ffff ffff ffff";    
String depressionManu   = "65 2 8 0 ffff ffff ffff";    

String departUV         = "65 3 3 0 ffff ffff ffff";    
String arretUV          = "65 3 4 0 ffff ffff ffff";    

String flecheHaute      = "65 5 b 0 ffff ffff ffff";
String flecheBasse      = "65 5 c 0 ffff ffff ffff";

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

bool EventChoix = false; // Booléen pour contrôle si le choix de réparation est fait

const int EncodeurPression = 5; //  sortie Encodeur Pression
const int EncodeurDepression = 6; //  sortie Encodeur Depression
const int PressionDeBase = 7; //  sortie Encodeur Depression

static long EtatPression = 0; 
int EncodeurPressionLast = LOW;
int EtatEncodeur = LOW;

const int Electrovanne1 = 3; // Led verte Pression
const int Electrovanne2 = 2; // Led bleu Depression
const int Pompe = 4; // LOW Pompe coupé, HIGH en marche // Led rouge
int CapteurPression1 = A0; // Capteur Pression sur A0

float atm = 1.56;
int millibar = 0;
const int ArretUrgence = 12;
int ArretUrgenceState = 0;

unsigned long Temps = 0;
int i = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(Electrovanne1, OUTPUT);
  pinMode(Electrovanne2, OUTPUT);
  pinMode(ArretUrgence, INPUT);
  
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
  digitalWrite(Pompe, HIGH);
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
      Pression_1 = -400;
      Cycle_1 = 50;
      Pression_2 = 2500;
      Cycle_2 = 25;
      Pression_3 = -400;
      Cycle_3 = 50;
      Pression_4 = 3000;
      Cycle_4 = 50;
      CycleTotal = Cycle_1 + Cycle_2 + Cycle_3 + Cycle_4;
      EventChoix = true;
      AvancementAzero(); // Met à 0 les barres d'avancement
    }
    else if ( message == ReparationA )
    {
      Pression_1 = -400;
      Cycle_1 = 3;
      Pression_2 = 2500;
      Cycle_2 = 3;
      Pression_3 = -400;
      Cycle_3 = 3;
      Pression_4 = 3000;
      Cycle_4 = 6;
      CycleTotal = Cycle_1 + Cycle_2 + Cycle_3 + Cycle_4;
      EventChoix = true;
      AvancementAzero(); // Met à 0 les barres d'avancement
    }
    else if ( message == ReparationB )
    {
      Pression_1 = -400;
      Cycle_1 = 60;
      Pression_2 = 2500;
      Cycle_2 = 30;
      Pression_3 = -400;
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
      EtatPression = 54; // Affichage
      nextionSerial.print("z0.val=");
      nextionSerial.print(EtatPression);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      delay(1);
        

      // Boucle du programme de réparation manuel
      // Dans cette réparation c est l encodeur qui gère la pression
      // En tournant l encodeur la jauge et la valeur des millibars changent
      // La pression s ajuste pour suivre la jauge
      int timer = 0;
      int secondes = 0;
      int minutes = 0;
      bool goManu = false;
      while ( EventChoix != true )
      {
        message = nex.listen(); // Attente de validation du départ de la réparation manu
        if ( message == departManu )
        {
          goManu = true;
        }
        // C est parti on répare
        while ( goManu == true )
        {
            // Timer pour mode manu
            timer+=1;
            delay(1);
            // Mise à jour secondes
            if ( timer > 1000 ) 
            { 
              timer = 0; 
              secondes+=1;
              nextionSerial.print("n2.val=");
              nextionSerial.print( secondes );
              nextionSerial.write(0xff);
              nextionSerial.write(0xff);
              nextionSerial.write(0xff);
              delay(1);
            } else {}
            // Mise à jour minutes
            if ( secondes > 59 ) 
            { 
              secondes = 0; 
              minutes+=1;
              nextionSerial.print("n1.val=");
              nextionSerial.print( minutes );
              nextionSerial.write(0xff);
              nextionSerial.write(0xff);
              nextionSerial.write(0xff);
              delay(1);
            } else {}
            
          // L arrêt d urgence au cas ( boutton poussoir )
          if ( message == arretManu ) 
          {
             EventChoix = true; goManu = false;
          } 
          else 
          {
             // On continue
          }

          message = nex.listen();
          // Condition pour jouer avec les fèches
            if ( ( message == flecheBasse ) && ( EtatPression >= 0 ) )
            {
                EtatPression -= 5;
            }
            else if ( ( message == flecheHaute ) && ( EtatPression <= 216 ) )
            {
                EtatPression += 5;
            }
            Temps +=1;
            if ( Temps > 50 ) 
            { 
              Temps = 0;
              // Mise à jour de la jauge de pression demandée par l'encodeur
              nextionSerial.print("z0.val=");
              nextionSerial.print(EtatPression);
              nextionSerial.write(0xff);
              nextionSerial.write(0xff);
              nextionSerial.write(0xff);
              delay(1);
              // Mise à jour de la valeur de pression demandée par l'encodeur
              nextionSerial.print("n0.val=");
              nextionSerial.print( RecuperationValeurCapteurPression ( CapteurPression1, atm ) );
              nextionSerial.write(0xff);
              nextionSerial.write(0xff);
              nextionSerial.write(0xff);
              delay(1);
            }
            else { }
        EquilibragePression ( map( EtatPression, 0, 216, -1000, 3000 ), RecuperationValeurCapteurPression ( CapteurPression1, atm ) );
        delay(1);
        }
        // En quittant le programme on éteint tout
        digitalWrite(Pompe, HIGH);
        digitalWrite(Electrovanne1, LOW);
        digitalWrite(Electrovanne2, LOW);
      }
      // On annule de choix de réparation validé pour de nouveau en choisir un
      EventChoix = false;
    }
    else { }
  }

  String message = nex.listen(); // Attente de validation du départ de la réparation auto
  // Tant que c'est validé on reste sur cette réparation auto
  if ( message == departAuto  )
  {
    delay(1000);
    
    // ------------------- Partie 1/4 ------------------- 
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Fermé Led verte Pression
    digitalWrite(Electrovanne2, HIGH); // Electrovanne 2 Ouverte Led bleu Depression
    MiseEnPression ( Pression_1, Cycle_1, CapteurPression1, 0, Cycle_1, atm ); // -800

    // ------------------- Partie 2/4 ------------------- 
    digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Ouverte Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Fermé Led bleu Depression
    MiseEnPression ( Pression_2, Cycle_2, CapteurPression1, Cycle_1, Cycle_1 + Cycle_2, atm ); // 2500

    // ------------------- Partie 3/4 ------------------- 
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Fermé Led verte Pression
    digitalWrite(Electrovanne2, HIGH); // Electrovanne 2 Ouverte Led bleu Depression
    MiseEnPression ( Pression_3, Cycle_3, CapteurPression1, Cycle_1 + Cycle_2, Cycle_1 + Cycle_2 + Cycle_3, atm );  // -800

    // ------------------- Partie 4/4 ------------------- 
    digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Ouverte Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Fermé Led bleu Depression
    MiseEnPression ( Pression_4, Cycle_4, CapteurPression1, Cycle_1 + Cycle_2 + Cycle_3, CycleTotal, atm ); // 3000

    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Led bleu Depression
    digitalWrite(Pompe, HIGH);
    nextionSerial.print("j0.val=");
    nextionSerial.print(100);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    delay(1);
    nextionSerial.print("j1.val=");
    nextionSerial.print(100);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    EventChoix = false;
  }
  else { }
}

/* 
Cela permet de lancer la procedure pour effectuer une mise en pression
Etapes :
- On récupère la valeur du capteur de pression
- On attend que la pression soit atteint avant de lancer la temporisation
- On a atteint la pression voulue, on commence la temporisation, et on ajuste en cours de route au cas où
Elle retourne aussi grace à un booléen l'état de la mise en pression
*/
bool MiseEnPression ( int Pression, long Cycle, int CapteurPression1, int Etape, int EtapeSuivante, float atm )
{ 
    int ArretUrgence = 12;
    bool Urgence = false;
    // On récupère la valeur du capteur de pression
    nextionSerial.print("z0.val=");
    nextionSerial.print(map(RecuperationValeurCapteurPression ( CapteurPression1, atm ), -1000, 3000, 0, 216));
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    delay(1);
    
    // On attend que la pression soit atteint avant de lancer la temporisation
    bool VerifPression = 0;
    while ( ( VerifPression != 1 ) && ( Urgence != true ) )
    {
      VerifPression = EquilibragePression ( Pression, RecuperationValeurCapteurPression ( CapteurPression1, atm ) );
      nextionSerial.print("z0.val=");
      nextionSerial.print(map( RecuperationValeurCapteurPression ( CapteurPression1, atm ), -1000, 3000, 0, 216 ) );
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      delay(1);
      nextionSerial.print("n0.val=");
      nextionSerial.print( RecuperationValeurCapteurPression ( CapteurPression1, atm ) );
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      delay(1);
      if ( DemandeArretUrgence ( ArretUrgence ) == true) 
      {
        // On continue
      } 
      else 
      {
        Urgence = true;
      }
    }

    // On a atteint la pression voulue, on commence la temporisation, et on ajuste en cours de route au cas où
    unsigned long previousMillis = millis();
    unsigned long currentMillis = millis();
    int EtapeTemporaire = 0;
    int EtapeSuivanteTemporaire = 0;
    Cycle = Cycle * 1000;
    int Temps = 0;
    while ( ( (currentMillis - previousMillis) < Cycle ) && ( Urgence != true )  )
    {
        currentMillis = millis();
        // Visuel de l'étape en cours
    
        Temps +=1;
        if ( Temps > 50 ) 
        { 
            Temps = 0;
            nextionSerial.print("j0.val=");
            nextionSerial.print(map(currentMillis - previousMillis, 0, Cycle, 0, 100));
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            delay(1);
            // Visuel de l'étape total
            EtapeTemporaire = map(Etape, 0, CycleTotal, 0, 100);
            EtapeSuivanteTemporaire = map(EtapeSuivante, 0, CycleTotal, 0, 100);
            nextionSerial.print("j1.val=");
            nextionSerial.print(map(currentMillis - previousMillis, 0, Cycle, EtapeTemporaire, EtapeSuivanteTemporaire ));
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            delay(1);
            nextionSerial.print("z0.val=");
            nextionSerial.print(map( RecuperationValeurCapteurPression ( CapteurPression1, atm ), -1000, 3000, 0, 216 ) );
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            delay(1);
            nextionSerial.print("n0.val=");
            nextionSerial.print( RecuperationValeurCapteurPression ( CapteurPression1, atm ) );
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            nextionSerial.write(0xff);
            delay(1);
        }
        else {}
        // Equilibrage de secours en cas de manque
        EquilibragePression ( Pression, RecuperationValeurCapteurPression ( CapteurPression1, atm ) ); 
        // Arrêt d'urgence si demandé
        if ( DemandeArretUrgence ( ArretUrgence ) == true) 
        {
          // On continue
        } 
        else 
        {
          Urgence = true;
        }
    }
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Led bleu Depression
    digitalWrite(Pompe, HIGH);
}

// Gestion de l'arrêt d'urgence avec un bouton poussoir NO extérieur au nextion 
bool DemandeArretUrgence ( int ArretUrgence )
{
  int ArretUrgenceState = digitalRead(ArretUrgence);
  if (ArretUrgenceState != HIGH) // Bouton NO
  {
    // On continue
    return true;
  } 
  else 
  {
    // On arrete tout
    return false;
  }
}

// Mise à zéro des visuels d'avancements de la réparation
void AvancementAzero ( void )
{
        delay(100);
        nextionSerial.print("j0.val=0");
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
        delay(1);

        nextionSerial.print("j1.val=0");
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
        nextionSerial.write(0xff);
        delay(1);
}

// Cela pilote les pompes et électrovannes pour chercher à atteindre la pression demandé
// Elle retourne aussi l'état de l'équilibre
// 50 correspond à la tolérance sur le capteur de pression pour éviter aux pompe et électrovannes 
// de rechercher constamment l'équilibre
bool EquilibragePression ( int PressionAtrouver, int ValeurCapteurPression )
{
    PressionAtrouver = map( PressionAtrouver, -1000, 3000, 0, 4000 );
    ValeurCapteurPression = map( ValeurCapteurPression, -1000, 3000, 0, 4000 );
    // On va à fond vers notre objectif de pression
    if ( ValeurCapteurPression <= PressionAtrouver - 50 )
    {
      digitalWrite(Pompe, LOW);
      digitalWrite(Electrovanne1, HIGH);
      digitalWrite(Electrovanne2, LOW);
      return 0;
    }
    else if ( ValeurCapteurPression >= PressionAtrouver + 50  )
    {
      digitalWrite(Pompe, LOW);
      digitalWrite(Electrovanne1, LOW);
      digitalWrite(Electrovanne2, HIGH);
      return 0;
    }
    // On stoppe si on a atteint notre pression recherché avec +ou- 50 de tolérance
    else if ( ( PressionAtrouver - 50 < ValeurCapteurPression < PressionAtrouver + 50 )  )
    {
      digitalWrite(Pompe, HIGH);
      return 1;
    }
    delay(1);
}

// Cela récupére la valeur du capteur de pression
int RecuperationValeurCapteurPression ( const int CapteurPression1, float atm )
{
    int pressureSensorRaw = analogRead(CapteurPression1); //Reads the sensor raw value on analog port 0
    float pressureSensorVoltage = pressureSensorRaw * (5.0 / 1023.0);  // convert the raw reading to voltage
    float bar = (pressureSensorVoltage + 0.2) * 7/4.5;
    bar = bar - atm;
    int millibar = (int) ( bar * 1000 );
    return millibar;
    delay(1);
}
