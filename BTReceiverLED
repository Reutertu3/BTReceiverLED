/*
 * BT Receiver
 * 
 * LCK 17
 * BCK 15
 * DATA 2
*/
#include <driver/rtc_io.h>
#include "BluetoothA2DPSink.h"


#define AUTOCONNECT_TRY_NUM 2
#define WAIT 500
BluetoothA2DPSink a2dp_sink;
const int ledPinStandby = 5;
const int ledPinBlue = 23;
const int ledPinRed = 19;
const int ledInternal = 2;
const int PIN_BUTTON = 27;  
int button_state = 0;
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 200; 
// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup() {

pinMode(ledInternal, OUTPUT);  
pinMode (ledPinStandby, OUTPUT);
pinMode (ledPinBlue, OUTPUT);
pinMode (ledPinRed, OUTPUT);
pinMode(PIN_BUTTON, INPUT_PULLDOWN);
digitalWrite (ledPinRed, HIGH);
digitalWrite (ledInternal, HIGH);
rtc_gpio_pulldown_en((gpio_num_t)GPIO_NUM_27);                                            //Enables ESP32 pullup or pulldown resistor during deep sleep
esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_NUM_27, RISING);
delay(1000);
a2dp_sink.start("BluetoothESP32");
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop() 
{

button_state = digitalRead(PIN_BUTTON);
if (button_state == HIGH) {
    // Power Off
    digitalWrite (ledInternal, LOW);
    delay (400);
    esp_deep_sleep_start();
  }

switch (a2dp_sink.get_connection_state()) {
        case ESP_A2D_CONNECTION_STATE_CONNECTED:
            digitalWrite (ledPinStandby, LOW);
            digitalWrite (ledPinRed, LOW);
            digitalWrite (ledPinBlue, HIGH);
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING:
            digitalWrite (ledPinStandby, LOW);
            digitalWrite (ledPinRed, LOW);
            //digitalWrite (ledPinBlue, LOW);
            blink(23);
            break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
            digitalWrite (ledPinBlue, LOW);
            digitalWrite (ledPinRed, LOW);
            blink(5);
            break;

      }
      
}

void blink(int ledPin)
{

unsigned long currentMillis = millis();

if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
            previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
          if (ledState == LOW) {
                 ledState = HIGH;
          } else {
                 ledState = LOW;
          }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
}
  
}




  
