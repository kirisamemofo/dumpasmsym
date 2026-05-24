/*
	Copyright (c) 2025 Devon Artmeier

	Permission to use, copy, modify, and /or distribute this software
	for any purpose with or without fee is hereby granted.

	THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
	WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIE
	WARRANTIES OF MERCHANTABILITY AND FITNESS.IN NO EVENT SHALL THE
	AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
	DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
	PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
	TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
	PERFORMANCE OF THIS SOFTWARE.
*/

#include "shared.hpp"

static long long ReadInputNumber(std::ifstream& input, bool is_signed)
{
	unsigned char read_buffer[4] = { 0 };
	long long     value          = 0;

	ReadInput(input, reinterpret_cast<char*>(read_buffer), 4);
	for (int i = 3; i >= 0; i--) {
		value = (value << 8) | read_buffer[i];
	}

	if (is_signed && (value & (1LL << 31))) {
		value |= ~(static_cast<long long>(1LL << 32) - 1);
	}

	return value;
}

static std::string ReadInputString(std::ifstream& input)
{
	unsigned char char_count;
	ReadInput(input, reinterpret_cast<char*>(&char_count), 1);

	char* string_buffer = new char[char_count + 1];
	ReadInput(input, string_buffer, char_count);
	string_buffer[char_count] = '\0';

	std::string string = string_buffer;
	delete[] string_buffer;

	return string;
}

bool Symbols::LoadPsyqSymbols(const std::string& file_name)
{
	std::ifstream input(file_name, std::ios::binary);
	if (!input.is_open()) {
		throw std::runtime_error(("Cannot open \"" + file_name + "\" for reading.").c_str());
	}

	input.seekg(0, input.end);
	if (input.tellg() == 0) {
		return false;
	}
	input.seekg(0, input.beg);

	char read_buffer[4];
	ReadInput(input, read_buffer, 3);
	read_buffer[3] = '\0';

	if (strcmp(read_buffer, "MND")) {
		return false;
	}

	ReadInput(input, read_buffer, 1);
	if (read_buffer[0] != 1) {
		return false;
	}

	input.seekg(8, input.beg);

	while (true) {
		if (input.peek() == -1) {
			break;
		}

		long long value = ReadInputNumber(input, true);
		char type;
		ReadInput(input, &type, 1);

		if (type == 8) {
			input.seekg(1, input.cur);
		} else {
			std::string name = ReadInputString(input);
			if (type == 1 || type == 2) {
				this->AddSymbol(name, value);
			}
		}
	}

	return true;
}
