
#include "Arduino.h"
#include "CircuitCube.h"
#include "LEGORemote.h"
#include "BLEDevice.h"

#define TRAIN_CONTROL 

int vel[2] = {0,0};
int acc[2] = {0,0};
int stop = 0;

#ifdef TRAIN_CONTROL
CircuitCube myCircuitCube1 ("FC:58:FA:86:E9:38");
CircuitCube myCircuitCube2 ("FC:58:FA:86:EB:D6");
#else
CircuitCube myCircuitCube1;
#endif
LEGORemote myLEGORemote;

void ButtonCallbackTrain (int button, int state)
{
    int idx = (button==LEGORemote::LEFT) ? 0 : 1;
    switch(state)
    {
        case(LEGORemote::RELEASED):
            acc[idx] = 0;
            break;
        case(LEGORemote::UP):
            acc[idx] = +10;
            break;
        case(LEGORemote::DOWN):
            acc[idx] = -10;
            break;
        case(LEGORemote::STOP):
            acc[idx] = 0;
            vel[idx] = 0;
            break;
    }
}

void ButtonCallbackTank (int button, int state)
{
    int idx = (button==LEGORemote::LEFT) ? 0 : 1;
    switch(state)
    {
        case(LEGORemote::RELEASED):
            vel[idx] = 0;
            break;
        case(LEGORemote::UP):
            vel[idx] = 200;
            break;
        case(LEGORemote::DOWN):
            vel[idx] = -200;
            break;
        case(LEGORemote::STOP):
            stop = (idx==0) ? 200 : 0;
            break;
    }
}

void setup()
{
  Serial.begin(115200);
  BLEDevice::init ("");
  myLEGORemote.Init ();
  myCircuitCube1.Init ();
#ifdef TRAIN_CONTROL
  myCircuitCube2.Init ();
  myLEGORemote.SetButtonCallback (ButtonCallbackTrain);
#else
  myLEGORemote.SetButtonCallback (ButtonCallbackTank);
#endif
}

// main loop
void loop()
{
    delay(100);
#ifdef TRAIN_CONTROL
    for (int i=0; i<2; ++i) 
    {
        vel[i] += acc[i];
        if (vel[i] >  200) { vel[i] =  200; }
        if (vel[i] < -200) { vel[i] = -200; }
    }
    myCircuitCube1.SetVelocities (vel[0], vel[0], vel[0]);
    myCircuitCube2.SetVelocities (vel[1], vel[1], vel[1]);
#else
    myCircuitCube1.SetVelocities (vel[0], stop, vel[1]);
#endif
    log_i ("v1: %d, a1: %d  -  v2: %d, a2: %d", vel[0], acc[0], vel[1], acc[1]);
} // End of loop
