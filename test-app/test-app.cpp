﻿//MIT License
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

#include <stdio.h>
#include <cuavcan.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

std::string test_data = "\
0103f20a bf b0 01 00 1f a5 02 8d\n\
0103f20a 00 01 00 03 00 66 ba 2d\n\
0103f20a 04 00 1f a1 05 00 00 0d\n\
0104060a 00 00 4d";

class canFrame
{
public:
	uint32_t id;
	uint8_t data[8];
	uint8_t len;
	std::string toString()
	{
		char c_str[256];
		size_t n = 0;
		n += snprintf(c_str, 256 - n, "%08x [", id);
		for (int i = 0; i < len; ++i)
		{
			n += snprintf(c_str + n, 256 - n, "%02x ", data[i]);
		}
		snprintf(c_str + n, 256 - n, "]\n");
		return std::string(c_str);
	}
} can_frame_t;

canFrame parseLine(const std::string &line)
{
	canFrame ret;

	std::stringstream ss(line);
	std::string number_str;
	uint32_t number;
	std::vector<std::string> tokens;
	ret.len = 0;
	ss >> number_str;
	number = std::stoul(number_str, 0, 16);
	ret.id = number;
	while (ss >> number_str) {
		ret.data[ret.len] = static_cast<uint8_t>(std::stoul(number_str, 0, 16));
		++ret.len;
	}
	return ret;

}

std::vector<canFrame> parseInput(const std::string &str)
{
	std::vector<canFrame> ret;
	std::stringstream ss(str);
	std::string line;
	while (std::getline(ss, line))
	{
		ret.push_back(parseLine(line));
	}
	return ret;
}

int main()
{
	cuavcan_instance_t uavcan;
	uint16_t subscribed_ids[] = { 1010, 1030 };
	cuavcan_init(&uavcan, subscribed_ids, 2);
	std::vector<canFrame> frames = parseInput(test_data);
	for (std::vector<canFrame>::iterator frame = frames.begin(); frame != frames.end(); ++frame)
	{
		//std::cout << (*frame).toString();
		cuavcan_handle_can_frame(&uavcan, (*frame).id, (*frame).data, (*frame).len);
	}
	return 0;
}

