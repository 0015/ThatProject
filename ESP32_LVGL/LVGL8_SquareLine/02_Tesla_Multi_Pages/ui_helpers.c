// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.2
// LVGL VERSION: 8.2
// PROJECT: Tesla_Multi_Pages

#include "ui_helpers.h"

void _ui_bar_set_property(lv_obj_t * target, int id, int val)
{
    if(id == _UI_BAR_PROPERTY_VALUE_WITH_ANIM) lv_bar_set_value(target, val, LV_ANIM_ON);
    if(id == _UI_BAR_PROPERTY_VALUE) lv_bar_set_value(target, val, LV_ANIM_OFF);
}

void _ui_basic_set_property(lv_obj_t * target, int id, int val)
{
    if(id == _UI_BASIC_PROPERTY_POSITION_X) lv_obj_set_x(target, val);
    if(id == _UI_BASIC_PROPERTY_POSITION_Y) lv_obj_set_y(target, val);
    if(id == _UI_BASIC_PROPERTY_WIDTH) lv_obj_set_width(target, val);
    if(id == _UI_BASIC_PROPERTY_HEIGHT) lv_obj_set_height(target, val);
}


void _ui_dropdown_set_property(lv_obj_t * target, int id, int val)
{
    if(id == _UI_DROPDOWN_PROPERTY_SELECTED) lv_dropdown_set_selected(target, val);
}

void _ui_image_set_property(lv_obj_t * target, int id, uint8_t * val)
{
    if(id == _UI_IMAGE_PROPERTY_IMAGE) lv_img_set_src(target, val);
}

void _ui_label_set_property(lv_obj_t * target, int id, char * val)
{
    if(id == _UI_LABEL_PROPERTY_TEXT) lv_label_set_text(target, val);
}


void _ui_roller_set_property(lv_obj_t * target, int id, int val)
{
    if(id == _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM) lv_roller_set_selected(target, val, LV_ANIM_ON);
    if(id == _UI_ROLLER_PROPERTY_SELECTED) lv_roller_set_selected(target, val, LV_ANIM_OFF);
}

void _ui_slider_set_property(lv_obj_t * target, int id, int val)
{
    if(id == _UI_SLIDER_PROPERTY_VALUE_WITH_ANIM) lv_slider_set_value(target, val, LV_ANIM_ON);
    if(id == _UI_SLIDER_PROPERTY_VALUE) lv_slider_set_value(target, val, LV_ANIM_OFF);
}


void _ui_screen_change(lv_obj_t * target, lv_scr_load_anim_t fademode, int spd, int delay)
{
    lv_scr_load_anim(target, fademode, spd, delay, false);
}

void _ui_arc_increment(lv_obj_t * target, lv_scr_load_anim_t fademode, int spd, int delay)
{
    lv_scr_load_anim(target, fademode, spd, delay, false);
}

void _ui_bar_increment(lv_obj_t * target, lv_scr_load_anim_t fademode, int spd, int delay)
{
    lv_scr_load_anim(target, fademode, spd, delay, false);
}

void _ui_slider_increment(lv_obj_t * target, lv_scr_load_anim_t fademode, int spd, int delay)
{
    lv_scr_load_anim(target, fademode, spd, delay, false);
}

void _ui_flag_modify(lv_obj_t * target, int32_t flag, int value)
{
    if(value == _UI_MODIFY_FLAG_TOGGLE) {
        if(lv_obj_has_flag(target, flag)) lv_obj_clear_flag(target, flag);
        else lv_obj_add_flag(target, flag);
    }
    else if(value == _UI_MODIFY_FLAG_ADD) lv_obj_add_flag(target, flag);
    else lv_obj_clear_flag(target, flag);
}
void _ui_state_modify(lv_obj_t * target, int32_t state, int value)
{
    if(value == _UI_MODIFY_STATE_TOGGLE) {
        if(lv_obj_has_state(target, state)) lv_obj_clear_state(target, state);
        else lv_obj_add_state(target, state);
    }
    else if(value == _UI_MODIFY_STATE_ADD) lv_obj_add_state(target, state);
    else lv_obj_clear_state(target, state);
}

void _ui_opacity_set(lv_obj_t * target, int val)
{
    lv_obj_set_style_opa(target, val, 0);
}

void _ui_anim_callback_set_x(lv_anim_t * a, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)a->user_data, v);
}

void _ui_anim_callback_set_y(lv_anim_t * a, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)a->user_data, v);
}

void _ui_anim_callback_set_width(lv_anim_t * a, int32_t v)
{
    lv_obj_set_width((lv_obj_t *)a->user_data, v);
}

void _ui_anim_callback_set_height(lv_anim_t * a, int32_t v)
{
    lv_obj_set_height((lv_obj_t *)a->user_data, v);
}

void _ui_anim_callback_set_opacity(lv_anim_t * a, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)a->user_data, v, 0);
}

void _ui_anim_callback_set_image_zoom(lv_anim_t * a, int32_t v)
{
    lv_img_set_zoom((lv_obj_t *)a->user_data, v);
}

void _ui_anim_callback_set_image_angle(lv_anim_t * a, int32_t v)
{
    lv_img_set_angle((lv_obj_t *)a->user_data, v);
}


int32_t _ui_anim_callback_get_x(lv_anim_t * a)
{
    return lv_obj_get_x_aligned((lv_obj_t *)a->user_data);
}

int32_t _ui_anim_callback_get_y(lv_anim_t * a)
{
    return lv_obj_get_y_aligned((lv_obj_t *)a->user_data);
}

int32_t _ui_anim_callback_get_width(lv_anim_t * a)
{
    return lv_obj_get_width((lv_obj_t *)a->user_data);
}

int32_t _ui_anim_callback_get_height(lv_anim_t * a)
{
    return lv_obj_get_height((lv_obj_t *)a->user_data);
}

int32_t _ui_anim_callback_get_opacity(lv_anim_t * a)
{
    return lv_obj_get_style_opa((lv_obj_t *)a->user_data, 0);
}

int32_t _ui_anim_callback_get_image_zoom(lv_anim_t * a)
{
    return lv_img_get_zoom((lv_obj_t *)a->user_data);
}

int32_t _ui_anim_callback_get_image_angle(lv_anim_t * a)
{
    return lv_img_get_angle((lv_obj_t *)a->user_data);
}

void _ui_arc_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * prefix, char * postfix)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE];
    lv_snprintf(buf, sizeof(buf), "%s%d%s", prefix, (int)lv_arc_get_value(src), postfix);
    lv_label_set_text(trg, buf);
}

void _ui_slider_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * prefix, char * postfix)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE];
    lv_snprintf(buf, sizeof(buf), "%s%d%s", prefix, (int)lv_slider_get_value(src), postfix);
    lv_label_set_text(trg, buf);
}
void _ui_checked_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * txt_on, char * txt_off)
{
    if(lv_obj_has_state(src, LV_STATE_CHECKED)) lv_label_set_text(trg, txt_on);
    else lv_label_set_text(trg, txt_off);
}


