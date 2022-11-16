#include <Arduino.h>
#define MOTOR_A_SPEED D1
#define MOTOR_A_DIR D3
#define MOTOR_B_SPEED D2
#define MOTOR_B_DIR D4

#define TICK_SPEED 10

long unsigned int LastTick = 0;

float X = 0;
float Y = 0;
float P = 0;
float I = 0;
float D = 0;

double Integral, PreviousError;

int pid(int current, int target, double P, double I, double D) {
  int error = target - current;
  Serial.printf("Error: %8i, Delta: %8i, Result: %8i\n", error, millis() - LastTick, error * (millis() - LastTick));
  Integral += error * float((millis() - LastTick));
  Serial.println(Integral);
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
    analogWriteFreq(300);
    analogWriteRange(1023);
    pinMode(MOTOR_A_DIR, OUTPUT);
    pinMode(MOTOR_A_SPEED, OUTPUT);
    pinMode(MOTOR_B_DIR, OUTPUT);
    pinMode(MOTOR_B_SPEED, OUTPUT);
}

void loop() {
    String incoming_string = Serial.readStringUntil('\n');
    Serial.println(incoming_string);
    
    String x = "";
    String y = "";
    String p = "";
    String i = "";
    String d = "";
    
    enum state {state_x, state_y, state_p, state_i, state_d, state_none};
    state current_state = state_none;

    for(auto iterator: incoming_string) {
        if      (String(iterator) == "x") {current_state = state_x; continue;}
        else if (String(iterator) == "y") {current_state = state_y; continue;}
        else if (String(iterator) == "p") {current_state = state_p; continue;}
        else if (String(iterator) == "i") {current_state = state_i; continue;}
        else if (String(iterator) == "d") {current_state = state_d; continue;}
        
        if (current_state != state_none) {
            if(current_state == state_x) {x += iterator;}
            if(current_state == state_y) {y += iterator;}
            if(current_state == state_p) {p += iterator;}
            if(current_state == state_i) {i += iterator;}
            if(current_state == state_d) {d += iterator;}
        }
    }
    if (x != "") {X = x.toFloat();}
    if (y != "") {Y = y.toFloat();}
    if (p != "") {P = p.toFloat();}
    if (i != "") {I = i.toFloat(); Integral = 0;}
    if (d != "") {D = d.toFloat();}
    Serial.printf("X: %f\nY: %f\nP: %f\nI: %f\nD: %f \n\n", X, Y, P, I, D);
    
    //digitalWrite(MOTOR_A_DIR, X>=0);
    int aSpeed = pid(X, 0, P, I, D);
    digitalWrite(MOTOR_A_DIR, aSpeed<=0);
    analogWrite(MOTOR_A_SPEED, map(abs(aSpeed),0,1023,0,300));
    //analogWrite(MOTOR_B_SPEED, abs(Speed * Y));
    
    if (abs(Y) < 150) {Y = 0;}
    
}