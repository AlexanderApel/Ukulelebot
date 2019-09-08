/***************************************************
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 8 servos, one after the other on the
  first 8 pins of the PCA9685

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These drivers use I2C to communicate, 2 pins are required to
  interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(&Wire, 0x40);

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  250 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  450 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
//uint8_t servonum = 0;

char aenderung = 0;

int length = 300;

void setup() {
  Serial.begin(9600);
  Serial.println("8 channel Servo test!");

  pwm1.begin();

  pwm1.setPWMFreq(50);  // Analog servos run at ~60 Hz updates

  delay(10);
}
void move()
{
  for (uint16_t servonum = 0; servonum < 16; servonum++)
    {
      pwm1.setPWM(servonum, 0, length);
    }
}

//Wenn Knopf gedrückt wird dreht sich der servo so lange wie man den knopf hält in eine Richtung beim nächsten mal in die andere
void loop() {

  if (Serial.available()>1)
  {
    aenderung = Serial.read();
    delay(10);
    
    
    
  }

}

