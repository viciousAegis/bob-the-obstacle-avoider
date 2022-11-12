#include <MotorDriver.h>
#include <Servo.h>

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

void setup()
{
  startTime = millis();
  myservo.attach(10);
  myservo.write(90);
  delay(1000);

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
