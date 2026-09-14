#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#include <windows.h>
#undef byte
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <sstream>
#include "Global.h"
#include "KeyMaterial.h"
#include "CheckDecl.h"
using namespace std;

// IMPLEMENTATIONS

const int MAGIC_TARGET = 0x6 ^ 0x2D ^ 0xF6 ^ 0xD2;
const int USER_MIN_LEN = 4;
const int USER_MAX_LEN = 16;
const int XOR_KEY = 0x36;

// InitKeyState
//   Store the raw inputs. Split serial on '-' into a, b, c. Set parsed = true
//   only if you got exactly three non-empty groups. Zero the accumulator.

void InitKeyState(KeyState& s, const std::string& user, const std::string& serial) {
	s.user = user;
	s.serial = serial;
	s.magic = 0;
	s.parsed = false;

	size_t pos1 = serial.find('-');
	size_t pos2 = serial.find('-', pos1 + 1);

	if (pos1 != std::string::npos && pos2 != std::string::npos) {
		s.a = serial.substr(0, pos1);
		s.b = serial.substr(pos1 + 1, pos2 - pos1 - 1);
		s.c = serial.substr(pos2 + 1);
		s.parsed = true;
	}
}
// RunGate
//   A switch over the Gate enum that calls the matching Check* function.

Result RunGate(Gate g, KeyState& s) {
	switch (g) {
	case Gate::LengthGate: return CheckLength(s);
	case Gate::ChecksumGate: return CheckChecksum(s);
	case Gate::XorGate: return CheckXor(s);
	case Gate::TransformGate: return CheckTransform(s);
	case Gate::FinalGate: return CheckFinal(s);
	}
	return Result::Fail;
}

// GateName
// A return value to print an error for you to know which part of the vault has failed.
std::string GateName(Gate g) {
	switch (g) {
	case Gate::LengthGate: return "length";
	case Gate::ChecksumGate: return "one";
	case Gate::XorGate: return "two";
	case Gate::TransformGate: return "three";
	case Gate::FinalGate: return "something but don't know where.";
	}
	return "unknown";
}


// Gates

// CheckLength
//   Fail unless parsed is true AND the username length sits in [MIN, MAX].

Result CheckLength(KeyState& s) {
	if (s.user.size() < USER_MIN_LEN || s.user.size() > USER_MAX_LEN) {
		return Result::Fail;
	}
	s.magic ^= 0x6;
	return Result::Pass;
}
// CheckChecksum   (operates on group A)
//   Reduce group A's bytes to a single value (a weighted sum)
//   and require it to match a target derived from the username length.
//   Keep it invertible enough that a keygen can build a passing A.

Result CheckChecksum(KeyState& s) {
	unsigned int sum = 0;
	for (size_t i = 0; i < s.a.size(); i++) {
		sum += (unsigned char)s.a[i] * (i + 1);
	}
	unsigned int target = (unsigned int)s.user.size() * 37 + 100;
	if ((sum & 0xFF) != (target & 0xFF)) {
		return Result::Fail;
	}
	s.magic ^= 0x2D;
	return Result::Pass;

}

// CheckXor        (operates on group B)
//   Store a hidden secret word as a byte array already XOR'd with XOR_KEY.
//   Decode it at runtime with DecodeSecret, then require group B == secret.
//   The plaintext must NEVER appear in the source as a literal. That is the
//   whole lesson: it won't show up in `strings`.

Result CheckXor(KeyState& s) {
	unsigned char blob[] = { 0x08, 0x69, 0x0C };
	std::string secret;
	for (int i = 0; i < 3; i++) {
		secret += (char)(blob[i] ^ XOR_KEY);
	}
	if (s.b != secret) {
		return Result::Fail;
	}
	s.magic ^= 0xF6;
	return Result::Pass;
}

// CheckTransform  (operates on group C)
//   Run each char of C through a per-index, reversible transform
//   (e.g. rotate-left by index, then XOR with something) and compare the
//   result to a stored expected buffer. Reversible so a keygen can invert it.

unsigned char expected[] = { 0x20, 0x79, 0xD9, 0x80 };

Result CheckTransform(KeyState& s) {
	for (size_t i = 0; i < s.c.size(); i++) {
		unsigned char transformed = RotateLeft((unsigned char)s.c[i], i) ^ 0x11;

		if (transformed != expected[i]) {
			return Result::Fail;
		}
		s.magic ^= 0xD2;
		return Result::Pass;
	}
	return Result::Fail;

}

// CheckFinal
//   Pass only if state.magic == MAGIC_TARGET.
//   Also a decoy written here so it shows up in 'strings'.It exists to solely waste a reverser's time in there.

Result CheckFinal(KeyState& s) {
	if (s.serial == "RE-VERSE-ME") {
		std::cout << "HaHaHa";
	}
	if (s.magic == MAGIC_TARGET) {
		return Result::Pass;
	}
	return Result::Fail;
}

// Optional implementations. Comment out if you dont want to do these. 
// If you comment it out make sure to go back into the groups and take out that portion of the code there also.

// DecodeSecret   - XOR each byte of the buffer with the key into an out string.

void DecodeSecret(const unsigned char* blob, int len, unsigned char key, std::string& out) {
	out.clear();
	for (int i = 0; i < len; i++) {
		out += (char)(blob[i] ^ key);
	}
}

// RotateLeft     - classic 8-bit rotate. Shows up as ROL.

unsigned char RotateLeft(unsigned char v, int n) {
	n &= 7;
	return (unsigned char)((v << n) | (v >> ((8 - n) & 7)));
}

// Self explainatory. I personally am not good at these yet so I will do this for myself later.
// Feel free to comment it out if you also don't feel like doing it.

bool DebuggerDetected() {
	if (IsDebuggerPresent())
		return true;

	BOOL remoteDebug = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &remoteDebug);
	if (remoteDebug)
		return true;

	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if (GetThreadContext(GetCurrentThread(), &ctx)) {
		if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0)
			return true;
	}

	return false;
}


