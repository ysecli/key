# The Vault

A five-gate serial validator. Enter a username and a three-part serial (`A-B-C`). Each gate checks one part. Get them all right, the vault opens.

> **SPOILER WARNING:** This repo contains the solution. If you want to reverse this blind, **stop reading after the Build section**. Come back here when you're stuck or when you've solved it.

## Build

    cl /EHsc main.cpp CheckImpl.cpp /Fe:vault.exe     # MSVC (release)
    g++ -std=c++17 -Wall main.cpp CheckImpl.cpp -o vault  # GCC

No dependencies beyond the standard library.

## Run

    vault.exe

It prompts for a username and a serial in the format `GroupA-GroupB-GroupC`. Each group is validated in order.

---

<details>
<summary><b>SOLUTION (click to expand)</b></summary>

### How it works

Five gates, each folding a value into a `magic` accumulator:

1. **LengthGate** — username must be within the min/max range. Folds `0x<value>` on pass.
2. **ChecksumGate** — Group A is validated by a weighted checksum against username length.
   - Target derived from username length
   - Checksum: weighted sum of bytes
   - Folds `0x<value>` on pass.
3. **XorGate** — Group B is the plaintext obfuscated by XOR with `0x<XOR_KEY>`.
   - Stored as a byte array
   - Decode: `byte ^ 0x<XOR_KEY>` for each
   - Folds `0x<value>` on pass.
4. **TransformGate** — Group C is transformed character-by-character: rotate left by index, then XOR with constant.
   - Expected bytes stored
   - Folds `0x<value>` on pass.
5. **FinalGate** — checks `magic == MAGIC_TARGET` (the XOR of all four contributions).
   - `MAGIC_TARGET = 0x<value1> ^ 0x<value2> ^ 0x<value3> ^ 0x<value4>`

### Valid key

See `CheckImpl.cpp` for the constants and expected values. The valid username/serial combination is determined by your specific username.
**Keygen approach:**
- Write a program that takes any username and computes a valid `A-B-C` from the logic you recovered

### Architecture

- **CheckImpl.cpp** — all five gate implementations + dispatch table (`RunGate`)
- **Global.h** — constants (length bounds, XOR key, magic target, per-gate values)
- **main.cpp** — read-only pipeline and menu
- **CheckDecl.h** — function declarations

The **decoy** in `CheckFinal` compares the raw serial to a hardcoded string but changes nothing, it's there to waste a reverser's time.

</details>
