#include "Arduino.h"
#include "CircuitCube.h"
#include <sstream>
#include <iomanip>

const std::string CIRCUIT_CUBE_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
const std::string CIRCUIT_CUBE_TX_CHRACTERISITCS_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
const std::string CIRCUIT_CUBE_RX_CHRACTERISITCS_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

CircuitCube::CircuitCube (std::string address /*=""*/)
    : m_Client {nullptr }  
    , m_TXChar { nullptr } 
    , m_RXChar { nullptr } 
    , m_Address (address)
    {}

void CircuitCube::Init ()
{
    BLEScan *pScan = BLEDevice::getScan();
    BLEScanResults results = pScan->start(2);
    
    BLEUUID circuitCubeServiceUuid (CIRCUIT_CUBE_SERVICE_UUID);
    
    for(int i = 0; i < results.getCount(); i++) 
    {
        BLEAdvertisedDevice device = results.getDevice(i);
        log_i("found device: %s", device.toString().c_str() );
        if ( (m_Address.empty() || (device.getAddress() == BLEAddress(m_Address)))
             && (device.isAdvertisingService (circuitCubeServiceUuid))) 
        {
            m_Client = BLEDevice::createClient();
            log_i ("found circuit cube %s", device.getName().c_str() );
            if (m_Client->connect(&device)) 
            {
                BLERemoteService* pService = m_Client->getService (circuitCubeServiceUuid);
                if (pService)
                {
                    m_TXChar = pService->getCharacteristic (CIRCUIT_CUBE_TX_CHRACTERISITCS_UUID);
                    m_RXChar = pService->getCharacteristic (CIRCUIT_CUBE_RX_CHRACTERISITCS_UUID);
                    
                    if (m_TXChar && m_RXChar) 
                    {
                        // read battery state
                        m_TXChar->writeValue("b");
                        std::string value = m_RXChar->readValue();
                        log_i("read %s", value.c_str());
                    }
                }
            }
        }
    }

}

void CircuitCube::SetVelocities (int vel1, int vel2, int vel3)
{
    if (m_TXChar)
    {
        m_TXChar->writeValue (
              BuildVelocityCommand (0, vel1)
            + BuildVelocityCommand (1, vel2)
            + BuildVelocityCommand (2, vel3) );
    }

}

void CircuitCube::SetVelocity (int channel, int vel)
{
    if (m_TXChar)
    {
        m_TXChar->writeValue (BuildVelocityCommand (channel, vel));
    }
}

std::string CircuitCube::BuildVelocityCommand (int channel, int velocity)
{
    std::stringstream cmd;
    cmd << ((velocity < 0) ? "-" : "+")
        << std::setfill('0') << std::setw(3) << ((velocity==0) ? 0 : (55 + abs(velocity)))
        << char('a' + channel);
    return cmd.str ();
}
