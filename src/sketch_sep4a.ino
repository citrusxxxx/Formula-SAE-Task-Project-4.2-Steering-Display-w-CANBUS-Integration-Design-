#include "../lib/lvgl/lvgl.h"
#include "../lib/TFT_eSPI/TFT_eSPI.h"
#include "ui/ui.h"
#include "ui/vars.h"
#include "driver/twai.h"
#include "driver/gpio.h"


#define LANDSCAPE_MODE 1

#define ESP32_STANDARD_BAUD_RATE 115200

#define ADC_PIN 34

#define DIVIDER_RATIO (5.6f / (5.6f + 2.2f)) // 2.2k / 5.6k (bottom / (top + bottom))


// Riverdi RVT35HITNWC00-B High Brightness TFT-LCD Display
// Swapped HxV to VxH for hardware rotation to landscape mode via TFT_eSPI1 
#define HORIZONTAL_RES 480
#define VERTICAL_RES   320


// Buffer Declaration and Definition
#define BUFFER_LINES 40 
static lv_color_t draw_buffer1[HORIZONTAL_RES * BUFFER_LINES];
static lv_color_t draw_buffer2[HORIZONTAL_RES * BUFFER_LINES];


/*// Texas Instrument TCAN332
#define CAN_TX 1 // Transmit
#define CAN_RX 4 // Receive*/


// GLOBAL
TFT_eSPI tft = TFT_eSPI();


// Blocks I/O until finished, but shouldn't really matter
// in this case.
void display_flush(
    lv_display_t*     display,
    const lv_area_t*  area,
    uint8_t*          pixel_map
)
{
    uint32_t width  = lv_area_get_width(area);
    uint32_t height = lv_area_get_height(area);

    tft.startWrite();

    tft.setAddrWindow(
        area->x1,
        area->y1,
        width,
        height
    );

    tft.pushPixels(
        (uint16_t*)pixel_map,
        width * height
    );

    tft.endWrite();
    
    lv_display_flush_ready(display);
    return;
}


static uint32_t tick(void)
{
    return millis();
}


void do_fucking_everything_except_brake_bias(twai_message_t message)
{
    if (message.rtr || message.extd)
        return;

    uint32_t message_id = message.identifier;

    switch (message_id)
    {
    case 0x118:
    {
        uint8_t index = 0;

        index = message.data[0];
        set_var_rpm(index);

        index = message.data[2];
        set_var_mph(index);

        index = message.data[3];
        set_var_coolant_temp(index);

        break;
    }

    case 0x64D:
    {
        uint8_t index = 0;

        index = message.data[6];
        index = index & 0x0F;
        set_var_gear(index);

        break;
    }


    default:
        return; // Not a CAN BUS message that I care about
    }
}

void do_fucking_everything_related_to_brake_bias()
{
    uint32_t raw_data  = analogRead(ADC_PIN);          //0-4095 at 12 bits
    float    volts_adc = (raw_data / 4095.0f) * 3.3f;  // makes the bits back into voltage when it reached adc

    float volts_sensor = volts_adc / DIVIDER_RATIO;    // turns it back to original voltage before the resistor divider

    // pretend i parse it here

    int  written = 0;                  // because snprintf returns int and is not designed for embedded.
    char brake_bias_front_string[10];
    char brake_bias_rear_string [10];

    written = snprintf(
        brake_bias_front_string,
        10,
        "%.1f%%",
        volts_sensor // pretend this is parsed data
    );

    if (!(written >= 10))
    {
        set_var_brake_bias_front(brake_bias_front_string);
    }
    else
    {
        Serial.printf("Truncation happened while attempting to write to brake bias front's buffer. Check buffer size\n");
        fflush(stdout);
    }

    written = snprintf(
        brake_bias_rear_string,
        10,
        "%.1f%%",
        volts_sensor //again, pretend this is parsed data
    );

    if (!(written >= 10))
    {
        set_var_brake_bias_rear(brake_bias_rear_string);
    }
    else
    {
        Serial.printf("Truncation happened while attempting to write to brake bias front's buffer. Check buffer size\n");
        fflush(stdout);
    } 

    return;
}

void hang()
{
    Serial.printf("Press ENTER to continue: ");
    Serial.flush();

    while (Serial.available())
        (void)Serial.read();

    while (true)
    {
        if (Serial.available())
        {
            int lol = Serial.read();

            if (lol == '\n'
            || lol == '\r')
                break;
        }

        delay(1);
    }
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(ESP32_STANDARD_BAUD_RATE);

    analogReadResolution   (12);
    analogSetPinAttenuation(ADC_PIN, ADC_11db);

    static const twai_general_config_t general_config = TWAI_GENERAL_CONFIG_DEFAULT(
        GPIO_NUM_27,
        GPIO_NUM_35,
        TWAI_MODE_LISTEN_ONLY);

    static const twai_timing_config_t timing_config = TWAI_TIMING_CONFIG_1MBITS    ();
    static const twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(
            &general_config,
            &timing_config,
            &filter_config) == ESP_OK)
    {
        printf("Driver install OK\n");
    }

    else
    {
		printf("Driver install error\n");
        hang();
        exit(-1);
    }

    ESP_ERROR_CHECK(twai_start());

    tft.init();
    tft.setRotation (LANDSCAPE_MODE);
    tft.setSwapBytes(true); // MCU is little-endian; Display controller expects big-endian
    tft.fillScreen  (TFT_BLACK);

    lv_init();
    lv_tick_set_cb(tick);

    lv_display_t* display = lv_display_create(HORIZONTAL_RES, VERTICAL_RES);
    lv_display_set_flush_cb(display, display_flush);
    lv_display_set_buffers(
        display, 
        draw_buffer1, 
        draw_buffer2, 
        sizeof(draw_buffer1), 
        LV_DISPLAY_RENDER_MODE_PARTIAL);
    

    ui_init();

}

void loop()
{
    // put your main code here, to run repeatedly:

    twai_message_t message;

    if (twai_receive(&message, pdMS_TO_TICKS(10)) == ESP_OK)
    {
        printf("Received a message\n");
        do_fucking_everything_except_brake_bias(message);
    }
    else
    {
        printf("No messages recieved\n");
    }

    do_fucking_everything_related_to_brake_bias();
    lv_timer_handler();
    ui_tick         ();
    delay           (1);
}
