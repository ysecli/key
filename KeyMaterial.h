#pragma once
#include <string>
#include "Global.h"

// KeyState carries everything derived from the two inputs through the pipeline.
// main.cpp never reads its fields directly, only InitKeyState fills it and the
// gates consume it.
//
//   user, serial            - the raw inputs
//   a, b, c                 - serial split on '-' into three groups
//   magic                   - an accumulator the gates fold into as they pass
//   parsed                  - did the serial split into exactly three groups

struct KeyState {
	std::string user, serial, a, b, c;
	int magic = 0;
	bool parsed = false;
};
