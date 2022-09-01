// Copyright 2020-2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include "xs3_math_types.h"

/**
 * @page page_bfp_complex_s32_h  bfp_complex_s32.h
 *
 * This header contains functions implementing arithmetic operations on 32-bit complex block
 * floating-point vectors.
 *
 * Functions for initializing complex BFP vectors can be found in @ref page_bfp_init_h.
 *
 * @note This header is included automatically through `bfp_math.h`.
 *
 * @see `bfp_complex_s32_t`
 *
 * @ingroup xs3_math_header_file
 */


/**
 * @brief Set all elements of a complex 32-bit BFP vector to a specified value.
 * 
 * The exponent of `a` is set to `exp`, and each element's mantissa is set to `b`.
 * 
 * After performing this operation, all elements will represent the same value @math{b \cdot 2^{exp}}.
 * 
 * `a` must have been initialized (see bfp_complex_s32_init()).
 * 
 * @param[out] a         BFP vector to update
 * @param[in]  b         New value each complex mantissa is set to
 * @param[in]  exp       New exponent for the BFP vector
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_set(
    bfp_complex_s32_t* a,
    const complex_s32_t b,
    const exponent_t exp);


/**
 * @brief Modify a complex 32-bit BFP vector to use a specified exponent.
 * 
 * This function forces complex BFP vector @vector{A} to use a specified exponent. The mantissa
 * vector @vector{a} will be bit-shifted left or right to compensate for the changed exponent.
 * 
 * This function can be used, for example, before calling a fixed-point arithmetic function to 
 * ensure the underlying mantissa vector has the needed Q-format. As another example, this may be
 * useful when communicating with peripheral devices (e.g. via I2S) that require sample data to
 * be in a specified format.
 * 
 * Note that this sets the _current_ encoding, and does not _fix_ the exponent permanently (i.e.
 * subsequent operations may change the exponent as usual).
 * 
 * If the required fixed-point Q-format is `QX.Y`, where `Y` is the number of fractional bits in the
 * resulting mantissas, then the associated exponent (and value for parameter `exp`) is `-Y`.
 * 
 * `a` points to input BFP vector @vector{A}, with complex mantissa vector @vector{a} and exponent
 * @math{a\_exp}. `a` is updated in place to produce resulting BFP vector @math{ \tilde{A} } with
 * complex mantissa vector @math{ \tilde{a} } and exponent @math{\tilde{a}\_exp}.
 * 
 * `exp` is @math{\tilde{a}\_exp}, the required exponent. @math{\Delta{}p = \tilde{a}\_exp - a\_exp}
 * is the required change in exponent.
 * 
 * If @math{\Delta{}p = 0}, the BFP vector is left unmodified.
 * 
 * If @math{\Delta{}p > 0}, the required exponent is larger than the current exponent and an
 * arithmetic right-shift of @math{\Delta{}p} bits is applied to the mantissas @vector{a}. When
 * applying a right-shift, precision may be lost by discarding the @math{\Delta{}p} least
 * significant bits.
 * 
 * If @math{\Delta{}p < 0}, the required exponent is smaller than the current exponent and a
 * left-shift of @math{\Delta{}p} bits is applied to the mantissas @vector{a}. When left-shifting,
 * saturation logic will be applied such that any element that can't be represented exactly with
 * the new exponent will saturate to the 32-bit saturation bounds.
 * 
 * The exponent and headroom of `a` are updated by this function.
 * 
 * @operation{
 * &    \Delta{}p = \tilde{a}\_exp - a\_exp
 * &    \tilde{a_k} \leftarrow sat_{32}( a_k \cdot 2^{-\Delta{}p} )   \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                     \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{A} \text{ (in elements) }
 * }
 * 
 * @param[inout]  a     Input BFP vector @vector{A} / Output BFP vector @math{\tilde{A}}
 * @param[in]     exp   The required exponent, @math{\tilde{a}\_exp}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_use_exponent(
    bfp_complex_s32_t* a,
    const exponent_t exp);


/** 
 * @brief Get the headroom of a complex 32-bit BFP vector.
 * 
 * The headroom of a complex vector is the number of bits that the real and imaginary parts of each of its elements can 
 * be left-shifted without losing any information. It conveys information about the range of values that vector may 
 * contain, which is useful for determining how best to preserve precision in potentially lossy block floating-point 
 * operations.
 * 
 * In a BFP context, headroom applies to mantissas only, not exponents.
 * 
 * In particular, if the complex 32-bit mantissa vector @vector{x} has @math{N} bits of headroom, then for any element 
 * @math{x_k} of @vector{x}
 * 
 * @math{-2^{31-N} \le Re\\{x_k\\} \lt 2^{31-N}}
 * 
 * and
 * 
 * @math{-2^{31-N} \le Im\\{x_k\\} \lt 2^{31-N}}
 * 
 * And for any element @math{X_k = x_k \cdot 2^{x\_exp}} of a complex BFP vector @vector{X}
 * 
 * @math{-2^{31 + x\_exp - N} \le Re\\{X_k\\} \lt 2^{31 + x\_exp - N} }
 * 
 * and
 * 
 * @math{-2^{31 + x\_exp - N} \le Im\\{X_k\\} \lt 2^{31 + x\_exp - N} }
 * 
 * This function determines the headroom of `b`, updates `b->hr` with that value, and then returns `b->hr`.
 *
 * @param   b         complex BFP vector to get the headroom of
 * 
 * @returns    Headroom of complex BFP vector `b` 
 * 
 * @ingroup bfp32_func
 */
