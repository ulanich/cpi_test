/*
 *  Copyright (C) 2017 - This file is part of libecc project
 *
 *  Authors:
 *      Ryad BENADJILA <ryadbenadjila@gmail.com>
 *      Arnaud EBALARD <arnaud.ebalard@ssi.gouv.fr>
 *      Jean-Pierre FLORI <jean-pierre.flori@ssi.gouv.fr>
 *
 *  Contributors:
 *      Nicolas VIVET <nicolas.vivet@ssi.gouv.fr>
 *      Karim KHALFALLAH <karim.khalfallah@ssi.gouv.fr>
 *
 *  This software is licensed under a dual BSD and GPL v2 license.
 *  See LICENSE file at the root folder of the project.
 */

#include "rand.h"
#include <stdint.h>
#include <stdlib.h>
//-----------------
#include <string.h>
//-----------------

#if defined(RAND) && (RAND == 1)
int get_random(unsigned char *buf, u16 len)
{
	for (int i=0;i<len;i++)
		buf[i]=i;
	return 0;
}

#else

uint32_t a[16] = {0};
uint8_t seed_material[48] = {0};
uint8_t ret[48] = {0};
uint32_t B[16] = {0};
uint8_t Key [32] = {0};
uint8_t V [16] = {0};

void Block_encrypt(uint8_t *out)
{
	memcpy(out, V, 16);
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, Key);
    AES_ECB_encrypt(&ctx, out);

}

uint32_t get_trash()
{
	uint32_t reg;
	CLK_CNTR->KEY=_KEY_;
	CLK_CNTR->PER1_CLK|=(1<<23);
	CLK_CNTR->ADC0_CLK=(0x0d<<28)|(1<<16)|(32<<0);
	ADC0->KEY=_KEY_;
	ADC0->CONFIG1=0;
	ADC0->CONFIG2=(0x80<<ADCx_CONFIG2_REFTRIMR_Pos);
	ADC0->CONFIG0=(1<<ADCx_CONFIG0_ADCEN_Pos);
	for (int i=0;i<16;i++)
	{
		ADC0->CONTROL=1;
		while(!(ADC0->STATRCH0&1));
		reg=(reg<<2)|(ADC0->RESULTCH_xx[0]&0x3);
	}
	return reg;
}

void instantiate_function()
{

	uint32_t entropy_input[12];
	for (int i = 0; i< 12; i++)
	{
		entropy_input[i] = get_trash();
	}

	get_entropy_input(entropy_input);
	get_entropy_input(entropy_input);
	memcpy(seed_material, a, 48);
	update(1);

}

static void reseed_function()
{
	uint32_t entropy_input[12];
	for (int i = 0; i< 12; i++)
	{
		entropy_input[i] = get_trash();
	}

	get_entropy_input(entropy_input);
	memcpy(seed_material, B, 48);
	update(1);

}

void get_entropy_input (uint32_t *input)
{
	uint32_t temp;
	for (int i = 0; i < 12; i++)
	{
		temp = (a[0]>>1)^(a[0]<<1&0xffffffff)^(a[9]^0x07ceabf4)^input[i];
		for (int j = 0; j < 15; j++)
		{
			a[j] = a[j+1];
		}
		a[15] = temp;
	}

	sha256(a, 64, B);
	for (int i = 0; i < 12; i++)
	{
		temp = (a[0]>>1)^(a[0]<<1&0xffffffff)^(a[9]^0x07ceabf4)^B[i];
		for (int j = 0; j < 15; j++)
		{
			a[j] = a[j+1];
		}
		a[15] = temp;
	}

	sha256(a, 64, &B[8]);
}

void update (int seed)
{
	uint8_t temp[48] = {0};
	for (int i = 0; i < 3; i++)
	{
		mpz_t Vi;
		mpz_init(Vi);
		mpz_set_bytes(Vi, V, 16);
		mpz_add_ui(Vi, Vi, 1);
		mpz_mod_ui(Vi, Vi, 2^16); //V = (V+1)%(2^16);
		memcpy(V, mpz_get_bytes(NULL, 16, Vi), 16);

		uint8_t output_block[16];
		Block_encrypt(output_block);
		memcpy(temp+i*16,output_block,16);
	}
	if (seed != 0 )
	{
		mpz_t TEMP;
		mpz_t SEED;
		mpz_init(TEMP);
		mpz_init(SEED);
		mpz_set_bytes(TEMP, temp, 48);
		mpz_set_bytes(SEED, seed_material, 48);
		mpz_xor(TEMP, TEMP, SEED);
		memcpy(temp, mpz_get_bytes(NULL, 48, TEMP), 48);
	}
	memcpy(Key, temp, 32);
	memcpy(V, temp+32, 16);

}

static void generate_function(unsigned char *buf, int req_b)
{
	uint8_t temp[48] = {0};
	for (int i = 0; i < 3; i++)
	{
		mpz_t Vi;
		mpz_init(Vi);
		mpz_set_bytes(Vi, V, 16);
		mpz_add_ui(Vi, Vi, 1);
		mpz_mod_ui(Vi, Vi, 2^16); //V = (V+1)%(2^16);
		memcpy(V, mpz_get_bytes(NULL, 16, Vi), 16);
		uint8_t output_block[16];
		Block_encrypt(output_block);
		memcpy(temp+i*16,output_block,16);
	}
	memcpy(buf, temp, req_b);
	update(0);

}

int get_random(unsigned char *buf, u16 bytes_len)
{
	reseed_function();

	if (bytes_len>48)
	{
		int i;
		for (i = 0; i < bytes_len/48; i++)
		{
			generate_function(ret, 48);
			memcpy(buf + i*48, ret, 48);
		}
		generate_function(ret, bytes_len%48);// тут неправильную длину дает
		memcpy(buf + i*48, ret, bytes_len%48);
	}

	else
	{
		generate_function(ret, bytes_len);
		memcpy(buf, ret, bytes_len);
	}

	return 0;
}
#endif
