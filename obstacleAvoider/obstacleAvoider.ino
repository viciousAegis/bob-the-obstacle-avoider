#include <MotorDriver.h>
#include <Servo.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "time.h"
#include <ArduinoJson.h>

#define LEFT_MOTOR 3
#define RIGHT_MOTOR 2
#define TURN_SPEED 450
#define STOP_SPEED 0
#define TURN_DELAY 400 
#define FORWARD_SPEED 10008
#define BACKWARD_SPEED 500
#define SAFE_DISTANCE 25
#define PROBE_ANGLE 90

unsigned long long startTime;

#define FORWARD_DELAY 20
#define BACKWARD_DELAY 400

#define LEFT 0
#define RIGHT 1

MotorDriver m;
Servo myservo;

int echoPin = A5;
int triggerPin = A0;
#define SOUND_SPEED 0.034 // in cm/micro second

double distance = 0;

float x = 0;
float y = 0;
float angle = 0;

char *wifi_ssid = "kriti";
char *wifi_pwd = "abcd1234";
int status = WL_IDLE_STATUS;
WiFiServer server(80);
String cse_ip = "127.0.0.1"; 
String cse_port = "8080";
String server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String ae = "BobMapping";
String cnt = "node1";
WiFiClient wifi;
HttpClient Hclient = HttpClient(wifi, cse_ip, cse_port);
WiFiClient client = server.available();

void createCI(const String &val)
{ // add the lines in step 3-6 inside this function
    HTTPClient http;
    http.begin(server + ae + "/" + cnt + "/");
    http.addHeader("X-M2M-Origin", "admin:admin");
    http.addHeader("Content-Type", "application/json;ty=4");
    int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": \"" + String(val) + "\"}}"); 
    // Check if the request has been sent and close the connection
    Serial.println(code);
    if (code == -1)
    {
        Serial.println("UNABLE TO CONNECT TO THE SERVER");
    }
    http.end();
}

void setup()
{
  startTime = millis();
  myservo.attach(10);
  myservo.write(90);
  delay(1000);

  enable_WiFi();
  connect_WiFi();

  server.begin()
  printWifiStatus();

  while (WiFi.status() != WL_CONNECTED)
  {
      delay(500);
      Serial.print("Connecting...");
  }
  Serial.println("Connected");

  Serial.begin(9600);
}

void print_pos()
{
  Serial.print("x = ");
  Serial.print(x);
  Serial.print(" y = ");
  Serial.println(y);
}

void loop()
{
  client = server.available();
  
  Serial.print("\t\t\t\t\t\t");
  Serial.println(millis() - startTime);
  if(millis() - startTime > 1000){
    startTime = millis();
    
     Serial.print("PROBE HAHAHA   ");
     
     probe();
     startTime = millis();
     
  }
  distance = getDistance();
  if (distance < SAFE_DISTANCE)
  {
    createCI(distance);
    avoidObstacle();
  }
  moveForward();
  print_pos();
  delay(FORWARD_DELAY);
}

void avoidObstacle()
{
  
  meNoMove(100);
  moveBackward();
  delay(BACKWARD_DELAY);
  int turn = meNoMoveCheck();

  if (turn == LEFT)
  {
    Serial.println("LEFT LEFT LEFT");
    turnLeft();
  }
  else
  {
    Serial.println("RIGHT RIGHT RIGHT");
    turnRight();
  }

  startTime = millis();
}

double getDistance()
{
  // Clears the trigPin
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(triggerPin, HIGH);

  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH, 40000);

  double distanceCm;
//  Serial.println(duration);

  // Calculate the distance
  if (duration == 0)
  {
    distanceCm = 1000;
    Serial.println("No obstacle detected");
  }
  else
  {
    distanceCm = duration * SOUND_SPEED / 2;

    // Prints the distance in the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
  }

  return distanceCm;
}

double checkObstacle()
{
  int l_distance = lookLeft();
  int r_distance = lookRight();

  if (l_distance > r_distance)
    Serial.println("GOING LEFT");
  else
    Serial.println("GOING RIGHT");

  return l_distance > r_distance ? 1 : 0;
}

