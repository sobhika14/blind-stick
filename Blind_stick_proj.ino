
#include <SoftwareSerial.h>

#include <TinyGPS++.h>  //, this library provides compact and easy-to-use methods for extracting position, date, time, altitude, speed, and course from consumer GPS devices. 

int buttonpin = 6; //pushbutton -when pressed to send msg for help with location

float lattitude, longitude;

float a[2]; //2 decimal places

float *p;

SoftwareSerial gpsSerial(2, 3); //gps pin connected

SoftwareSerial gsmSerial(10, 11); //gsm pin connected to 

TinyGPSPlus gps;

const int trigPin1 = A1; //ultrasonic (1) sensors pins 

const int echoPin1 = A0; //input pin

long duration1;

int distance1;

/*const int trigPin2 = A2; //2nd ultrasonic sensor

const int echoPin2 = A3;

long duration2;

int distance2;*/

void setup()

{

  pinMode(trigPin1, OUTPUT); //ultrasonic sensor

  pinMode(echoPin1, INPUT);

  pinMode(5, OUTPUT);

  pinMode(A4, INPUT_PULLUP); //INPUT_PULLUP. This option is the same as INPUT (you read data from the sensor), but in addition to that, an internal pull up resistor – between 20k and 50k Ohm – is enabled, to keep the signal HIGH by default.

  Serial.begin(9600);

 /* pinMode(trigPin2, OUTPUT);

  pinMode(echoPin2, INPUT);

  pinMode(buttonpin, INPUT_PULLUP);


  Serial.begin(9600);*/

  delay(1000);

  gpsSerial.begin(9600);

  delay(1000);

  gsmSerial.begin(9600);

  delay(1000);

  Serial.print("—Tracking–");

  Serial.print("***Location***");

  gsmSerial.println("AT+CNMI=2,2,0,0,0"); //The AT command to receive a live SMS is “AT+CNMI=2,2,0,0,0” – we just need to send this command to the GSM module and apply a 1-second delay. Once you send this command, try sending an SMS to the SIM card number put inside the GSM module. You will see the SMS you had sent 

  delay(3000);

  Serial.print("Initializing……");

  delay(2000);

  Serial.print("System Ready  ");

  delay(1000);
} 

void loop()

{

  digitalWrite(trigPin1, LOW); //ultrasonic sensor

  delayMicroseconds(2);

  digitalWrite(trigPin1, HIGH); 

  delayMicroseconds(10); //delay

  digitalWrite(trigPin1, LOW);

  duration1 = pulseIn(echoPin1, HIGH); //The length of the pulse (in microseconds) or 0 if no pulse started before the timeout.

                                       //useful to be able to read data from certain sensors.
  distance1 = duration1 * 0.034 / 2; 
  
  //distance = duration*(0.034/2); // (speed in microseconds)  
  // Speed of sound wave (340 m/s)divided by 2 (forward and backward bounce)  
  // To display the distance on Serial Monitor  

  Serial.print("Distance1: ");

  Serial.println(distance1);

 /* digitalWrite(trigPin2, LOW);

  delayMicroseconds(2);

  digitalWrite(trigPin2, HIGH);

  delayMicroseconds(10);

  digitalWrite(trigPin2, LOW);

  duration2 = pulseIn(echoPin2, HIGH);

  distance2 = duration2 * 0.034 / 2;

  Serial.print("Distance2: ");

  Serial.println(distance2); */

  if (distance1 <= 20 ) {   //distance range for ultrasonic for sen1 
    
    digitalWrite(5, HIGH);

    delay(1000);

    digitalWrite(5, LOW);

    digitalWrite(5, HIGH);

    delay(1000);

    digitalWrite(5, LOW);

    digitalWrite(5, HIGH);

    delay(1000);

    digitalWrite(5, LOW);

  }

  else if (distance1 <= 15 ) {

    digitalWrite(5, HIGH);

    delay(500);

    digitalWrite(5, LOW);

    digitalWrite(5, HIGH);

    delay(500);

    digitalWrite(5, LOW);

    digitalWrite(5, HIGH);

    delay(500);

    digitalWrite(5, LOW);

  }

  else

    digitalWrite(5, LOW);

 /* int sensorValue = digitalRead(A4);

  if (sensorValue == 1) { //not using water sensor

    digitalWrite(5, HIGH);

    Serial.println("Rain Alert");

    delay(1500);

    digitalWrite(5, LOW);

    digitalWrite(5, HIGH);

    delay(1500);

  }

  else

    digitalWrite(5, LOW); */

  if (digitalRead(buttonpin) == LOW)

  {

    Serial.println("button pressed");

    delay(2000);

    SendMessage();
  }

  if (gsmSerial.available() > 0)  //available, to get the available number of bytes if it is greater than 0. The Serial.read( ) function will read the data from the data byte and print a message if the data is received. The data is sent from the serial monitor to the Arduino.

    Serial.write(gsmSerial.read()); 
  while (gsmSerial.available())

  {

    gsmSerial.read();
  }

  while (Serial.available())

  {

    Serial.read(); //Arduino reads the incoming serial data in the Arduino

  }

  get_gsm(); //get msg
}

