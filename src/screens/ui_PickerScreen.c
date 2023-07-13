// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "../ui.h"

void ui_PickerScreen_screen_init(void)
{
    ui_PickerScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_PickerScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label1 = lv_label_create(ui_PickerScreen);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label1, 0);
    lv_obj_set_y(ui_Label1, -80);
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_long_mode(ui_Label1, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ui_Label1, "Select Song");
    lv_obj_set_style_text_font(ui_Label1, &ui_font_Quadron, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_songList = lv_dropdown_create(ui_PickerScreen);
    lv_dropdown_set_options(ui_songList, "");
    lv_obj_set_width(ui_songList, 200);
    lv_obj_set_height(ui_songList, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_songList, -1);
    lv_obj_set_y(ui_songList, -34);
    lv_obj_set_align(ui_songList, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_songList, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_set_style_border_color(ui_songList, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_songList, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_songList, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_startPlay = lv_imgbtn_create(ui_PickerScreen);
    lv_imgbtn_set_src(ui_startPlay, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_1392977354, NULL);
    lv_imgbtn_set_src(ui_startPlay, LV_IMGBTN_STATE_PRESSED, NULL, &ui_img_1392977354, NULL);
    lv_obj_set_height(ui_startPlay, 48);
    lv_obj_set_width(ui_startPlay, LV_SIZE_CONTENT);   /// 48
    lv_obj_set_x(ui_startPlay, 0);
    lv_obj_set_y(ui_startPlay, 45);
    lv_obj_set_align(ui_startPlay, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_startPlay, ui_event_startPlay, LV_EVENT_ALL, NULL);

}
