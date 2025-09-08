# Challenge 05 - sshd

## The Crime Scene

We are greeted with this description for the challenge - Our server in the FLARE Intergalactic HQ has crashed! Now criminals are trying to sell me my own data!!! Do your part, random internet hacker, to help FLARE out and tell us what data they stole! We used the best forensic preservation technique of just copying all the files on the system for you.


So basically we're told that the FLARE Intergalactic HQ server crashed and criminals are trying to sell stolen data. Job is figure out what the malicious actor stole.

Ok, Let's get started

![Image](https://github.com/user-attachments/assets/687061d5-c5d7-428b-b7ac-0e91e10cc6ec)

The challenge gives us `ssh_container.tar` containing a partial Linux filesystem and a suspicious core dump: `./var/lib/systemd/coredump/sshd.core.93794.0.0.11.1725917676`

## Initial Investigation

First, I set up a proper environment to analyze this:

```bash
sudo chroot ssh_container /bin/bash
```

Then fired up GDB to examine the core dump:

<img width="840" height="706" alt="Image" src="https://github.com/user-attachments/assets/f7fe2f31-bdd0-4620-8068-89baae9c74ac" />

```bash
gdb sshd /var/lib/systemd/coredump/sshd.core.93794.0.0.11.1725917676
```

Classic null pointer crash - the program tried to execute code at address `0x0`. Time to dig deeper.

## Investigating the Crash

Used `info frame` to see where we were right before the crash:


<img width="840" height="706" alt="Image" src="https://github.com/user-attachments/assets/ca0284ea-b987-4068-8d09-9494094e5c9d" />

The saved RIP address `0x7f4a18c8f88f` tells us where the program would have returned to. Now I needed to find which library this belonged to.

## Mapping Memory

Running `info proc mappings` revealed the memory layout:

<img width="1316" height="706" alt="Image" src="https://github.com/user-attachments/assets/50f3b4f2-45a8-48f1-a393-b368d5e5a38b" />

Found the culprit: `/lib/x86_64-linux-gnu/liblzma.so.5.4.1` was loaded at `0x7f4a18c86000` but marked as deleted. Sketchyy.

## Reverse Engineering the Malicious Code

Loaded the library in IDA Pro and rebased it to match the crash address:

![Image](https://github.com/user-attachments/assets/54787274-d5dd-4a89-b265-5f5d2da7362d)

![Image](https://github.com/user-attachments/assets/d8ea90ed-ea4e-4d33-9ad0-af048188377c)

Searching for the saved RIP address `0x7f4a18c8f88f` led me to this code:
```nasm
oc_7F4A18C8F877:       ; handle
xor     edi, edi
call    _dlsym
mov     r8d, ebx
mov     rcx, r14
mov     rdx, r13
mov     rsi, rbp
mov     edi, r12d
call    rax
mov     rbx, [rsp+128h+var_40]
xor     rbx, fs:28h
jnz     loc_7F4A18C8F975 
```
The pattern was clear:
1. `call _dlsym` - lookup a symbol  
2. `call rax` - execute whatever dlsym returned
3. Crash because dlsym returned NULL

## Unpacking the Shellcode

Further analysis revealed a shellcode injection routine:

![Image](https://github.com/user-attachments/assets/fdfe0fce-679a-4a43-9b09-bf1e0e7595ee)

The malicious code:
1. Allocates executable memory with `mmap`
2. Copies encrypted data from `unk_7F4A18CA9960` 
3. Decrypts it in place
4. Executes the resulting shellcode

Found the encrypted blob size at `dword_7F4A18CB8360`:

![Image](https://github.com/user-attachments/assets/50546f22-6509-4489-818e-932c86f88344)

Exported the encrypted shellcode from `0x7F4A18CA9960` to `0x7F4A18CA9960 + 0xF96`:

![Image](https://github.com/user-attachments/assets/4327ea64-9a91-46a2-a9ca-fddb401c47bf)

## First Decryption Round

Located the ChaCha20 key and nonce in the crash state:

![Image](https://github.com/user-attachments/assets/baa19ee6-94a6-4863-914f-525f7c8738be)


Used CyberChef to decrypt the first layer:

![Image](https://github.com/user-attachments/assets/04323392-88fd-40c3-a38d-ceb089f54f97)

Select Algorithm as ChaCha, enter Key and Nonce, select rounds as 20 and select the input as the encrypted_shellcode.bin which we saved from IDA and save the result ie decrypted_shellcode.

## Analyzing the Decrypted Shellcode

Opened the decrypted shellcode in IDA and found it in sub_DC2 connecting to port 1337 and making lot of low-level syscalls:

![Image](https://github.com/user-attachments/assets/fe024c4f-3b32-41ce-aad8-f085f59feec0)

This shellcode was clearly exfiltrating data, but what data?

## Memory Archaeology

Dumped stack memory to find traces of the stolen data:

![Image](https://github.com/user-attachments/assets/cdc4cfc0-534f-4617-8e81-87a6af9d17b1)

```bash
dump memory memdump.bin $rsp - 0x2000 $rsp + 0x1000
```
Running strings on memdump

```bash
strings memdump.bin
```

![Image](https://github.com/user-attachments/assets/928d05b5-88af-4e66-82c4-db640f96e68f)

Found references to `/root/certificate_authority_signing_key.txt` and the telltale `expand 32-byte k(Chacha)` string.

## The Second Layer

Searched for the certificate path in memory and found it at `0x7ffcc6600c18`:

![Image](https://github.com/user-attachments/assets/916d6bb0-450a-4dd3-8bff-40deb9b57473)


Examined the surrounding memory:


![Image](https://github.com/user-attachments/assets/137feba0-1d48-4f2a-a142-95078967e3c9)


![Image](https://github.com/user-attachments/assets/adc3ef25-34cc-4f43-b182-506bd0aed5d7)

Pattern recognition time:
- 12 bytes of `0x11` = ChaCha20 nonce
- 32 bytes before that = ChaCha20 key  
- Data at `0x7ffcc6600c18` = encrypted certificate

At this point, I went into the rabbithole and spent a lot of time why the ChaCha was not decrypting it, or maybe there was more to the challenge I was missing.

## The Evil Detail

Here's where it gets nasty. The shellcode uses `expand 32-byte K` (capital K) instead of the standard `expand 32-byte k` (lowercase k):

![Image](https://github.com/user-attachments/assets/9dd350c6-82be-4158-87ba-3d72ec86aa43)

This meant I couldn't use standard ChaCha20 implementations. Had to implement custom ChaCha decryption code.

## Custom Decryption

Found a ChaCha20 implementation on GitHub( https://github.com/Ginurx/chacha20-c/blob/master/chacha20.c) and modified it:

```c
// Modified the magic constant
const uint8_t *magic_constant = (uint8_t*)"expand 32-byte K";
```

Compiled and ran it with our extracted key, nonce, and ciphertext:

![Image](https://github.com/user-attachments/assets/2083a90b-8411-409f-9f19-d17c518c2535)


There it is!!
It was one of the hardest challenge for me.
The combination of library injection, double encryption, and subtle implementation differences made this one of the more challenging reversing exercises. 


## Flag

**`supp1y_cha1n_sund4y@flare-on.com`**



