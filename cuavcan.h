//MIT License
//
//Copyright(c) 2018 High Flyers
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once

#include <stdint.h>

#include <cuavcan_config.h>

#define CUAVCAN_MESSAGE_MAX_LENGTH		128

typedef struct
{
	uint16_t id;
	uint8_t source;
	uint8_t payload[CUAVCAN_MESSAGE_MAX_LENGTH];
	uint8_t length;
} cuavcan_message_t;

typedef struct
{
	cuavcan_message_t msg;
	uint8_t is_complete;
} cuavcan_message_assembly_t;

typedef struct
{
	uint16_t node_id;
	cuavcan_message_assembly_t msgs[CUAVCAN_MAX_SUBSCRIBED_MESSAGES];
} cuavcan_instance_t;

void cuavcan_init(cuavcan_instance_t *uavcan, uint16_t *subscribed_ids, uint8_t subscribed_ids_length);

void cuavcan_handle_can_frame(cuavcan_instance_t *uavcan, uint32_t id, uint8_t *payload, uint8_t length);

