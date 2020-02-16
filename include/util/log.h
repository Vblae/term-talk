#ifndef __TERM_TALK_LOG_H__
#define __TERM_TALK_LOG_H__

#include <stdio.h>

#include "util/ansi.h"

#define LOGD(fmt, ...) printf(fmt, ##__VA_ARGS__)

#define LOGE(fmt, ...) printf(ANSI_RED fmt ANSI_CLEAR, ##__VA_ARGS__)

#define LOGW(fmt, ...) printf(ANSI_YELLOW fmt ANSI_CLEAR, ##__VA_ARGS__)

#endif

