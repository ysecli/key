#pragma once
#include <string>
#include "Global.h"
#include "KeyMaterial.h"

// DECLARATIONS ONLY.

//   InitKeyState   - fills a KeyState from a username and a serial

void InitKeyState(KeyState& s, const std::string& user, const std::string& serial);

//   RunGate        - runs one Gate against a KeyState and reports a Result

Result RunGate(Gate g, KeyState& s);

//   GateName       - turns a Gate into text for the denial message

std::string GateName(Gate g);

//   DebuggerDetected - reports whether a debugger is attached

bool DebuggerDetected();


Result CheckLength(KeyState& s);
Result CheckChecksum(KeyState& s);
Result CheckXor(KeyState& s);
Result CheckTransform(KeyState& s);
Result CheckFinal(KeyState& s);

//   DecodeSecret   - XOR a stored byte buffer with the key into a string
void DecodeSecret(const unsigned char* blob, int len, unsigned char key, std::string& out);;

//   RotateLeft     - rotate the bits of one byte left by n (used in transform)

unsigned char RotateLeft(unsigned char v, int n);