C_API
headroom_t bfp_complex_s32_headroom(
    bfp_complex_s32_t* b);


/** 
 * @brief Apply a left-shift to the mantissas of a complex 32-bit BFP vector.
 * 
 * Each complex mantissa of input BFP vector @vector{B} is left-shifted `b_shl` bits and stored in the corresponding 
 * element of output BFP vector @vector{A}.
 * 
 * This operation can be used to add or remove headroom from a BFP vector.
 * 
 * `b_shl` is the number of bits that the real and imaginary parts of each mantissa will be left-shifted. This shift is 
 * signed and arithmetic, so negative values for `b_shl` will right-shift the mantissas.
 * 
 * `a` and `b` must have been initialized (see bfp_complex_s32_init()), and must be the same length.
 * 
 * This operation can be performed safely in-place on `b`.
 * 
 * Note that this operation bypasses the logic protecting the caller from saturation or underflows. Output values 
 * saturate to the symmetric 32-bit range (@math{-2^{31} \lt \lt 2^{31}}). To avoid saturation, `b_shl` should be no
 * greater than the headroom of `b` (`b->hr`).
 * 
 * @operation{
 * &    Re\\{a_k\\} \leftarrow sat_{32}( \lfloor Re\\{b_k\\} \cdot 2^{b\_shl} \rfloor )     \\
 * &    Im\\{a_k\\} \leftarrow sat_{32}( \lfloor Im\\{b_k\\} \cdot 2^{b\_shl} \rfloor )     \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                           \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}         \\
 * &        \qquad\text{  and } b_k \text{ and } a_k \text{ are the } k\text{th mantissas from } 
 *              \bar{B}\text{ and } \bar{A}\text{ respectively}
 * }
 * 
 * @param[out] a        Complex output BFP vector @vector{A}
 * @param[in]  b        Complex input BFP vector @vector{B}
 * @param[in]  b_shl    Signed arithmetic left-shift to be applied to mantissas of @vector{B}.
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_shl(
    bfp_complex_s32_t* a,
    const bfp_complex_s32_t* b,
    const left_shift_t b_shl);


/** 
 * @brief Multiply a complex 32-bit BFP vector element-wise by a real 32-bit BFP vector.
 * 
 * Each complex output element @math{A_k} of complex output BFP vector @vector{A} is set to the complex product of 
 * @math{B_k} and @math{C_k}, the corresponding elements of complex input BFP vector @vector{B} and real input BFP 
 * vector @vector{C} respectively.
 * 
 * `a`, `b` and `c` must have been initialized (see bfp_complex_s32_init() and bfp_s32_init()), and must be the same 
 * length.
 * 
 * This operation can be performed safely in-place on `b`.
 * 
 * @operation{
 * &    A_k \leftarrow B_k \cdot C_k                    \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}\text{ and }\bar{C}
 * }
 * 
 * @param[out] a     Output complex BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * @param[in]  c     Input real BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_real_mul(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b, 
    const bfp_s32_t* c);


/** 
 * @brief Multiply one complex 32-bit BFP vector element-wise by another.
 * 
 * Each complex output element @math{A_k} of complex output BFP vector @vector{A} is set to the complex product of 
 * @math{B_k} and @math{C_k}, the corresponding elements of complex input BFP vectors @vector{B} and @vector{C} 
 * respectively.
 * 
 * `a`, `b` and `c` must have been initialized (see bfp_complex_s32_init()), and must be the same length.
 * 
 * This operation can be performed safely in-place on `b` or `c`.
 * 
 * @operation{
 * &    A_k \leftarrow B_k \cdot C_k                    \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}\text{ and }\bar{C}
 * }
 * 
 * @param[out] a     Output complex BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * @param[in]  c     Input complex BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_mul(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b, 
    const bfp_complex_s32_t* c);
    

/** 
 * @brief Multiply one complex 32-bit BFP vector element-wise by the complex conjugate of another.
 * 
 * Each complex output element @math{A_k} of complex output BFP vector @vector{A} is set to the complex product of 
 * @math{B_k}, the corresponding element of complex input BFP vectors @vector{B}, and @math{(C_k)^*}, the complex 
 * conjugate of the corresponding element of complex input BFP vector @vector{C}.
 * 
 * `a`, `b` and `c` must have been initialized (see bfp_complex_s32_init()), and must be the same length.
 * 
 * This operation can be performed safely in-place on `b` or `c`.
 * 
 * @operation{
 * &    A_k \leftarrow B_k \cdot (C_k)^*                                                \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                                       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}\text{ and }\bar{C}  \\
 * &        \qquad\text{and } (C_k)^* \text{ is the complex conjugate of } C_k
 * }
 * 
 * @param[out] a     Output complex BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * @param[in]  c     Input complex BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_conj_mul(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b, 
    const bfp_complex_s32_t* c);


/**
 * @brief Multiply one complex 32-bit BFP vector by another element-wise and add the result to a
 * third vector.
 * 
 * @operation{
 * &    A_k \leftarrow A_k + (B_k \cdot C_k)            \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}\text{ and }\bar{C}
 * }
 * 
 * @param[inout]  acc   Input/Output accumulator complex BFP vector @vector{A}
 * @param[in]     b     Input complex BFP vector @vector{B}
 * @param[in]     c     Input complex BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_macc(
    bfp_complex_s32_t* acc, 
    const bfp_complex_s32_t* b, 
    const bfp_complex_s32_t* c);


/**
 * @brief Multiply one complex 32-bit BFP vector by another element-wise and subtract the result
 * from a third vector.
 * 
 * @operation{
 * &    A_k \leftarrow A_k - (B_k \cdot C_k)            \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}\text{ and }\bar{C}
 * }
 * 
 * @param[inout]  acc   Input/Output accumulator complex BFP vector @vector{A}
 * @param[in]     b     Input complex BFP vector @vector{B}
 * @param[in]     c     Input complex BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_nmacc(
    bfp_complex_s32_t* acc, 
    const bfp_complex_s32_t* b, 
    const bfp_complex_s32_t* c);


/**
 * @brief Multiply one complex 32-bit BFP vector by the complex conjugate of another element-wise
 * and add the result to a third vector.
 * 
 * @operation{
 * &    A_k \leftarrow A_k + (B_k \cdot C_k^*)                                          \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                                       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}\text{ and }\bar{C}  \\
 * &        \qquad\text{and } (C_k)^* \text{ is the complex conjugate of } C_k
 * }
 * 
 * @param[inout]  acc   Input/Output accumulator complex BFP vector @vector{A}
 * @param[in]     b     Input complex BFP vector @vector{B}
 * @param[in]     c     Input complex BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_conj_macc(
    bfp_complex_s32_t* acc, 
    const bfp_complex_s32_t* b, 
    const bfp_complex_s32_t* c);


/**
 * @brief Multiply one complex 32-bit BFP vector by the complex conjugate of another element-wise
 * and subtract the result from a third vector.
 * 
 * @operation{
 * &    A_k \leftarrow A_k - (B_k \cdot C_k^*)                                          \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                                       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}\text{ and }\bar{C}  \\
 * &        \qquad\text{and } (C_k)^* \text{ is the complex conjugate of } C_k
 * }
 * 
 * @param[inout]  acc   Input/Output accumulator complex BFP vector @vector{A}
 * @param[in]     b     Input complex BFP vector @vector{B}
 * @param[in]     c     Input complex BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_conj_nmacc(
    bfp_complex_s32_t* acc, 
    const bfp_complex_s32_t* b, 
    const bfp_complex_s32_t* c);


/** 
 * @brief Multiply a complex 32-bit BFP vector by a real scalar.
 * 
 * Each complex output element @math{A_k} of complex output BFP vector @vector{A} is set to the complex product of 
 * @math{B_k}, the corresponding element of complex input BFP vector @vector{B}, and real scalar 
 * @math{\alpha\cdot 2^{\alpha\_exp}}, where @math{\alpha} and @math{\alpha\_exp} are the mantissa and exponent 
 * respectively of parameter `alpha`.
 * 
 * `a` and `b` must have been initialized (see bfp_complex_s32_init()), and must be the same length.
 * 
 * This operation can be performed safely in-place on `b`.
 * 
 * @operation{
 *      \bar{A} \leftarrow \bar{B} \cdot \left( \alpha \cdot 2^{\alpha\_exp} \right)
 * }
 * 
 * @param[out] a            Output complex BFP vector @vector{A}
 * @param[in]  b            Input complex BFP vector @vector{B}
 * @param[in]  alpha        Real scalar by which @vector{B} is multiplied
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_real_scale(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b, 
    const float_s32_t alpha);
    

/** 
 * @brief Multiply a complex 32-bit BFP vector by a complex scalar.
 * 
 * Each complex output element @math{A_k} of complex output BFP vector @vector{A} is set to the complex product of 
 * @math{B_k}, the corresponding element of complex input BFP vector @vector{B}, and complex scalar 
 * @math{\alpha\cdot 2^{\alpha\_exp}}, where @math{\alpha} and @math{\alpha\_exp} are the complex mantissa and exponent 
 * respectively of parameter `alpha`.
 * 
 * `a` and `b` must have been initialized (see bfp_complex_s32_init()), and must be the same length.
 * 
 * This operation can be performed safely in-place on `b`.
 * 
 * @operation{
 *      \bar{A} \leftarrow \bar{B} \cdot \left( \alpha \cdot 2^{\alpha\_exp} \right)
 * }
 * 
 * @param[out] a            Output complex BFP vector @vector{A}
 * @param[in]  b            Input complex BFP vector @vector{B}
 * @param[in]  alpha        Complex scalar by which @vector{B} is multiplied
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_scale(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b, 
    const float_complex_s32_t alpha);
    

/** 
 * @brief Add one complex 32-bit BFP vector to another.
 * 
 * Each complex output element @math{A_k} of complex output BFP vector @vector{A} is set to the sum of @math{B_k} and 
 * @math{C_k}, the corresponding elements of complex input BFP vectors @vector{B} and @vector{C} respectively.
 * 
 * `a`, `b` and `c` must have been initialized (see bfp_complex_s32_init()), and must be the same length.
 * 
 * This operation can be performed safely in-place on `b` or `c`.
 * 
 * 
 * @operation{
 *      \bar{A} \leftarrow \bar{B} + \bar{C}
 * }
 * 
 * @param[out] a     Output complex BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * @param[in]  c     Input complex BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_add(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b, 
    const bfp_complex_s32_t* c);
    

/**
 * @brief Add a complex scalar to a complex 32-bit BFP vector.
 * 
 * Add a real scalar @math{c} to input BFP vector @vector{B} and store the result in BFP vector
 * @vector{A}. 
 * 
 * `a`, and `b` must have been initialized (see bfp_complex_s32_init()), and must be the same
 * length.
 * 
 * This operation can be performed safely in-place on `b`.
 * 
 * @operation{
 *      \bar{A} \leftarrow \bar{B} + c  
 * }
 * 
 * @param[out] a     Output complex BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * @param[in]  c     Input complex scalar @math{c}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_add_scalar(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b, 
    const float_complex_s32_t c);


/** 
 * @brief Subtract one complex 32-bit BFP vector from another.
 * 
 * Each complex output element @math{A_k} of complex output BFP vector @vector{A} is set to the difference between 
 * @math{B_k} and @math{C_k}, the corresponding elements of complex input BFP vectors @vector{B} and @vector{C} 
 * respectively.
 * 
 * `a`, `b` and `c` must have been initialized (see bfp_complex_s32_init()), and must be the same length.
 * 
 * This operation can be performed safely in-place on `b` or `c`.
 * 
 * 
 * @operation{
 *      \bar{A} \leftarrow \bar{B} - \bar{C}
 * }
 * 
 * @param[out] a     Output complex BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * @param[in]  c     Input complex BFP vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_sub(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b,
    const bfp_complex_s32_t* c);
    

/** 
 * @brief Convert a complex 32-bit BFP vector to a complex 16-bit BFP vector.
 * 
 * Each complex 16-bit output element @math{A_k} of complex output BFP vector @vector{A} is set to the value of 
 * @math{B_k}, the corresponding element of complex 32-bit input BFP vector @vector{B}, with its bit-depth reduced to
 * 16 bits.
 * 
 * `a` and `b` must have been initialized (see bfp_complex_s16_init() and bfp_complex_s32_init()), and must be the 
 * same length.
 * 
 * This function preserves as much precision as possible.
 * 
 * @operation{
 * &    A_k \overset{16-bit}{\longleftarrow} B_k    \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)   \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}
 * }
 * 
 * @param[out] a     Output complex 16-bit BFP vector @vector{A}
 * @param[in]  b     Input complex 32-bit BFP vector @vector{B}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_to_complex_s16(
    bfp_complex_s16_t* a, 
    const bfp_complex_s32_t* b);
    

/** 
 * @brief Get the squared magnitude of each element of a complex 32-bit BFP vector.
 * 
 * Each element @math{A_k} of real output BFP vector @vector{A} is set to the squared magnitude of @math{B_k}, the 
 * corresponding element of complex input BFP vector @vector{B}.
 * 
 * `a` and `b` must have been initialized (see bfp_s32_init() bfp_complex_s32_init()), and must be the same length.
 * 
 * @operation{
 * &    A_k \leftarrow B_k \cdot (B_k)^*                                \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}     \\
 * &        \qquad\text{  and } (B_k)^* \text{ is the complex conjugate of } B_k
 * }
 * 
 * @param[out] a     Output real BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_squared_mag(
    bfp_s32_t* a, 
    const bfp_complex_s32_t* b);


/** 
 * @brief Get the magnitude of each element of a complex 32-bit BFP vector.
 * 
 * Each element @math{A_k} of real output BFP vector @vector{A} is set to the magnitude of @math{B_k}, the corresponding 
 * element of complex input BFP vector @vector{B}.
 * 
 * `a` and `b` must have been initialized (see bfp_s32_init() bfp_complex_s32_init()), and must be the same length.
 * 
 * @operation{
 * &    A_k \leftarrow  \left| B_k \right|                              \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}
 * }
 * 
 * @param[out] a     Output real BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_mag(
    bfp_s32_t* a, 
    const bfp_complex_s32_t* b);
    

/** 
 * @brief Get the sum of elements of a complex 32-bit BFP vector.
 * 
 * The elements of complex input BFP vector @vector{B} are summed together. The result is a complex 64-bit 
 * floating-point scalar @math{a}, which is returned.
 * 
 * `b` must have been initialized (see bfp_complex_s32_init()).
 * 
 * @operation{
 * &    a \leftarrow \sum_{k=0}^{N-1} \left( b_k \cdot 2^{B\_exp} \right)   \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                           \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}
 * }
 * 
 * @param[in]  b     Input complex BFP vector @vector{B}
 * 
 * @returns     @math{a}, the sum of vector @vector{B}'s elements
 * 
 * @ingroup bfp32_func
 */
