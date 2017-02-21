/*
Programme pour réparer des parebrises
Matos :
2 Electrovannes
1 capteur de pression
1 pompe à air comprimé
*/
#include <Nextion.h>
#include <NextionPage.h>
#include <NextionPicture.h>
#include <SoftwareSerial.h>
SoftwareSerial nextionSerial(10, 11); // RX, TX

Nextion nex(nextionSerial);
NextionPage pgPicture(nex, 1, 0, "page1");
NextionPicture picture(nex, 1, 1, "b0");

bool event = false;

const int Electrovanne1 = 2; // Led verte Pression
const int Electrovanne2 = 3; // Led bleu Depression

const int Pompe = 4; // LOW Pompe coupé, HIGH en marche // Led rouge

const int CapteurPression1 = A1;
float LecturePression1 = 0;

int avancement = 0;

/*
int EncodeurRotatif1 = 0; //0 rotation sens aiguille montre, 1 l'inverse
int EncodeurRotatif2 = 0; //0 rotation sens aiguille montre, 1 l'inverse
*/

int ChoixReparation = 0 ;

void setup()
{
  Serial.begin(9600);
  pinMode(Electrovanne1, OUTPUT);
  pinMode(Electrovanne2, OUTPUT);

  nextionSerial.begin(9600);
  nex.init();

  Serial.println(pgPicture.show());

  Serial.println(picture.attachCallback(&callback));
    
  // Electrovanne 1
  pinMode(Electrovanne1, OUTPUT);
  digitalWrite(Electrovanne1, LOW);
  // Electrovanne 2
  pinMode(Electrovanne1, OUTPUT);
  digitalWrite(Electrovanne1, LOW);
  // Pompe
  pinMode(Pompe, OUTPUT);
  digitalWrite(Pompe, LOW);

}

void loop()
{
  //Réparation rapide
  nex.poll();

}

void callback(NextionEventType type, INextionTouchable *widget)
{
  if (type == NEX_EVENT_PUSH && event == false )
  {
    delay(1000);
    // Partie 1/4
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Fermé Led verte Pression
    digitalWrite(Electrovanne2, HIGH); // Electrovanne 2 Ouverte Led bleu Depression
    // Tant que la pression n'est pas atteinte
    while ( LecturePression1 > -0.8 )
    {
      LecturePression1 = analogRead(CapteurPression1);
      digitalWrite(Pompe, HIGH);
      LecturePression1 = -0.8;
    }

    nextionSerial.print("z0.val=7"); // Mise à jour sur le Nextion 7 => -0.8 
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    
    //delay(30000);
    // Effet visuel de la progression sur 30 secondes
    for (int i=0; i <= 24; i++)
    {
      nextionSerial.print("z1.val=");
      nextionSerial.print(i*5);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);

      nextionSerial.print("z2.val=");
      nextionSerial.print(i);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      delay(125);
    }

    // Partie 2/4
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Led bleu Depression
    digitalWrite(Pompe, LOW);
    delay(500);
    //
    digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Ouverte Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Fermé Led bleu Depression
    // Tant que la pression n'est pas atteinte
    while ( LecturePression1 < 2.5 )
    {
      LecturePression1 = analogRead(CapteurPression1);
      digitalWrite(Pompe, HIGH);
      LecturePression1 = 2.5; // Valeur forcée pour tester
    }
    
    nextionSerial.print("z0.val=105"); // Mise à jour sur le Nextion 105 => 2.5
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);

    //delay(30000);
    // Effet visuel de la progression sur 30 secondes
    for (int i=24; i <= 48; i++)
    {
      nextionSerial.print("z1.val=");
      nextionSerial.print((i-24)*5);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);

      nextionSerial.print("z2.val=");
      nextionSerial.print(i);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      delay(125);
    }
    // Partie 3/4
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Led bleu Depression
    digitalWrite(Pompe, LOW);
    delay(500);
    //
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Fermé Led verte Pression
    digitalWrite(Electrovanne2, HIGH); // Electrovanne 2 Ouverte Led bleu Depression
    // Tant que la pression n'est pas atteinte
    while ( LecturePression1 > -0.8 )
    {
      LecturePression1 = analogRead(CapteurPression1);
      digitalWrite(Pompe, HIGH);
      LecturePression1 = -0.8; // Valeur forcée pour tester
    }
    
    nextionSerial.print("z0.val=7"); // Mise à jour sur le Nextion 7 => -0.8
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);

    //delay(30000);
    // Effet visuel de la progression sur 30 secondes
    for (int i=48; i <= 72; i++)
    {
      nextionSerial.print("z1.val=");
      nextionSerial.print((i-48)*5);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);

      nextionSerial.print("z2.val=");
      nextionSerial.print(i);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      delay(125);
    }
    // Partie 4/4
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Led bleu Depression
    digitalWrite(Pompe, LOW);
    delay(500);
    //
    digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Ouverte Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Fermé Led bleu Depression
    // Tant que la pression n'est pas atteinte
    while ( LecturePression1 < 3 )
    {
      LecturePression1 = analogRead(CapteurPression1);
      digitalWrite(Pompe, HIGH);
      LecturePression1 = 3; // Valeur forcée pour tester
    }
    
    nextionSerial.print("z0.val=120"); // Mise à jour sur le Nextion 7 => 3
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);
    nextionSerial.write(0xff);

    //delay(60000);
    // Effet visuel de la progression sur 60 secondes
    for (int i=72; i <= (120-24); i++)
    {
      nextionSerial.print("z1.val=");
      nextionSerial.print((i-72)*5);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);

      nextionSerial.print("z2.val=");
      nextionSerial.print(72+((i-72)*2));
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      nextionSerial.write(0xff);
      delay(250);
    }
    digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Led verte Pression
    digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Led bleu Depression
    digitalWrite(Pompe, LOW);

    /*
    // Fini phase test avec LED à commenter :)
    for (int i=0; i <= 10; i++)
    {
      digitalWrite(Electrovanne1, HIGH); // Electrovanne 1 Led verte Pression
      digitalWrite(Electrovanne2, HIGH); // Electrovanne 2 Led bleu Depression
      digitalWrite(Pompe, HIGH);
      delay(200);
      digitalWrite(Electrovanne1, LOW); // Electrovanne 1 Led verte Pression
      digitalWrite(Electrovanne2, LOW); // Electrovanne 2 Led bleu Depression
      digitalWrite(Pompe, LOW);
      delay(200);

    }
    */
    event = true;
  }
  else  if (type == NEX_EVENT_PUSH && event == true )
  {
    digitalWrite(Pompe, LOW);
    event = false;
  }

}



