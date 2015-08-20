//
// Created by myth on 8/18/15.
//

#include "HardwareHandler.h"
#include "UpnpCore.h"
#include <wiringPi.h>
#include <stdio.h>

const int pinRed = 0;
const int pinBlue = 1;
const int pinGree = 2;

int setupWiringPi() {
    wiringPiSetup();
    wiringPiSetupGpio();

    if(wiringPiSetup() == -1 || wiringPiSetupGpio() == -1){
        printf("setup wiringpi failed\n");
        return -1;
    }
}

void switchControl(bool setStatus) {

    hardwareSetup();
    switchStatus = setStatus;
    printf("Status Lampu: %s\n", switchStatus ? "on" : "off");
}

void dimmingControl(int dimValue) {

    hardwareSetup();
    if(switchStatus)
        printf("Dimming: %d.\n", dimValue);
    else
        printf("Turn on Lamp First\n");
}

void colorControl(int redLevel, int greenLevel, int blueLevel) {

    hardwareSetup();
    if(switchStatus)
        printf("%d %d %d\n", redLevel, greenLevel, blueLevel);
    else
        printf("Turn on Lamp First\n");
}

void hardwareSetup() {
    if(setupWiringPi() != -1){
        pinMode(pinRed, OUTPUT);
        pinMode(pinBlue, OUTPUT);
        pinMode(pinGree, OUTPUT);
        printf("Hardware Configured\n");
    }
}
