//
// Created by myth on 8/18/15.
//

#include "../Header/HardwareHandler.h"
#include "../Header/UpnpCore.h"
#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>

const int pinRed = 6;
const int pinBlue = 4;
const int pinGreen = 5;

int redDump;
int greenDump;
int blueDump;

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
        redDump = 100;
        greenDump = 100;
        blueDump = 100;

        softPwmWrite(pinRed, redDump);
        softPwmWrite(pinGreen, greenDump);
        softPwmWrite(pinBlue, blueDump);
        printf("%d %d %d\n", redDump, greenDump, blueDump);
    }
    else {
        softPwmWrite(pinRed, 0);
        softPwmWrite(pinGreen, 0);
        softPwmWrite(pinBlue, 0);
    }
}

void dimmingControl(int _dimValue) {
    if (dimValue > 100)
        _dimValue = 100;
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

  // if(!switchStatus){
  //   redDump = 100;
  //   greenDump = 100;
  //   blueDump = 100;
  // }

  int redDimm = mapRGB(redDump, 0, 100, 0, dimm);
  int greenDimm = mapRGB(greenDump, 0, 100, 0, dimm);
  int blueDimm = mapRGB(blueDump, 0, 100, 0, dimm);

  softPwmWrite(pinRed, redDimm);
  softPwmWrite(pinGreen, greenDimm);
  softPwmWrite(pinBlue, blueDimm);

  printf("redDimm: %d, greenDimm: %d, blueDimm: %d\n", redDimm, greenDimm, blueDimm);

}
