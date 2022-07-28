#include "../include/shared_buf.h"

PI_L1 float buffer[CORES];
PI_L1 float g_acc;
PI_L1 float tmp1_data[N_CH_EXT];
PI_L1 float tmp2_data[N_CH_EXT * N_CH_EXT];
PI_L1 float tmp3_data[N_MU * N_CH_EXT];
PI_L1 float tmp4_data[N_MU];
PI_L1 float tmp5_data[N_CH_EXT * Q];
PI_L1 float tmp6_data[N_CH_EXT * EXT_WIN];
PI_L1 uint8_t tmp7_data[N_MU * N_SAMPLES];