#include "Arduino.h"
#include "LEGORemote.h"
using namespace std::placeholders;

const std::string LEGO_REMOTE_SERVICE_UUID = "00001623-1212-efde-1623-785feabcd123";
const std::string LEGO_REMOTE_CHARACTERISTICS_UUID = "00001624-1212-efde-1623-785feabcd123";


LEGORemote::LEGORemote ()
{}

void LEGORemote::Init ()
{
    BLEScan *pScan = BLEDevice::getScan();
    BLEScanResults results = pScan->start(2);
    
    BLEUUID LEGOServiceUuid (LEGO_REMOTE_SERVICE_UUID);
    
    for(int i = 0; i < results.getCount(); i++) 
    {
        BLEAdvertisedDevice device = results.getDevice(i);
        log_i("found device: %s", device.toString().c_str() );
        if (device.isAdvertisingService(LEGOServiceUuid)) 
        {
            m_Client = BLEDevice::createClient();
            log_i ("found LEGO Remote %s", device.getName().c_str() );
            if (m_Client->connect(&device)) 
            {
                BLERemoteService* pService = m_Client->getService (LEGOServiceUuid);
                if (pService)
                {
                    m_Char = pService->getCharacteristic (LEGO_REMOTE_CHARACTERISTICS_UUID);
                    if (m_Char && m_Char->canNotify())
                    {
                        m_Char->registerForNotify (std::bind (&LEGORemote::Callback, this, _1, _2, _3, _4));
                        byte activatePortDeviceMessage[8] = {0x41, LEGORemote::LEFT, 0x0, 0x01, 0x00, 0x00, 0x00, 0x01};
                        WriteValue (activatePortDeviceMessage, 8);
                        activatePortDeviceMessage[1] = LEGORemote::RIGHT;
                        WriteValue (activatePortDeviceMessage, 8);
                    }
                }
            }
        }
    }

}

void LEGORemote::SetButtonCallback (std::function<void(int, int)> callback)
{
    m_Callback = callback;
}

void LEGORemote::Callback (BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
    log_d("LEGORemote::callback (%s, , %d, %d)", pBLERemoteCharacteristic->toString().c_str(), length, isNotify ? 1 :0);
    for (size_t i=0; i<length; i++)
    {
        log_d("   0x%00X", pData[i]);
    }
    if (pData[2] != PORT_VALUE_SINGLE)
    {
        return;
    }
    int button = pData[3];
    int state = pData[4];
    if (m_Callback)
    {
        m_Callback(button, state);
    }

}
void LEGORemote::WriteValue (byte command[], int size)
{
    if (m_Char)
    {   
        byte cmd[size + 2] = {(byte)(size + 2), 0x00};
        memcpy (cmd + 2, command, size);
        m_Char->writeValue (cmd, sizeof(cmd), false);
    }
}

