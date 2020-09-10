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
#include "libec.h"
/* We include the printf external dependency for printf output */
#include "print.h"

/*
 * The purpose of this example is to implement a 'toy'
 * ECDH (Elliptic curve Diffie-Hellman) protocol. Alice
 * and Bob want to derive a secret 'x' without sharing the
 * same secret key (using asymmetric cryptography). In order
 * to do this, they agree upon a public Elliptic Curve with
 * a generator G. Alice (resp. Bob) generates a private value
 * d_Alice (resp. d_Bob) < q, where q is the order of G.
 * Alice (resp. Bob) computes and shares Q_Alice = d_Alice x G
 * (resp. Q_Bob = d_Bob x G) over a public channel. Alice
 * and Bob now both can compute the same point Q such that
 * Q = d_Alice x Q_Bob = d_Bob x Q_Alice, and the shared
 * secret 'x' is the first coordinate of the curve point Q.
 * External passive observers cannot compute 'x'.
 *
 * NOTE: We don't seek for communication bandwidth
 *       optimization here, this is why we use arrays to
 *       exchange projective coordinates points (and not
 *       affine points and/or only the x coordinate since the
 *	 curve equation can be used).
 */

/* Zero buffer to detect empty buffers */
static u8 zero[3 * NN_MAX_BYTE_LEN] = { 0 };

/*
 * The following global variables simulate our shared "data bus"
 * where Alice and Bob exchange data.
 */

/* Global array holding Alice to Bob public value
 * Q_Alice = d_Alice x G.
 * This is a serialized projective EC point, holding
 * 3 coordinates, meaning that its maximum size is
 * 3 * NN_MAX_BYTE_LEN (i.e. this will work for
 * all our curves).
 */
static u8 Alice_to_Bob[3 * NN_MAX_BYTE_LEN] = { 0 };

/* Global array holding Bob to Alice public value
 * Q_Bob = d_Bob x G.
 * This is a serialized projective EC point, holding
 * 3 coordinates, meaning that its maximum size is
 * 3 * NN_MAX_BYTE_LEN. (i.e. this will work for
 * all our curves).
 */
static u8 Bob_to_Alice[3 * NN_MAX_BYTE_LEN] = { 0 };

