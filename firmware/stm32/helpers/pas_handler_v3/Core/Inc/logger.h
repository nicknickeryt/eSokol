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

void send_string(char *s);
void send_uint8(uint8_t num);
void send_int(int num);
void send_float(float num);

#endif /* __LOGGER_H */