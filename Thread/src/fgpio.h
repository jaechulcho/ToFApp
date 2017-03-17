/*
 * fgpio.h
 *
 *  Created on: 2017. 3. 15.
 *      Author: han
 */

#ifndef FGPIO_H_
#define FGPIO_H_

#include <string>
using namespace std;

class fgpio
{
public:
    void gpio_use_sel(int gpio_num);
    void gpio_in_out_sel(string sel, int gpio_num);
    int gpio_inmode_value(int gpio_num);
    void gpio_outmode_value(int gpio_num, int value);
    void gpio_unuse_pin(int gpio_num);
};

#endif /* FGPIO_H_ */
