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

#include <cuavcan.h>
#include <string.h>

void cuavcan_init(cuavcan_instance_t * uavcan, uint16_t * subscribed_ids, uint8_t subscribed_ids_length, void(*on_new_message)(cuavcan_message_t*))
{
	CUAVCAN_DEBUG(__FUNCTION__);
	uavcan->on_new_message = on_new_message;
	uavcan->subscribed_ids = subscribed_ids;
	uavcan->subscribed_ids_length = subscribed_ids_length;
	for (unsigned i = 0; i < subscribed_ids_length; ++i)
	{
		cuavcan_message_assembly_t *msg = (uavcan->msgs) + i;
		cuavcan_message_assembly_reset(msg);
		msg->msg.id = subscribed_ids[i];
	}
}

void cuavcan_message_assembly_reset(cuavcan_message_assembly_t * message)
{
	message->expected_toggle = 0;
	message->msg.length = 0;
	message->is_complete = 0;
	message->is_initialized = 0;
}

void cuavcan_handle_can_frame(cuavcan_instance_t * uavcan, uint32_t id, uint8_t * payload, uint8_t length)
{
	uint16_t msg_id = cuavcan_get_message_id(id);
	cuavcan_message_assembly_t *msg = cuavcan_find_message_assembly(uavcan, msg_id);

	CUAVCAN_DEBUG_NO_NEWLINE(__FUNCTION__ ": msg_id = %d ", msg_id);
	if (msg != NULL)
	{
		cuavcan_tail_byte_t tail_byte;
		cuavcan_parse_tail_byte(payload, length, &tail_byte);
		if (cuavcan_is_transfer_single_frame(&tail_byte))
		{
			memcpy(payload, msg->msg.payload, length - 1);
			msg->msg.length = length - 1;
			CUAVCAN_DEBUG("single-frame message");
			uavcan->on_new_message(&msg->msg);
		}
		else
		{
			CUAVCAN_DEBUG("multi-frame message");
		}
	}
	else
	{
		CUAVCAN_DEBUG("Not subscribed to message: %d", msg_id);
	}
}

uint32_t cuavcan_get_message_id(uint32_t frame_id)
{
	return (frame_id >> 8) & 0xFFFF;
}

uint8_t cuavcan_get_node_id(uint32_t frame_id)
{
	return frame_id & 0x7F;
}

void cuavcan_parse_tail_byte(uint8_t * payload, uint8_t length, cuavcan_tail_byte_t * dst)
{
	uint8_t tail_byte = payload[length];
	dst->data = tail_byte;
	dst->is_start_of_transfer = tail_byte & (1 << 7);
	dst->is_end_of_transfer = tail_byte & (1 << 6);
	dst->toggle_bit = tail_byte & (1 << 5);
	dst->transfer_id = tail_byte & (0x1F);
}

bool cuavcan_is_transfer_single_frame(cuavcan_tail_byte_t * tail)
{
	return tail->is_start_of_transfer && tail->is_end_of_transfer;
}

cuavcan_message_assembly_t * cuavcan_find_message_assembly(cuavcan_instance_t *uavcan, uint16_t msg_id)
{
	cuavcan_message_assembly_t *ret = NULL;
	for (unsigned i = 0; i < uavcan->subscribed_ids_length; ++i)
	{
		if (msg_id == ((uavcan->msgs) + i)->msg.id)
		{
			ret = (uavcan->msgs) + i;
			break;
		}
	}
	return ret;
}
