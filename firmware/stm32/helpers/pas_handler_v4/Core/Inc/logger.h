/*
 * logger.h
 *
 * Contains logging utilities
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#ifndef __LOGGER_H
#define __LOGGER_H

#include <stdint.h>

char* logger_floatToChar(float f);

void logger_sendChar(char *s);
void logger_sendUint8(uint8_t num);
void logger_sendUint32(uint32_t num);
void logger_sendInt(int num);
void logger_sendFloat(float num);

#endif /* __LOGGER_H */