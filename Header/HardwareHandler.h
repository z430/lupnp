//
// Created by myth on 8/18/15.
//

#ifndef LUPNP_HARDWAREHANDLER_H
#define LUPNP_HARDWAREHANDLER_H

#include <wiringPi.h>
#include <stdbool.h>

bool switchStatus;
int dhtValue;
int pirValue;

void readSensor();
void hardwareSetup();
void switchControl(bool setStatus);
void dimmingControl(int dimValue);
void colorControl(int redLevel, int greenLevel, int blueLevel);
void dimColorChange();
long mapRGB(int value, long in_min, long in_max, long out_min, long out_max);

#endif //LUPNP_HARDWAREHANDLER_H
