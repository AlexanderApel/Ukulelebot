#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <MIDI.h>

//default address 0x40
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver();

#define SERVOMIN  100 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  500 // this is the 'maximum' pulse length count (out of 4096)

#define GreifDauer 10 //Dauer die es braucht bis die servos gegriffen haben
#define buttonPin 5 //Knopf Input pin

#define benutze_Knopf 0 //0: Knopf wird ignoriert 1: Knopf wird verwendet


int grad(int winkel)
{
  return map(winkel, 0, 180, SERVOMIN, SERVOMAX);
}




//Grad wandelt zahl in grad um
int Zupfen_ANFANGS_Position [] =
{
  grad(100),//0
  grad(95),//1
  grad(95),//2
  grad(95)//3
};
int Zupfen_END_Position [] =
{
  grad(125),//0
  grad(110),//1
  grad(110),//2
  grad(115)//3
};

int Greif_ANFANGS_Position [] = {
  grad(90), //4
  grad(90), //5
  grad(90), //6
  grad(90), //7
  grad(90), //8
  grad(90), //9
  grad(90), //10
  grad(90), //11
  grad(90), //12
  grad(90), //13
  grad(90), //14
  grad(90) //15

};

int Greif_END_position [] = {
  grad(92), //4
  grad(92), //5
  grad(92), //6
  grad(92), //7
  grad(92), //8
  grad(92), //9
  grad(92), //10
  grad(92), //11
  grad(92), //12
  grad(92), //13
  grad(92), //14
  grad(92) //15

};


//Definition der Servo Positionen!
//Servo 0,1,2,3 sind die Servos zum zupfen unten
//Servo 4-15 sind zum greifen
//Shield 1 Servos: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
//G C E A gestimmt sieh Bild hier https://d3j2w99mq7338e.cloudfront.net/86-JbNwTPraGg.png

MIDI_CREATE_DEFAULT_INSTANCE();//Default MIDI SETUP

uint8_t zupfPos[] = {0, 0, 0, 0};//0 für start Postion, 1 für bewegt

uint8_t buttonPressed = 0;//Knopf gedrückt

bool on = false;//Start/stop
int buttonTimer = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("PROGRAMM BEGINNT");

  pinMode(buttonPin, INPUT_PULLUP);//Lese Knopf Input

  pwm1.begin(); //Starte shield

  pwm1.setPWMFreq(50);  // Analog servos run at ~60 Hz updates

  MIDI.setHandleNoteOn(handleNoteOn); //Wenn eine Note gespielt werden soll wird diese Funktion aufgerufen
  MIDI.setHandleNoteOff(handleNoteOff);//Wenn die Note nicht mehr gespielt wird diese

  MIDI.begin();//Starte Midi

  delay(10);
  
  if (!benutze_Knopf) {
    //Setze alle Servos in Startposition
    for (int i = 0; i < 12; i++)
    {
      pwm1.setPWM(i, 0, Greif_ANFANGS_Position[i]);
    }
    for (int i = 0; i < 4; i++)
    {
      pwm1.setPWM(i, 0, Zupfen_ANFANGS_Position[i]);
    }
    delay(100);
  }
}

void loop() {
  //Bei Knopfdruck Setze on auf wahr/falsch
  buttonPressed = !digitalRead(buttonPin);
  if (buttonPressed == HIGH && benutze_Knopf)
  {
    if (on)
    {
      on = false;
      Serial.println(0);
      delay(5000);
    }
    else
    {
      on = true;
      Serial.println(1);
      delay(5000);
    }

    //Setze alle Servos in Startposition
    for (int i = 0; i < 12; i++)
    {
      pwm1.setPWM(i, 0, Greif_ANFANGS_Position[i]);
    }
    for (int i = 0; i < 4; i++)
    {
      pwm1.setPWM(i, 0, Zupfen_ANFANGS_Position[i]);
    }
    delay(100);

  }


  if (on || !benutze_Knopf) MIDI.read();//wenn der Knopf einmal gedrückt wurde beginnt das Programm

}





//Regelt das Ansteuern der Zupf Motoren
void zupfen(int servoNummer) {
  if (zupfPos[servoNummer] == 0)
  {
    pwm1.setPWM(servoNummer, 0, Zupfen_END_Position[servoNummer]);
    zupfPos[servoNummer] = 1;
  }
  else
  {
    pwm1.setPWM(servoNummer, 0, Zupfen_ANFANGS_Position[servoNummer]);
    zupfPos[servoNummer] = 0;
  }
}

//Einzelne Servo ansteuerung je nach Note
//Case 0:C 1:C# 2:D 3:D# 4:E 5:F 6:F# 7:G 8:G# 9:A 10:A# 11:B siehe https://www.researchgate.net/profile/Davide_Baccherini/publication/221276841/figure/tbl1/AS:669024811773964@1536519353526/A-representation-of-notes-in-the-MIDI-standard.png
//Notenplatzierung: https://d3j2w99mq7338e.cloudfront.net/86-JbNwTPraGg.png

void tonSpielen(int servoNummer, int zupfServoNummer, int zustand)
{
  if (servoNummer > 3) {
    if (zustand == 1) {
      pwm1.setPWM(servoNummer, 0, Greif_END_position[servoNummer - 4]); //Der Servo greift
      delay(GreifDauer);//Abwarten bis der Servo an die Zielposition gelangt ist

      zupfen(zupfServoNummer);//Die Seite Zupfen
    }
    else pwm1.setPWM(servoNummer, 0, Greif_ANFANGS_Position[servoNummer - 4]);
  }
  else zupfen(servoNummer);
}

void waehleTon(int note, int zustand)
{
  //Wenn Noten eine Oktave höher sind
  if (note >= 60)
  {
    switch (note % 12)
    {
      case 0: tonSpielen(13, 3, zustand); break;//C5
      case 1: tonSpielen(14, 3, zustand); break;//C#5
      case 2: tonSpielen(15, 3, zustand); break;//D#5
    }
  }
  else {
    switch (note % 12) {
      case 7://G
        tonSpielen(0, 0, zustand);
        break;

      case 0://C
        tonSpielen(1, 1, zustand);
        break;

      case 4://E
        tonSpielen(2, 2, zustand);
        break;

      case 8://G#Ab
        tonSpielen(4, 0, zustand);
        break;

      case 1://C#Db
        tonSpielen(5, 1, zustand);
        break;

      case 5://F
        tonSpielen(6, 2, zustand);
        break;

      case 10://A#Bb
        tonSpielen(7, 3, zustand);
        break;

      case 9://A
        //tonSpielen(Servonummer(4-15), zupfservonummer(0,1,2,3),zustand bedeutet soll die note gespielt werden oder aufhören zu spielen)
        tonSpielen(8, 0, zustand);
        break;

      case 2://D
        tonSpielen(9, 1, zustand);
        break;

      case 6://F#Gb
        tonSpielen(10, 2, zustand);
        break;

      case 11://B
        tonSpielen(11, 3, zustand); //0:C 1:C# 2:D 3:D# 4:E 5:F 6:F# 7:G 8:G# 9:A 10:A# 11:B
        break;

      case 3://D#Eb
        tonSpielen(12, 1, zustand);
        break;
    }
  }
}



//Wird aufgerufen wenn eine Note empfangen wird
void handleNoteOn(byte kanal, byte note, byte lautstaerke)
{

  waehleTon((int)note, 1);

}

//Wird aufgerufen wenn die Note nicht mehr gespielt werden soll
void handleNoteOff(byte kanal, byte note, byte lautstaerke)
{

  waehleTon((int)note, 0);//Greif servo soll nicht mehr spielen
}
