#pragma once

#define CUAVCAN_MAX_SUBSCRIBED_MESSAGES		3

#include <stdio.h>
#define CUAVCAN_DEBUG(fmt, ...) printf(fmt, ##__VA_ARGS__);printf("\n");