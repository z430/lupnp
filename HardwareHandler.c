//
// Created by myth on 8/18/15.
//

#include "HardwareHandler.h"
#include "UpnpCore.h"
#include <wiringPi.h>
#include <stdio.h>

int wiringPiSetup()
{

}

void switchControl(bool setStatus) {

    switchStatus = setStatus;
    printf("Status Lampu: %s\n", switchStatus ? "on" : "off");
}

void dimmingControl(int dimValue) {

    if(switchStatus)
        printf("Dimming: %d.\n", dimValue);
    else
        printf("Turn on Lamp First\n");
}

void colorControl(int redLevel, int greenLevel, int blueLevel) {

    if(switchStatus)
        printf("%d %d %d\n", redLevel, greenLevel, blueLevel);
    else
        printf("Turn on Lamp First\n");
}
