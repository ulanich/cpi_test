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
#ifndef __LIB_ECC_CONFIG_H__
#define __LIB_ECC_CONFIG_H__

/*
 * This configuration file provides various knobs to configure
 * what will be built in the library (supported curves, hash
 * algorithms and signature/verification schemes).
 */

/* It is possible to override the LIBECC configuration by defining 
 * the WITH_LIBECC_CONFIG_OVERRIDE preprocessing flag in the CFLAGS. When
 * this is done, it is expected that the user defines the curves,
 * hash algorithms and signature schemes in the compilation
 * command line (e.g. via the CFLAGS).
 * For instance, in order to only use FRP256V1, SHA-256 and ECDSA, add to the CFLAGS:
 *
 *   -DWITH_LIBECC_CONFIG_OVERRIDE -DWITH_CURVE_FRP256V1 -DWITH_HASH_SHA256 -DWITH_SIG_ECDSA
 *
 */
#ifndef WITH_LIBECC_CONFIG_OVERRIDE

/* Supported curves */
#undef WITH_CURVE_FRP256V1
#undef WITH_CURVE_SECP192R1
#undef WITH_CURVE_SECP224R1
#undef WITH_CURVE_SECP256R1
#undef WITH_CURVE_SECP384R1
#define WITH_CURVE_SECP521R1
#undef WITH_CURVE_BRAINPOOLP224R1
#undef WITH_CURVE_BRAINPOOLP256R1
#undef WITH_CURVE_BRAINPOOLP384R1
#undef WITH_CURVE_BRAINPOOLP512R1
#undef WITH_CURVE_GOST256
#undef WITH_CURVE_GOST512
/* ADD curves define here */
/* XXX: Do not remove the comment above, as it is
 * used by external tools as a placeholder to add or
 * remove automatically generated code.
 */

/* Supported hash algorithms */
#undef WITH_HASH_SHA224
#undef WITH_HASH_SHA256
#undef WITH_HASH_SHA384
#undef WITH_HASH_SHA512
#undef WITH_HASH_SHA3_224
#undef WITH_HASH_SHA3_256
#undef WITH_HASH_SHA3_384
#undef WITH_HASH_SHA3_512

/* Supported sig/verif schemes */
#undef WITH_SIG_ECDSA
#undef WITH_SIG_ECKCDSA
#undef WITH_SIG_ECSDSA
#undef WITH_SIG_ECOSDSA
#undef WITH_SIG_ECFSDSA
#undef WITH_SIG_ECGDSA
#undef WITH_SIG_ECRDSA

#endif /* WITH_LIBECC_CONFIG_OVERRIDE */

#endif /* __LIB_ECC_CONFIG_H__ */
