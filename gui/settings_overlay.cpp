#include "lvgl.h"
#include "settings_overlay.h"

#include <map>
#include <string>

static lv_obj_t* overlay_root = nullptr;
static lv_obj_t* sidebar = nullptr;
static lv_obj_t* content_area = nullptr;
static std::map<std::string, lv_obj_t*> category_panels;
static lv_group_t* input_group = nullptr;

static void switch_to_category(const std::string& id) {
    for (auto& [key, panel] : category_panels) {
        lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_clear_flag(category_panels[id], LV_OBJ_FLAG_HIDDEN);
}

static void on_category_btn_click(lv_event_t* e) {
    printf("HI");
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED) return;

    const char* category_id = static_cast<const char*>(lv_event_get_user_data(e));
    switch_to_category(category_id);
}

static lv_obj_t* create_category_panel(const std::string& id, const char* title_text) {
    lv_obj_t* panel = lv_obj_create(content_area);
    lv_obj_set_size(panel, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  //  lv_obj_set_style_bg_opa(panel, LV_OPA_0, 0);
    lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN);
    lv_obj_t* label = lv_label_create(panel);
    lv_label_set_text_fmt(label, "%s Settings", title_text);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
    return panel;
}

void settings_overlay_create() {
    if (overlay_root) return; // already exists

    // Create root object
    overlay_root = lv_obj_create(lv_scr_act());

    // Set size: 80% of screen, centered
    lv_coord_t scr_w = lv_obj_get_width(lv_scr_act());
    lv_coord_t scr_h = lv_obj_get_height(lv_scr_act());

    lv_obj_set_size(overlay_root, LV_PCT(80),  LV_PCT(100));
    lv_obj_align(overlay_root,  LV_ALIGN_LEFT_MID, 0, 0);

    // Semi-transparent bluish background
    lv_obj_set_style_bg_color(overlay_root, lv_color_make(30, 60, 150), 0);
  //  lv_obj_set_style_bg_opa(overlay_root, LV_OPA_80, 0);
    lv_obj_clear_flag(overlay_root, LV_OBJ_FLAG_SCROLLABLE);

    // Flex layout
    lv_obj_set_flex_flow(overlay_root, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(overlay_root, 8, 0);

    // Sidebar
    sidebar = lv_obj_create(overlay_root);
    lv_obj_set_size(sidebar, LV_PCT(30),  LV_PCT(100));
    lv_obj_set_style_bg_color(sidebar, lv_color_make(30, 60, 150), 0);
  //  lv_obj_set_style_bg_opa(sidebar, LV_OPA_80, 0);
    lv_obj_set_flex_flow(sidebar, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(sidebar, 8, 0);
    lv_obj_set_style_pad_gap(sidebar, 6, 0);

    // Content area
    content_area = lv_obj_create(overlay_root);
    lv_obj_set_size(content_area, LV_PCT(50),  LV_PCT(100));
    lv_obj_set_style_bg_color(content_area, lv_color_make(30, 60, 150), 0);
 //   lv_obj_set_style_bg_opa(content_area, LV_OPA_80, 0);
    lv_obj_set_flex_grow(content_area, 1);
    lv_obj_set_style_pad_all(content_area, 12, 0);
    lv_obj_set_scroll_dir(content_area, LV_DIR_VER);


    // Create input group for joystick
    input_group = lv_group_create();
 //   lv_indev_set_group(joystick_indev, input_group);

    // Categories
    struct {
        const char* id;
        const char* label;
        const char* icon;
    } categories[] = {
        { "status", "Status", LV_SYMBOL_WARNING },
        { "camera", "Camera", LV_SYMBOL_IMAGE },
        { "video", "Video", LV_SYMBOL_VIDEO },
        { "transmission", "Transmission", LV_SYMBOL_WIFI },
        { "display", "Display", LV_SYMBOL_SETTINGS },
        { "recording", "Recording", LV_SYMBOL_SD_CARD },
        { "osd", "OSD", LV_SYMBOL_EYE_OPEN },
        { "device", "Device", LV_SYMBOL_CHARGE },
        { "about", "About", LV_SYMBOL_HOME }
    };

    for (const auto& cat : categories) {
        lv_obj_t* btn = lv_btn_create(sidebar);
        lv_obj_set_width(btn, LV_PCT(100));
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "%s  %s", cat.icon, cat.label);
        lv_obj_center(label);

        lv_obj_add_event_cb(btn, on_category_btn_click, LV_EVENT_CLICKED, (void*)cat.id);

        lv_group_add_obj(input_group, btn);

        // Create empty panel for now
        category_panels[cat.id] = create_category_panel(cat.id, cat.label);
    }

    switch_to_category("camera"); // default view
}

void settings_overlay_destroy() {
    if (!overlay_root) return;
    lv_obj_del(overlay_root);
    overlay_root = nullptr;
    category_panels.clear();
    input_group = nullptr;
}
