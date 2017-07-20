
#include <LGATTUUID.h>
#include "LGATT_Service.h"

#ifdef APP_LOG
#undef APP_LOG
#endif

#define APP_LOG(...) Serial.printf(__VA_ARGS__); \
  Serial.println();
Sensor_Service sensor_service;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if (!LGATTServer.begin(1, &sensor_service)) {
    APP_LOG("GATT Server failed");
  }
  else {
    APP_LOG("GATT Server started!");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  float data_reading;
  LGATTServer.handleEvents();
  if (sensor_service.isConnected())
  {
      
      
      LGATTAttributeValue value = {0};
      //write your sensor code here

      //to send read data from sensor    
      String data_string = String(data_reading);
      const char *str = data_string.c_str();//to convert to standard c format 
      value.len = strlen(str);
      memcpy(value.value, str, value.len);
  
      boolean ret = sensor_service.sendIndication(value, sensor_service.getHandleNotify(), false);
      if (!ret)
      {
          APP_LOG("[FAILED] send [%d]", ret);
      }
      APP_LOG("send [%s]", value.value);
  }

}
