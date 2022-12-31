#include "../include/lcd_lvgl.h"
#include "../include/sht4x.h"
#include "../include/vl53l0x.h"
#include <TFT_eSPI.h>
#include "lvgl.h"

#define BTN_PIN 27

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_HEIGHT * 10];

int last_btn_state = HIGH;

TFT_eSPI tft = TFT_eSPI();
lv_obj_t *tof_data_chart;
lv_chart_series_t *tof_data_series;
lv_obj_t *tof_measuring_sw;
lv_coord_t y_min_value = -1, y_max_value = -1;
lv_obj_t *y_next_value_label;

sensors_event_t sht4x_temp, sht4x_humidity;
lv_obj_t *temp_label;
lv_obj_t *humd_label;
lv_obj_t *btn_label;

lv_timer_t *btn_state_timer;
lv_timer_t *sht4x_update_timer;
lv_timer_t *vl53l0x_update_timer;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

static void temperature_update(lv_timer_t *timer);
static void tof_update(lv_timer_t *timer);
static void btn_pressed_check(lv_timer_t *timer);

static void draw_event_cb(lv_event_t *e);
lv_coord_t find_min_in_array(lv_coord_t array[]);
lv_coord_t find_max_in_array(lv_coord_t array[]);

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void lvgl_user_setup() {
    lv_init();
    tft.begin();
    tft.setRotation(1);
	
	lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_HEIGHT * 10);

	/*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = TFT_HEIGHT;
    disp_drv.ver_res = TFT_WIDTH;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
}

void lcd_basic_layout() {
    tof_data_chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(tof_data_chart, 260, 140);
    lv_obj_align(tof_data_chart, LV_ALIGN_CENTER, 20, 20);
    lv_chart_set_type(tof_data_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_update_mode(tof_data_chart, LV_CHART_UPDATE_MODE_SHIFT);

    tof_data_series = lv_chart_add_series(tof_data_chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(tof_data_chart, 10);
    
    lv_chart_set_axis_tick(tof_data_chart, LV_CHART_AXIS_PRIMARY_X, 8, 4, 10, 1, true, 30);
    lv_chart_set_axis_tick(tof_data_chart, LV_CHART_AXIS_PRIMARY_Y, 8, 4, 3, 5, true, 60);
    lv_chart_set_range(tof_data_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 2000);
    
    lv_obj_add_event_cb(tof_data_chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_chart_refresh(tof_data_chart);

    /* Labels */
    lv_obj_t *chart_title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(chart_title_label, "TOF Distance(mm)");
    lv_obj_align_to(chart_title_label, tof_data_chart, LV_ALIGN_OUT_TOP_RIGHT, 0, 0);

    temp_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_label, "Temp: 0 °C");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN);
    lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, 10, 10);

    humd_label = lv_label_create(lv_scr_act());
    lv_label_set_text(humd_label, "Humidity: 0% rH");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN);
    lv_obj_align(humd_label, LV_ALIGN_TOP_LEFT, 10, 30);

    btn_label = lv_label_create(lv_scr_act());
    lv_label_set_text(btn_label, "TOF measuring");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN);
    lv_obj_align(btn_label, LV_ALIGN_CENTER, 50, -100);

    y_next_value_label = lv_label_create(lv_scr_act());
    lv_label_set_text(y_next_value_label, "0");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN);
    lv_obj_align_to(y_next_value_label, tof_data_chart, LV_ALIGN_OUT_RIGHT_MID, -40, 0);

    /* Switch */
    tof_measuring_sw = lv_switch_create(lv_scr_act());
    lv_obj_set_size(tof_measuring_sw, 40, 20);
    lv_obj_align(tof_measuring_sw, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_add_state(tof_measuring_sw, LV_STATE_CHECKED);

    /* Timers */
    sht4x_update_timer = lv_timer_create(temperature_update, 1000, NULL); /* Always working */

    vl53l0x_update_timer = lv_timer_create(tof_update, 250, NULL); /* Working when switch is on */

    btn_state_timer = lv_timer_create(btn_pressed_check, 30, NULL);
}

static void temperature_update(lv_timer_t *timer) {
    sht40_get_measured(&sht4x_humidity, &sht4x_temp);
    lv_label_set_text_fmt(temp_label, "Temp: %.1f °C", sht4x_temp.temperature);
    lv_label_set_text_fmt(humd_label, "Humidity: %.1f% rH", sht4x_humidity.relative_humidity);
}

static void tof_update(lv_timer_t *timer) {
    float tof_new_data = vl53l0x_get_measured();

    lv_chart_set_next_value(tof_data_chart, tof_data_series, (int)tof_new_data);
    lv_chart_refresh(tof_data_chart);

    lv_label_set_text_fmt(y_next_value_label, "%.1f", tof_new_data);
    if (tof_new_data > 1000) {
        lv_obj_align_to(y_next_value_label, tof_data_chart, LV_ALIGN_OUT_RIGHT_MID, -40, 10);
    }
    else {
        lv_obj_align_to(y_next_value_label, tof_data_chart, LV_ALIGN_OUT_RIGHT_MID, -40, -10);
    }
}

static void btn_pressed_check(lv_timer_t *timer) {
    int reading = digitalRead(BTN_PIN);

    if (reading != last_btn_state) {
        if (reading == LOW) {
            if (lv_obj_has_state(tof_measuring_sw, LV_STATE_CHECKED)) {
                lv_obj_clear_state(tof_measuring_sw, LV_STATE_CHECKED);
                vl53l0x_stop();
                digitalWrite(LED_BUILTIN, LOW);
                lv_timer_pause(vl53l0x_update_timer);
                lv_label_set_text(btn_label, "TOF idle");
            }
            else {
                lv_obj_add_state(tof_measuring_sw, LV_STATE_CHECKED);
                vl53l0x_start();
                digitalWrite(LED_BUILTIN, HIGH);
                lv_timer_resume(vl53l0x_update_timer);
                lv_label_set_text(btn_label, "TOF measuring");
            } 
        }
    }
    last_btn_state = reading;
}

static void draw_event_cb(lv_event_t *e) {
    const char *x_index[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);

    if (!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL))
        return;

    if (dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%s", x_index[dsc->value]);
    }
}

lv_coord_t find_min_in_array(lv_coord_t array[]) {
    lv_coord_t val = array[0];

    for (int i = 0; i < 10; i++) {
        if (val < array[i])
            val = array[i];
    }

    return val;
}

lv_coord_t find_max_in_array(lv_coord_t array[]) {
    lv_coord_t val = array[0];

    for (int i = 0; i < 10; i++) {
        if (val > array[i])
            val = array[i];
    }

    return val;
}