double lookLeft()
{
  myservo.write(180);
  delay(90);
  int distance = getDistance();
  myservo.write(90);
  delay(90);
  return distance;
} //

double lookRight()
{
  myservo.write(0);
  delay(90);
  int distance = getDistance();
  myservo.write(90);
  delay(90);
  return distance;
}

void meNoMove(int duration)
{
  for (int i = 0; i < duration / 2; i++)
  {
    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(2);
  }
  Serial.println("stopped");
}

int meNoMoveCheck()
{
  int turn;

  int servoPosition = 90;
  for (int i = 1; i <= PROBE_ANGLE*2; i++)
  {
    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(1);
    if (i % PROBE_ANGLE/5 == 0)
    {
      servoPosition = servoPosition + PROBE_ANGLE/10;
      myservo.write(servoPosition);
    }
  }

  int left = getDistance();

  for (int i = 1; i <= PROBE_ANGLE*2; i++)
  {
    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(1); // move from 90 to 0
    if (i % PROBE_ANGLE/5 == 0)
    {
      servoPosition = servoPosition - PROBE_ANGLE/10;
      myservo.write(servoPosition);
    }
  }
  for (int i = 0; i < PROBE_ANGLE*2; i++)
  {
    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(1);
    if (i % PROBE_ANGLE/5 == 0)
    {
      servoPosition = servoPosition - PROBE_ANGLE/10;
      myservo.write(servoPosition);
    }
  }

  int right = getDistance();
  for (int i = 1; i <= PROBE_ANGLE*2; i++)
  {

    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(1);
    if (i % PROBE_ANGLE/5 == 0)
    {
      servoPosition = servoPosition + PROBE_ANGLE/10;
      myservo.write(servoPosition);
    }
  }

  return left > right ? LEFT : RIGHT;
}

int turnLeft()
{
  angle += TURN_ANGLE;
  m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
  m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
  delay(TURN_DELAY);
}

void turnRight()
{
  angle -= TURN_ANGLE;
  x = x + TURN_SPEED*cos(angle*pi/180);
  y = y + TURN_SPEED*sin(angle*pi/180);     
  m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
  m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
  delay(TURN_DELAY);
}

void moveForward()
{
  x = x + FORWARD_SPEED*FORWARD_DELAY*cos(angle*pi/180);
  y = y + FORWARD_SPEED*FORWARD_DELAY*sin(angle*pi/180);
  m.motor(LEFT_MOTOR, FORWARD, FORWARD_SPEED);
  m.motor(RIGHT_MOTOR, FORWARD, FORWARD_SPEED);
  Serial.println("fwd");
}

void moveBackward()
{
  x = x - BACKWARD_SPEED*BACKWARD_DELAY*cos(angle*pi/180);
  y = y - BACKWARD_SPEED*BACKWARD_DELAY*sin(angle*pi/180);
  m.motor(LEFT_MOTOR, BACKWARD, BACKWARD_SPEED);
  m.motor(RIGHT_MOTOR, BACKWARD, BACKWARD_SPEED);
  Serial.println("bwd");
}

int checkDistanceInProbe(int probe_distance)
{
  startTime = millis();
  if(probe_distance < SAFE_DISTANCE){
      myservo.write(90);
      delay(300);
      Serial.println("AVOIDING OBSTACLE");
      avoidObstacle();
      
      return 1;
   }

   return 0;
}

void probe(){

  int servoPos = 90;
  
  for(int i = 1; i <= 4; i++){
    
    servoPos = servoPos + 15;
    myservo.write(servoPos);
    delay(150);
    int probe_distance = getDistance();
    Serial.println("PROBING LEFT");
    if(checkDistanceInProbe(probe_distance)) return;
    
  }

  myservo.write(90);
  servoPos = 90;
  delay(300);

   for(int i = 1; i <= 4; i++){
    servoPos = servoPos - 15;
    myservo.write(servoPos);
    delay(150);
    int probe_distance = getDistance();
    Serial.println("PROBING RIGHT");
    if(checkDistanceInProbe(probe_distance)) return;
  }

  myservo.write(90);
  delay(100);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}