/* Example code to demonstrate attached external sensors to the ESP8266 on the NodeMCU board
 *  The sensors comprise of: 
 *    onewire temperature sensor DS18B20 attached to D4
 *    PIR presense sensor attached to D1
 *    Push button switch attached to D2
 *    Potentiometer attached to the analogue input A0
 *  
 *  Interrupts are used to:
 *    capture the rising edge when the PIR detects a person
 *    capture the falling edge when the switch is pressed
 */

#include <OneWire.h>
#include <DallasTemperature.h>

//#define PIN D3    // GPIO0 labelled pin D3 on NodeMCU board
//#define BLUELED D4   // GPIO2 labelled pin D4 on NodeMCU board this pin is also connected to the LED cathode on the module
//#define REDLED D0    // GPIO16 labelled pin D0 on NodeMCU board this pin is also connected to the LED cathode on the NodeMCU board and used to wake the board

#define ADC A0

#define PIR D1   //GPIO5 labelled D1 on NodeMCU 1.0 board
#define BUTTON D2
#define ONE_WIRE_BUS D4   // DS18B20 pin 

#define SLEEP_DELAY_IN_SECONDS  30

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

char temperatureString[6];


void setup() {
// Initialise serial link to console  
  Serial.begin(115200);
  Serial.println("");
   
//Prepare GPIO PINs
  pinMode(PIR, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR), pir_int, RISING);

  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), button_int, FALLING);

  DS18B20.begin();
  Serial.println("Setup Complete");
}

float getTemperature() {
  // Function to read temperature from DS18B20 sensor
  Serial.println("Requesting DS18B20 temperature...");
  float temp;
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("");
  Serial.println("Main Loop");
  
  int ADCReading = analogRead(ADC);
  Serial.print("ADC Value: ");
  Serial.println(ADCReading);
  

  float temperature = getTemperature();
  // convert temperature to a string with two digits before the comma and 2 digits for precision
  //dtostrf(temperature, 2, 2, temperatureString);
  // send temperature to the serial console
  Serial.print("Temperature: ");
  Serial.println(temperature);

  delay(500);
  
}

void button_int(void){
  //Interupt function for switch press
  Serial.println("Switch Pressed");
}

void pir_int(void){
  //Interupt function for PIR sense detect
  Serial.println("Presense Sensed");
}
