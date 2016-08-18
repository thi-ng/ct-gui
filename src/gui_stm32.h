#pragma once

#include "stm32746g_discovery_ts.h"

#include "gui.h"

CT_BEGIN_DECLS

void ctgui_update_touch(TS_StateTypeDef *raw, CTGUI_TouchState *touch);

CT_END_DECLS
