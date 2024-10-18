#include <IRremote.h>
#include <FastLED.h>

// Pin definitions for MD10 R2 Motor Driver Shield
#define DIR_PIN 7     // Direction control pin
#define PWM_PIN 6     // PWM (speed control) pin
#define BUTTON_PIN 2  // Button input pin

#define NUM_LEDS 60
#define LED_PIN 5

CRGB leds[NUM_LEDS];

// int buttonState = 0;
// int Steps = 0;
// bool Direction = true;
// unsigned long last_time = micros(); // Initialize last_time
// unsigned long currentMillis;
// int steps_left = 16384;
// long time = 0; // Initialize time
const int REC_V_PIN = 9;
const int NUM_BUTTONS = 17;
const unsigned long code[NUM_BUTTONS] = {0xBA45FF00, 0xB946FF00, 0xB847FF00, 0xBB44FF00, 0xBF40FF00, 0xBC43FF00, 0xF807FF00, 0xEA15FF00, 0xF609FF00, 0xE619FF00, 0xE916FF00, 0xF20DFF00, 0xE718FF00, 0xF708FF00, 0xA55AFF00, 0xAD52FF00, 0xE31CFF00};
// const unsigned long BUTTON_CODE = 0xE31CFF00;
int count = 0;

void setup() {
  // Set the DIR_PIN, PWM_PIN, and BUTTON_PIN as outputs/inputs
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100); //0-255
  IrReceiver.begin(REC_V_PIN, ENABLE_LED_FEEDBACK);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Use internal pull-up resistor for the button
}

void loop() {
  // Check if the button is pressed (LOW means pressed due to INPUT_PULLUP)
  if(IrReceiver.decode()){
      Serial.print("Received raw data");
      Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.resume();
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();


  if ((digitalRead(BUTTON_PIN) == LOW) || (inArray(code, NUM_BUTTONS, IrReceiver.decodedIRData.decodedRawData))) {

    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    
    // Motor rotates forward
    digitalWrite(DIR_PIN, LOW);  // Swap rotations: LOW = forward
    analogWrite(PWM_PIN, 150);   // Run at ~78% speed
    delay(13500);                 // Run for 5 seconds
        // Add a 2-second pause between forward and reverse

    analogWrite(PWM_PIN, 0);     // Stop motor briefly during pause
    delay(2000);                 // Pause for 2 seconds

    // Motor rotates reverse
    digitalWrite(DIR_PIN, HIGH); // Swap: HIGH = reverse
    analogWrite(PWM_PIN, 200);   // Run at ~58% speed
    delay(10000);                 // Run for 5 seconds

    // Stop the motor after running in both directions
    analogWrite(PWM_PIN, 0);     // Stop motor
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
}

bool inArray(const unsigned long codes[], int size, unsigned long compareCode){
  for(int i = 0; i < size; i++){
    if(codes[i] == compareCode){
      return true;
    }
  }
  return false;
}

