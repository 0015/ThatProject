// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: GPS_SPEEDOMETER

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t *ui_Screen1;
lv_obj_t *ui_RootPanel;
lv_obj_t *ui_TopPanel;
void ui_event_TopButton1( lv_event_t * e);
lv_obj_t *ui_TopButton1;
lv_obj_t *ui_Title;
lv_obj_t *ui_SpeedPanel;
lv_obj_t *ui_SpeedBG;
lv_obj_t *ui_SpeedSlider;
lv_obj_t *ui_SpeedMinLabel;
lv_obj_t *ui_SpeedMaxLabel;
lv_obj_t *ui_CompassPanel;
lv_obj_t *ui_CompassImg;
lv_obj_t *ui_CompassLabel;
lv_obj_t *ui_SpeedTextPanel;
lv_obj_t *ui_SpeedUnitLabel;
lv_obj_t *ui_SpeedLabelBack;
lv_obj_t *ui_SpeedLabel;
lv_obj_t *ui_GPSWaitingSpinner;
lv_obj_t *ui_Screen2;
lv_obj_t *ui_RootPanel1;
lv_obj_t *ui_TopPanel1;
void ui_event_TopButton2( lv_event_t * e);
lv_obj_t *ui_TopButton2;
lv_obj_t *ui_Title1;
lv_obj_t *ui_BodyPanel;
lv_obj_t *ui_SatellitesPanel;
lv_obj_t *ui_SatellitesTitle;
lv_obj_t *ui_SatellitesLabel;
lv_obj_t *ui_DegreePanel;
lv_obj_t *ui_DegreeTitle;
lv_obj_t *ui_DegreeLabel;
lv_obj_t *ui_CoordinatesPanel;
lv_obj_t *ui_CoordinatesTitle;
lv_obj_t *ui_CoordinatesText;
lv_obj_t *ui_CoordinatesLabel;
lv_obj_t *ui_SpeedPanel2;
lv_obj_t *ui_SpeedTitle2;
lv_obj_t *ui_SpeedLabel2;
lv_obj_t *ui_AltitudePanel;
lv_obj_t *ui_AltitudeTitle;
lv_obj_t *ui_AltitudeLabel;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_TopButton1( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      _ui_screen_change( ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 50, 0);
}
}
void ui_event_TopButton2( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
if ( event_code == LV_EVENT_CLICKED) {
      _ui_screen_change( ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 50, 0);
}
}

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{
ui_Screen1 = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Screen1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_RootPanel = lv_obj_create(ui_Screen1);
lv_obj_set_width( ui_RootPanel, lv_pct(100));
lv_obj_set_height( ui_RootPanel, lv_pct(100));
lv_obj_set_align( ui_RootPanel, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_RootPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_RootPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_RootPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_RootPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_TopPanel = lv_obj_create(ui_RootPanel);
lv_obj_set_height( ui_TopPanel, 50);
lv_obj_set_width( ui_TopPanel, lv_pct(80));
lv_obj_set_x( ui_TopPanel, 0 );
lv_obj_set_y( ui_TopPanel, -22 );
lv_obj_set_align( ui_TopPanel, LV_ALIGN_TOP_MID );
lv_obj_clear_flag( ui_TopPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_border_color(ui_TopPanel, lv_color_hex(0x6A6A75), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_TopPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_TopPanel, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_side(ui_TopPanel, LV_BORDER_SIDE_FULL, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_TopPanel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_TopPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_TopPanel, 5, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_TopPanel, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(ui_TopPanel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(ui_TopPanel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_TopButton1 = lv_btn_create(ui_TopPanel);
lv_obj_set_height( ui_TopButton1, 50);
lv_obj_set_width( ui_TopButton1, lv_pct(100));
lv_obj_set_align( ui_TopButton1, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_TopButton1, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_TopButton1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_TopButton1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_TopButton1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Title = lv_label_create(ui_TopPanel);
lv_obj_set_width( ui_Title, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Title, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Title, LV_ALIGN_CENTER );
lv_label_set_text(ui_Title,"Speedometer");
lv_obj_set_style_text_font(ui_Title, &ui_font_Display24, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedPanel = lv_obj_create(ui_Screen1);
lv_obj_set_height( ui_SpeedPanel, 140);
lv_obj_set_width( ui_SpeedPanel, lv_pct(100));
lv_obj_set_x( ui_SpeedPanel, 0 );
lv_obj_set_y( ui_SpeedPanel, -100 );
lv_obj_set_align( ui_SpeedPanel, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_SpeedPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_SpeedPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_SpeedPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_SpeedPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_SpeedPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedBG = lv_img_create(ui_SpeedPanel);
lv_img_set_src(ui_SpeedBG, &ui_img_speedbg_png);
lv_obj_set_width( ui_SpeedBG, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SpeedBG, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_SpeedBG, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_SpeedBG, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_SpeedBG, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_SpeedSlider = lv_slider_create(ui_SpeedBG);
lv_slider_set_range(ui_SpeedSlider, 0,200);
lv_slider_set_value( ui_SpeedSlider, 120, LV_ANIM_OFF);
if (lv_slider_get_mode(ui_SpeedSlider)==LV_SLIDER_MODE_RANGE ) lv_slider_set_left_value( ui_SpeedSlider, 0, LV_ANIM_OFF);
lv_obj_set_width( ui_SpeedSlider, 320);
lv_obj_set_height( ui_SpeedSlider, 140);
lv_obj_set_align( ui_SpeedSlider, LV_ALIGN_CENTER );
lv_obj_set_style_radius(ui_SpeedSlider, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_SpeedSlider, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_SpeedSlider, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_radius(ui_SpeedSlider, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_SpeedSlider, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_SpeedSlider, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_bg_img_src( ui_SpeedSlider, &ui_img_speedfg_png, LV_PART_INDICATOR | LV_STATE_DEFAULT );

lv_obj_set_style_radius(ui_SpeedSlider, 0, LV_PART_KNOB| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_SpeedSlider, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_SpeedSlider, 0, LV_PART_KNOB| LV_STATE_DEFAULT);

ui_SpeedMinLabel = lv_label_create(ui_SpeedPanel);
lv_obj_set_width( ui_SpeedMinLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SpeedMinLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_SpeedMinLabel, 0 );
lv_obj_set_y( ui_SpeedMinLabel, 10 );
lv_obj_set_align( ui_SpeedMinLabel, LV_ALIGN_BOTTOM_LEFT );
lv_label_set_text(ui_SpeedMinLabel,"0");
lv_obj_set_style_text_font(ui_SpeedMinLabel, &ui_font_Display24, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedMaxLabel = lv_label_create(ui_SpeedPanel);
lv_obj_set_width( ui_SpeedMaxLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SpeedMaxLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_SpeedMaxLabel, 0 );
lv_obj_set_y( ui_SpeedMaxLabel, -20 );
lv_obj_set_align( ui_SpeedMaxLabel, LV_ALIGN_TOP_RIGHT );
lv_label_set_text(ui_SpeedMaxLabel,"200\n");
lv_obj_set_style_text_font(ui_SpeedMaxLabel, &ui_font_Display24, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_CompassPanel = lv_obj_create(ui_Screen1);
lv_obj_set_width( ui_CompassPanel, lv_pct(100));
lv_obj_set_height( ui_CompassPanel, lv_pct(60));
lv_obj_set_x( ui_CompassPanel, 0 );
lv_obj_set_y( ui_CompassPanel, 14 );
lv_obj_set_align( ui_CompassPanel, LV_ALIGN_BOTTOM_MID );
lv_obj_clear_flag( ui_CompassPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_CompassPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_CompassPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_CompassPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_CompassPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_CompassImg = lv_img_create(ui_CompassPanel);
lv_img_set_src(ui_CompassImg, &ui_img_compass_png);
lv_obj_set_width( ui_CompassImg, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_CompassImg, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_CompassImg, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_CompassImg, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_CompassImg, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_CompassLabel = lv_label_create(ui_CompassPanel);
lv_obj_set_width( ui_CompassLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_CompassLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_CompassLabel, LV_ALIGN_CENTER );
lv_label_set_text(ui_CompassLabel,"360");
lv_obj_set_style_text_color(ui_CompassLabel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_CompassLabel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_CompassLabel, &ui_font_Display40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedTextPanel = lv_obj_create(ui_Screen1);
lv_obj_set_height( ui_SpeedTextPanel, 80);
lv_obj_set_width( ui_SpeedTextPanel, lv_pct(100));
lv_obj_set_x( ui_SpeedTextPanel, 0 );
lv_obj_set_y( ui_SpeedTextPanel, -40 );
lv_obj_set_align( ui_SpeedTextPanel, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_SpeedTextPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_SpeedTextPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_SpeedTextPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_SpeedTextPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedUnitLabel = lv_label_create(ui_SpeedTextPanel);
lv_obj_set_width( ui_SpeedUnitLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SpeedUnitLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_SpeedUnitLabel, LV_ALIGN_BOTTOM_RIGHT );
lv_label_set_text(ui_SpeedUnitLabel,"mph");
lv_obj_set_style_text_font(ui_SpeedUnitLabel, &ui_font_Display24, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedLabelBack = lv_label_create(ui_SpeedTextPanel);
lv_obj_set_width( ui_SpeedLabelBack, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SpeedLabelBack, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_SpeedLabelBack, -44 );
lv_obj_set_y( ui_SpeedLabelBack, 10 );
lv_obj_set_align( ui_SpeedLabelBack, LV_ALIGN_BOTTOM_RIGHT );
lv_label_set_text(ui_SpeedLabelBack,"000");
lv_obj_set_style_text_color(ui_SpeedLabelBack, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_SpeedLabelBack, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_SpeedLabelBack, &ui_font_Display90, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedLabel = lv_label_create(ui_SpeedTextPanel);
lv_obj_set_width( ui_SpeedLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SpeedLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_SpeedLabel, -44 );
lv_obj_set_y( ui_SpeedLabel, 10 );
lv_obj_set_align( ui_SpeedLabel, LV_ALIGN_BOTTOM_RIGHT );
lv_label_set_text(ui_SpeedLabel,"120");
lv_obj_set_style_text_color(ui_SpeedLabel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_SpeedLabel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_SpeedLabel, &ui_font_Display90, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_GPSWaitingSpinner = lv_spinner_create(ui_Screen1,1000,90);
lv_obj_set_width( ui_GPSWaitingSpinner, 20);
lv_obj_set_height( ui_GPSWaitingSpinner, 20);
lv_obj_set_x( ui_GPSWaitingSpinner, -12 );
lv_obj_set_y( ui_GPSWaitingSpinner, 12 );
lv_obj_set_align( ui_GPSWaitingSpinner, LV_ALIGN_TOP_RIGHT );
lv_obj_clear_flag( ui_GPSWaitingSpinner, LV_OBJ_FLAG_CLICKABLE );    /// Flags
lv_obj_set_style_arc_color(ui_GPSWaitingSpinner, lv_color_hex(0x393C39), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_GPSWaitingSpinner, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_arc_width(ui_GPSWaitingSpinner, 4, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_arc_color(ui_GPSWaitingSpinner, lv_color_hex(0x00FFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_GPSWaitingSpinner, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_arc_width(ui_GPSWaitingSpinner, 4, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_TopButton1, ui_event_TopButton1, LV_EVENT_ALL, NULL);

}
void ui_Screen2_screen_init(void)
{
ui_Screen2 = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Screen2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_RootPanel1 = lv_obj_create(ui_Screen2);
lv_obj_set_width( ui_RootPanel1, lv_pct(100));
lv_obj_set_height( ui_RootPanel1, lv_pct(100));
lv_obj_set_align( ui_RootPanel1, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_RootPanel1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_RootPanel1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_RootPanel1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_RootPanel1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_TopPanel1 = lv_obj_create(ui_RootPanel1);
lv_obj_set_height( ui_TopPanel1, 50);
lv_obj_set_width( ui_TopPanel1, lv_pct(80));
lv_obj_set_x( ui_TopPanel1, 0 );
lv_obj_set_y( ui_TopPanel1, -22 );
lv_obj_set_align( ui_TopPanel1, LV_ALIGN_TOP_MID );
lv_obj_clear_flag( ui_TopPanel1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_border_color(ui_TopPanel1, lv_color_hex(0x6A6A75), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_TopPanel1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_TopPanel1, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_side(ui_TopPanel1, LV_BORDER_SIDE_FULL, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_TopPanel1, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_TopPanel1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_TopPanel1, 5, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_TopPanel1, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(ui_TopPanel1, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(ui_TopPanel1, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_TopButton2 = lv_btn_create(ui_TopPanel1);
lv_obj_set_height( ui_TopButton2, 50);
lv_obj_set_width( ui_TopButton2, lv_pct(100));
lv_obj_set_align( ui_TopButton2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_TopButton2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_TopButton2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_TopButton2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_TopButton2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Title1 = lv_label_create(ui_TopPanel1);
lv_obj_set_width( ui_Title1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Title1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Title1, LV_ALIGN_CENTER );
lv_label_set_text(ui_Title1,"GPS Information");
lv_obj_set_style_text_font(ui_Title1, &ui_font_Display24, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_BodyPanel = lv_obj_create(ui_Screen2);
lv_obj_set_height( ui_BodyPanel, 420);
lv_obj_set_width( ui_BodyPanel, lv_pct(100));
lv_obj_set_x( ui_BodyPanel, 0 );
lv_obj_set_y( ui_BodyPanel, -10 );
lv_obj_set_align( ui_BodyPanel, LV_ALIGN_BOTTOM_MID );
lv_obj_clear_flag( ui_BodyPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_BodyPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_BodyPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_BodyPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_BodyPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SatellitesPanel = lv_obj_create(ui_BodyPanel);
lv_obj_set_width( ui_SatellitesPanel, 120);
lv_obj_set_height( ui_SatellitesPanel, 80);
lv_obj_set_x( ui_SatellitesPanel, 10 );
lv_obj_set_y( ui_SatellitesPanel, 0 );
lv_obj_clear_flag( ui_SatellitesPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_border_color(ui_SatellitesPanel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_SatellitesPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_SatellitesPanel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SatellitesTitle = lv_label_create(ui_SatellitesPanel);
lv_obj_set_width( ui_SatellitesTitle, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SatellitesTitle, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_SatellitesTitle, 0 );
lv_obj_set_y( ui_SatellitesTitle, -10 );
lv_label_set_text(ui_SatellitesTitle,"Satellites");
lv_obj_set_style_text_font(ui_SatellitesTitle, &ui_font_Display16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SatellitesLabel = lv_label_create(ui_SatellitesPanel);
lv_obj_set_width( ui_SatellitesLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SatellitesLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_SatellitesLabel, LV_ALIGN_CENTER );
lv_label_set_text(ui_SatellitesLabel,"4");
lv_obj_set_style_text_color(ui_SatellitesLabel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_SatellitesLabel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_SatellitesLabel, &ui_font_Display40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_DegreePanel = lv_obj_create(ui_BodyPanel);
lv_obj_set_width( ui_DegreePanel, 120);
lv_obj_set_height( ui_DegreePanel, 80);
lv_obj_set_x( ui_DegreePanel, -10 );
lv_obj_set_y( ui_DegreePanel, 0 );
lv_obj_set_align( ui_DegreePanel, LV_ALIGN_TOP_RIGHT );
lv_obj_clear_flag( ui_DegreePanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_border_color(ui_DegreePanel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_DegreePanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_DegreePanel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_DegreeTitle = lv_label_create(ui_DegreePanel);
lv_obj_set_width( ui_DegreeTitle, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_DegreeTitle, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_DegreeTitle, 0 );
lv_obj_set_y( ui_DegreeTitle, -10 );
lv_label_set_text(ui_DegreeTitle,"Degree");
lv_obj_set_style_text_font(ui_DegreeTitle, &ui_font_Display16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_DegreeLabel = lv_label_create(ui_DegreePanel);
lv_obj_set_width( ui_DegreeLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_DegreeLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_DegreeLabel, LV_ALIGN_CENTER );
lv_label_set_text(ui_DegreeLabel,"360");
lv_obj_set_style_text_color(ui_DegreeLabel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_DegreeLabel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_DegreeLabel, &ui_font_Display40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_CoordinatesPanel = lv_obj_create(ui_BodyPanel);
lv_obj_set_width( ui_CoordinatesPanel, 274);
lv_obj_set_height( ui_CoordinatesPanel, 120);
lv_obj_set_x( ui_CoordinatesPanel, 10 );
lv_obj_set_y( ui_CoordinatesPanel, 94 );
lv_obj_clear_flag( ui_CoordinatesPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_border_color(ui_CoordinatesPanel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_CoordinatesPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_CoordinatesPanel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_CoordinatesTitle = lv_label_create(ui_CoordinatesPanel);
lv_obj_set_width( ui_CoordinatesTitle, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_CoordinatesTitle, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_CoordinatesTitle, 0 );
lv_obj_set_y( ui_CoordinatesTitle, -10 );
lv_label_set_text(ui_CoordinatesTitle,"Coordinates");
lv_obj_set_style_text_font(ui_CoordinatesTitle, &ui_font_Display16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_CoordinatesText = lv_label_create(ui_CoordinatesPanel);
lv_obj_set_width( ui_CoordinatesText, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_CoordinatesText, LV_SIZE_CONTENT);   /// 1
lv_label_set_text(ui_CoordinatesText,"LAT:\nLNG:");
lv_obj_set_style_text_color(ui_CoordinatesText, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_CoordinatesText, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_CoordinatesText, &ui_font_Display40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_CoordinatesLabel = lv_label_create(ui_CoordinatesPanel);
lv_obj_set_width( ui_CoordinatesLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_CoordinatesLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_CoordinatesLabel, 62 );
lv_obj_set_y( ui_CoordinatesLabel, 0 );
lv_label_set_text(ui_CoordinatesLabel,"37.3346\n-122.0090");
lv_obj_set_style_text_color(ui_CoordinatesLabel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_CoordinatesLabel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_CoordinatesLabel, &ui_font_Display40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedPanel2 = lv_obj_create(ui_BodyPanel);
lv_obj_set_width( ui_SpeedPanel2, 274);
lv_obj_set_height( ui_SpeedPanel2, 80);
lv_obj_set_x( ui_SpeedPanel2, 10 );
lv_obj_set_y( ui_SpeedPanel2, 228 );
lv_obj_clear_flag( ui_SpeedPanel2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_border_color(ui_SpeedPanel2, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_SpeedPanel2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_SpeedPanel2, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedTitle2 = lv_label_create(ui_SpeedPanel2);
lv_obj_set_width( ui_SpeedTitle2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SpeedTitle2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_SpeedTitle2, 0 );
lv_obj_set_y( ui_SpeedTitle2, -10 );
lv_label_set_text(ui_SpeedTitle2,"SPEED (MPH)");
lv_obj_set_style_text_font(ui_SpeedTitle2, &ui_font_Display16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpeedLabel2 = lv_label_create(ui_SpeedPanel2);
lv_obj_set_width( ui_SpeedLabel2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_SpeedLabel2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_SpeedLabel2, LV_ALIGN_CENTER );
lv_label_set_text(ui_SpeedLabel2,"120");
lv_obj_set_style_text_color(ui_SpeedLabel2, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_SpeedLabel2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_SpeedLabel2, &ui_font_Display40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_AltitudePanel = lv_obj_create(ui_BodyPanel);
lv_obj_set_width( ui_AltitudePanel, 274);
lv_obj_set_height( ui_AltitudePanel, 80);
lv_obj_set_x( ui_AltitudePanel, 10 );
lv_obj_set_y( ui_AltitudePanel, 322 );
lv_obj_clear_flag( ui_AltitudePanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_border_color(ui_AltitudePanel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_AltitudePanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_AltitudePanel, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_AltitudeTitle = lv_label_create(ui_AltitudePanel);
lv_obj_set_width( ui_AltitudeTitle, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_AltitudeTitle, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_AltitudeTitle, 0 );
lv_obj_set_y( ui_AltitudeTitle, -10 );
lv_label_set_text(ui_AltitudeTitle,"Altitude (FEET)");
lv_obj_set_style_text_font(ui_AltitudeTitle, &ui_font_Display16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_AltitudeLabel = lv_label_create(ui_AltitudePanel);
lv_obj_set_width( ui_AltitudeLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_AltitudeLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_AltitudeLabel, LV_ALIGN_CENTER );
lv_label_set_text(ui_AltitudeLabel,"150.00");
lv_obj_set_style_text_color(ui_AltitudeLabel, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_AltitudeLabel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_AltitudeLabel, &ui_font_Display40, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_TopButton2, ui_event_TopButton2, LV_EVENT_ALL, NULL);

}

void ui_init( void )
{
lv_disp_t *dispp = lv_disp_get_default();
lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
lv_disp_set_theme(dispp, theme);
ui_Screen1_screen_init();
ui_Screen2_screen_init();
lv_disp_load_scr( ui_Screen1);
}
