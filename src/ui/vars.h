#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_RPM = 0,
    FLOW_GLOBAL_VARIABLE_GEAR = 1,
    FLOW_GLOBAL_VARIABLE_COOLANT_TEMP = 2,
    FLOW_GLOBAL_VARIABLE_MPH = 3,
    FLOW_GLOBAL_VARIABLE_BRAKE_BIAS_FRONT = 4,
    FLOW_GLOBAL_VARIABLE_BRAKE_BIAS_REAR = 5
};

// Native global variables

extern uint8_t     get_var_rpm             ();
extern void        set_var_rpm             (uint8_t value);
extern int8_t      get_var_gear            ();
extern void        set_var_gear            (int8_t value);
extern uint8_t     get_var_coolant_temp    ();
extern void        set_var_coolant_temp    (uint8_t value);
extern uint8_t     get_var_mph             ();
extern void        set_var_mph             (uint8_t value);
extern const char* get_var_brake_bias_front();
extern void        set_var_brake_bias_front(const char *value);
extern const char* get_var_brake_bias_rear ();
extern void        set_var_brake_bias_rear (const char *value);


#ifdef __cplusplus
}
#endif

#endif 