int ECDH_helper(const u8 *curve_name, const u8 *role);
int ECDH_helper(const u8 *curve_name, const u8 *role)
{
	int ret;
	/* The projective point we will use */
	prj_pt Q;
	/* The equivalent affine point */
	aff_pt Q_aff;
	/* The private scalar value for Alice and Bob, as well as their
	 * respective shared secrets.
	 * These are 'static' in order to keep them across multiple calls
	 * of the function.
	 */
	static nn d_Alice, d_Bob;
	nn_t d;
	static fp x_Alice, x_Bob, y_Alice, y_Bob;
	fp_t x, y;
	const char *x_str, *y_str;
	/* Pointers to the communication buffers */
	u8 *our_public_buffer, *other_public_buffer;

	const ec_str_params *the_curve_const_parameters;
	/* libecc internal structure holding the curve parameters */
	ec_params curve_params;

	/****** Alice => Bob *********************************************************/
	if (are_equal
	    (role, "Alice", local_strnlen("Alice", sizeof("Alice")) + 1)) {
		our_public_buffer = Alice_to_Bob;
		other_public_buffer = Bob_to_Alice;
		d = &d_Alice;
		x = &x_Alice;
		x_str = "  x_Alice";
		y = &y_Alice;
		y_str = "  y_Alice";
	}
	/****** Bob => Alice *********************************************************/
	else if (are_equal
		 (role, "Bob", local_strnlen("Bob", sizeof("Bob")) + 1)) {
		our_public_buffer = Bob_to_Alice;
		other_public_buffer = Alice_to_Bob;
		d = &d_Bob;
		x = &x_Bob;
		x_str = "  x_Bob  ";
		y = &y_Bob;
		y_str = "  y_Bob  ";
	} else {
		/* Unknown role, get out */
		ext_printf("  Error: unknown role %s for ECDH\n", role);
		ret = -1;
		goto out;
	}

	/* Importing specific curve parameters from the constant static
	 * buffers describing it:
	 * It is possible to import a curve set of parameters by its name.
	 */
	the_curve_const_parameters =
		ec_get_curve_params_by_name(curve_name,
					    (u8)local_strnlen((const char *)
							      curve_name,
							      MAX_CURVE_NAME_LEN)
					    + 1);
	/* Get out if getting the parameters went wrong */
	if (the_curve_const_parameters == NULL) {
		ext_printf("  Error: error when importing curve %s "
			   "parameters ...\n", curve_name);
		ret = -1;
		goto out;
	}
	/* Now map the curve parameters to our libecc internal representation */
	import_params(&curve_params, the_curve_const_parameters);

	/* Initialize our projective point with the curve parameters */
	prj_pt_init(&Q, &(curve_params.ec_curve));
	if (!are_equal(our_public_buffer, zero, sizeof(zero))) {
		/* We have already generated and sent our parameters, skip to
		 * the state where we wait for the other party to generate and
		 * send us data.
		 */
		goto generate_shared_secret;
	}

	/* Generate our ECDH parameters: a private scalar d and a public value Q = dG where G is the
	 * curve generator.
	 * d = random mod (q) where q is the order of the generator G.
	 */
	nn_init(d, 0);
	if (nn_get_random_mod(d, &(curve_params.ec_gen_order))) {
		ret = -1;
		goto out;
	}

	for (int i=17;i>0;i--)
			{
				ext_printf("%08x", d->val[i-1]);
			}
	ext_printf("\n x:");

	/* Q = dG */
	prj_pt_mul_monty(&Q, d, &(curve_params.ec_gen));
	prj_pt_to_aff(&Q_aff, &Q);

	for (int i=17;i>0;i--)
			{
				ext_printf("%08x", Q_aff.x.fp_val.val[i-1]);
			}
	ext_printf("\n y:");
	for (int i=17;i>0;i--)
			{
				ext_printf("%08x", Q_aff.y.fp_val.val[i-1]);
			}
	ext_printf("\n");

	/* Now send the public value Q to the other party, get the other party
	 * public value and compute the shared secret.
	 * Our export size is exactly 3 coordinates in Fp, so this should be 3 times the size
	 * of an element in Fp.
	 */
	prj_pt_export_to_buf(&Q, our_public_buffer,
			     3 * BYTECEIL(curve_params.ec_fp.p_bitlen));

 generate_shared_secret:
	/* Now (non blocking) wait for the other party to send us its public value */
	if (are_equal(other_public_buffer, zero, sizeof(zero))) {
		/* Other party has not sent its public value yet! */
		ret = 0;
		goto out;
	}
	/* If our private value d is not initialized, this means that we have already
	 * done the job of computing the shared secret!
	 */
	if (!nn_is_initialized(d)) {
		ret = 1;
		goto out;
	}
	/* Import the shared value as a projective point */
	prj_pt_import_from_buf(&Q, other_public_buffer,
			       3 * BYTECEIL(curve_params.ec_fp.p_bitlen),
			       &(curve_params.ec_curve));
	/* Compute the shared value = first coordinate of dQ */
	prj_pt_mul_monty(&Q, d, &Q);
	/* Compute the affine coordinates to get the unique (x, y) representation
	 * (projective points are equivalent by a z scalar)
	 */
	prj_pt_to_aff(&Q_aff, &Q);
	ext_printf("  ECDH shared secret computed by %s:\n", role);
	/* The shared secret 'x' is the first coordinate of Q */
	fp_init(x, &(curve_params.ec_fp));
	fp_copy(x, &(Q_aff.x));
	fp_print(x_str, x);
	fp_init(y, &(curve_params.ec_fp));
	fp_copy(y, &(Q_aff.y));
	fp_print(y_str, y);
	ret = 1;

	/* Uninit local variables */
	prj_pt_uninit(&Q);
	aff_pt_uninit(&Q_aff);
	fp_uninit(x);
	fp_uninit(y);
	nn_uninit(d);

 out:
	return ret;
}
#define CURVE_ECDH
#ifdef CURVE_ECDH

uint8_t charbuf[2048*2];



int mainkey()
{
	u8 curve_name[MAX_CURVE_NAME_LEN] = { 0 };

	local_memset(Alice_to_Bob, 0, sizeof(Alice_to_Bob));
	local_memset(Bob_to_Alice, 0, sizeof(Bob_to_Alice));

	ec_get_curve_name_by_type(ec_maps[5].type, curve_name,
				  sizeof(curve_name));

	ext_printf("[+] ECDH on curve %s\n", curve_name);
	ECDH_helper(curve_name, (const u8 *)"Alice");
	ECDH_helper(curve_name, (const u8 *)"Bob");

	ECDH_helper(curve_name, (const u8 *)"Alice");

	return 0;
}
#endif /* CURVE_ECDH */