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
    if (setStatus) {
        redDump = 100;
        greenDump = 100;
        blueDump = 100;

        softPwmWrite(pinRed, redDump);
        softPwmWrite(pinGreen, greenDump);
        softPwmWrite(pinBlue, blueDump);

        notify_status_change(switchStatus);
//        set_all_status(switchStatus);
    }
    else {
        softPwmWrite(pinRed, 0);
        softPwmWrite(pinGreen, 0);
        softPwmWrite(pinBlue, 0);
    }
}

void dimmingControl(int _dimValue) {

    dimValue = mapRGB(_dimValue, 0, 255, 0, 100);
    if(switchStatus) {
        dimColorChange();
        notify_load_level_change(_dimValue);
//        set_all_load_level(_dimValue);
    }
    else
        printf("Turn on Lamp First\n");

}

void colorControl(int redLevel, int greenLevel, int blueLevel) {

    redDump = mapRGB(redLevel, 0, 255, 0, 100);
    greenDump = mapRGB(greenLevel, 0, 255, 0, 100);
    blueDump = mapRGB(blueLevel, 0, 255, 0, 100);

    if(switchStatus){
        dimColorChange();
        notify_color_change(redLevel, greenLevel, blueLevel);
//        set_all_color_status(redLevel, greenLevel, blueLevel);
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

  printf("Status %s. R: %d G: %d B: %d\n", switchStatus ? "on" : "off", redDimm, greenDimm, blueDimm);
  fflush(stdout);

}
