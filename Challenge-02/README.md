# Challenge 02 - checksum

## Initial Analysis

We're given a binary named `checksum` that presents us with random math addition questions followed by a checksum input prompt.

<img width="1608" height="863" alt="Image" src="https://github.com/user-attachments/assets/86ed4043-fee3-46b8-8591-ab333cc31267" />

After answering the math questions correctly, the program asks for a checksum input. If the wrong checksum is provided, the program exits.

<img width="1499" height="800" alt="Image" src="https://github.com/user-attachments/assets/8447a186-38f5-4452-ae56-9f8cb8e63f95" />

## Reverse Engineering

Opening the binary in IDA Pro reveals it's written in Go (evident from the `main_main` function structure typical of Go binaries).

<img width="1919" height="1018" alt="Image" src="https://github.com/user-attachments/assets/23d5072c-22e4-4649-8133-e8de015e6b3d" />

The key functions of interest are:
- `main_main` - Entry point
- `main_a` - Checksum validation logic  
- `main_b` 

We can see the maths equations logic
<img width="1463" height="709" alt="Image" src="https://github.com/user-attachments/assets/83ef9a2f-6d3c-443f-990d-63396da179db" />

The program computes SHA256 hashes and calls cryptographic functions before validating the checksum through `main_a`.

<img width="1896" height="730" alt="Image" src="https://github.com/user-attachments/assets/5529b7d7-166a-4b72-b017-f8b16621474b" />

## Finding the Correct Checksum

Analyzing the `main_a` function reveals the checksum validation logic:


<img width="1652" height="725" alt="Image" src="https://github.com/user-attachments/assets/a24d372f-c42e-45c2-a152-a48c4f4130e1" />

The function:
1. XORs the input checksum with the string `FlareOn2024`
2. Encodes the result to base64
3. Compares it against a hardcoded base64 string: `cQoFRQErX1YAVw1zVQdFUSxfAQNRBXUNAxBSe15QCVRVJ1pQEwd/WFBUAlElCFBFUnlaB1ULByRdBEFdfVtWVA==`

Since these operations are possibly reversible, I used CyberChef to decode the checksum:

**CyberChef Recipe:**
1. From Base64 (A-Za-z0-9+/=)
2. XOR with key: `FlareOn2024`
3. Input: `cQoFRQErX1YAVw1zVQdFUSxfAQNRBXUNAxBSe15QCVRVJ1pQEwd/WFBUAlElCFBFUnlaB1ULByRdBEFdfVtWVA==`

<img width="1919" height="867" alt="Image" src="https://github.com/user-attachments/assets/fdcff52d-b8c0-4b87-bd4c-b825c059f07c" />

## Getting the Flag

Entering the decoded checksum into the program gives us a "Noice!!" message, but there is no flag yet!?.

<img width="1474" height="757" alt="Image" src="https://github.com/user-attachments/assets/b69d1f66-00ef-4847-bcc2-17abd85ecf60" />

When `main_a` returns true, the program writes data to a file in the user's AppData directory. Checking `%LOCALAPPDATA%` reveals the flag file.


<img width="1644" height="667" alt="Image" src="https://github.com/user-attachments/assets/ed5ac8c9-5558-42bb-aaec-53a084c8d003" />

<img width="1393" height="698" alt="Image" src="https://github.com/user-attachments/assets/ea1376f7-6ea1-4db5-87c5-8e9cd6a91c55" />


## Flag

**Flag:** `Th3_M4tH_Do_b3_mAth1ng@flare-on.com`