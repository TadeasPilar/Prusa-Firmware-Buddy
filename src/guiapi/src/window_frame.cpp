// window_frame.c
#include "window_frame.hpp"
#include "gui.hpp"
#include "sound_C_wrapper.h"

void window_frame_init(window_frame_t *window) {
    if (rect_empty_ui16(window->rect)) //use display rect curent is empty
        window->rect = rect_ui16(0, 0, display::GetW(), display::GetH());
    window->flg |= WINDOW_FLG_ENABLED | WINDOW_FLG_PARENT;
    window->color_back = COLOR_BLACK;
}

void window_frame_done(window_frame_t *window) {
}

void window_frame_draw(window_frame_t *window) {
    if (window->f_visible) {
        if (window->f_invalid) {
            rect_ui16_t rc = window->rect;
            display::FillRect(rc, window->color_back);
            window->f_invalid = 0;
            window_invalidate_children(window->id);
        }
        window_draw_children(window->id);
    }
}

void window_frame_event(window_frame_t *window, uint8_t event, void *param) {
    int16_t id;
    int dif;
    switch (event) {
    case WINDOW_EVENT_BTN_DN:
        if (window_focused_ptr && window_focused_ptr->f_tag)
            screen_dispatch_event(window_focused_ptr, WINDOW_EVENT_CLICK, (void *)(int)window_focused_ptr->f_tag);
        window_set_capture(window_focused());
        break;
    case WINDOW_EVENT_ENC_DN:
        dif = (int)param;
        id = window_focused();
        while (dif--) {
            id = window_prev_enabled(id);
        }
        if (id >= 0) {
            window_set_focus(id);
        } else {
            // End indicator of the frames list ->
            Sound_Play(eSOUND_TYPE_BlindAlert);
        }
        break;
    case WINDOW_EVENT_ENC_UP:
        dif = (int)param;
        id = window_focused();
        while (dif--) {
            id = window_next_enabled(id);
        }
        if (id >= 0) {
            window_set_focus(id);
        } else {
            // Start indicator of the frames list <-
            Sound_Play(eSOUND_TYPE_BlindAlert);
        }
        break;
    case WINDOW_EVENT_CAPT_0:
        break;
    case WINDOW_EVENT_CAPT_1:
        if (window_parent(window_focused()) != window->id) {
            id = window_first_child(0);
            if (!window_is_enabled(id))
                id = window_next_enabled(id);
            window_set_focus(id);
        }
        break;
    }
}

const window_class_frame_t window_class_frame = {
    {
        WINDOW_CLS_FRAME,
        sizeof(window_frame_t),
        (window_init_t *)window_frame_init,
        (window_done_t *)window_frame_done,
        (window_draw_t *)window_frame_draw,
        (window_event_t *)window_frame_event,
    },
};