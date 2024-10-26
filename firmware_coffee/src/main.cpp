#include <Arduino.h>
#include <BluetoothSerial.h>

#define STATE_BLE_CONNECTION 0
#define STATE_BLE_COMMAND 1
#define STATE_COFFEE_BEHAVIOR 2

#define flag_connected SerialBT.hasClient()

int state = 0;
bool flag_command = false;

char command;

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  SerialBT.begin("ESP32_Server"); // Inicia Bluetooth como servidor
  Serial.println("Bluetooth iniciado. Aguardando conexão...");

}

void loop() {
  switch (state) {
    case STATE_BLE_CONNECTION:
      if(flag_connected) {
        state = STATE_BLE_COMMAND;
      }      
      break;
    
    case STATE_BLE_COMMAND:
      if (SerialBT.available()) {
        command = SerialBT.read(); 
        Serial.print("Recebido: "); 
        Serial.println(command);
        state = STATE_COFFEE_BEHAVIOR;
      } else {
        state = STATE_BLE_CONNECTION;
      }
      break;
    
    case STATE_COFFEE_BEHAVIOR:

      if (command == 's') { // start coffee
        // Connect with Database
        // Make search
        // Realize search
        // Give the return to eletronic system
      } 
      else if(command == 'o'); // turn off

      state = STATE_BLE_CONNECTION;

      break;
    
  }

}
