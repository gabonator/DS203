#pragma once

//#include <stdbool.h>
//#include <stm32f10x.h>

// Use this macro to declare an IO pin for a particular purpose.
// For example DECLARE_GPIO(led1, GPIOC, 3) creates these functions:
//
// static GPIO_TypeDef* gpio_led1_port = GPIOC;
// static int gpio_led1_pin = 3;
// void gpio_led1_set(bool value);
// bool gpio_led1_get();
// void gpio_led1_mode(enum gpio_cnf mode);
//
#define DECLARE_GPIO(name, port, pin) \
static GPIO_TypeDef * const _GPIO_PASTE_NAME(gpio_, name, _port) = port; \
static void _GPIO_PASTE_NAME(gpio_, name, _set)(bool value) { \
    port->BSRR = (1 << (pin)) << (value ? 0 : 16); \
} \
static bool _GPIO_PASTE_NAME(gpio_, name, _get)() { \
    return (port->IDR >> (pin)) & 1; \
} 

/*
static int _GPIO_PASTE_NAME(gpio_, name, _pin) = pin; \

static void _GPIO_PASTE_NAME(gpio_, name, _mode)(enum gpio_cnf mode) { \
    volatile uint32_t *conf = ((pin) < 8) ? (&port->CRL) : (&port->CRH); \
    int shift = ((pin) & 7) * 4; \
    *conf = (*conf & ~(15 << shift)) | (mode << shift); \
}
*/

// GPIO mode configuration
enum gpio_cnf {
    // Choose one of the following
    GPIO_ANALOG_INPUT = 0,
    GPIO_HIGHZ_INPUT = 4,
    GPIO_PULL_INPUT = 8,
    GPIO_OUT_10 = 1,
    GPIO_OUT_2 = 2,
    GPIO_OUT_50 = 3,
    GPIO_AFOUT_10 = 9,
    GPIO_AFOUT_2 = 10,
    GPIO_AFOUT_50 = 11,
    
    // This can be combined with the output modes above
    GPIO_OPEN_DRAIN = 4,
};

// Used to form the function names
#define _GPIO_PASTE_NAME(x, y, z) _GPIO_PASTE_TOKENS(x, y, z)
#define _GPIO_PASTE_TOKENS(x, y, z) x ## y ## z
