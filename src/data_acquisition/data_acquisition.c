#include "../ui/vars.h"
#include <stdint.h>


static uint8_t mph                = 0;
static uint8_t rpm                = 0;
static int8_t  gear               = 0;  // NOTE: CAN BUS message for gear is actually a signed 4 bit message
static uint8_t coolant_temp       = 0;
static char*   brake_bias_front   = 0;     
static char*   brake_bias_rear    = 0;


uint8_t get_var_mph()
{
    return mph;
}

void set_var_mph(uint8_t input_mph)
{
     mph = input_mph;
}


uint8_t get_var_rpm()
{
    return rpm;
}

void set_var_rpm(uint8_t input_rpm)
{
    rpm = input_rpm;
}


int8_t get_var_gear()
{
    return gear;
}

void set_var_gear(int8_t input_gear)
{
    gear = input_gear;
}


uint8_t get_var_coolant_temp()
{
    return coolant_temp;
}

void set_var_coolant_temp(uint8_t input_coolant_temp)
{
    coolant_temp = input_coolant_temp;
}


char* get_var_brake_bias_front()
{
    return brake_bias_front;
}

void set_var_brake_bias_front(char* input_brake_bias_front)
{
    brake_bias_front = input_brake_bias_front;
}


char* get_var_brake_bias_rear()
{
    return brake_bias_rear;
}

void set_var_brake_bias_rear(char* input_brake_bias_rear)
{
    brake_bias_rear = input_brake_bias_rear;
}