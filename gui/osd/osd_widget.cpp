#include "osd_widget.h"

void OSDWidget::init_container(lv_obj_t* parent) {
    container = lv_obj_create(parent);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_outline_width(container, 0, 0);
    lv_obj_set_style_shadow_width(container, 0, 0);
}

void OSDWidget::set_visible(bool visible) {
    lv_obj_add_flag(container, visible ? LV_OBJ_FLAG_CLICKABLE : LV_OBJ_FLAG_HIDDEN);
}

void OSDWidget::set_text_prefix(const std::string& p) {
    prefix = p;
}

void OSDWidget::set_show_unit(bool show) {
    show_unit = show;
}

void OSDWidget::set_icon_src(const std::string& icon_path) {
    if (!icon) {
        icon = lv_img_create(container);
    }
    lv_img_set_src(icon, icon_path.c_str());
}

void OSDWidget::set_position(int norm_x, int norm_y) {
    if (!container) return;

    lv_obj_t* parent = lv_obj_get_parent(container);
    if (!parent) return;

    lv_coord_t screen_w = lv_obj_get_width(parent);
    lv_coord_t screen_h = lv_obj_get_height(parent);

    abs_x = (norm_x * screen_w) / 1000;
    abs_y = (norm_y * screen_h) / 1000;

    // Center alignment: subtract half of container size
    lv_coord_t obj_w = lv_obj_get_width(container);
    lv_coord_t obj_h = lv_obj_get_height(container);

    lv_obj_set_pos(container,
        abs_x - obj_w / 2,
        abs_y - obj_h / 2);
}

void OSDWidget::update_position() {
    if (!container) return;

    lv_obj_t* parent = lv_obj_get_parent(container);
    if (!parent) return;

    lv_coord_t screen_w = lv_obj_get_width(parent);
    lv_coord_t screen_h = lv_obj_get_height(parent);

    // Center alignment: subtract half of container size
    lv_coord_t obj_w = lv_obj_get_width(container);
    lv_coord_t obj_h = lv_obj_get_height(container);

    lv_obj_set_pos(container,
        abs_x - obj_w / 2,
        abs_y - obj_h / 2);
}

void OSDWidget::set_color(lv_color_t coloe) {

}
