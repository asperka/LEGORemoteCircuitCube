#ifndef LEGO_REMOTE_H_INCLUDED
#define LEGO_REMOTE_H_INCLUDED

#include "BLEDevice.h"

class LEGORemote
{
public:
    LEGORemote ();
    virtual ~LEGORemote () {}

    void Init ();
    void SetButtonCallback (std::function<void(int, int)> callback);

    enum
    {
        LEFT = 0x00,
        RIGHT = 0x01,
        PORT_VALUE_SINGLE = 0x45,
        PRESSED = 0x01,
        RELEASED = 0x00,
        UP = 0x01,
        DOWN = 0xff,
        STOP = 0x7f
    };

protected:
    BLEClient* m_Client;
    BLERemoteCharacteristic* m_Char;
    std::function<void(int, int)> m_Callback;

    void Callback (BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    void WriteValue(byte command[], int size);

};


#endif // LEGO_REMOTE_H_INCLUDED