float *get_gps() //get coordinates

{

  gpsSerial.listen(); //Enables the selected SoftwareSerial object to listen. Only one SoftwareSerial object can listen at a time; data that arrives for other ports will be discarded. Any data already received is discarded during the call to listen() function (unless the given instance is already listening)

  Serial.println("INSIDE get_gps");

  while (1)

  {

    while (gpsSerial.available() > 0)

    {
      gps.encode(gpsSerial.read()); //function encodes it properly and returns true and ultimately I get correct values of longitude and latitude
    }

    if (gps.location.isUpdated()) //isUpdated() indicates whether the object’s value has been updated (not necessarily changed) since the last time you queried it.

    {

      Serial.print("LAT=");
      Serial.println(gps.location.lat(), 6); //in tinygps 6-decimal accuracy

      Serial.print("LONG=");
      Serial.println(gps.location.lng(), 6);

      lattitude = gps.location.lat();

      longitude = gps.location.lng();

      break;
    }
  }

  a[0] = lattitude;

  a[1] = longitude;

  return a;
}

void get_gsm()

{

  gsmSerial.listen(); //enables software port to listen

  while (gsmSerial.available() > 0)
  
  {
    Serial.println("INSIDE gsmSerial.available");

    if (gsmSerial.find("Track"))

    {
      Serial.println("INSIDE track");

      gsmSerial.println("AT+CMGF=1");  //The AT command +CMGF (command name in text: Message Format) is used to select the operating mode of the GSM/GPRS modem or mobile phone. It takes one parameter. The value of the parameter can either be 0 or 1. The values 0 and 1 refer to SMS PDU mode and SMS text mode respectively.

      delay(1000);  

      gsmSerial.println("AT+CMGS=\"+9188305848xx\"\r");  //AT+CMGS At command is used to send SMS message.

      delay(1000);

      p = get_gps();

      gsmSerial.listen();

      Serial.print("Your Car Location: ");

      gsmSerial.print("Your Car Location: ");

      Serial.print("LATTITUDE=");
      Serial.print(*p, 6);

      gsmSerial.print("LATTITUDE=");
      gsmSerial.print(*p, 6);
      gsmSerial.print(",");  // The SMS text you want to send

      Serial.print("LONGITUDE=");
      Serial.print(*(p + 1), 6);

      gsmSerial.print("LONGITUDE=");
      gsmSerial.print(*(p + 1), 6);  // The SMS text you want to send

      delay(100);

      gsmSerial.println((char)26);  // ASCII code of CTRL+Z for saying the end of sms to  the module

      delay(1000);
    }
  }
}

void SendMessage()

{

  gsmSerial.println("AT+CMGF=1");  //Sets the GSM Module in Text Mode

  delay(1000);  // Delay of 1000 milli seconds or 1 second

  gsmSerial.println("AT+CMGS=\"+9188305848xx\"\r");  // Replace x with mobile number

  delay(1000);

  gsmSerial.println("I Am In Problem Plz Help Me");  // The SMS text you want to send

  delay(1000);

  p = get_gps();

  gsmSerial.listen();

  Serial.print("Your Position is : ");

  gsmSerial.print("position is : ");

  Serial.print("LATTITUDE=");
  Serial.print(*p, 6);

  gsmSerial.print("LATTITUDE=");
  gsmSerial.print(*p, 6);
  gsmSerial.print(",");  // The SMS text you want to send

  Serial.print("LONGITUDE=");
  Serial.print(*(p + 1), 6);

  gsmSerial.print("LONGITUDE=");
  gsmSerial.print(*(p + 1), 6);  // The SMS text you want to send

  delay(100);
  

  gsmSerial.println((char)26);
}
