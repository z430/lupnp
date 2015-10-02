//
// Created by myth on 8/18/15.
//

#include "HardwareHandler.h"
#include "UpnpCore.h"
#include <wiringPi.h>
#include <stdio.h>

const int pinRed = 6;
const int pinBlue = 4;
const int pinGreen = 5;

int redDump = 100;
int greenDump = 100;
int blueDump = 100;

int dimValue = 100;

void hardwareSetup() {

    wiringPiSetup();
    softPwmCreate(pinRed, 0, 100);
    softPwmCreate(pinGreen, 0, 100);
    softPwmCreate(pinBlue, 0, 100);

    softPwmWrite(pinRed, 0);
    softPwmWrite(pinGreen, 0);
    softPwmWrite(pinBlue, 0);
}

long mapRGB(int value, long in_min, long in_max, long out_min, long out_max){
	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void switchControl(bool setStatus) {

    switchStatus = setStatus;
    printf("Status Lampu: %s\n", switchStatus ? "on" : "off");
    if (setStatus) {
        softPwmWrite(pinRed, redDump);
        softPwmWrite(pinGreen, greenDump);
        softPwmWrite(pinBlue, blueDump);
    }
    else {
        softPwmWrite(pinRed, 0);
        softPwmWrite(pinGreen, 0);
        softPwmWrite(pinBlue, 0);
    }
}

void dimmingControl(int _dimValue) {

    dimValue = _dimValue;
    printf("dimmValue: %d\n", _dimValue);

    if(switchStatus){
        dimColorChange();
    }
    else
        printf("Turn on Lamp First\n");

}

void colorControl(int redLevel, int greenLevel, int blueLevel) {

    printf("red: %d, green: %d, blue: %d\n", redLevel, greenLevel, blueLevel);
    redDump = redLevel;
    greenDump = greenLevel;
    blueDump = blueLevel;

    if(switchStatus) {
      dimColorChange();
    }
    else
        printf("Turn on Lamp First\n");
}

void dimColorChange(){
  int dimm = dimValue;
  int redDimm = mapRGB(redDump, 0, 100, 0, dimm);
  int greenDimm = mapRGB(greenDump, 0, 100, 0, dimm);
  int blueDimm = mapRGB(blueDump, 0, 100, 0, dimm);

  softPwmWrite(pinRed, redDimm);
  softPwmWrite(pinGreen, greenDimm);
  softPwmWrite(pinBlue, blueDimm);

  printf("redDimm: %d, greenDimm: %d, blueDimm: %d\n", redDimm, greenDimm, blueDimm);

}
