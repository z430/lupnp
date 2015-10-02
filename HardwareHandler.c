//
// Created by myth on 8/18/15.
//

#include "HardwareHandler.h"
#include "UpnpCore.h"
#include <wiringPi.h>
#include <stdio.h>

const int pinRed = 6;
const int pinBlue = 5;
const int pinGreen = 4;

int redDump, greenDump, blueDump;

void hardwareSetup() {
    wiringPiSetup();
    softPwmCreate(pinRed, 0, 100);
    softPwmCreate(pinGreen, 0, 100);
    softPwmCreate(pinBlue, 0, 100);

    softPwmWrite(pinRed, 0);
    softPwmWrite(pinGreen, 0);
    softPwmWrite(pinBlue, 0);
}

long mapRGB(int value, long in_min, long in_max, long out_min, long out_max)
{
	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void switchControl(bool setStatus) {

    switchStatus = setStatus;
    printf("Status Lampu: %s\n", switchStatus ? "on" : "off");
    if (setStatus) {
        softPwmWrite(pinRed, 100);
        softPwmWrite(pinGreen, 100);
        softPwmWrite(pinBlue, 100);
    }
    else {
        softPwmWrite(pinRed, 0);
        softPwmWrite(pinGreen, 0);
        softPwmWrite(pinBlue, 0);
    }
}

void dimmingControl(int dimValue) {

    int newRed, newGreen, newBlue;

    // if(switchStatus)
    //     printf("Dimming: %d.\n", dimValue);
    // else
    //     printf("Turn on Lamp First\n");

    newRed = mapRGB(redDump, 0, 100, 0, dimValue);
    newGreen = mapRGB(greenDump, 0, 100, 0, dimValue);
    newBlue = mapRGB(blueDump, 0, 100, 0, dimValue);

  	softPwmWrite(pinRed, newRed);
  	softPwmWrite(pinGreen, newGreen);
  	softPwmWrite(pinBlue, newBlue);
}

void colorControl(int redLevel, int greenLevel, int blueLevel) {

    redDump = redLevel;
    greenDump = greenLevel;
    blueDump = blueLevel;

    softPwmWrite(pinRed, redDump);
    softPwmWrite(pinGreen, greenDump);
    softPwmWrite(pinBlue, blueDump);

    // if(switchStatus)
    //     printf("%d %d %d\n", redLevel, greenLevel, blueLevel);
    // else
    //     printf("Turn on Lamp First\n");
}
