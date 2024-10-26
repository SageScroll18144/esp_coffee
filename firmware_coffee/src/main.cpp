#include <Arduino.h>

#define STATE_BLE_CONNECTION 0
#define STATE_BLE_COMMAND 1
#define STATE_COFFEE_BEHAVIOR 2

int state = 0;
bool flag_conected = false, flag_command = false;

void setup() {

}

void loop() {
  switch (state) {
    case STATE_BLE_CONNECTION:
      if(flag_conected) {
        state = STATE_BLE_COMMAND;
        flag_conected = false;
      } else {
        // BLE conection
      }
      
      break;
    
    case STATE_BLE_COMMAND:
      if(flag_command) {
        state = STATE_COFFEE_BEHAVIOR;
        flag_command = false;
      } else {
        // Receive BLE command
      }

      break;
    
    case STATE_COFFEE_BEHAVIOR:

      // Connect with Database
      // Make search
      // Realize search
      // Give the return to eletronic system

      state = STATE_BLE_CONNECTION;

      break;
    
  }

}
