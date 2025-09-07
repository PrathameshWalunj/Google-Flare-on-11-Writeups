# Challenge 03 - aray

## Initial Analysis

YARA! Finally, a YARA rule challenge. We're given a `.yara` file that looks absolutely terrifying at first glance.

<img width="1919" height="997" alt="Image" src="https://github.com/user-attachments/assets/6eba8bde-4d68-4bbf-87a1-266e65ba54c8" />

Opening this file reveals what seems like a complete clusterfuck of conditions - hundreds of checks on file size, byte values, XOR operations, hash comparisons, and more. Thought this is gonna take forever!

But then after staring into YARAAA for some time I realized most of this is just noise. The real challenge is identifying the important constraints and ignoring the bullshit.

## The Strategy

Looking closer at the YARA rule, several key patterns can be found:
1. **Fixed file size**: 85 bytes
2. **Target MD5**: `b7dc94ca98aa58dabb5404541c812db2`
3. **Mathematical operations**: Simple additions, subtractions & XOR operations
4. **Hash constraints**: Specific MD5, SHA256, and CRC32 values for 2-byte chunks
5. **32-bit integer constraints**: Little-endian packed values

The approach? Brute force the hash constraints (only 65536 possibilities for 2 bytes), solve the mathematical equations directly, and let the target MD5 validate my solution.

## Solution Script

Time to write some Python

<img width="1919" height="1014" alt="Image" src="https://github.com/user-attachments/assets/2944fab1-a11a-4210-9b5c-bdbe6c5d7854" />

The script:
1. Creates an 85-byte array
2. Solves direct mathematical constraints (additions, subtractions, XOR)
3. Packs 32-bit integers using the given equations
4. Brute forces hash values for 2-byte chunks
5. Validates against the target MD5



Running the script and seeing the flag print out was cool. The reconstructed file contains our flag!

**Flag:** `1RuleADayK33p$Malw4r3Aw4y@flare-on.com`
