#ifndef SHARED_BUF_H
#define SHARED_BUF_H

#include "pmsis.h"

#define N_CH_EXT N_CH * FE
#define N_SAMPLES WIN_LEN * FS / 1000
#define EXT_WIN Q - FE + 1

/*
 * Reduction buffer (CORES x 1) and accumulator (1 x 1)
 */
PI_L1 extern float buffer[];
PI_L1 extern float g_acc;

/*
 * Tmp1 buffer for:
 * mean vector (N_CH_EXT)
 * DNN bias (N_TA/N_CA/N_OUT)
 * SVM coefficients (N_OUT * (N_OUT - 1) / 2 x N_MU)
 */
PI_L1 extern float tmp1_data[];  // size: N_CH_EXT

/*
 * Tmp2 buffer for:
 * whitening matrix (N_CH_EXT x N_CH_EXT)
 * DNN weight (N_TA x N_SAMPLES/N_CA x N_TA * N_MU/N_OUT x N_CA)
 * SVM intercept (N_OUT * (N_OUT - 1) x 1)
 */
PI_L1 extern float tmp2_data[];  // size: N_CH_EXT * N_CH_EXT

/*
 * Tmp3 buffer for:
 * separation matrix (N_MU x N_CH_EXT)
 * 1st DNN activation (N_MU x N_TA)
 * 3rd DNN activation (1 x N_OUT)
 * spike count (N_MU x 1)
 */
PI_L1 extern float tmp3_data[];  // size: N_MU * N_CH_EXT

/*
 * Tmp4 buffer for:
 * spike thresholds (N_MU x 1)
 * 2nd DNN activation (1 x N_CA)
 * SVM projection (N_OUT * (N_OUT - 1) x 1)
 */
PI_L1 extern float tmp4_data[];  // size: N_MU

/*
 * Tmp5 buffer for:
 * original sEMG slice (Q x N_CH)
 * whitened sEMG slice (N_CH_EXT x EXT_WIN)
 */
PI_L1 extern float tmp5_data[];  // size: N_CH_EXT * EXT_WIN

/*
 * Tmp6 buffer for:
 * extended sEMG slice (N_CH_EXT * EXT_WIN)
 * centered sEMG slice (N_CH_EXT * EXT_WIN)
 * MUAPT slice (N_MU)
 */
PI_L1 extern float tmp6_data[];  // size: N_CH_EXT * EXT_WIN

/*
 * Tmp7 buffer for:
 * - spike binary matrix (N_MU x N_SAMPLES)
 */
PI_L1 extern uint8_t tmp7_data[];

/* Tmp1 buffer for:
 * - original slice (FE x N_CH = N_CH_EXT)
 * - whitened slice (N_CH_EXT)
 * - slice of spikes (N_MU)
 * - DNN bias (N_TA/N_CA/N_OUT)
 * - spike count (N_MU)

PI_L1 extern float tmp1_data[];  // N_CH_EXT dimensions

*
 * Tmp2 buffer for:
 * - extended slice (N_CH_EXT)
 * - mean vector (N_CH_EXT)


PI_L1 extern float tmp2_data[];  // mean vector, columns of whitening matrix, separation matrix, DNN weights, SVM coefficients
PI_L1 extern float tmp3_data[];  // extended and centered slice, slice of MUAPT, first and second DNN activations, SVM intercepts
PI_L1 extern float tmp4_data[];  // flattened first DNN activation, third DNN activation
PI_L1 extern uint8_t tmp5_data[];  // spikes, SVM 1v1 report*/

#endif