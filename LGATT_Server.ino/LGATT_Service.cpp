#include "vmbtgatt.h"
#include "LGATT_Service.h"
 
#ifdef APP_LOG
#undef APP_LOG
#define APP_LOG(...) Serial.printf(__VA_ARGS__); \
    Serial.println();
#endif

static LGATTServiceInfo g_sensor_decl[] =
{
    {TYPE_SERVICE, "E20A39F4-73F5-4BC4-A12F-17D1AD07A961", TRUE, 0, 0, 0},
    {TYPE_CHARACTERISTIC, "08590F7E-DB05-467E-8757-72F6FAEB13D4", FALSE, VM_GATT_CHAR_PROP_NOTIFY | VM_GATT_CHAR_PROP_READ, VM_GATT_PERM_READ, 0},
    {TYPE_END, 0, 0, 0, 0, 0}
};
 
uint16_t Sensor_Service::getHandle(int32_t type)
{
    if (0 == type)
    {
        return _handle_notify;
    }
    else if (1 == type)
    {
        return _handle_write;
    }
    return 0;
}
 
// prepare the data for profile
LGATTServiceInfo *Sensor_Service::onLoadService(int32_t index)
{ 
    return g_sensor_decl;
}
 
// characteristic added
boolean Sensor_Service::onCharacteristicAdded(LGATTAttributeData &data)
{
    const VM_BT_UUID *uuid = &(data.uuid);
    APP_LOG("LGATTSUart::onCharacteristicAdded f[%d] uuid[12] = [0x%x] len[%d]", data.failed, uuid->uuid[12], uuid->len);
    if (!data.failed)
    {
        if (0x7E == uuid->uuid[12])
        {
            _handle_notify = data.handle;
        }
        else if (0x7F == uuid->uuid[12])
        {
            _handle_write = data.handle;
        }
    }
    return true;
}
 
// connected or disconnected
boolean Sensor_Service::onConnection(const LGATTAddress &addr, boolean connected)
{
    _connected = connected;
    APP_LOG("LGATTSUart::onConnection connected [%d], [%x:%x:%x:%x:%x:%x]", _connected, 
        addr.addr[5], addr.addr[4], addr.addr[3], addr.addr[2], addr.addr[1], addr.addr[0]);
 
    return true;
}
 
// read action comming from master
boolean Sensor_Service::onRead(LGATTReadRequest &data)
{
    APP_LOG("LGATTSUart::onRead _connected [%d]", _connected);
    if (_connected)
    {

        
        LGATTAttributeValue value = {0};
        const char *str="value";
        memcpy(value.value, str, strlen(str));
        value.len = strlen(str);
         
        APP_LOG("LGATTSUart::onRead onRead [%d][%s]", value.len, value.value);
         
        data.ackOK(value);
    }
    return true;
}
 
// write action comming from master
boolean Sensor_Service::onWrite(LGATTWriteRequest &data)
{
    APP_LOG("LGATTSUart::onWrite _connected [%d]", _connected);
     
    if (_connected)
    {
        // if need to rsp to central.
        if (data.need_rsp)
        {
            LGATTAttributeValue value;
            value.len = 0;
            data.ackOK();
        }
    }
    return true;
}
