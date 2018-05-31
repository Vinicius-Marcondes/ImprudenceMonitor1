/*-----------------------------------------------------------------------
CREATE DATABASE MDI;
CREATE TABLE MDI.REGISTRO (
  ID INTEGER PRIMARY KEY AUTO_INCREMENT,
  ID_ARDUINO INT NOT NULL,
  FUNCIONARIO CHAR(40),
  CONT INT,
  VALOR_X FLOAT,
  VALOR_Y FLOAT,
  RECORDED TIMESTAMP
 );
  -------------------------------------------------------------------------*/
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <SparkFun_MMA8452Q.h>
MMA8452Q accel;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//mac do arduino
char ID [64];
char URL[128];
char X[16];
char Y[16];
char c;
char CONT;
int ID_ARDUINO = 1;
int t = 0;
float f;
float b;
float delta;
IPAddress server(191,232,52,179); //ip da internet
IPAddress ip(192, 168, 100, 60); //ip do arduino
EthernetClient client;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  accel.init();
  delay(5000);
  Serial.println("connecting...");
  while (!client.connect(server, 8080));
  sprintf(ID, "GET /cont.php?ID_ARDUINO=%d", ID_ARDUINO);
  client.println(ID);
  delay(2000);
  c = client.read();
  CONT = c - 48;
  client.println("Connection: close");
  Serial.println("Connected!");
  delay(2000);
  client.stop();
}

void loop() {
  delay(100);
  if (accel.available()) {
    accel.read();
    printCalculatedAccels();
    Serial.println();
    if (t == 0) {
      f = accel.cy;
      t = 1;
    } else {
      b = accel.cy;
      t = 0;
    }
    delta = (f - b);
    if (accel.cx > 1 || accel.cx < -1 || delta > 0.7 || accel.cy >1 || accel.cy <-1) {
      while (!client.connect(server, 8080));
      float VALOR_X = accel.cx;
      float VALOR_Y = accel.cy;
      CONT++;
      Serial.println("INFRACAO");
      dtostrf(VALOR_X, 1, 2, X);
      dtostrf(VALOR_Y, 1, 2, Y);
      sprintf(URL, "GET /update.php?ID_ARDUINO=%d&CONT=%d&VALOR_X=%s&VALOR_Y=%s", ID_ARDUINO, CONT, X, Y);
      client.println(URL);//ENVIA A URL USANDO GET
      Serial.println("URL enviada: ");
      Serial.println(URL);
      client.println("Connection: close");
      client.stop();
      delay(500);
    }
  }
}

void printCalculatedAccels() {
  Serial.print(accel.cx, 3);
  Serial.print('\t');
  Serial.print(accel.cy, 3);
  Serial.print('\t');
}

//INSERT INTO MDI.REGISTRO (ID_ARDUINO,FUNCIONARIO,CONT,VALOR_X,VALOR_Y) VALUES (1, 'JOZIAS', 0, 0, 0);



