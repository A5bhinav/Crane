#include <IRremote.h>
#include <FastLED.h>
#include <AccelStepper.h>
// #include <iostream>
// #include <vector>
// #include <algorithms>

#define MotorInterfaceType 8

AccelStepper stepper1 = AccelStepper(MotorInterfaceType, 8, 9, 10, 11);

#define NUM_LEDS 60
#define LED_PIN 4

CRGB leds[NUM_LEDS];


#define IN1  8
#define IN2  9
#define IN3  10
#define IN4  11
#define IN5  7

int buttonState = 0;
int Steps = 0;
bool Direction = true;
unsigned long last_time = micros(); // Initialize last_time
unsigned long currentMillis;
int steps_left = 16384;
long time = 0; // Initialize time
const int REC_V_PIN = 2;
const int NUM_BUTTONS = 17;
const unsigned long code[NUM_BUTTONS] = {0xBA45FF00, 0xB946FF00, 0xB847FF00, 0xBB44FF00, 0xBF40FF00, 0xBC43FF00, 0xF807FF00, 0xEA15FF00, 0xF609FF00, 0xE619FF00, 0xE916FF00, 0xF20DFF00, 0xE718FF00, 0xF708FF00, 0xA55AFF00, 0xAD52FF00, 0xE31CFF00};
// const unsigned long BUTTON_CODE = 0xE31CFF00;
int count = 0;


void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(100); //0-255
    pinMode(IN1, OUTPUT); 
    pinMode(IN2, OUTPUT); 
    pinMode(IN3, OUTPUT); 
    pinMode(IN4, OUTPUT); 
    pinMode(IN5, INPUT);
    stepper1.setMaxSpeed(1500);
    stepper1.setAcceleration(500);
    IrReceiver.begin(REC_V_PIN, ENABLE_LED_FEEDBACK);
    Serial.println("Connection has been established");
}

void loop() {
    if(IrReceiver.decode()){
      Serial.print("Received raw data");
      Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.resume();
    }

    buttonState = digitalRead(IN5);
    if ((buttonState == HIGH) || (inArray(code, NUM_BUTTONS, IrReceiver.decodedIRData.decodedRawData))) {
       for(int i = 0; i < NUM_LEDS; i++){
        leds[i] = CRGB::Red;
        FastLED.show();
        delay(50);
       }
        while ((steps_left > 0)) {
            currentMillis = micros();
            if (currentMillis - last_time >= 1000) {
                stepper(1); 
                time += micros() - last_time;
                last_time = micros();
                steps_left--;
            }
            if(steps_left == 0){
              count++;
              Direction = !Direction;
              steps_left = 16384;
              if(count == 2){
                break;
              }
              delay(2000);
            }
        }
        count = 0;
        for(int i = NUM_LEDS-1; i >= 0; i--){
          leds[i] = CRGB::Black;
          FastLED.show();
          delay(50);
        }
        // Serial.print("Rotations count: " + count);
        // Serial.println(time);
        // Serial.println("Wait...!");
        // delay(2000);
        // Direction = !Direction;
        // steps_left = 16384;
    }
}

void stepper(int xw) {
    for (int x = 0; x < xw; x++) {
        switch (Steps) {
            case 0:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, HIGH);
                break; 
            case 1:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, HIGH);
                break; 
            case 2:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
                break; 
            case 3:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
                break; 
            case 4:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                break; 
            case 5:
                digitalWrite(IN1, HIGH); 
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                break; 
            case 6:
                digitalWrite(IN1, HIGH); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                break; 
            case 7:
                digitalWrite(IN1, HIGH); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, HIGH);
                break; 
            default:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                break; 
        }
        SetDirection();
    }
}

void SetDirection() {
    if (Direction) {
        Steps++;
    } else {
        Steps--;
    }
    if (Steps > 7) { Steps = 0; }
    if (Steps < 0) { Steps = 7; }
}

bool inArray(const unsigned long codes[], int size, unsigned long compareCode){
  for(int i = 0; i < size; i++){
    if(codes[i] == compareCode){
      return true;
    }
  }
  return false;
}