C_API
float_complex_s64_t bfp_complex_s32_sum( 
    const bfp_complex_s32_t* b);


/** 
 * @brief Get the complex conjugate of each element of a complex 32-bit BFP vector.
 * 
 * Each element @math{A_k} of complex output BFP vector @vector{A} is set to the complex conjugate
 * of @math{B_k}, the corresponding element of complex input BFP vector @vector{B}.
 * 
 * @operation{
 * &    A_k \leftarrow B_k^*                                                            \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                                       \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}\text{ and }\bar{C}  \\
 * &        \qquad\text{and } B_k^* \text{ is the complex conjugate of } B_k
 * }
 * 
 * @param[out] a     Output complex BFP vector @vector{A}
 * @param[in]  b     Input complex BFP vector @vector{B}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_conjugate(
    bfp_complex_s32_t* a, 
    const bfp_complex_s32_t* b);


/**
 * @brief Get the energy of a complex 32-bit BFP vector.
 *
 * The energy of a complex 32-bit BFP vector here is the sum of the squared magnitudes of each of
 * the vector's elements.
 * 
 * @operation{
 * &    a \leftarrow \sum_{k=0}^{N-1} \left( \left|b_k \cdot 2^{B\_exp}\right|^2 \right)    \\
 * &        \qquad\text{for } k \in 0\ ...\ (N-1)                                           \\
 * &        \qquad\text{where } N \text{ is the length of } \bar{B}
 * }
 * 
 * @param[in] b   Input complex BFP vector @vector{B}
 * 
 * @returns     @math{a}, the energy of vector @vector{B}
 * 
 * @ingroup bfp32_func
 */
