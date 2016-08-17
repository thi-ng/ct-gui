#pragma once

#define UI_BG_COLOR ((uint32_t)0xff59626c)
#define UI_TEXT_COLOR ((uint32_t)0xffcccccc)
#define UI_FONT Font12
#define UI_SENSITIVITY 0.02f

typedef struct {
  uint16_t touchx[1];
  uint16_t touchy[1];
  uint32_t last_touch;
  uint8_t touch_detected;
  uint8_t touch_update;
} CTGUI_TouchState;

typedef struct CTGUI_Element CTGUI_Element;

typedef void (*CTGUI_ElementHandler)(CTGUI_Element *button,
				     CTGUI_TouchState *touchState);
typedef void (*CTGUI_ElementRenderer)(CTGUI_Element *button);
typedef void (*CTGUI_Callback)(CTGUI_Element *button);

// element state flags
enum {
  CTGUI_OFF        = 1,
  CTGUI_ON         = 2,
  CTGUI_HOVER      = 4,
  CTGUI_DIRTY      = 8,
  CTGUI_ONOFF_MASK = CTGUI_ON | CTGUI_OFF
};

typedef struct {
  uint16_t sprite_width;
  uint16_t sprite_height;
  uint32_t format;
  const uint8_t *pixels;
  uint8_t num_sprites;
} CTGUI_SpriteSheet;

struct CTGUI_Element {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint32_t state;
  char *label;
  void *user_data;
  CTGUI_SpriteSheet *sprite;
  CTGUI_ElementHandler handler;
  CTGUI_ElementRenderer render;
  CTGUI_Callback callback;
  uint8_t id;
};

typedef struct {
  float value;
  float start_value;
  float sensitivity;
  uint16_t startx;
  uint16_t starty;
} CTGUI_DialButtonState;

typedef struct { float value; } CTGUI_PushButtonState;

typedef struct {
  CTGUI_Element **items;
  uint32_t col_bg;
  uint32_t col_text;
  void *font;
  uint8_t num_items;
  int8_t selected;
  uint8_t redraw;
} CTGUI;

void ctgui_handle_togglebutton(CTGUI_Element *bt, CTGUI_TouchState *touch);
void ctgui_handle_radiobutton(CTGUI_Element *bt, CTGUI_TouchState *touch);
void ctgui_render_button(CTGUI_Element *bt);

void ctgui_handle_dial(CTGUI_Element *bt, CTGUI_TouchState *touch);
void ctgui_render_dial(CTGUI_Element *bt);

void ctgui_draw_sprite(uint16_t x,
		       uint16_t y,
		       uint8_t id,
		       CTGUI_SpriteSheet *sprite);
void ctgui_draw_raw(uint16_t x,
		    uint16_t y,
		    uint16_t width,
		    uint16_t height,
		    uint8_t *pixels,
		    uint32_t colorMode);

CTGUI *ctgui_init(uint8_t num, sFONT *font, uint32_t bgCol, uint32_t textCol);
void ctgui_force_redraw(CTGUI *gui);
void ctgui_update(CTGUI *gui, CTGUI_TouchState *touch);
void ctgui_draw(CTGUI *gui);
void ctgui_draw_element_label(CTGUI_Element *e);

CTGUI_Element *ctgui_elementcommon(uint8_t id,
				   char *label,
				   uint16_t x,
				   uint16_t y,
				   CTGUI_SpriteSheet *sprite,
				   CTGUI_Callback cb);
CTGUI_Element *ctgui_togglebutton(uint8_t id,
				  char *label,
				  uint16_t x,
				  uint16_t y,
				  float val,
				  CTGUI_SpriteSheet *sprite,
				  CTGUI_Callback cb);
CTGUI_Element *ctgui_radiobutton(uint8_t id,
				 char *label,
				 uint16_t x,
				 uint16_t y,
				 float val,
				 CTGUI_SpriteSheet *sprite,
				 CTGUI_Callback cb);
CTGUI_Element *ctgui_dialbutton(uint8_t id,
				char *label,
				uint16_t x,
				uint16_t y,
				float val,
				float sens,
				CTGUI_SpriteSheet *sprite,
				CTGUI_Callback cb);
