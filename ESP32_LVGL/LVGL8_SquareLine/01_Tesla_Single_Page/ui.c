// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.0
// LVGL VERSION: 8.2
// PROJECT: 0_Tesla_Page

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
lv_obj_t * ui_Image_Background;
lv_obj_t * ui_BottomPanel;
lv_obj_t * ui_BTN_Custom_Order;
lv_obj_t * ui_BTN_LABEL;
lv_obj_t * ui_BTN_Custom_Order_copy;
lv_obj_t * ui_BTN_LABEL1;
lv_obj_t * ui_Label_Title;
lv_obj_t * ui_Label_Subtile;

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{

    // Screen1

    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

    // Image_Background

    ui_Image_Background = lv_img_create(ui_Screen1);
    lv_img_set_src(ui_Image_Background, &ui_img_tesla_model_3_png);

    lv_obj_set_width(ui_Image_Background, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image_Background, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Image_Background, 0);
    lv_obj_set_y(ui_Image_Background, 0);

    lv_obj_set_align(ui_Image_Background, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_Image_Background, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image_Background, LV_OBJ_FLAG_SCROLLABLE);

    // BottomPanel

    ui_BottomPanel = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_BottomPanel, 400);
    lv_obj_set_height(ui_BottomPanel, 50);

    lv_obj_set_x(ui_BottomPanel, 0);
    lv_obj_set_y(ui_BottomPanel, -20);

    lv_obj_set_align(ui_BottomPanel, LV_ALIGN_BOTTOM_MID);

    lv_obj_clear_flag(ui_BottomPanel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_scrollbar_mode(ui_BottomPanel, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_style_bg_color(ui_BottomPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BottomPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BottomPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BottomPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_Custom_Order

    ui_BTN_Custom_Order = lv_btn_create(ui_BottomPanel);

    lv_obj_set_width(ui_BTN_Custom_Order, 160);
    lv_obj_set_height(ui_BTN_Custom_Order, 24);

    lv_obj_set_x(ui_BTN_Custom_Order, 0);
    lv_obj_set_y(ui_BTN_Custom_Order, 5);

    lv_obj_set_align(ui_BTN_Custom_Order, LV_ALIGN_BOTTOM_LEFT);

    lv_obj_add_flag(ui_BTN_Custom_Order, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_Custom_Order, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_radius(ui_BTN_Custom_Order, 48, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BTN_Custom_Order, lv_color_hex(0x383C3F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_Custom_Order, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_LABEL

    ui_BTN_LABEL = lv_label_create(ui_BTN_Custom_Order);

    lv_obj_set_width(ui_BTN_LABEL, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_BTN_LABEL, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_BTN_LABEL, 0);
    lv_obj_set_y(ui_BTN_LABEL, 0);

    lv_obj_set_align(ui_BTN_LABEL, LV_ALIGN_CENTER);

    lv_label_set_text(ui_BTN_LABEL, "CUSTOM ORDER");

    lv_obj_clear_flag(ui_BTN_LABEL, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_text_align(ui_BTN_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_BTN_LABEL, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_Custom_Order_copy

    ui_BTN_Custom_Order_copy = lv_btn_create(ui_BottomPanel);

    lv_obj_set_width(ui_BTN_Custom_Order_copy, 160);
    lv_obj_set_height(ui_BTN_Custom_Order_copy, 24);

    lv_obj_set_x(ui_BTN_Custom_Order_copy, 0);
    lv_obj_set_y(ui_BTN_Custom_Order_copy, 5);

    lv_obj_set_align(ui_BTN_Custom_Order_copy, LV_ALIGN_BOTTOM_RIGHT);

    lv_obj_add_flag(ui_BTN_Custom_Order_copy, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BTN_Custom_Order_copy, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_radius(ui_BTN_Custom_Order_copy, 48, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BTN_Custom_Order_copy, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BTN_Custom_Order_copy, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // BTN_LABEL1

    ui_BTN_LABEL1 = lv_label_create(ui_BTN_Custom_Order_copy);

    lv_obj_set_width(ui_BTN_LABEL1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_BTN_LABEL1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_BTN_LABEL1, 0);
    lv_obj_set_y(ui_BTN_LABEL1, 0);

    lv_obj_set_align(ui_BTN_LABEL1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_BTN_LABEL1, "EXISTING INVENTORY");

    lv_obj_clear_flag(ui_BTN_LABEL1, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_text_color(ui_BTN_LABEL1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_BTN_LABEL1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_BTN_LABEL1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_BTN_LABEL1, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Label_Title

    ui_Label_Title = lv_label_create(ui_Screen1);

    lv_obj_set_width(ui_Label_Title, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label_Title, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label_Title, 0);
    lv_obj_set_y(ui_Label_Title, 40);

    lv_obj_set_align(ui_Label_Title, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_Label_Title, "Model 3");

    lv_obj_clear_flag(ui_Label_Title, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_text_color(ui_Label_Title, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label_Title, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Label_Title, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Label_Subtile

    ui_Label_Subtile = lv_label_create(ui_Screen1);

    lv_obj_set_width(ui_Label_Subtile, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label_Subtile, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label_Subtile, 0);
    lv_obj_set_y(ui_Label_Subtile, 64);

    lv_obj_set_align(ui_Label_Subtile, LV_ALIGN_TOP_MID);

    lv_label_set_text(ui_Label_Subtile, "Order Online for Touchless Delivery");

    lv_obj_clear_flag(ui_Label_Subtile, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_text_color(ui_Label_Subtile, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label_Subtile, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Label_Subtile, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void ui_init(void)
{
    ui_Screen1_screen_init();
    lv_disp_load_scr(ui_Screen1);
}

