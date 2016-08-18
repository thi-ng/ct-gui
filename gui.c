#include <stdlib.h>

#include "ct-head/features.h"
#include "ct-head/math.h"
#include "gui.h"

ct_inline int point_inside(const CTGUI_Element *e, uint16_t x, uint16_t y) {
  return (x >= e->x && x < e->x + e->width && y >= e->y &&
          y < e->y + e->height);
}

// push button functions

void ctgui_handle_togglebutton(CTGUI_Element *e,
                               const CTGUI_TouchState *touch) {
  if (touch->touch_detected) {
    uint16_t x = touch->touchx[0];
    uint16_t y = touch->touchy[0];
    if (point_inside(e, x, y)) {
      switch (e->state) {
        case CTGUI_OFF:
          e->state |= CTGUI_HOVER | CTGUI_DIRTY;
          break;
        case CTGUI_ON:
          e->state |= CTGUI_HOVER | CTGUI_DIRTY;
          break;
        default:
          break;
      }
    }
  } else if (e->state & CTGUI_HOVER) {
    e->state &= ~((uint16_t)CTGUI_HOVER);
    e->state |= CTGUI_DIRTY;
    e->state ^= CTGUI_ONOFF_MASK;
    if (e->callback != NULL) {
      e->callback(e);
    }
  }
}

void ctgui_handle_radiobutton(CTGUI_Element *e, const CTGUI_TouchState *touch) {
  if (touch->touch_detected) {
    uint16_t x = touch->touchx[0];
    uint16_t y = touch->touchy[0];
    if (point_inside(e, x, y)) {
      if (e->state == CTGUI_OFF) {
        e->state |= CTGUI_HOVER | CTGUI_DIRTY;
      }
    }
  } else if (e->state & CTGUI_HOVER) {
    e->state = CTGUI_ON | CTGUI_DIRTY;
    if (e->callback != NULL) {
      e->callback(e);
    }
  }
}

void ctgui_render_button(const CTGUI_Element *e) {
  CTGUI_SpriteSheet *sprite = e->sprite;
  uint8_t state             = e->state & CTGUI_ONOFF_MASK;
  uint8_t id                = state == CTGUI_ON ? 1 : 0;
  ctgui_draw_sprite(sprite, e->x, e->y, id);
  ctgui_draw_element_label(e);
}

// dial button functions

void ctgui_handle_dial(CTGUI_Element *e, const CTGUI_TouchState *touch) {
  if (touch->touch_detected) {
    uint16_t x                = touch->touchx[0];
    uint16_t y                = touch->touchy[0];
    CTGUI_DialButtonState *db = (CTGUI_DialButtonState *)e->user_data;
    if (e->state == CTGUI_HOVER) {
      // int16_t dx = (x - db->startx);
      // int16_t dy = (y - db->starty);
      // int16_t delta = abs(dx) > abs(dy) ? dx : dy;
      int16_t delta = (x - db->startx);
      float newVal  = db->start_value + db->sensitivity * delta;
      db->value     = ct_clampf(newVal, 0.0f, 1.0f);
      e->state |= CTGUI_DIRTY;
      if (e->callback != NULL) {
        e->callback(e);
      }
    } else if (point_inside(e, x, y)) {
      e->state        = CTGUI_HOVER;
      db->startx      = x;
      db->starty      = y;
      db->start_value = db->value;
    }
  } else if (e->state == CTGUI_HOVER) {
    e->state = CTGUI_OFF | CTGUI_DIRTY;
  }
}

void ctgui_render_dial(const CTGUI_Element *e) {
  CTGUI_SpriteSheet *sprite = e->sprite;
  CTGUI_DialButtonState *db = (CTGUI_DialButtonState *)e->user_data;
  uint8_t id = (uint8_t)(db->value * (float)(sprite->num_sprites - 1));
  ctgui_draw_sprite(sprite, e->x, e->y, id);
  ctgui_draw_element_label(e);
}

// CTGUI element constructors

CTGUI_Element *ctgui_element_init(CTGUI *gui,
                                  uint32_t id,
                                  char *label,
                                  uint16_t x,
                                  uint16_t y,
                                  CTGUI_SpriteSheet *sprite,
                                  CTGUI_Callback cb) {
  CTGUI_Element *e = &gui->items[id];
  e->id            = id;
  e->x             = x;
  e->y             = y;
  e->width         = sprite->sprite_width;
  e->height        = sprite->sprite_height;
  e->sprite        = sprite;
  e->label         = label;
  e->callback      = cb;
  e->state         = CTGUI_OFF | CTGUI_DIRTY;
  return e;
}

CTGUI_Element *ctgui_dialbutton(CTGUI *gui,
                                uint32_t id,
                                char *label,
                                uint16_t x,
                                uint16_t y,
                                float val,
                                float sens,
                                CTGUI_SpriteSheet *sprite,
                                CTGUI_Callback cb) {
  CTGUI_Element *e = ctgui_element_init(gui, id, label, x, y, sprite, cb);
  CTGUI_DialButtonState *db = calloc(1, sizeof(CTGUI_DialButtonState));
  e->handler                = ctgui_handle_dial;
  e->render                 = ctgui_render_dial;
  e->user_data              = db;
  db->value                 = val;
  db->sensitivity           = sens;
  return e;
}

CTGUI_Element *ctgui_togglebutton(CTGUI *gui,
                                  uint32_t id,
                                  char *label,
                                  uint16_t x,
                                  uint16_t y,
                                  float val,
                                  CTGUI_SpriteSheet *sprite,
                                  CTGUI_Callback cb) {
  CTGUI_Element *e = ctgui_element_init(gui, id, label, x, y, sprite, cb);
  CTGUI_PushButtonState *pb = calloc(1, sizeof(CTGUI_PushButtonState));
  e->handler                = ctgui_handle_togglebutton;
  e->render                 = ctgui_render_button;
  e->user_data              = pb;
  pb->value                 = val;
  return e;
}

CTGUI_Element *ctgui_radiobutton(CTGUI *gui,
                                 uint32_t id,
                                 char *label,
                                 uint16_t x,
                                 uint16_t y,
                                 float val,
                                 CTGUI_SpriteSheet *sprite,
                                 CTGUI_Callback cb) {
  CTGUI_Element *e = ctgui_element_init(gui, id, label, x, y, sprite, cb);
  CTGUI_PushButtonState *pb = calloc(1, sizeof(CTGUI_PushButtonState));
  e->handler                = ctgui_handle_radiobutton;
  e->render                 = ctgui_render_button;
  e->user_data              = pb;
  pb->value                 = val;
  return e;
}

CTGUI *ctgui_init(CTGUI *gui,
                  uint32_t num,
                  void *font,
                  uint32_t bgCol,
                  uint32_t textCol) {
  gui->items     = calloc(num, sizeof(CTGUI_Element));
  gui->num_items = num;
  gui->font      = font;
  gui->col_bg    = bgCol;
  gui->col_text  = textCol;
  return gui;
}

void ctgui_force_redraw(CTGUI *gui) {
  gui->redraw = 1;
  for (size_t i = 0; i < gui->num_items; i++) {
    gui->items[i].state |= CTGUI_DIRTY;
  }
}

void ctgui_update(CTGUI *gui, const CTGUI_TouchState *touch) {
  for (size_t i = 0; i < gui->num_items; i++) {
    CTGUI_Element *e = &gui->items[i];
    e->handler(e, touch);
  }
}
