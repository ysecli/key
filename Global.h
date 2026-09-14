#pragma once

// USER_MIN_LEN, USER_MAX_LEN   - allowed username length, inclusive

extern const int USER_MIN_LEN;
extern const int USER_MAX_LEN;

// XOR_KEY                      - a single byte used to obfuscate a hidden secret

extern const int XOR_KEY;

// MAGIC_TARGET                 - the value FinalGate tests.

extern const int MAGIC_TARGET;

// Group  - UNSCOPED enum: GroupA, GroupB, GroupC   (the three serial fields)

enum group {GroupA, GroupB, GroupC};

// Gate   - SCOPED enum, underlying type int:
//          LengthGate, ChecksumGate, XorGate, TransformGate, FinalGate

enum class Gate {LengthGate = 0x01, ChecksumGate = 0x02, XorGate = 0x03, TransformGate = 0x04, FinalGate};

// Result - SCOPED enum: Pass, Fail, Tampered

enum class Result {Pass, Fail, Tampered};
