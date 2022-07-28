#include <string.h>
#include "../include/decomp.h"
#include "../include/shared_buf.h"
#include "../include/pulp_train_utils_fp32.h"
#include "../include/pulp_matmul_fp32.h"

/*
 * Function executed by each core in the cluster
 */
void decomp_fn(void *args) {
    const size_t core_id = pi_core_id();

    // Unpack arguments
    Matrix *emg_l2 = ((DecompArgs *) args)->emg;
    Matrix *mean_vec_l2 = ((DecompArgs *) args)->mean_vec;
    Matrix *white_mtx_l2 = ((DecompArgs *) args)->white_mtx;
    Matrix *sep_mtx_l2 = ((DecompArgs *) args)->sep_mtx;
    Matrix *spike_th_l2 = ((DecompArgs *) args)->spike_th;

    // Prepare L1 matrices
    Matrix cur_slice_l1 = {
        .data = tmp1_data,
        .height = FE,
        .width = N_CH,
        .offset = N_CH,
        .on_cluster = true
    };
    Matrix whitened_slice_l1 = {
        .data = tmp1_data,
        .height = N_CH_EXT,
        .width = 1,
        .offset = 1,
        .on_cluster = true
    };
    Matrix mean_vec_l1 = {
        .data = tmp2_data,
        .height = N_CH_EXT,
        .width = 1,
        .offset = 1,
        .on_cluster = true
    };
    Matrix white_mtx_row_l1 = {
        .data = tmp2_data,
        .height = 1,
        .width = N_CH_EXT,
        .offset = N_CH_EXT,
        .on_cluster = true
    };
    Matrix sep_mtx_l1 = {
        .data = tmp2_data,
        .height = N_MU,
        .width = N_CH_EXT,
        .offset = N_CH_EXT,
        .on_cluster = true
    };
    Matrix spike_th_l1 = {
        .data = tmp2_data,
        .height = N_MU,
        .width = 1,
        .offset = 1,
        .on_cluster = true
    };
    Matrix cur_slice_ext = {
        .data = tmp3_data,
        .height = N_CH_EXT,
        .width = 1,
        .offset = 1,
        .on_cluster = true
    };
    Matrix muapt_slice_l1 = {
        .data = tmp3_data,
        .height = N_MU,
        .width = 1,
        .offset = 1,
        .on_cluster = true
    };

    // Iterate over time steps
    for (int t = FE - 1; t < N_SAMPLES; t++) {
        // Take slice of signal
        size_t from = t - (FE - 1);
        size_t to = t;
        Matrix cur_slice_l2 = slice(emg_l2, from, to, 0, N_CH - 1);

        // Preprocessing: extension
        if (core_id == 0) {
            // Move cur_slice and mean_vec to L1
            pi_cl_dma_cmd_t copy1, copy2;
            mat_fc2cl_async(&cur_slice_l2, &cur_slice_l1, &copy1);
            mat_fc2cl_async(mean_vec_l2, &mean_vec_l1, &copy2);
            pi_cl_dma_cmd_wait(&copy1);
            pi_cl_dma_cmd_wait(&copy2);
        }
        pi_cl_team_barrier();

        size_t i_chunk = (N_CH_EXT + CORES - 1) / CORES;
        size_t i_start = core_id * i_chunk;
        size_t i_end = i_start + i_chunk < N_CH_EXT ? i_start + i_chunk : N_CH_EXT;
        for (size_t i_ext = i_start; i_ext < i_end; i_ext++) {
            size_t j = i_ext / FE;
            size_t i = FE - (i_ext % FE) - 1;
            MAT_CELL(&cur_slice_ext, i_ext, 0) = MAT_CELL(&cur_slice_l1, i, j);
        }
        pi_cl_team_barrier();

        // Preprocessing: centering
        sub_col_par1(&cur_slice_ext, &mean_vec_l1);

        // Preprocessing: whitening
        for (size_t i = 0; i < N_CH_EXT; i++) {
            if (core_id == 0) {
                // Move current white_mtx row to L1
                Matrix white_mtx_row_l2 = slice(white_mtx_l2, i, i, 0, N_CH_EXT - 1);
                mat_fc2cl(&white_mtx_row_l2, &white_mtx_row_l1);
            }
            pi_cl_team_barrier();
            MAT_CELL(&whitened_slice_l1, i, 0) = dot_par(&white_mtx_row_l1, &cur_slice_ext);
        }
        
        // Decomposition
        if (core_id == 0) {
            // Move sep_mtx to L1
            mat_fc2cl(sep_mtx_l2, &sep_mtx_l1);
            // Clear memory for results
            memset(muapt_slice_l1.data, 0, N_MU * sizeof(float));
        }
        pi_cl_team_barrier();
        struct matMul_args mm_arg1 = {
            .A = sep_mtx_l1.data,
            .B = whitened_slice_l1.data,
            .C = muapt_slice_l1.data,
            .N = N_MU,
            .M = 1,
            .K = N_CH_EXT
        };
        mm_M_unroll_8x1((void *) &mm_arg1);
        // mat_mul_par1(&muapt_slice_l1, &sep_mtx_l1, &whitened_slice_l1);  // (N_MU x N_CH_EXT) @ (N_CH_EXT x 1) -> (N_MU x 1)

        // Post-processing: spike detection
        if (core_id == 0) {
            print_mat(&muapt_slice_l1);
            // Move spike_th to L1
            mat_fc2cl(spike_th_l2, &spike_th_l1);
        }
        pi_cl_team_barrier();
        
        i_chunk = (N_MU + CORES - 1) / CORES;
        i_start = core_id * i_chunk;
        i_end = i_start + i_chunk < N_MU ? i_start + i_chunk : N_MU;
        for (int i = i_start; i < i_end; i++) {
            tmp7_data[(t - FE + 1) * N_MU + i] = MAT_CELL(&muapt_slice_l1, i, 0) * MAT_CELL(&muapt_slice_l1, i, 0) >= MAT_CELL(&spike_th_l1, i, 0) ? 1 : 0;
        }
    }
}

/*
 * Cluster entry-point
 */
void decomp_entry(void *args) {
    pi_perf_conf(
        1 << PI_PERF_CYCLES
    );
    pi_perf_stop();
    pi_perf_reset();
    pi_perf_start();

    // Spawn team of parallel processes
    pi_cl_team_fork(CORES, decomp_fn, args);

    pi_perf_stop();

    uint32_t cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    printf("Clock Cycles: %d\n", cycles_cnt);

    for (size_t i = 0; i < N_SAMPLES; i++) {
        for (size_t j = 0; j < N_MU; j++) {
            printf("%d ", tmp7_data[i * N_MU + j]);
        }
        printf("\n");
    }
}