SET(SOURCES screens/ui_PickerScreen.c
    screens/ui_PlayerScreen.c
    ui.c
    ui_helpers.c
    ui_events.cpp
    images/ui_img_1392977354.c
    images/ui_img_1487412202.c
    images/ui_img_1013023930.c
    images/ui_img_1136121667.c
    images/ui_img_11094916.c
    fonts/ui_font_Quadron.c)

add_library(ui ${SOURCES})
