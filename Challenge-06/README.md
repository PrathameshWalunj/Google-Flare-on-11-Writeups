# Challenge 06 - bloke2

## The Missing Researcher Mystery

The challenge description sets an ominous tone: *"You've been so helpful lately, and that was very good work you did. Yes, I'm going to put it right here, on the refrigerator, very good job indeed. You're the perfect person to help me with another issue that come up. One of our lab researchers has mysteriously disappeared. He was working on the prototype for a hashing IP block that worked very much like, but not identically to, the common Blake2 hash family. Last we heard from him, he was working on the testbenches for the unit. One of his labmates swears she knew of a secret message that could be extracted with the testbenches, but she couldn't quite recall how to trigger it. Maybe you could help?"*

A Verilog hardware challenge. I hadn't touched Verilog since my undergrad days, but here we go.

## Initial Lookup

The challenge provides a bunch of Verilog files implementing a Blake2-like hashing algorithm, plus a helpful README which includes info about how to build files and some hints:

![Image](https://github.com/user-attachments/assets/59b16aa8-9de9-4b2a-9be0-cf54864974d0)

Few insights from the README:
- You can solve this by modifying testbenches alone
- Some diagnostic code is commented out that might be useful
- Brute-forcing won't work - they've modified the Blake2 implementation specifically to prevent this..hmm Okay

Shoutout to Dave for the hardware hacking challenge

## Diving into the Verilog Soup

Opening the various `.v` files revealed typical digital design modules - functional units, sequential logic, the usual suspects but they were clean or it was not obvious to me. But one file stood out: `data_mgr.v`.

This module handles data management for the Blake2-like hash, and buried in the code was something suspicious:

![Image](https://github.com/user-attachments/assets/01a26796-9e23-4f2e-8d84-3f0951d6f46b)

```verilog
localparam TEST_VAL = 512'h3c9cf0addf2e45ef548b011f736cc99144bdfee0d69df4090c8a39c520e18ec3bdc1277aad1706f756affca41178dac066e4beb8ab7dd2d1402c4d624aaabe40;
```

That's a 512-bit constant that looks suspiciously like encoded data. I need to know how this was used:

![Image](https://github.com/user-attachments/assets/8ddfcee0-1e98-407e-9a82-48f3bda9e42a)

```verilog
if (h_rdy) begin
    out_cnt <= W;
    h <= h_in ^ (TEST_VAL & {(W*16){tst}});
end
```

Here we can see that the hash output gets XORed with `TEST_VAL`, but only if the mask `{(W*16){tst}}` allows it.

## The Debug Investigation

To understand what was happening with this `tst` signal, I added a debug statement:

![Image](https://github.com/user-attachments/assets/2515d1ef-d54c-40ba-853e-523e60bd6b6e)

```verilog
$display("h_in=%h, TEST_VAL=%s, tst=%b, mask=%h, result=%s", h_in, TEST_VAL, tst, {(W*16){tst}}, h);
```
This debug line will print the internal values of the variables involved in the XOR logic using $display

Running the testbench revealed the problem:

![Image](https://github.com/user-attachments/assets/6db0ee5a-7522-4603-bda3-29ced0fe8d89)

The `tst` signal was always 0, which meant:
- Mask: `{(W*16){0}} = 0x0000...0000` (all zeros)
- XOR operation: `h <= h_in ^ (TEST_VAL & 0) = h_in ^ 0 = h_in`

The test value was being masked out completely! The secret message must be there, but hidden behind this test mode flag.

## Fixing the tst

Looking at how `tst` gets set:

```verilog
tst <= finish;
```

The `finish` signal is normally 0 during regular operation, so `tst` stays 0. But what if we force test mode to always be active?

I made one change:

![Image](https://github.com/user-attachments/assets/5b8995b0-3242-4eda-85d5-57191bf47c9e)

```verilog
// Change this:
tst <= finish;

// To this:
tst <= 1'b1;  // Force test mode always on, b to force set 1 bit binary
```

This modification means:
- `tst` is always 1
- Mask becomes: `{(W*16){1}} = 0xFFFF...FFFF` (all ones)
- XOR operation becomes: `h <= h_in ^ TEST_VAL` (full XOR with the secret)

## Verilog spits out the flag

Running `make tests` with the modified code revealed the hidden message:

![Image](https://github.com/user-attachments/assets/c0f101fa-1aa9-4d16-be74-2eb0cd030983)

This challenge was on the easier side, perhaps after frog.

**Flag:** `please_send_help_i_am_trapped_in_a_ctf_flag_factory@flare-on.com`

