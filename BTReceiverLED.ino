/*
 * BT Receiver
 * 
 * LCK 15
 * BCK 4
 * DATA 2
*/
#include <driver/rtc_io.h>                      
#include "BluetoothA2DPSink.h"
#define AUTOCONNECT_TRY_NUM 2

BluetoothA2DPSink a2dp_sink;

RTC_DATA_ATTR int bootCounter = 0;              //Persistent boot counter
const int freq = 20000;
const int ledChannelRed = 0;
const int ledChannelGreen = 1;
const int ledChannelBlue = 2;
const int resolution = 11;

const int ledPinGreen = 33;
const int ledPinBlue = 32;
const int ledPinRed = 25;
//const int ledInternal = 2;
const int PIN_BUTTON = 13;  
int button_state = 0;
int ledState = LOW;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 500;                          //Interval for pulse length
// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup() {
  i2s_pin_config_t my_pin_config = {
        .bck_io_num = 4,
        .ws_io_num = 15,
        .data_out_num = 2,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
  a2dp_sink.set_pin_config(my_pin_config);

  static i2s_config_t i2s_config = {
      .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 44100, // updated automatically by A2DP
      .bits_per_sample = (i2s_bits_per_sample_t)32,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0, // default interrupt priority
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = true,
      .tx_desc_auto_clear = true // avoiding noise in case of data unavailability
  };
  a2dp_sink.set_i2s_config(i2s_config);
    
ledcSetup(ledChannelRed, freq, resolution);
ledcSetup(ledChannelGreen, freq, resolution);
ledcSetup(ledChannelBlue, freq, resolution);
ledcAttachPin(ledPinRed, ledChannelRed);
ledcAttachPin(ledPinGreen, ledChannelGreen);
ledcAttachPin(ledPinBlue, ledChannelBlue);

//pinMode(ledInternal, OUTPUT);  
//pinMode (ledPinGreen, OUTPUT);
//pinMode (ledPinBlue, OUTPUT);
//pinMode (ledPinRed, OUTPUT);
pinMode(PIN_BUTTON, INPUT_PULLDOWN);
//digitalWrite (ledPinRed, HIGH);
rtc_gpio_pulldown_en((gpio_num_t)GPIO_NUM_13);                                            //Enables ESP32 pullup or pulldown resistor during deep sleep
esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_NUM_13, RISING);
firstBoot();                                                                             //If first time booting, go directly into sleep (prevents full startup after providing power)
cycle (ledChannelRed, 0, 200, 1);
cycle (ledChannelBlue, 0, 200, 1);
cycle (ledChannelGreen, 0, 200, 1);
delay(50);
a2dp_sink.start("BTReceiverV1");
}

void loop() 
{
  
currentMillis = millis();

button_state = digitalRead(PIN_BUTTON);
if (button_state == HIGH) {
    ledcWrite(ledChannelGreen, 0);
    ledcWrite(ledChannelBlue, 0);
    ledcWrite(ledChannelRed, 150); 
    delay(500);    
    for(int dutyCycle = 200; dutyCycle >= 0; dutyCycle--){
    ledcWrite(ledChannelRed, dutyCycle);
    delay (3);
    }
    esp_deep_sleep_start();
  }

switch (a2dp_sink.get_connection_state()) {
        case ESP_A2D_CONNECTION_STATE_CONNECTED: //Verbunden - Durchgehend blau
            ledcWrite(ledChannelRed, 0);
            ledcWrite(ledChannelGreen, 0);
            ledcWrite(ledChannelBlue, 150);           
            break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING: //Verbindungsaufbau - Blau blinkend
            ledcWrite(ledChannelRed, 0);
            ledcWrite(ledChannelGreen, 0);
            cycle (ledChannelBlue, 20, 200, 3);
            break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED: //Bereit für Verbundung - Grün blinkend
            ledcWrite(ledChannelRed, 0);
            ledcWrite(ledChannelBlue, 0);
            cycle (ledChannelGreen, 20, 200, 3);
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


void cycle (int ledChannel, int minDutyCycle, int maxDutyCycle, int blinkDelay)
{
    for(int dutyCycle = minDutyCycle; dutyCycle <= maxDutyCycle; dutyCycle++){ 
    ledcWrite(ledChannel, dutyCycle);
    delay (blinkDelay);
    }
    for(int dutyCycle = maxDutyCycle; dutyCycle >= minDutyCycle; dutyCycle--){
    ledcWrite(ledChannel, dutyCycle);
    delay (blinkDelay);
    }
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




  
