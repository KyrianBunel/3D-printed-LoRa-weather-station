#include <Wire.h>
#include "Arduino.h"
#include "LoRaWan_APP.h"
#include "Seeed_BME280.h"
#include "DFRobot_LTR390UV.h"
#include "DFRobot_RainfallSensor.h"

#define timetillsleep 200
#define timetillwakeup 1800000  // 30 min sleep
//#define timetillwakeup 10000  // 10s sleep # for dev only

DFRobot_LTR390UV ltr390(/*addr = */ LTR390UV_DEVICE_ADDR, /*pWire = */ &Wire);
DFRobot_RainfallSensor_I2C Sensor(&Wire);
BME280 bme280;

float temperature;
float humidity;
float pressure;
float luminosity;
float pluie;
float BATTvoltage;

static TimerEvent_t sleep;
static TimerEvent_t wakeUp;
uint8_t lowpower = 1;


#include "LoRaWan_APP.h"
#include "Arduino.h"

#define RF_FREQUENCY 868000000  // Hz

#define TX_OUTPUT_POWER 14  // dBm

#define LORA_BANDWIDTH 0         // [0: 125 kHz, \
                                 //  1: 250 kHz, \
                                 //  2: 500 kHz, \
                                 //  3: Reserved]
#define LORA_SPREADING_FACTOR 7  // [SF7..SF12]
#define LORA_CODINGRATE 1        // [1: 4/5, \
                                 //  2: 4/6, \
                                 //  3: 4/7, \
                                 //  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8   // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0    // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#define RX_TIMEOUT_VALUE 1000
#define BUFFER_SIZE 80  // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;
float txNumber;

bool lora_idle = true;
bool debug = false;


void OnTxDone(void) {
  if (debug) {
    Serial.println("TX done......");
  }

  lora_idle = true;
}

void OnTxTimeout(void) {
  if (debug) {
    Serial.println("TX Timeout......");
  }
  Radio.Sleep();
  lora_idle = true;
}

void onSleep() {
  if (debug) {
    Serial.println("Sleep mode during " + String(timetillwakeup / 60000) + "min");
  }
  lowpower = 1;
  //timetillwakeup ms later wake up;
  TimerSetValue(&wakeUp, timetillwakeup);
  TimerStart(&wakeUp);
}

void onWakeUp() {
  lowpower = 0;
  temperature = bme280.getTemperature() - 2;
  humidity = bme280.getHumidity();
  pressure = bme280.getPressure() / 100;
  luminosity = ltr390.readALSTransformData();
  uint16_t voltage = getBatteryVoltage();
  BATTvoltage = (float)voltage / 1000;

  if (lora_idle == true) {
    delay(1000);
    txNumber += 0.01;
    snprintf(txpacket, BUFFER_SIZE, "Temp: %.2fC, Hum: %.2f%%, Press: %.2fHpa, Lum: %.2fLux, Batt: %.2fV", temperature, humidity, pressure, luminosity, BATTvoltage);
    Radio.Send((uint8_t *)txpacket, strlen(txpacket));  //send the package out
    lora_idle = false;
  }

  if (debug) {
    Serial.println(txpacket);
    Serial.println("Woke up, sending data:");
    Serial.println("----------------------------");
    Serial.print("⎜Temp         ⎜");
    Serial.print(temperature);
    Serial.println(" °C     ⎜");  //The unit for  Celsius because original arduino don't support special symbols
    Serial.print("⎜Humidity     ⎜");
    Serial.print(humidity);
    Serial.println(" %      ⎜");
    Serial.print("⎜Pressure     ⎜");
    Serial.print(pressure);
    Serial.println(" hPa  ⎜");
    Serial.print("⎜Luminosity   ⎜");
    Serial.print(luminosity);
    Serial.println(" Lux     ⎜");
    Serial.print("⎜Battery      ⎜");
    Serial.print(BATTvoltage);
    Serial.println(" V       ⎜");
    Serial.println("----------------------------");

    /*
    Serial.print("rainfall raw: ");
    Serial.print(Sensor.getRawData());
    Serial.println("    ");
    //Get the accumulated rainfall within 1 hour of the system (function parameter optional 1-24)
    Serial.print("1 Hour Rainfall: ");
    Serial.print(Sensor.getRainfall(1));
    Serial.println(" mm");
    */
  }


  //timetillsleep ms later into lowpower mode;
  TimerSetValue(&sleep, timetillsleep);
  TimerStart(&sleep);
}

void setup() {
  Serial.begin(115200);

  if (!bme280.init()) {
    Serial.println("Temp sensor initialize failed!");
  }

  while (ltr390.begin() != 0) {
    Serial.println("Luminosity sensor initialize failed!");
  }

  /*
  while (!Sensor.begin()) {
    Serial.println("Rain sensor initialize failed!");
  }
  */
  Serial.println(" Sensors  initialize success!!");
  ltr390.setALSOrUVSMeasRate(ltr390.e18bit, ltr390.e100ms);  //18-bit data, sampling time of 100ms
  ltr390.setALSOrUVSGain(ltr390.eGain3);                     //Gain of 3
  ltr390.setMode(ltr390.eALSMode);                           //Set ambient light mode


  txNumber = 0;

  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, 3000);


  Radio.Sleep();
  TimerInit(&sleep, onSleep);
  TimerInit(&wakeUp, onWakeUp);
  //onSleep();
  onWakeUp();
}

void loop() {
  if (lowpower) {
    lowPowerHandler();
  }
}