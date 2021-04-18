#ifndef CIRCUIT_CUBE_H_INCLUDED
#define CIRCUIT_CUBE_H_INCLUDED

#include "BLEDevice.h"

class CircuitCube
{
public:
    explicit CircuitCube (std::string address = "");
    virtual ~CircuitCube (){}

    void Init ();

    void SetVelocities (int vel1, int vel2, int vel3);
    void SetVelocity (int channel, int vel);

protected:
    std::string BuildVelocityCommand (int channel, int velocity);

    BLEClient* m_Client;
    BLERemoteCharacteristic* m_TXChar;
    BLERemoteCharacteristic* m_RXChar;
    std::string m_Address;
};


#endif // CIRCUIT_CUBE_H_INCLUDED