C_API
float_s64_t bfp_complex_s32_energy(
    const bfp_complex_s32_t* b);



/**
 * @brief Create complex 32-bit BFP vector from real and imaginary parts.
 * 
 * Create a complex 32-bit BFP vector as the sum of a real vector @vector{B} and imaginary vector
 * @math{\bar{C} i}. 
 * 
 * `a`, `b` and `c` must have been initialized (see bfp_complex_s32_init() and bfp_s32_init()), must
 * be the same length. `&a->data[0]` must be a double-word-aligned address.
 * 
 * @operation{
 * & \bar{A} \leftarrow  \bar{B} + \bar{C} i
 * }
 * 
 * @param[out]  a   Complex BFP output vector @vector{A}
 * @param[in]   b   Real BFP input vector @vector{B}
 * @param[in]   c   Real BFP input vector @vector{C}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_make(
    bfp_complex_s32_t* a,
    const bfp_s32_t* b,
    const bfp_s32_t* c);


/**
 * @brief Extract the real part of a complex 32-bit BFP vector.
 * 
 * This function populates the real 32-bit BFP vector @vector{A} with the real part of complex 
 * 32-bit BFP vector @vector{B}.
 * 
 * `&b->data[0]` must be a double-word-aligned address.
 * 
 * @operation{
 * &  \bar{A} \leftarrow  {Real}\\{\bar{B}\\}
 * } 
 * 
 * @param[out]  a   Real BFP output vector @vector{A}
 * @param[in]   b   Complex BFP input vector @vector{B}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_real_part(
    bfp_s32_t* a,
    const bfp_complex_s32_t* b);


/**
 * @brief Extract the imaginary part of a complex 32-bit BFP vector.
 * 
 * This function populates the real 32-bit BFP vector @vector{A} with the imaginary part of complex 
 * 32-bit BFP vector @vector{B}.
 * 
 * `&b->data[0]` must be a double-word-aligned address.
 * 
 * @operation{
 * &  \bar{A} \leftarrow  {Imag}\\{\bar{B}\\}
 * } 
 * 
 * @param[out]  a   Real BFP output vector @vector{A}
 * @param[in]   b   Complex BFP input vector @vector{B}
 * 
 * @ingroup bfp32_func
 */
C_API
void bfp_complex_s32_imag_part(
    bfp_s32_t* a,
    const bfp_complex_s32_t* b);