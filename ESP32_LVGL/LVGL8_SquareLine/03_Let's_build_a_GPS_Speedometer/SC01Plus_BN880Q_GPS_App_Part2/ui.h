// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.3.3
// PROJECT: GPS_SPEEDOMETER

#ifndef _GPS_SPEEDOMETER_UI_H
#define _GPS_SPEEDOMETER_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_RootPanel;
extern lv_obj_t *ui_TopPanel;
void ui_event_TopButton1( lv_event_t * e);
extern lv_obj_t *ui_TopButton1;
extern lv_obj_t *ui_Title;
extern lv_obj_t *ui_SpeedPanel;
extern lv_obj_t *ui_SpeedBG;
extern lv_obj_t *ui_SpeedSlider;
extern lv_obj_t *ui_SpeedMinLabel;
extern lv_obj_t *ui_SpeedMaxLabel;
extern lv_obj_t *ui_CompassPanel;
extern lv_obj_t *ui_CompassImg;
extern lv_obj_t *ui_CompassLabel;
extern lv_obj_t *ui_SpeedTextPanel;
extern lv_obj_t *ui_SpeedUnitLabel;
extern lv_obj_t *ui_SpeedLabelBack;
extern lv_obj_t *ui_SpeedLabel;
extern lv_obj_t *ui_GPSWaitingSpinner;
extern lv_obj_t *ui_Screen2;
extern lv_obj_t *ui_RootPanel1;
extern lv_obj_t *ui_TopPanel1;
void ui_event_TopButton2( lv_event_t * e);
extern lv_obj_t *ui_TopButton2;
extern lv_obj_t *ui_Title1;
extern lv_obj_t *ui_BodyPanel;
extern lv_obj_t *ui_SatellitesPanel;
extern lv_obj_t *ui_SatellitesTitle;
extern lv_obj_t *ui_SatellitesLabel;
extern lv_obj_t *ui_DegreePanel;
extern lv_obj_t *ui_DegreeTitle;
extern lv_obj_t *ui_DegreeLabel;
extern lv_obj_t *ui_CoordinatesPanel;
extern lv_obj_t *ui_CoordinatesTitle;
extern lv_obj_t *ui_CoordinatesText;
extern lv_obj_t *ui_CoordinatesLabel;
extern lv_obj_t *ui_SpeedPanel2;
extern lv_obj_t *ui_SpeedTitle2;
extern lv_obj_t *ui_SpeedLabel2;
extern lv_obj_t *ui_AltitudePanel;
extern lv_obj_t *ui_AltitudeTitle;
extern lv_obj_t *ui_AltitudeLabel;


LV_IMG_DECLARE( ui_img_speedbg_png);   // assets/speedBG.png
LV_IMG_DECLARE( ui_img_speedfg_png);   // assets/SpeedFG.png
LV_IMG_DECLARE( ui_img_compass_png);   // assets/compass.png


LV_FONT_DECLARE( ui_font_Display16);
LV_FONT_DECLARE( ui_font_Display24);
LV_FONT_DECLARE( ui_font_Display90);
LV_FONT_DECLARE( ui_font_Display40);


void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
