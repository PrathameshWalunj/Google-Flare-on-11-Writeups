#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>



struct chacha20_context
{
	uint32_t keystream32[16];
	size_t position;
	uint8_t key[32];
	uint8_t nonce[12];
	uint64_t counter;
	uint32_t state[16];
};

void chacha20_init_context(struct chacha20_context *ctx, uint8_t key[], uint8_t nonce[], uint64_t counter);
void chacha20_xor(struct chacha20_context *ctx, uint8_t *bytes, size_t n_bytes);
static void chacha20_init_block(struct chacha20_context *ctx, uint8_t key[], uint8_t nonce[]);
static void chacha20_block_set_counter(struct chacha20_context *ctx, uint64_t counter);
static void chacha20_block_next(struct chacha20_context *ctx);




static uint32_t rotl32(uint32_t x, int n)
{
	return (x << n) | (x >> (32 - n));
}

static uint32_t pack4(const uint8_t *a)
{
	uint32_t res = 0;
	res |= (uint32_t)a[0] << 0 * 8;
	res |= (uint32_t)a[1] << 1 * 8;
	res |= (uint32_t)a[2] << 2 * 8;
	res |= (uint32_t)a[3] << 3 * 8;
	return res;
}

static void chacha20_init_block(struct chacha20_context *ctx, uint8_t key[], uint8_t nonce[])
{
	memcpy(ctx->key, key, sizeof(ctx->key));
	memcpy(ctx->nonce, nonce, sizeof(ctx->nonce));

	// Change the magic constant to "K"
	const uint8_t *magic_constant = (uint8_t*)"expand 32-byte K";

	ctx->state[0] = pack4(magic_constant + 0 * 4);
	ctx->state[1] = pack4(magic_constant + 1 * 4);
	ctx->state[2] = pack4(magic_constant + 2 * 4);
	ctx->state[3] = pack4(magic_constant + 3 * 4);
	ctx->state[4] = pack4(key + 0 * 4);
	ctx->state[5] = pack4(key + 1 * 4);
	ctx->state[6] = pack4(key + 2 * 4);
	ctx->state[7] = pack4(key + 3 * 4);
	ctx->state[8] = pack4(key + 4 * 4);
	ctx->state[9] = pack4(key + 5 * 4);
	ctx->state[10] = pack4(key + 6 * 4);
	ctx->state[11] = pack4(key + 7 * 4);
	ctx->state[12] = 0; // Counter
	ctx->state[13] = pack4(nonce + 0 * 4);
	ctx->state[14] = pack4(nonce + 1 * 4);
	ctx->state[15] = pack4(nonce + 2 * 4);

	memcpy(ctx->nonce, nonce, sizeof(ctx->nonce));
}

static void chacha20_block_set_counter(struct chacha20_context *ctx, uint64_t counter)
{
	ctx->state[12] = (uint32_t)counter;
	ctx->state[13] = pack4(ctx->nonce + 0 * 4) + (uint32_t)(counter >> 32);
}

#define CHACHA20_QUARTERROUND(x, a, b, c, d) \
    x[a] += x[b]; x[d] = rotl32(x[d] ^ x[a], 16); \
    x[c] += x[d]; x[b] = rotl32(x[b] ^ x[c], 12); \
    x[a] += x[b]; x[d] = rotl32(x[d] ^ x[a], 8); \
    x[c] += x[d]; x[b] = rotl32(x[b] ^ x[c], 7);

static void chacha20_block_next(struct chacha20_context *ctx) {
	for (int i = 0; i < 16; i++) ctx->keystream32[i] = ctx->state[i];

	for (int i = 0; i < 10; i++)
	{
		CHACHA20_QUARTERROUND(ctx->keystream32, 0, 4, 8, 12)
		CHACHA20_QUARTERROUND(ctx->keystream32, 1, 5, 9, 13)
		CHACHA20_QUARTERROUND(ctx->keystream32, 2, 6, 10, 14)
		CHACHA20_QUARTERROUND(ctx->keystream32, 3, 7, 11, 15)
		CHACHA20_QUARTERROUND(ctx->keystream32, 0, 5, 10, 15)
		CHACHA20_QUARTERROUND(ctx->keystream32, 1, 6, 11, 12)
		CHACHA20_QUARTERROUND(ctx->keystream32, 2, 7, 8, 13)
		CHACHA20_QUARTERROUND(ctx->keystream32, 3, 4, 9, 14)
	}

	for (int i = 0; i < 16; i++) ctx->keystream32[i] += ctx->state[i];

	uint32_t *counter = ctx->state + 12;
	counter[0]++;
	if (0 == counter[0])
	{
		counter[1]++;
		assert(0 != counter[1]);
	}
}

void chacha20_init_context(struct chacha20_context *ctx, uint8_t key[], uint8_t nonce[], uint64_t counter)
{
	memset(ctx, 0, sizeof(struct chacha20_context));
	chacha20_init_block(ctx, key, nonce);
	chacha20_block_set_counter(ctx, counter);
	ctx->counter = counter;
	ctx->position = 64;
}

void chacha20_xor(struct chacha20_context *ctx, uint8_t *bytes, size_t n_bytes)
{
	uint8_t *keystream8 = (uint8_t*)ctx->keystream32;
	for (size_t i = 0; i < n_bytes; i++)
	{
		if (ctx->position >= 64)
		{
			chacha20_block_next(ctx);
			ctx->position = 0;
		}
		bytes[i] ^= keystream8[ctx->position];
		ctx->position++;
	}
}


// Code Credit: Taken from: https://github.com/Ginurx/chacha20-c/blob/master/chacha20.c

int main() {
    
    uint8_t key[32] = {
        0x8d, 0xec, 0x91, 0x12, 0xeb, 0x76, 0x0e, 0xda, 0x7c, 0x7d, 0x87, 0xa4,
        0x43, 0x27, 0x1c, 0x35, 0xd9, 0xe0, 0xcb, 0x87, 0x89, 0x93, 0xb4, 0xd9,
        0x04, 0xae, 0xf9, 0x34, 0xfa, 0x21, 0x66, 0xd7
    };

    uint8_t nonce[12] = {
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
    };
    
    // This buffer holds the ciphertext 
    uint8_t buffer[] = {
        0xA9, 0xF6, 0x34, 0x08, 0x42, 0x2A, 0x9E, 0x1C, 0x0C, 0x03, 0xA8, 0x08,
        0x94, 0x70, 0xBB, 0x8D, 0xAA, 0xDC, 0x6D, 0x7B, 0x24, 0xFF, 0x7F, 0x24,
        0x7C, 0xDA, 0x83, 0x9E, 0x92, 0xF7, 0x07, 0x1D, 0x02, 0x63, 0x90, 0x2E,
        0xC1, 0x58
    };
    
    
    struct chacha20_context ctx;
    
    // Initialize the context with the key, nonce, and a starting counter of 0.
    chacha20_init_context(&ctx, key, nonce, 0);
    
    // Decrypt the buffer.
    chacha20_xor(&ctx, buffer, sizeof(buffer));
    
    
    printf("Decrypting!\n");
    printf("%s", buffer);
    
    return 0;
}
