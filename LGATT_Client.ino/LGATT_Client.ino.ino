
#include <LGATTUUID.h>
#include <LGATTServer.h>
#include <LGATTClient.h>


#ifdef APP_LOG
#undef APP_LOG
#endif


#define APP_LOG(...) Serial.printf(__VA_ARGS__); \
    Serial.println();
    
static LGATTUUID test_uuid("B4B4B4B4-B4B4-B4B4-B4B4-B4B4B4B4B4B4");//client uuid

LGATTUUID uuidService("E20A39F4-73F5-4BC4-A12F-17D1AD07A961");//UUId of sevice which we want to access(smae as given in server)

LGATTClient c;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if (c.begin(test_uuid))
  {
      Serial.println("BLE client is successfully register.");
  }
  else
  {
      Serial.println("Failed to register BLE client");
      delay(20000);
  }

}


boolean has_Specific_Device_Found(const LGATTDeviceInfo &info)//to find server device
{
  
  int addrs[6]={0x6b,0x2d,0x6e,0x19,0xe6,0xd3};//address of server device
  for(int i=0;i<6;i++)
  {
    if(addrs[i]!=info.bd_addr.addr[i])//checking for server device
      return false;
     else
      continue;
  }
  return true;
}

void accessBleService(const LGATTUUID inputServiceUUID)
{
    float sensor_reading=0;
    // query all services, to find if your specified service exist or not.
    int num = c.getServiceCount();
    
    //  all of the services
    Serial.printf("number of services:%d\n",num);
    Serial.println();

    for (int i = 0; i < num; i++)
    {
        LGATTUUID serviceUUID;
        boolean isPrimary = false;//for service
        boolean isPrimary1=false;//for characteristics (note:you can use same variable)
        
        LGATTAttributeValue value = {0};
       
        boolean ret1= c.getServiceInfo(i, serviceUUID, isPrimary) ;//getting service infornmation
            
        if(!ret1)
        {
          continue;
        }
        Serial.print("service id :");
        Serial.print(serviceUUID);
        Serial.println();
        if ((serviceUUID == inputServiceUUID))//checking whether obtained service id is same as required service id
        {
          
            boolean ret;

            while (1)
            {
                delay(1000);
                LGATTUUID characteristicUUID;
                LGATT_CHAR_PROPERTIES properties = 0;

                // polling all characteristics of current service
                ret = c.getCharacteristicInfo(serviceUUID, isPrimary1,characteristicUUID, properties);
             
                if (ret)
                {
                    // characteristic processing here
                    Serial.printf("inside if");
                    if (c.setupNotification(true, serviceUUID, isPrimary1, characteristicUUID))
                    {
                        Serial.println("notification already enabled");
                    }
            
                    // query if the notification has come.(as sensor will be sending data using notification
                    
                
                    if (c.queryNotification(serviceUUID, isPrimary1, characteristicUUID, value))
                    {
                        Serial.printf("Sensor data in string format: %s\n",value.value);
                      //retrieving data 
                        const char *str=(char *)value.value;
                        sensor_reading=atof(str);//converting data from string format to float(as I'll reuire this readings in furntuher calculations )
                        Serial.printf("sensor reading %f\n",sensor_reading);
                       
                        delay(1000);           
                             
                    }
                    else{
                      Serial.println("[LGATTC ino] Client can't not find the notification data");
                    }
                    
                }
                else
                {
                    Serial.printf("charatceristic not found\n");
                    Serial.println();
                   break;
                }

            }
            break;
        }
        else
        {
            delay(2000);
            Serial.printf("service not matched\n");
        }

    }

}


void loop() {
  // put your main code here, to run repeatedly:
  
    LGATTDeviceInfo info={0};
    
    while(1)
    {
        int num =c.scan(6);//scanning for bluetooth device with timeout of 6 sec
        Serial.printf("number ofdevices scan:%d",num);
        for(int i=0;i<num;i++)
        {
            c.getScanResult(i, info);//getting information of sacnned device
            Serial.printf("adds %2x:%2x:%2x:%2x:%2x:%2x:\n",info.bd_addr.addr[5],info.bd_addr.addr[4],info.bd_addr.addr[3],info.bd_addr.addr[2],info.bd_addr.addr[1],info.bd_addr.addr[0]);
            if(has_Specific_Device_Found(info))
            {
              if(!c.connect(info.bd_addr))//connecting with server  device
              {
                Serial.printf("failed to connect ...\n");
              }
              else
              {
                while(1)
                {
                   accessBleService(uuidService);//accessing particular ble service
                   delay(1000);
                }
              }
            }
        }         
            
            
    }   

}
