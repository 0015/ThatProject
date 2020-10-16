#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

/* Littlevgl specific */
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"

/*********************
 *      DEFINES
 *********************/

#define LV_TICK_PERIOD_MS 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void lv_layout(void);
static void lv_update_status_label(const char *status_msg);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *lmeter_label;
static lv_obj_t *lmeter;
static lv_obj_t *status_label;
static lv_anim_t a;
static int userSetTime;

/**********************
 *   APPLICATION MAIN
 **********************/
void app_main()
{
	xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
}

SemaphoreHandle_t xGuiSemaphore;

static void guiTask(void *pvParameter)
{

	(void)pvParameter;
	xGuiSemaphore = xSemaphoreCreateMutex();

	lv_init();

	/* Initialize SPI or I2C bus used by the drivers */
	lvgl_driver_init();

	static lv_color_t buf1[DISP_BUF_SIZE];
	static lv_color_t *buf2 = NULL;
	static lv_disp_buf_t disp_buf;
	uint32_t size_in_px = DISP_BUF_SIZE;
	lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = disp_driver_flush;
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);

	/* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.read_cb = touch_driver_read;
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	lv_indev_drv_register(&indev_drv);
#endif

	/* Create and start a periodic timer interrupt to call lv_tick_inc */
	const esp_timer_create_args_t periodic_timer_args = {
		.callback = &lv_tick_task,
		.name = "periodic_gui"};
	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

	lv_layout();

	while (1)
	{
		
		vTaskDelay(pdMS_TO_TICKS(10));

		if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
		{
			lv_task_handler();
			xSemaphoreGive(xGuiSemaphore);
		}
	}

	vTaskDelete(NULL);
}

static void lv_tick_task(void *arg)
{
	(void)arg;
	lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void startTimer(int second)
{
	lv_anim_set_time(&a, second);
	lv_anim_start(&a);
}

static void shotTrigger(bool isTriggered)
{
	if (userSetTime == 0)
	{
		//Todo: Triggering immediately
	}
	else
	{
		startTimer(userSetTime);
	}
}

static void button_event_handler(lv_obj_t *obj, lv_event_t event)
{
	switch (event)
	{
	case LV_EVENT_PRESSED:
		printf("Pressed\n");
		shotTrigger(true);
		break;

	case LV_EVENT_RELEASED:
		printf("Released\n");
		shotTrigger(false);
		break;
	}
}

static void ddlist_event_handler(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED)
	{
		uint16_t itemIdx = lv_dropdown_get_selected(obj);
		printf("itemIdx: %d\n", itemIdx);

		switch (itemIdx)
		{
		case 0:
			userSetTime = 0;
			break;

		case 1:
			userSetTime = 3000;
			break;

		case 2:
			userSetTime = 5000;
			break;

		case 3:
			userSetTime = 10000;
			break;

		case 4:
			userSetTime = 20000;
			break;

		default:
			userSetTime = 0;
			break;
		}

		char buf[6];
		lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
		lv_label_set_text(lmeter_label, buf);
		lv_obj_align(lmeter_label, lmeter, LV_ALIGN_CENTER, 0, 0);
	}
}
static void linemeter_anim(lv_obj_t *linemeter, lv_anim_value_t value)
{
	lv_linemeter_set_value(linemeter, value);
	float timeValue = userSetTime / 100 * value;
	float remainingSec = (userSetTime - timeValue) / 1000;
	static char buf[64];
	lv_snprintf(buf, sizeof(buf), "%.2f sec", remainingSec);
	lv_obj_t *label = lv_obj_get_child(linemeter, NULL);
	lv_label_set_text(label, buf);
	lv_obj_align(label, linemeter, LV_ALIGN_CENTER, 0, 0);
}

static void lv_layout(void)
{
	lv_obj_t *scr = lv_obj_create(NULL, NULL);
	lv_scr_load(scr);

	lv_obj_t *bg_top = lv_obj_create(scr, NULL);
	lv_obj_clean_style_list(bg_top, LV_OBJ_PART_MAIN);
	lv_obj_set_size(bg_top, LV_HOR_RES, 40);

	status_label = lv_label_create(bg_top, NULL);
	lv_label_set_long_mode(status_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_obj_set_width(status_label, LV_HOR_RES / 2);
	lv_label_set_text(status_label, "Status Label");
	lv_obj_align(status_label, NULL, LV_ALIGN_IN_LEFT_MID, 20, 0);

	lv_obj_t *ddlist = lv_dropdown_create(bg_top, NULL);
	lv_obj_clean_style_list(ddlist, LV_OBJ_PART_MAIN);
	lv_dropdown_set_options(ddlist, "None\n"
									"3s\n"
									"5s\n"
									"10s\n"
									"20s");

	lv_obj_align(ddlist, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 8);
	lv_obj_set_event_cb(ddlist, ddlist_event_handler);

	static lv_style_t button_style;
	lv_style_init(&button_style);
	lv_style_set_radius(&button_style, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&button_style, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_border_color(&button_style, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_bg_color(&button_style, LV_STATE_PRESSED, LV_COLOR_RED);
	lv_style_set_border_color(&button_style, LV_STATE_PRESSED, LV_COLOR_RED);

	static lv_style_t button_label_style;
	lv_style_init(&button_label_style);
	lv_style_set_text_opa(&button_label_style, LV_STATE_DEFAULT, LV_OPA_50);
	lv_style_set_text_color(&button_label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);

	lv_obj_t *shot_btn = lv_btn_create(scr, NULL);
	lv_obj_set_event_cb(shot_btn, button_event_handler);
	lv_obj_set_size(shot_btn, 144, 64);
	lv_obj_align(shot_btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -80);

	lv_obj_t *shot_btn_label = lv_label_create(shot_btn, NULL);
	lv_obj_add_style(shot_btn_label, LV_LABEL_PART_MAIN, &button_label_style);
	lv_label_set_text(shot_btn_label, "Shot");
	lv_obj_add_style(shot_btn, LV_BTN_PART_MAIN, &button_style);

	lmeter = lv_linemeter_create(scr, NULL);
	lv_obj_set_drag_parent(lmeter, true);
	lv_linemeter_set_value(lmeter, 0);
	lv_obj_set_size(lmeter, 160, 160);
	lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, -40);

	lmeter_label = lv_label_create(lmeter, NULL);
	lv_label_set_text(lmeter_label, "Timer");
	lv_obj_align(lmeter_label, lmeter, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_local_text_font(lmeter_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());

	lv_anim_init(&a);
	lv_anim_set_var(&a, lmeter);
	lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)linemeter_anim);
	lv_anim_set_values(&a, 0, 100);
	lv_anim_set_playback_time(&a, 400);
	lv_anim_set_repeat_count(&a, 1);

	lv_update_status_label("LVGL Done!");
}

static void lv_update_status_label(const char *status_msg)
{
	if (status_label != NULL)
	{
		lv_label_set_text(status_label, status_msg);
	}
}