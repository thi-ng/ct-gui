#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

extern LTDC_HandleTypeDef hLtdcHandler;

extern void LL_ConvertLineToARGB8888(void *src, void *dst, uint32_t xstride,
				     uint32_t color_mode);

void ctgui_draw_element_label(CTGUI_Element *e) {
  if (e->label != NULL) {
    CTGUI_SpriteSheet *sprite = e->sprite;
    BSP_LCD_DisplayStringAt(e->x, e->y + sprite->sprite_height + 4,
			    (uint8_t *)e->label, LEFT_MODE);
  }
}

static uint8_t colormode_strides[] = {4, 3, 2, 2, 2};

void ctgui_draw_sprite(uint16_t x, uint16_t y, uint8_t id,
		       CTGUI_SpriteSheet *sprite) {
  uint32_t lcdWidth = BSP_LCD_GetXSize();
  uint32_t address = hLtdcHandler.LayerCfg[LTDC_ACTIVE_LAYER].FBStartAdress +
		     (((lcdWidth * y) + x) << 2);
  uint16_t width = sprite->sprite_width;
  uint16_t height = sprite->sprite_height;
  uint8_t *pixels = (uint8_t *)sprite->pixels;
  uint32_t stride = colormode_strides[sprite->format];
  pixels += (id * width * height) * stride;
  stride *= width;
  lcdWidth <<= 2;
  while (--height) {
    LL_ConvertLineToARGB8888(pixels, (uint32_t *)address, width,
			     sprite->format);
    address += lcdWidth;
    pixels += stride;
  }
}

void ctgui_draw_raw(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		    uint8_t *pixels, uint32_t color_mode) {
  uint32_t lcdWidth = BSP_LCD_GetXSize();
  uint32_t address = hLtdcHandler.LayerCfg[LTDC_ACTIVE_LAYER].FBStartAdress +
		     (((lcdWidth * y) + x) << 2);
  uint16_t stride = width * colormode_strides[color_mode];
  lcdWidth <<= 2;
  while (--height) {
    LL_ConvertLineToARGB8888(pixels, (uint32_t *)address, width, color_mode);
    address += lcdWidth;
    pixels += stride;
  }
}
