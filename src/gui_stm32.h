#pragma once

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

#include "gui.h"

#ifndef CTGUI_FONT
#define CTGUI_FONT Font12
#endif

CT_BEGIN_DECLS

void ctgui_update_touch(TS_StateTypeDef *raw, CTGUI_TouchState *touch);

CT_END_DECLS
