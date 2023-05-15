#include "Arduino.h"
#include "GBusHelpers.h"
#include "GBusWifiMesh.h"
#include <EEPROM.h>
#include <WiFi.h>
#include "driver/adc.h"

#define FWVERSION 1.0"
#define LogLevel ESP_LOG_NONE

MeshApp mesh;

// Prototypes
void meshMessage(String payload, String from, int flag);
void SentNodeInfo();

uint8_t ModulType = 255;

void setup()
{
  Serial.begin(115200);
  /**
   * @brief Set the log level for serial port printing.
   */
  esp_log_level_set("*", ESP_LOG_NONE);
  esp_log_level_set(TAG, ESP_LOG_NONE);

  Serial.printf("ModuleType: %u\n", ModulType);

  mesh.onMessage(meshMessage);
  mesh.start(true);

}

void loop()
{
}

void SentNodeInfo()
{
  // uint32_t OwnId = WiFi.macAddress();
  wifi_power_t WifiPower;
  WifiPower = WiFi.getTxPower();

  String Msg = "NodeId:" + String(mesh.GetNodeId()) + " MAC:" + WiFi.macAddress() + 
               " Type:" + String(ModulType) + " FW: " + FWVERSION + " WifiPo:" + String(WifiPower);
  // Serial.println("SentNodeInfo");
  // Serial.println(Msg);

  mesh.SendMessage(Msg);
}

void meshMessage(String payload, String from, int flag)
{
  String msg = payload;

  Serial.printf("Rec msg %u: %s\n", msg.length(), msg);
  Serial.println(msg);
  Serial.println("New Message!");

  String Type = getValue(msg, ' ', 0);
  String Number = getValue(msg, ' ', 1);
  String Command = getValue(msg, ' ', 2);
  uint8_t NumberInt = Number.toInt();

  if (Type == "Output")
  {
    
  }
  else if (msg.startsWith("I'm Root!"))
  {
    Serial.println("Gateway hold alive received");
    LastCheckForRootNodeMillis = millis();
  }
  else if (Type == "Config")
  {
    Serial.printf("Config\n");
    String ConfigType = getValue(msg, ' ', 1);
    // Config ModulType 2|4|6
    if (ConfigType == "ModulType")
    {
      String Type = getValue(msg, ' ', 2);
      Serial.printf("New ModulType: %s\n", Type.c_str());
      EEPROM.write(0, (uint8_t)Type.toInt());
      EEPROM.commit();
      ESP.restart();
    }
    else if (ConfigType == "WifiPower")
    {
      String Type = getValue(msg, ' ', 2);
      Serial.printf("WifiPower: %s\n", Type.c_str());
      EEPROM.write(2, (uint8_t)Type.toInt());
      EEPROM.commit();
      ESP.restart();
    }
  }
  else if (Type == "GetNodeInfo")
  {
    SentNodeInfo();
  }
  else if (Type == "Reboot")
  {
    ESP.restart();
  }
}
