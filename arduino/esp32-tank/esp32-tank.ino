#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <ESP32Servo.h>

using namespace websockets;

// ---------------- WiFi & WS ----------------
const char* ssid = "CG_NET";
const char* password = "CG@54321";
const char* ws_server = "wss://YOUR_RENDER_WS_URL"; // Render WS URL

WebsocketsClient ws;

// ---------------- Motors & Servo ----------------
const int IN1 = 25, IN2 = 26, ENA = 33;
const int IN3 = 27, IN4 = 14, ENB = 32;
const int turretServoPin = 4, fireServoPin = 2;

Servo turretServo;
Servo fireServo;

int motorSpeed = 150;

// ---------------- Motor functions ----------------
void moveForward(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); analogWrite(ENA,motorSpeed); analogWrite(ENB,motorSpeed);}
void moveBackward(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); analogWrite(ENA,motorSpeed); analogWrite(ENB,motorSpeed);}
void turnLeft(){ digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); analogWrite(ENA,motorSpeed); analogWrite(ENB,motorSpeed);}
void turnRight(){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); analogWrite(ENA,motorSpeed); analogWrite(ENB,motorSpeed);}
void stopMotors(){ digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,LOW); analogWrite(ENA,0); analogWrite(ENB,0);}
void performFire(){ /* Fire servo sweep code */ }

// ---------------- WebSocket message ----------------
void onMessage(WebsocketsMessage msg){
    String cmd = msg.data();
    Serial.println("Received: " + cmd);

    if(cmd == "FWD") moveForward();
    else if(cmd == "BWD") moveBackward();
    else if(cmd == "LEFT") turnLeft();
    else if(cmd == "RIGHT") turnRight();
    else if(cmd == "STOP") stopMotors();
    else if(cmd.startsWith("speed=")) motorSpeed = cmd.substring(6).toInt();
    else if(cmd.startsWith("turret=")) turretServo.write(cmd.substring(7).toInt());
    else if(cmd == "FIRE") performFire();
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED){ delay(500); Serial.print("."); }
  Serial.println("WiFi connected");

  // Servo attach
  turretServo.attach(turretServoPin); turretServo.write(90);
  fireServo.attach(fireServoPin); fireServo.write(90);

  // Motor pins
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);

  // WebSocket connect
  ws.onMessage(onMessage);
  ws.connect(ws_server);
  ws.send("ESP32_CONNECT");
}

void loop(){
  ws.poll(); // listen for commands
}
