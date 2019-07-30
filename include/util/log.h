#ifndef __TERM_TALK_LOG_H__
#define __TERM_TALK_LOG_H__

#include <stdio.h>

#include "util/ansi.h"

#define m_log(fmt, ...) printf(fmt, ##__VA_ARGS__)

#define m_log_error(fmt, ...) printf(ANSI_RED fmt ANSI_CLEAR, ##__VA_ARGS__)

#define m_log_warning(fmt, ...) printf(ANSI_YELLOW fmt ANSI_CLEAR, ##__VA_ARGS__)


#endif

