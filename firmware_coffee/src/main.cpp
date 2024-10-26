#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
#include "SimplePgSQL.h"

#define STATE_BLE_CONNECTION 0
#define STATE_BLE_COMMAND 1
#define STATE_COFFEE_BEHAVIOR 2

#define flag_connected SerialBT.hasClient()

int state = 0;
bool flag_command = false;

char command;

BluetoothSerial SerialBT;

IPAddress PGIP(192,168,1,5);        // your PostgreSQL server IP

const char ssid[] = "network_ssid";      //  your network SSID (name)
const char pass[] = "network_pass";      // your network password

const char user[] = "db_username";       // your database user
const char password[] = "db_password";   // your database password
const char dbname[] = "db_name";         // your database name

int WiFiStatus;
WiFiClient client;

char buffer[1024];
PGconnection conn(&client, 0, 1024, buffer);

int pg_status = 0;

static PROGMEM const char query[] = "\
SELECT n.nspname as \"Schema\",\
  c.relname as \"Name\",\
  CASE c.relkind WHEN 'r' THEN 'table' WHEN 'v' THEN 'view' WHEN 'm' THEN 'materialized view' WHEN 'i' THEN 'index' WHEN 'S' THEN 'sequence' WHEN 's' THEN 'special' WHEN 'f' THEN 'foreign table' END as \"Type\",\
  pg_catalog.pg_get_userbyid(c.relowner) as \"Owner\"\
 FROM pg_catalog.pg_class c\
     LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace\
 WHERE c.relkind IN ('r','v','m','S','f','')\
      AND n.nspname <> 'pg_catalog'\
      AND n.nspname <> 'information_schema'\
      AND n.nspname !~ '^pg_toast'\
  AND pg_catalog.pg_table_is_visible(c.oid)\
 ORDER BY 1,2";

char *msg;
int rc;

void setup() {
  Serial.begin(115200);

  SerialBT.begin("ESP32_Server"); 
  Serial.println("Bluetooth iniciado. Aguardando conexão...");
  
  WiFi.begin((char *)ssid, pass);
}

void checkConnection()
{
    int status = WiFi.status();
    if (status != WL_CONNECTED) {
        if (WiFiStatus == WL_CONNECTED) {
            Serial.println("Connection lost");
            WiFiStatus = status;
        }
    }
    else {
        if (WiFiStatus != WL_CONNECTED) {
            Serial.println("Connected");
            WiFiStatus = status;
        }
    }
}

void doPg(void) {
    if (!pg_status) {
        conn.setDbLogin(PGIP, user, password, dbname, "utf8");
        pg_status = 1;
        return;
    }

    if (pg_status == 1) {
        rc = conn.status();
        if (rc == CONNECTION_BAD || rc == CONNECTION_NEEDED) {
            char *c = conn.getMessage();
            if (c) Serial.println(c);
            pg_status = -1;
        } else if (rc == CONNECTION_OK) {
            pg_status = 2;
            Serial.println("Conexão bem-sucedida. Executando consulta...");
        }
        return;
    }

    if (pg_status == 2) {
        // Executa a consulta definida
        if (conn.execute(query)) goto error;
        Serial.println("Consulta executada. Processando resultados...");
        pg_status = 3; // Avança para a próxima fase
    }

    if (pg_status == 3) {
        rc = conn.getData();
        int i;
        if (rc < 0) goto error;
        if (!rc) return;
        if (rc & PG_RSTAT_HAVE_COLUMNS) {
            for (i = 0; i < conn.nfields(); i++) {
                if (i) Serial.print(" | ");
                Serial.print(conn.getColumn(i));
            }
            Serial.println("\n==========");
        } else if (rc & PG_RSTAT_HAVE_ROW) {
            for (i = 0; i < conn.nfields(); i++) {
                if (i) Serial.print(" | ");
                msg = conn.getValue(i);
                if (!msg) msg = (char *)"NULL";
                Serial.print(msg);
            }
            Serial.println();
        } else if (rc & PG_RSTAT_HAVE_SUMMARY) {
            Serial.print("Linhas afetadas: ");
            Serial.println(conn.ntuples());
        } else if (rc & PG_RSTAT_HAVE_MESSAGE) {
            msg = conn.getMessage();
            if (msg) Serial.println(msg);
        }
        if (rc & PG_RSTAT_READY) {
            pg_status = 2; // Retorna ao estado de espera de uma nova consulta
            Serial.println("Consulta processada. Pronto para a próxima consulta.");
        }
    }
    return;

error:
    msg = conn.getMessage();
    if (msg) Serial.println(msg);
    else Serial.println("ERRO DESCONHECIDO");
    if (conn.status() == CONNECTION_BAD) {
        Serial.println("A conexão está ruim");
        pg_status = -1;
    }
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
        checkConnection();
        doPg();
      } 
      else if(command == 'o'); // turn off

      state = STATE_BLE_CONNECTION;

      break;
    
  }

}
