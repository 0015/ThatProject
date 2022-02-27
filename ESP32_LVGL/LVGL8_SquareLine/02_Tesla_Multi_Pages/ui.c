// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.2
// LVGL VERSION: 8.2
// PROJECT: Tesla_Multi_Pages

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
lv_obj_t * ui_BackgroundImageBTN;
lv_obj_t * ui_BOTTOM_PANEL;
lv_obj_t * ui_BTN_CUSTOM_ORDER;
lv_obj_t * ui_LABEL_CUSTOM_ORDER;
lv_obj_t * ui_BTN_EXISTING_INVENTORY;
lv_obj_t * ui_LABEL_EXISTING_INVENTORY;
lv_obj_t * ui_TITLE_HEADER;
lv_obj_t * ui_TITLE_SUB;
lv_obj_t * ui_Screen2;
lv_obj_t * ui_BackgroundImageBTN1;
lv_obj_t * ui_BOTTOM_PANEL1;
lv_obj_t * ui_BTN_CUSTOM_ORDER1;
lv_obj_t * ui_LABEL_CUSTOM_ORDER1;
lv_obj_t * ui_BTN_EXISTING_INVENTORY1;
lv_obj_t * ui_LABEL_EXISTING_INVENTORY1;
lv_obj_t * ui_TITLE_HEADER1;
lv_obj_t * ui_TITLE_SUB1;
lv_obj_t * ui_Screen3;
lv_obj_t * ui_BackgroundImageBTN2;
lv_obj_t * ui_BOTTOM_PANEL2;
lv_obj_t * ui_BTN_CUSTOM_ORDER2;
lv_obj_t * ui_LABEL_CUSTOM_ORDER2;
lv_obj_t * ui_BTN_EXISTING_INVENTORY2;
lv_obj_t * ui_LABEL_EXISTING_INVENTORY2;
lv_obj_t * ui_TITLE_HEADER2;
lv_obj_t * ui_TITLE_SUB2;
lv_obj_t * ui_Screen4;
lv_obj_t * ui_BackgroundImageBTN3;
lv_obj_t * ui_BOTTOM_PANEL3;
lv_obj_t * ui_BTN_CUSTOM_ORDER3;
lv_obj_t * ui_LABEL_CUSTOM_ORDER3;
lv_obj_t * ui_BTN_EXISTING_INVENTORY3;
lv_obj_t * ui_LABEL_EXISTING_INVENTORY3;
lv_obj_t * ui_TITLE_HEADER3;
lv_obj_t * ui_TITLE_SUB3;

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
static void ui_event_BackgroundImageBTN(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }
}
static void ui_event_BTN_CUSTOM_ORDER(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
}
static void ui_event_BackgroundImageBTN1(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen3, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }
}
static void ui_event_BTN_CUSTOM_ORDER1(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
}
static void ui_event_BackgroundImageBTN2(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }
}
static void ui_event_BTN_CUSTOM_ORDER2(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
}
static void ui_event_BackgroundImageBTN3(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }
}
static void ui_event_BTN_CUSTOM_ORDER3(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
}

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{

    // Screen1

    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

    // BackgroundImageBTN

    ui_BackgroundImageBTN = lv_imgbtn_create(ui_Screen1);
    lv_imgbtn_set_src(ui_BackgroundImageBTN, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_tesla_model_3_png, NULL);
    lv_imgbtn_set_src(ui_BackgroundImageBTN, LV_IMGBTN_STATE_PRESSED, NULL, &ui_img_tesla_model_3_png, NULL);
    lv_imgbtn_set_src(ui_BackgroundImageBTN, LV_IMGBTN_STATE_DISABLED, NULL, &ui_img_tesla_model_3_png, NULL);

    lv_obj_set_width(ui_BackgroundImageBTN, lv_pct(100));
    lv_obj_set_height(ui_BackgroundImageBTN, lv_pct(100));

    lv_obj_set_x(ui_BackgroundImageBTN, 0);
    lv_obj_set_y(ui_BackgroundImageBTN, 0);

    lv_obj_set_align(ui_BackgroundImageBTN, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_BackgroundImageBTN, ui_event_BackgroundImageBTN, LV_EVENT_ALL, NULL);

    // BOTTOM_PANEL

    ui_BOTTOM_PANEL = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_BOTTOM_PANEL, 400);
    lv_obj_set_height(ui_BOTTOM_PANEL, 50);

    lv_obj_set_x(ui_BOTTOM_PANEL, 0);
    lv_obj_set_y(ui_BOTTOM_PANEL, -20);

    lv_obj_set_align(ui_BOTTOM_PANEL, LV_ALIGN_BOTTOM_MID);

    lv_obj_clear_flag(ui_BOTTOM_PANEL, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BOTTOM_PANEL, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BOTTOM_PANEL, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BOTTOM_PANEL, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BOTTOM_PANEL, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_CUSTOM_ORDER

    ui_BTN_CUSTOM_ORDER = lv_btn_create(ui_BOTTOM_PANEL);

    lv_obj_set_width(ui_BTN_CUSTOM_ORDER, 160);
    lv_obj_set_height(ui_BTN_CUSTOM_ORDER, 24);

    lv_obj_set_x(ui_BTN_CUSTOM_ORDER, 0);
    lv_obj_set_y(ui_BTN_CUSTOM_ORDER, 0);

    lv_obj_set_align(ui_BTN_CUSTOM_ORDER, LV_ALIGN_LEFT_MID);

    lv_obj_add_flag(ui_BTN_CUSTOM_ORDER, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_CUSTOM_ORDER, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_BTN_CUSTOM_ORDER, ui_event_BTN_CUSTOM_ORDER, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_BTN_CUSTOM_ORDER, lv_color_hex(0x373C3F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_CUSTOM_ORDER, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // LABEL_CUSTOM_ORDER

    ui_LABEL_CUSTOM_ORDER = lv_label_create(ui_BTN_CUSTOM_ORDER);

    lv_obj_set_width(ui_LABEL_CUSTOM_ORDER, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LABEL_CUSTOM_ORDER, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LABEL_CUSTOM_ORDER, 0);
    lv_obj_set_y(ui_LABEL_CUSTOM_ORDER, 0);

    lv_obj_set_align(ui_LABEL_CUSTOM_ORDER, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LABEL_CUSTOM_ORDER, "CUSTOM ORDER");

    lv_obj_set_style_text_font(ui_LABEL_CUSTOM_ORDER, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_EXISTING_INVENTORY

    ui_BTN_EXISTING_INVENTORY = lv_btn_create(ui_BOTTOM_PANEL);

    lv_obj_set_width(ui_BTN_EXISTING_INVENTORY, 160);
    lv_obj_set_height(ui_BTN_EXISTING_INVENTORY, 24);

    lv_obj_set_x(ui_BTN_EXISTING_INVENTORY, 0);
    lv_obj_set_y(ui_BTN_EXISTING_INVENTORY, 0);

    lv_obj_set_align(ui_BTN_EXISTING_INVENTORY, LV_ALIGN_RIGHT_MID);

    lv_obj_add_flag(ui_BTN_EXISTING_INVENTORY, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_EXISTING_INVENTORY, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BTN_EXISTING_INVENTORY, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_EXISTING_INVENTORY, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // LABEL_EXISTING_INVENTORY

    ui_LABEL_EXISTING_INVENTORY = lv_label_create(ui_BTN_EXISTING_INVENTORY);

    lv_obj_set_width(ui_LABEL_EXISTING_INVENTORY, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LABEL_EXISTING_INVENTORY, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LABEL_EXISTING_INVENTORY, 0);
    lv_obj_set_y(ui_LABEL_EXISTING_INVENTORY, 0);

    lv_obj_set_align(ui_LABEL_EXISTING_INVENTORY, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LABEL_EXISTING_INVENTORY, "EXISTING INVENTORY");

    lv_obj_set_style_text_color(ui_LABEL_EXISTING_INVENTORY, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LABEL_EXISTING_INVENTORY, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LABEL_EXISTING_INVENTORY, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // TITLE_HEADER

    ui_TITLE_HEADER = lv_label_create(ui_Screen1);

    lv_obj_set_width(ui_TITLE_HEADER, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_TITLE_HEADER, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_TITLE_HEADER, 0);
    lv_obj_set_y(ui_TITLE_HEADER, 35);

    lv_obj_set_align(ui_TITLE_HEADER, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_TITLE_HEADER, "MODEL 3");

    lv_obj_set_style_text_color(ui_TITLE_HEADER, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TITLE_HEADER, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TITLE_HEADER, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // TITLE_SUB

    ui_TITLE_SUB = lv_label_create(ui_Screen1);

    lv_obj_set_width(ui_TITLE_SUB, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_TITLE_SUB, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_TITLE_SUB, 0);
    lv_obj_set_y(ui_TITLE_SUB, 60);

    lv_obj_set_align(ui_TITLE_SUB, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_TITLE_SUB, "Order Online for Touchless Delivery");

    lv_obj_set_style_text_color(ui_TITLE_SUB, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TITLE_SUB, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TITLE_SUB, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

}
void ui_Screen2_screen_init(void)
{

    // Screen2

    ui_Screen2 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);

    // BackgroundImageBTN1

    ui_BackgroundImageBTN1 = lv_imgbtn_create(ui_Screen2);
    lv_imgbtn_set_src(ui_BackgroundImageBTN1, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_tesla_model_y_png, NULL);
    lv_imgbtn_set_src(ui_BackgroundImageBTN1, LV_IMGBTN_STATE_PRESSED, NULL, &ui_img_tesla_model_y_png, NULL);
    lv_imgbtn_set_src(ui_BackgroundImageBTN1, LV_IMGBTN_STATE_DISABLED, NULL, &ui_img_tesla_model_y_png, NULL);

    lv_obj_set_width(ui_BackgroundImageBTN1, lv_pct(100));
    lv_obj_set_height(ui_BackgroundImageBTN1, lv_pct(100));

    lv_obj_set_x(ui_BackgroundImageBTN1, 0);
    lv_obj_set_y(ui_BackgroundImageBTN1, 0);

    lv_obj_set_align(ui_BackgroundImageBTN1, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_BackgroundImageBTN1, ui_event_BackgroundImageBTN1, LV_EVENT_ALL, NULL);

    // BOTTOM_PANEL1

    ui_BOTTOM_PANEL1 = lv_obj_create(ui_Screen2);

    lv_obj_set_width(ui_BOTTOM_PANEL1, 400);
    lv_obj_set_height(ui_BOTTOM_PANEL1, 50);

    lv_obj_set_x(ui_BOTTOM_PANEL1, 0);
    lv_obj_set_y(ui_BOTTOM_PANEL1, -20);

    lv_obj_set_align(ui_BOTTOM_PANEL1, LV_ALIGN_BOTTOM_MID);

    lv_obj_clear_flag(ui_BOTTOM_PANEL1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BOTTOM_PANEL1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BOTTOM_PANEL1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BOTTOM_PANEL1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BOTTOM_PANEL1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_CUSTOM_ORDER1

    ui_BTN_CUSTOM_ORDER1 = lv_btn_create(ui_BOTTOM_PANEL1);

    lv_obj_set_width(ui_BTN_CUSTOM_ORDER1, 160);
    lv_obj_set_height(ui_BTN_CUSTOM_ORDER1, 24);

    lv_obj_set_x(ui_BTN_CUSTOM_ORDER1, 0);
    lv_obj_set_y(ui_BTN_CUSTOM_ORDER1, 0);

    lv_obj_set_align(ui_BTN_CUSTOM_ORDER1, LV_ALIGN_LEFT_MID);

    lv_obj_add_flag(ui_BTN_CUSTOM_ORDER1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_CUSTOM_ORDER1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_BTN_CUSTOM_ORDER1, ui_event_BTN_CUSTOM_ORDER1, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_BTN_CUSTOM_ORDER1, lv_color_hex(0x373C3F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_CUSTOM_ORDER1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // LABEL_CUSTOM_ORDER1

    ui_LABEL_CUSTOM_ORDER1 = lv_label_create(ui_BTN_CUSTOM_ORDER1);

    lv_obj_set_width(ui_LABEL_CUSTOM_ORDER1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LABEL_CUSTOM_ORDER1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LABEL_CUSTOM_ORDER1, 0);
    lv_obj_set_y(ui_LABEL_CUSTOM_ORDER1, 0);

    lv_obj_set_align(ui_LABEL_CUSTOM_ORDER1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LABEL_CUSTOM_ORDER1, "CUSTOM ORDER");

    lv_obj_set_style_text_font(ui_LABEL_CUSTOM_ORDER1, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_EXISTING_INVENTORY1

    ui_BTN_EXISTING_INVENTORY1 = lv_btn_create(ui_BOTTOM_PANEL1);

    lv_obj_set_width(ui_BTN_EXISTING_INVENTORY1, 160);
    lv_obj_set_height(ui_BTN_EXISTING_INVENTORY1, 24);

    lv_obj_set_x(ui_BTN_EXISTING_INVENTORY1, 0);
    lv_obj_set_y(ui_BTN_EXISTING_INVENTORY1, 0);

    lv_obj_set_align(ui_BTN_EXISTING_INVENTORY1, LV_ALIGN_RIGHT_MID);

    lv_obj_add_flag(ui_BTN_EXISTING_INVENTORY1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_EXISTING_INVENTORY1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BTN_EXISTING_INVENTORY1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_EXISTING_INVENTORY1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // LABEL_EXISTING_INVENTORY1

    ui_LABEL_EXISTING_INVENTORY1 = lv_label_create(ui_BTN_EXISTING_INVENTORY1);

    lv_obj_set_width(ui_LABEL_EXISTING_INVENTORY1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LABEL_EXISTING_INVENTORY1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LABEL_EXISTING_INVENTORY1, 0);
    lv_obj_set_y(ui_LABEL_EXISTING_INVENTORY1, 0);

    lv_obj_set_align(ui_LABEL_EXISTING_INVENTORY1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LABEL_EXISTING_INVENTORY1, "EXISTING INVENTORY");

    lv_obj_set_style_text_color(ui_LABEL_EXISTING_INVENTORY1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LABEL_EXISTING_INVENTORY1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LABEL_EXISTING_INVENTORY1, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // TITLE_HEADER1

    ui_TITLE_HEADER1 = lv_label_create(ui_Screen2);

    lv_obj_set_width(ui_TITLE_HEADER1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_TITLE_HEADER1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_TITLE_HEADER1, 0);
    lv_obj_set_y(ui_TITLE_HEADER1, 35);

    lv_obj_set_align(ui_TITLE_HEADER1, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_TITLE_HEADER1, "MODEL Y");

    lv_obj_set_style_text_color(ui_TITLE_HEADER1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TITLE_HEADER1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TITLE_HEADER1, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // TITLE_SUB1

    ui_TITLE_SUB1 = lv_label_create(ui_Screen2);

    lv_obj_set_width(ui_TITLE_SUB1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_TITLE_SUB1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_TITLE_SUB1, 0);
    lv_obj_set_y(ui_TITLE_SUB1, 60);

    lv_obj_set_align(ui_TITLE_SUB1, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_TITLE_SUB1, "Order Online for Touchless Delivery");

    lv_obj_set_style_text_color(ui_TITLE_SUB1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TITLE_SUB1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TITLE_SUB1, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

}
void ui_Screen3_screen_init(void)
{

    // Screen3

    ui_Screen3 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);

    // BackgroundImageBTN2

    ui_BackgroundImageBTN2 = lv_imgbtn_create(ui_Screen3);
    lv_imgbtn_set_src(ui_BackgroundImageBTN2, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_tesla_model_s_png, NULL);
    lv_imgbtn_set_src(ui_BackgroundImageBTN2, LV_IMGBTN_STATE_PRESSED, NULL, &ui_img_tesla_model_s_png, NULL);
    lv_imgbtn_set_src(ui_BackgroundImageBTN2, LV_IMGBTN_STATE_DISABLED, NULL, &ui_img_tesla_model_s_png, NULL);

    lv_obj_set_width(ui_BackgroundImageBTN2, lv_pct(100));
    lv_obj_set_height(ui_BackgroundImageBTN2, lv_pct(100));

    lv_obj_set_x(ui_BackgroundImageBTN2, 0);
    lv_obj_set_y(ui_BackgroundImageBTN2, 0);

    lv_obj_set_align(ui_BackgroundImageBTN2, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_BackgroundImageBTN2, ui_event_BackgroundImageBTN2, LV_EVENT_ALL, NULL);

    // BOTTOM_PANEL2

    ui_BOTTOM_PANEL2 = lv_obj_create(ui_Screen3);

    lv_obj_set_width(ui_BOTTOM_PANEL2, 400);
    lv_obj_set_height(ui_BOTTOM_PANEL2, 50);

    lv_obj_set_x(ui_BOTTOM_PANEL2, 0);
    lv_obj_set_y(ui_BOTTOM_PANEL2, -20);

    lv_obj_set_align(ui_BOTTOM_PANEL2, LV_ALIGN_BOTTOM_MID);

    lv_obj_clear_flag(ui_BOTTOM_PANEL2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BOTTOM_PANEL2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BOTTOM_PANEL2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BOTTOM_PANEL2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BOTTOM_PANEL2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_CUSTOM_ORDER2

    ui_BTN_CUSTOM_ORDER2 = lv_btn_create(ui_BOTTOM_PANEL2);

    lv_obj_set_width(ui_BTN_CUSTOM_ORDER2, 160);
    lv_obj_set_height(ui_BTN_CUSTOM_ORDER2, 24);

    lv_obj_set_x(ui_BTN_CUSTOM_ORDER2, 0);
    lv_obj_set_y(ui_BTN_CUSTOM_ORDER2, 0);

    lv_obj_set_align(ui_BTN_CUSTOM_ORDER2, LV_ALIGN_LEFT_MID);

    lv_obj_add_flag(ui_BTN_CUSTOM_ORDER2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_CUSTOM_ORDER2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_BTN_CUSTOM_ORDER2, ui_event_BTN_CUSTOM_ORDER2, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_BTN_CUSTOM_ORDER2, lv_color_hex(0x373C3F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_CUSTOM_ORDER2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // LABEL_CUSTOM_ORDER2

    ui_LABEL_CUSTOM_ORDER2 = lv_label_create(ui_BTN_CUSTOM_ORDER2);

    lv_obj_set_width(ui_LABEL_CUSTOM_ORDER2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LABEL_CUSTOM_ORDER2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LABEL_CUSTOM_ORDER2, 0);
    lv_obj_set_y(ui_LABEL_CUSTOM_ORDER2, 0);

    lv_obj_set_align(ui_LABEL_CUSTOM_ORDER2, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LABEL_CUSTOM_ORDER2, "CUSTOM ORDER");

    lv_obj_set_style_text_font(ui_LABEL_CUSTOM_ORDER2, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_EXISTING_INVENTORY2

    ui_BTN_EXISTING_INVENTORY2 = lv_btn_create(ui_BOTTOM_PANEL2);

    lv_obj_set_width(ui_BTN_EXISTING_INVENTORY2, 160);
    lv_obj_set_height(ui_BTN_EXISTING_INVENTORY2, 24);

    lv_obj_set_x(ui_BTN_EXISTING_INVENTORY2, 0);
    lv_obj_set_y(ui_BTN_EXISTING_INVENTORY2, 0);

    lv_obj_set_align(ui_BTN_EXISTING_INVENTORY2, LV_ALIGN_RIGHT_MID);

    lv_obj_add_flag(ui_BTN_EXISTING_INVENTORY2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_EXISTING_INVENTORY2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BTN_EXISTING_INVENTORY2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_EXISTING_INVENTORY2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // LABEL_EXISTING_INVENTORY2

    ui_LABEL_EXISTING_INVENTORY2 = lv_label_create(ui_BTN_EXISTING_INVENTORY2);

    lv_obj_set_width(ui_LABEL_EXISTING_INVENTORY2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LABEL_EXISTING_INVENTORY2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LABEL_EXISTING_INVENTORY2, 0);
    lv_obj_set_y(ui_LABEL_EXISTING_INVENTORY2, 0);

    lv_obj_set_align(ui_LABEL_EXISTING_INVENTORY2, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LABEL_EXISTING_INVENTORY2, "EXISTING INVENTORY");

    lv_obj_set_style_text_color(ui_LABEL_EXISTING_INVENTORY2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LABEL_EXISTING_INVENTORY2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LABEL_EXISTING_INVENTORY2, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // TITLE_HEADER2

    ui_TITLE_HEADER2 = lv_label_create(ui_Screen3);

    lv_obj_set_width(ui_TITLE_HEADER2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_TITLE_HEADER2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_TITLE_HEADER2, 0);
    lv_obj_set_y(ui_TITLE_HEADER2, 35);

    lv_obj_set_align(ui_TITLE_HEADER2, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_TITLE_HEADER2, "MODEL S");

    lv_obj_set_style_text_color(ui_TITLE_HEADER2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TITLE_HEADER2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TITLE_HEADER2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // TITLE_SUB2

    ui_TITLE_SUB2 = lv_label_create(ui_Screen3);

    lv_obj_set_width(ui_TITLE_SUB2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_TITLE_SUB2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_TITLE_SUB2, 0);
    lv_obj_set_y(ui_TITLE_SUB2, 60);

    lv_obj_set_align(ui_TITLE_SUB2, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_TITLE_SUB2, "Order Online for Touchless Delivery");

    lv_obj_set_style_text_color(ui_TITLE_SUB2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TITLE_SUB2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TITLE_SUB2, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

}
void ui_Screen4_screen_init(void)
{

    // Screen4

    ui_Screen4 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen4, LV_OBJ_FLAG_SCROLLABLE);

    // BackgroundImageBTN3

    ui_BackgroundImageBTN3 = lv_imgbtn_create(ui_Screen4);
    lv_imgbtn_set_src(ui_BackgroundImageBTN3, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_tesla_model_x_png, NULL);
    lv_imgbtn_set_src(ui_BackgroundImageBTN3, LV_IMGBTN_STATE_PRESSED, NULL, &ui_img_tesla_model_x_png, NULL);
    lv_imgbtn_set_src(ui_BackgroundImageBTN3, LV_IMGBTN_STATE_DISABLED, NULL, &ui_img_tesla_model_x_png, NULL);

    lv_obj_set_width(ui_BackgroundImageBTN3, lv_pct(100));
    lv_obj_set_height(ui_BackgroundImageBTN3, lv_pct(100));

    lv_obj_set_x(ui_BackgroundImageBTN3, 0);
    lv_obj_set_y(ui_BackgroundImageBTN3, 0);

    lv_obj_set_align(ui_BackgroundImageBTN3, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_BackgroundImageBTN3, ui_event_BackgroundImageBTN3, LV_EVENT_ALL, NULL);

    // BOTTOM_PANEL3

    ui_BOTTOM_PANEL3 = lv_obj_create(ui_Screen4);

    lv_obj_set_width(ui_BOTTOM_PANEL3, 400);
    lv_obj_set_height(ui_BOTTOM_PANEL3, 50);

    lv_obj_set_x(ui_BOTTOM_PANEL3, 0);
    lv_obj_set_y(ui_BOTTOM_PANEL3, -20);

    lv_obj_set_align(ui_BOTTOM_PANEL3, LV_ALIGN_BOTTOM_MID);

    lv_obj_clear_flag(ui_BOTTOM_PANEL3, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BOTTOM_PANEL3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BOTTOM_PANEL3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BOTTOM_PANEL3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BOTTOM_PANEL3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_CUSTOM_ORDER3

    ui_BTN_CUSTOM_ORDER3 = lv_btn_create(ui_BOTTOM_PANEL3);

    lv_obj_set_width(ui_BTN_CUSTOM_ORDER3, 160);
    lv_obj_set_height(ui_BTN_CUSTOM_ORDER3, 24);

    lv_obj_set_x(ui_BTN_CUSTOM_ORDER3, 0);
    lv_obj_set_y(ui_BTN_CUSTOM_ORDER3, 0);

    lv_obj_set_align(ui_BTN_CUSTOM_ORDER3, LV_ALIGN_LEFT_MID);

    lv_obj_add_flag(ui_BTN_CUSTOM_ORDER3, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_CUSTOM_ORDER3, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_BTN_CUSTOM_ORDER3, ui_event_BTN_CUSTOM_ORDER3, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_BTN_CUSTOM_ORDER3, lv_color_hex(0x373C3F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_CUSTOM_ORDER3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // LABEL_CUSTOM_ORDER3

    ui_LABEL_CUSTOM_ORDER3 = lv_label_create(ui_BTN_CUSTOM_ORDER3);

    lv_obj_set_width(ui_LABEL_CUSTOM_ORDER3, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LABEL_CUSTOM_ORDER3, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LABEL_CUSTOM_ORDER3, 0);
    lv_obj_set_y(ui_LABEL_CUSTOM_ORDER3, 0);

    lv_obj_set_align(ui_LABEL_CUSTOM_ORDER3, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LABEL_CUSTOM_ORDER3, "CUSTOM ORDER");

    lv_obj_set_style_text_font(ui_LABEL_CUSTOM_ORDER3, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_EXISTING_INVENTORY3

    ui_BTN_EXISTING_INVENTORY3 = lv_btn_create(ui_BOTTOM_PANEL3);

    lv_obj_set_width(ui_BTN_EXISTING_INVENTORY3, 160);
    lv_obj_set_height(ui_BTN_EXISTING_INVENTORY3, 24);

    lv_obj_set_x(ui_BTN_EXISTING_INVENTORY3, 0);
    lv_obj_set_y(ui_BTN_EXISTING_INVENTORY3, 0);

    lv_obj_set_align(ui_BTN_EXISTING_INVENTORY3, LV_ALIGN_RIGHT_MID);

    lv_obj_add_flag(ui_BTN_EXISTING_INVENTORY3, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_EXISTING_INVENTORY3, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BTN_EXISTING_INVENTORY3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_EXISTING_INVENTORY3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // LABEL_EXISTING_INVENTORY3

    ui_LABEL_EXISTING_INVENTORY3 = lv_label_create(ui_BTN_EXISTING_INVENTORY3);

    lv_obj_set_width(ui_LABEL_EXISTING_INVENTORY3, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LABEL_EXISTING_INVENTORY3, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LABEL_EXISTING_INVENTORY3, 0);
    lv_obj_set_y(ui_LABEL_EXISTING_INVENTORY3, 0);

    lv_obj_set_align(ui_LABEL_EXISTING_INVENTORY3, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LABEL_EXISTING_INVENTORY3, "EXISTING INVENTORY");

    lv_obj_set_style_text_color(ui_LABEL_EXISTING_INVENTORY3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LABEL_EXISTING_INVENTORY3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LABEL_EXISTING_INVENTORY3, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // TITLE_HEADER3

    ui_TITLE_HEADER3 = lv_label_create(ui_Screen4);

    lv_obj_set_width(ui_TITLE_HEADER3, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_TITLE_HEADER3, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_TITLE_HEADER3, 0);
    lv_obj_set_y(ui_TITLE_HEADER3, 35);

    lv_obj_set_align(ui_TITLE_HEADER3, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_TITLE_HEADER3, "MODEL X");

    lv_obj_set_style_text_color(ui_TITLE_HEADER3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TITLE_HEADER3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TITLE_HEADER3, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // TITLE_SUB3

    ui_TITLE_SUB3 = lv_label_create(ui_Screen4);

    lv_obj_set_width(ui_TITLE_SUB3, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_TITLE_SUB3, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_TITLE_SUB3, 0);
    lv_obj_set_y(ui_TITLE_SUB3, 60);

    lv_obj_set_align(ui_TITLE_SUB3, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_TITLE_SUB3, "Order Online for Touchless Delivery");

    lv_obj_set_style_text_color(ui_TITLE_SUB3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TITLE_SUB3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TITLE_SUB3, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void ui_init(void)
{
    ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    ui_Screen3_screen_init();
    ui_Screen4_screen_init();
    lv_disp_load_scr(ui_Screen1);
}

