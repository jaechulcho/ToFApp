//============================================================================
// Name        : gpio.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "fgpio.h"

using namespace std;
#define SYS_GPIO_DIR "/sys/class/gpio"

void fgpio::gpio_use_sel(int gpio_num){
    char path[256];
    sprintf(path, "%s/export", SYS_GPIO_DIR);
    std::ofstream ofs (path);
    ofs << gpio_num;
    ofs.close();
}

void fgpio::gpio_in_out_sel(string sel, int gpio_num){
    char path[256];
    sprintf(path, "%s/gpio%d/direction", SYS_GPIO_DIR, gpio_num);
    std::ofstream set_pin_type (path);
    set_pin_type << sel;
    set_pin_type.close();
}

int fgpio::gpio_inmode_value(int gpio_num){
    char path[256];
    int value;
    sprintf(path, "%s/gpio%d/value", SYS_GPIO_DIR, gpio_num);
    std::ifstream get_gpio_pin (path);
    get_gpio_pin >> value;
    get_gpio_pin.close();

    return value;
}

void fgpio::gpio_outmode_value(int gpio_num, int value){
    char path[256];
    sprintf(path, "%s/gpio%d/value", SYS_GPIO_DIR, gpio_num);
    std::ofstream get_gpio_pin (path);
    get_gpio_pin << value;
    get_gpio_pin.close();
}

void fgpio::gpio_unuse_pin(int gpio_num){
    char path[256];
    sprintf(path, "%s/unexport", SYS_GPIO_DIR);
    std::ofstream unusePin (path);
    unusePin << gpio_num;
    unusePin.close();
}
