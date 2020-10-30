#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_hidd_prf_api.h"
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "driver/gpio.h"
#include "hid_dev.h"

/* Littlevgl specific */
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"

/*********************
 *      DEFINES
 *********************/

#define LV_TICK_PERIOD_MS 1
#define PROJECT_TAG "BLE_HID"
#define HIDD_DEVICE_NAME "MY_CAMERA_TIMER"

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

static uint16_t hid_conn_id = 0;
static bool sec_conn = false;
static uint8_t hidd_service_uuid128[] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	//first uuid, 16bit, [12],[13] is the value
	0xfb,
	0x34,
	0x9b,
	0x5f,
	0x80,
	0x00,
	0x00,
	0x80,
	0x00,
	0x10,
	0x00,
	0x00,
	0x12,
	0x18,
	0x00,
	0x00,
};
static esp_ble_adv_data_t hidd_adv_data = {
	.set_scan_rsp = false,
	.include_name = true,
	.include_txpower = true,
	.min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
	.max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
	.appearance = 0x03c0,	//HID Generic,
	.manufacturer_len = 0,
	.p_manufacturer_data = NULL,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = sizeof(hidd_service_uuid128),
	.p_service_uuid = hidd_service_uuid128,
	.flag = 0x6,
};

static esp_ble_adv_params_t hidd_adv_params = {
	.adv_int_min = 0x20,
	.adv_int_max = 0x30,
	.adv_type = ADV_TYPE_IND,
	.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
	//.peer_addr            =
	//.peer_addr_type       =
	.channel_map = ADV_CHNL_ALL,
	.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
/**********************
 *  STATIC CALLBACKS
 **********************/
static void hidd_event_callback(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param)
{
	switch (event)
	{
	case ESP_HIDD_EVENT_REG_FINISH:
	{
		if (param->init_finish.state == ESP_HIDD_INIT_OK)
		{
			esp_ble_gap_set_device_name(HIDD_DEVICE_NAME);
			esp_ble_gap_config_adv_data(&hidd_adv_data);
		}
		break;
	}
	case ESP_BAT_EVENT_REG:
	{
		break;
	}
	case ESP_HIDD_EVENT_DEINIT_FINISH:
		break;
	case ESP_HIDD_EVENT_BLE_CONNECT:
	{
		ESP_LOGI(PROJECT_TAG, "ESP_HIDD_EVENT_BLE_CONNECT");
		hid_conn_id = param->connect.conn_id;
		break;
	}
	case ESP_HIDD_EVENT_BLE_DISCONNECT:
	{
		sec_conn = false;
		ESP_LOGI(PROJECT_TAG, "ESP_HIDD_EVENT_BLE_DISCONNECT");
		esp_ble_gap_start_advertising(&hidd_adv_params);
		lv_update_status_label("Device Disconnected!");
		break;
	}
	case ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT:
	{
		ESP_LOGI(PROJECT_TAG, "%s, ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT", __func__);
		ESP_LOG_BUFFER_HEX(PROJECT_TAG, param->vendor_write.data, param->vendor_write.length);
	}
	default:
		break;
	}
	return;
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	switch (event)
	{
	case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
		esp_ble_gap_start_advertising(&hidd_adv_params);
		lv_update_status_label("Start Advertising!");
		break;
	case ESP_GAP_BLE_SEC_REQ_EVT:
		for (int i = 0; i < ESP_BD_ADDR_LEN; i++)
		{
			ESP_LOGD(PROJECT_TAG, "%x:", param->ble_security.ble_req.bd_addr[i]);
		}
		esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
		break;
	case ESP_GAP_BLE_AUTH_CMPL_EVT:
		sec_conn = true;
		esp_bd_addr_t bd_addr;
		memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
		ESP_LOGI(PROJECT_TAG, "remote BD_ADDR: %08x%04x",
				 (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
				 (bd_addr[4] << 8) + bd_addr[5]);
		ESP_LOGI(PROJECT_TAG, "address type = %d", param->ble_security.auth_cmpl.addr_type);
		ESP_LOGI(PROJECT_TAG, "pair status = %s", param->ble_security.auth_cmpl.success ? "success" : "fail");

		lv_update_status_label("Device Connected!");
		if (!param->ble_security.auth_cmpl.success)
		{
			ESP_LOGE(PROJECT_TAG, "fail reason = 0x%x", param->ble_security.auth_cmpl.fail_reason);
			lv_update_status_label("Auth Failed!");
		}
		break;
	default:
		break;
	}
}

/**********************
 *   APPLICATION MAIN
 **********************/

void ble_hid_init()
{
	esp_err_t ret;
	// Initialize NVS.
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	ret = esp_bt_controller_init(&bt_cfg);
	if (ret)
	{
		ESP_LOGE(PROJECT_TAG, "%s initialize controller failed\n", __func__);
		return;
	}

	ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	if (ret)
	{
		ESP_LOGE(PROJECT_TAG, "%s enable controller failed\n", __func__);
		return;
	}

	ret = esp_bluedroid_init();
	if (ret)
	{
		ESP_LOGE(PROJECT_TAG, "%s init bluedroid failed\n", __func__);
		return;
	}

	ret = esp_bluedroid_enable();
	if (ret)
	{
		ESP_LOGE(PROJECT_TAG, "%s init bluedroid failed\n", __func__);
		return;
	}

	if ((ret = esp_hidd_profile_init()) != ESP_OK)
	{
		ESP_LOGE(PROJECT_TAG, "%s init bluedroid failed\n", __func__);
	}

	lv_update_status_label("BLE INIT DONE!");
	//register the callback function to the gap module
	esp_ble_gap_register_callback(gap_event_handler);
	esp_hidd_register_callbacks(hidd_event_callback);

	/* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
	esp_ble_auth_req_t auth_req = ESP_LE_AUTH_BOND; //bonding with peer device after authentication
	esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;		//set the IO capability to No output No input
	uint8_t key_size = 16;							//the key size should be 7~16 bytes
	uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
	/* If your BLE device act as a Slave, the init_key means you hope which types of key of the master should distribute to you,
    and the response key means which key you can distribute to the Master;
    If your BLE device act as a master, the response key means you hope which types of key of the slave should distribute to you, 
    and the init key means which key you can distribute to the slave. */
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
}

void app_main()
{
	xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
	ble_hid_init();
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
	if(!sec_conn)
		return;

	if (userSetTime == 0)
	{
		esp_hidd_send_consumer_value(hid_conn_id, HID_CONSUMER_VOLUME_UP, isTriggered);
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

	if (value > 99)
	{
		esp_hidd_send_consumer_value(hid_conn_id, HID_CONSUMER_VOLUME_UP, true);
		vTaskDelay(10);
		esp_hidd_send_consumer_value(hid_conn_id, HID_CONSUMER_VOLUME_UP, false);
	}
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
	lv_obj_add_style(shot_btn, LV_BTN_PART_MAIN, &button_style);
	lv_obj_set_event_cb(shot_btn, button_event_handler);
	lv_obj_set_size(shot_btn, 144, 64);
	lv_obj_align(shot_btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -80);

	lv_obj_t *shot_btn_label = lv_label_create(shot_btn, NULL);
	lv_obj_add_style(shot_btn_label, LV_LABEL_PART_MAIN, &button_label_style);
	lv_label_set_text(shot_btn_label, "Shot");

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