#include <driver/rtc_io.h>                      
#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

RTC_DATA_ATTR int bootCounter = 0;              //Persistent boot counter
const int freq = 5000;
const int ledChannelRed = 0;
const int ledChannelGreen = 1;
const int ledChannelBlue = 2;
const int resolution = 8;

const int ledPinGreen = 18;
const int ledPinBlue = 23;
const int ledPinRed = 19;
//const int ledInternal = 2;
const int PIN_BUTTON = 27;  
int button_state = 0;
int ledState = LOW;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 200;                          //Interval for pulse length
// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup() {

ledcSetup(ledChannelRed, freq, resolution);
ledcSetup(ledChannelGreen, freq, resolution);
ledcSetup(ledChannelBlue, freq, resolution);
ledcAttachPin(ledPinRed, ledChannelRed);
ledcAttachPin(ledPinGreen, ledChannelGreen);
ledcAttachPin(ledPinBlue, ledChannelBlue);

//pinMode(ledInternal, OUTPUT);  
pinMode (ledPinGreen, OUTPUT);
pinMode (ledPinBlue, OUTPUT);
pinMode (ledPinRed, OUTPUT);
pinMode(PIN_BUTTON, INPUT_PULLDOWN);
digitalWrite (ledPinRed, HIGH);
rtc_gpio_pulldown_en((gpio_num_t)GPIO_NUM_27);                                            //Enables ESP32 pullup or pulldown resistor during deep sleep
esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_NUM_27, RISING);
firstBoot();                                                                                                                    //If first time booting, go directly into sleep (prevents full startup after providing power)
cycle (ledChannelRed, 0, 200);
cycle (ledChannelBlue, 0, 200);
cycle (ledChannelGreen, 0, 200);
delay(50);
a2dp_sink.start("BluetoothESP32");
}

void loop() 
{
  
currentMillis = millis();

button_state = digitalRead(PIN_BUTTON);
if (button_state == HIGH) {
    ledcWrite(ledChannelGreen, 0);
    ledcWrite(ledChannelBlue, 0);
    ledcWrite(ledChannelRed, 230); 
    delay(200);    
    for(int dutyCycle = 230; dutyCycle >= 0; dutyCycle--){
    ledcWrite(ledChannelRed, dutyCycle);
    delay (3);
    }
    esp_deep_sleep_start();
  }

switch (a2dp_sink.get_connection_state()) {
        case ESP_A2D_CONNECTION_STATE_CONNECTED:
            ledcWrite(ledChannelRed, 0);
            ledcWrite(ledChannelGreen, 0);
            ledcWrite(ledChannelBlue, 150);           
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING:
            ledcWrite(ledChannelRed, 0);
            ledcWrite(ledChannelGreen, 0);
            //digitalWrite (ledPinBlue, LOW);
            PWMblink(ledChannelBlue, 0, 150);
            break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
            ledcWrite(ledChannelRed, 0);
            ledcWrite(ledChannelBlue, 0);
            PWMblink(ledChannelGreen, 0, 150);
            break;

      }
      
}


void firstBoot()
{
    if (bootCounter == 0)
    {
    bootCounter++;
    esp_deep_sleep_start();
    }
}


void cycle (int ledChannel, int minDutyCycle, int maxDutyCycle)
{
    for(int dutyCycle = minDutyCycle; dutyCycle <= maxDutyCycle; dutyCycle++){ 
    ledcWrite(ledChannel, dutyCycle);
    delay (1);
    }
    for(int dutyCycle = maxDutyCycle; dutyCycle >= minDutyCycle; dutyCycle--){
    ledcWrite(ledChannel, dutyCycle);
    delay (1);
    }
    ledcWrite(ledChannel,0);
}


void PWMblink(int ledChannel,int minDutyCycle,int maxDutyCycle)
{

//unsigned long currentMillis = millis();

if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
            previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
          if (ledState == minDutyCycle) {
                 ledState = maxDutyCycle;
          } else {
                 ledState = minDutyCycle;
          }

    // set the LED with the ledState of the variable:
    ledcWrite(ledChannel,ledState);
}
  
}




  
