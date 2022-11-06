#include <MotorDriver.h>
#include <Servo.h> 



#define LEFT_MOTOR 3
#define RIGHT_MOTOR 2
#define TURN_SPEED 450
#define STOP_SPEED 0
#define TURN_DELAY 1000
#define FORWARD_SPEED 1000
#define BACKWARD_SPEED 500
#define SAFE_DISTANCE 25

#define LEFT 0
#define RIGHT 1

MotorDriver m;
Servo myservo;   

int echoPin = A5;
int triggerPin = A0;
#define SOUND_SPEED 0.034 //in cm/micro second

double distance = 0;

void setup()
{
  myservo.attach(10);  
  myservo.write(90);
  delay(1000);

  Serial.begin(9600);
}

void loop()
{
    distance = getDistance();
    if(distance < SAFE_DISTANCE)
    {
       avoidObstacle();
    }
    moveForward();

  getDistance();
  delay(20);

}

void avoidObstacle()
{
  meNoMove(100);
  moveBackward();
  delay(400);
  int turn = meNoMoveCheck();

  if(turn == LEFT)
  {
    Serial.println("LEFT LEFT LEFT");
    turnLeft();
  }
  else
  {
    Serial.println("RIGHT RIGHT RIGHT");
    turnRight();
  }
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
  long duration = pulseIn(echoPin, HIGH,40000);

  double distanceCm;
  Serial.println(duration);

  
  // Calculate the distance
  if(duration == 0){
    distanceCm = 1000;
    Serial.println("No obstacle detected");
  }
  else
  {
    distanceCm = duration * SOUND_SPEED/2;

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

  if(l_distance > r_distance)
    Serial.println("GOING LEFT");
   else
    Serial.println("GOING RIGHT");

  return l_distance > r_distance ? 1: 0;
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
  for(int i = 0; i < duration / 2; i++)
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
  for(int i = 1; i <= 90; i++)
  {
    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(1);
    if(i % 9 == 0){
      servoPosition = servoPosition + 9;
      myservo.write(servoPosition);
    }
    

  }

  int left  = getDistance();
  
  for(int i = 1; i <= 90; i++)
  {
    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(1); // move from 90 to 0
      if(i % 9 == 0){
      servoPosition = servoPosition - 9;
      myservo.write(servoPosition);
    }
  }
  for(int i = 0; i < 90; i++)
  {
    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(1);
      if(i % 9 == 0){
      servoPosition = servoPosition - 9;
      myservo.write(servoPosition);
    }
  }

  int right = getDistance();
  for(int i = 1; i <= 90; i++)
  {
 
    m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
    delay(1);
    m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
    m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
    delay(1);
      if(i % 9 == 0){
      servoPosition = servoPosition + 9;
      myservo.write(servoPosition);
    }
  }
  
  return left > right ? LEFT  : RIGHT;
}


int turnLeft()
{
  m.motor(LEFT_MOTOR, BACKWARD, TURN_SPEED);
  m.motor(RIGHT_MOTOR, FORWARD, TURN_SPEED);
  delay(TURN_DELAY);

}

void turnRight()
{
  m.motor(LEFT_MOTOR, FORWARD, TURN_SPEED);
  m.motor(RIGHT_MOTOR, BACKWARD, TURN_SPEED);
  delay(TURN_DELAY);

}

void moveForward()
{
  
  m.motor(LEFT_MOTOR, FORWARD, FORWARD_SPEED);
  m.motor(RIGHT_MOTOR, FORWARD, FORWARD_SPEED);
  Serial.println("fwd");
}

void moveBackward()
{
  m.motor(LEFT_MOTOR, BACKWARD, BACKWARD_SPEED);
  m.motor(RIGHT_MOTOR, BACKWARD, BACKWARD_SPEED);
  Serial.println("bwd");
}