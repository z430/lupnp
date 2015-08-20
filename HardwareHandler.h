//
// Created by myth on 8/18/15.
//

#ifndef LUPNP_HARDWAREHANDLER_H
#define LUPNP_HARDWAREHANDLER_H

#include <wiringPi.h>
#include <stdbool.h>

bool switchStatus;

int setupWiringPi();
void hardwareSetup();
void switchControl(bool setStatus);
void dimmingControl(int dimValue);
void colorControl(int redLevel, int greenLevel, int blueLevel);

#endif //LUPNP_HARDWAREHANDLER_H
