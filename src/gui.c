#include <stdlib.h>

#include "ct-head/features.h"
#include "ct-head/math.h"
#include "gui.h"

extern void ctgui_draw_element_label(CTGUI_Element *e);
extern void ctgui_draw_sprite(uint16_t x,
			      uint16_t y,
			      uint8_t id,
			      CTGUI_SpriteSheet *sprite);
extern void ctgui_draw_raw(uint16_t x,
			   uint16_t y,
			   uint16_t width,
			   uint16_t height,
			   uint8_t *pixels,
			   uint32_t colorMode);

ct_inline int point_inside(const CTGUI_Element *e, uint16_t x, uint16_t y) {
  return (x >= e->x && x < e->x + e->width && y >= e->y &&
	  y < e->y + e->height);
}

// push button functions

void ctgui_handle_togglebutton(CTGUI_Element *e, CTGUI_TouchState *touch) {
  if (touch->touchDetected) {
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

void ctgui_handle_radiobutton(CTGUI_Element *e, CTGUI_TouchState *touch) {
  if (touch->touchDetected) {
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

void ctgui_render_button(CTGUI_Element *e) {
  if (e->state & CTGUI_DIRTY) {
    CTGUI_SpriteSheet *sprite = e->sprite;
    uint8_t state             = e->state & CTGUI_ONOFF_MASK;
    uint8_t id                = state == CTGUI_ON ? 1 : 0;
    ctgui_draw_sprite(e->x, e->y, id, sprite);
    ctgui_draw_element_label(e);
    e->state &= ~((uint16_t)CTGUI_DIRTY);
  }
}

// dial button functions

void ctgui_handle_dial(CTGUI_Element *e, CTGUI_TouchState *touch) {
  if (touch->touchDetected) {
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

void ctgui_render_dial(CTGUI_Element *e) {
  if (e->state & CTGUI_DIRTY) {
    CTGUI_SpriteSheet *sprite = e->sprite;
    CTGUI_DialButtonState *db = (CTGUI_DialButtonState *)e->user_data;
    uint8_t id = (uint8_t)(db->value * (float)(sprite->num_sprites - 1));
    ctgui_draw_sprite(e->x, e->y, id, sprite);
    ctgui_draw_element_label(e);
    e->state &= ~((uint16_t)CTGUI_DIRTY);
  }
}

// common functionality

// CTGUI element constructors

CTGUI_Element *ctgui_element_init(uint8_t id,
				  char *label,
				  uint16_t x,
				  uint16_t y,
				  CTGUI_SpriteSheet *sprite,
				  CTGUI_Callback cb) {
  CTGUI_Element *e = calloc(1, sizeof(CTGUI_Element));
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

CTGUI_Element *ctgui_dialbutton(uint8_t id,
				char *label,
				uint16_t x,
				uint16_t y,
				float val,
				float sens,
				CTGUI_SpriteSheet *sprite,
				CTGUI_Callback cb) {
  CTGUI_Element *e          = ctgui_element_init(id, label, x, y, sprite, cb);
  CTGUI_DialButtonState *db = calloc(1, sizeof(CTGUI_DialButtonState));
  e->handler                = handleDial;
  e->render                 = ctgui_render_dial;
  e->user_data              = db;
  db->value                 = val;
  db->sensitivity           = sens;
  return e;
}

CTGUI_Element *ctgui_togglebutton(uint8_t id,
				  char *label,
				  uint16_t x,
				  uint16_t y,
				  float val,
				  CTGUI_SpriteSheet *sprite,
				  CTGUI_Callback cb) {
  CTGUI_Element *e          = ctgui_element_init(id, label, x, y, sprite, cb);
  CTGUI_PushButtonState *pb = calloc(1, sizeof(CTGUI_PushButtonState));
  e->handler                = handleToggleButton;
  e->render                 = renderButton;
  e->user_data              = pb;
  pb->value                 = val;
  return e;
}

CTGUI_Element *ctgui_radiobutton(uint8_t id,
				 char *label,
				 uint16_t x,
				 uint16_t y,
				 float val,
				 CTGUI_SpriteSheet *sprite,
				 CTGUI_Callback cb) {
  CTGUI_Element *e          = ctgui_element_init(id, label, x, y, sprite, cb);
  CTGUI_PushButtonState *pb = calloc(1, sizeof(CTGUI_PushButtonState));
  e->handler                = handleRadioButton;
  e->render                 = renderButton;
  e->user_data              = pb;
  pb->value                 = val;
  return e;
}

CTGUI *ctgui_init(uint8_t num, void *font, uint32_t bgCol, uint32_t textCol) {
  CTGUI *gui     = calloc(1, sizeof(CTGUI));
  gui->items     = calloc(num, sizeof(CTGUI_Element *));
  gui->numItems  = num;
  gui->font      = font;
  gui->bgColor   = bgCol;
  gui->textColor = textCol;
  return gui;
}

void ctgui_force_redraw(CTGUI *gui) {
  gui->redraw = 1;
  for (uint8_t i = 0; i < gui->numItems; i++) {
    gui->items[i]->state |= CTGUI_DIRTY;
  }
}

void ctgui_update(CTGUI *gui, CTGUI_TouchState *touch) {
  for (uint8_t i = 0; i < gui->numItems; i++) {
    CTGUI_Element *e = gui->items[i];
    e->handler(e, touch);
  }
}

void ctgui_draw(CTGUI *gui) {
  BSP_LCD_SetFont(gui->font);
  BSP_LCD_SetBackColor(gui->bgColor);
  BSP_LCD_SetTextColor(gui->textColor);
  for (uint8_t i = 0; i < gui->numItems; i++) {
    CTGUI_Element *e = gui->items[i];
    e->render(e);
  }
  gui->redraw = 0;
}

void ctgui_update_touch(TS_StateTypeDef *raw, CTGUI_TouchState *touch) {
  touch->touchDetected = raw->touchDetected;
  touch->touchx[0]     = raw->touchx[0];
  touch->touchy[0]     = raw->touchy[0];
}
