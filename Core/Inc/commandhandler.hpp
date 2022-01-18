/*
 * commandhandler.hpp
 *
 *  Created on: Jan 17, 2022
 *      Author: peter
 */

#include "main.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void parseInput(uint8_t* buffer, size_t length);
void loop();

#ifdef __cplusplus
}
#endif
