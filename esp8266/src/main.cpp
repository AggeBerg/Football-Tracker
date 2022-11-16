#include <Arduino.h>
#define MOTOR_A_SPEED D1
#define MOTOR_A_DIR D3
#define MOTOR_B_SPEED D2
#define MOTOR_B_DIR D4

#define TICK_SPEED 10

long unsigned int LastTick = 0;

float X = 0;
float Y = 0;
float P = 1;
float I = 0;
float D = 0;
float Speed = 1;
float Damping = 0.95;

double Integral, PreviousError;

int pid(int current, int target, double P, double I, double D) {
  int error = target - current;
  Integral += error * (millis() - LastTick);
  LastTick = millis();
  double derivative = (error - PreviousError) / (TICK_SPEED / 1000.0);
  long int tempOutput = (P * error + I * Integral + D * derivative);
  int output = (tempOutput < 256) ? ((tempOutput > -256) ? tempOutput: -255): 255;

  Serial.printf(
      "current: %8i, target, %8i, output:  %8i, p: %8.2f, i: %8.2f, d: %8.2f\n",
      current, target, output, error * P, Integral * I, derivative * D);

  PreviousError = error;
  return output;
}

void setup() {
    Serial.begin(921600);
    
    pinMode(MOTOR_A_DIR, OUTPUT);
    pinMode(MOTOR_A_SPEED, OUTPUT);
    pinMode(MOTOR_B_DIR, OUTPUT);
    pinMode(MOTOR_B_SPEED, OUTPUT);
}

void loop() {
    String incoming_byte = Serial.readStringUntil('\n');
    
    String x = "";
    String y = "";
    String speed = "";
    String damping = "";
    
    enum state {state_x, state_y, state_speed, state_damping, state_none};
    state current_state = state_none;

    for(auto iterator: incoming_byte) {
        if      (String(iterator) == "x") {current_state = state_x;    continue;}
        else if (String(iterator) == "y") {current_state = state_y; continue;}
        else if (String(iterator) == "s") {current_state = state_speed; continue;}
        else if (String(iterator) == "d") {current_state = state_damping; continue;}
        
        if (current_state != state_none) {
            if(current_state == state_speed) {speed += iterator;}
            if(current_state == state_x) {x += iterator;}
            if(current_state == state_y) {y += iterator;}
            if(current_state == state_damping) {damping += iterator;}
        }
    }
    if (x != "") {X = x.toFloat();}
    if (y != "") {Y = y.toFloat();}
    if (speed != "") {Speed = speed.toFloat();}
    if (damping != "") {Damping = damping.toFloat();}
    Serial.printf("x: %f\ny: %f\ns: %f\nd: %f \n\n", X, Y, Speed, Damping);
    
    //digitalWrite(MOTOR_A_DIR, X>=0);
    int aSpeed = pid(X, 0, P, I, D);
    digitalWrite(MOTOR_A_DIR, aSpeed>=0);
    analogWrite(MOTOR_A_SPEED, aSpeed);
    //analogWrite(MOTOR_B_SPEED, abs(Speed * Y));
    
    X = X * Damping;
    Y = Y * Damping;
    
    if (abs(Y) < 150) {Y = 0;}
    
    delay(10);
}