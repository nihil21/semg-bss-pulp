#include <stdio.h>
#include <stdlib.h>
#include "../include/hand_open.h"
#include "../include/separator.h"
#include "../include/dnn.h"
#include "../include/svm.h"
#include "../include/shared_buf.h"
#include "../include/matrix.h"
#include "../include/decomp.h"
#include "../include/clf.h"

static void run_semg_bss() {
    // Prepare matrices:
    // 1. Input sEMG signal
    Matrix emg = {
        .data = emg_data,
        .height = N_SAMPLES,
        .width = N_CH,
        .offset = N_CH
    };
    // 2. Decomposition model
    Matrix mean_vec = {
        .data = mean_vec_data,
        .height = N_CH_EXT,
        .width = 1,
        .offset = 1
    };
    Matrix white_mtx = {
        .data = white_mtx_data,
        .height = N_CH_EXT,
        .width = N_CH_EXT,
        .offset = N_CH_EXT
    };
    Matrix sep_mtx = {
        .data = sep_mtx_data,
        .height = N_MU,
        .width = N_CH_EXT,
        .offset = N_CH_EXT
    };
    Matrix spike_th = {
        .data = spike_th_data,
        .height = N_MU,
        .width = 1,
        .offset = 1
    };
    // 3. DNN
    Matrix dnn1_w = {
        .data = dnn1_w_data,
        .height = N_TA,
        .width = N_SAMPLES,
        .offset = N_SAMPLES
    };
    Matrix dnn1_b = {
        .data = dnn1_b_data,
        .height = N_TA,
        .width = 1,
        .offset = 1
    };
    Matrix dnn2_w = {
        .data = dnn2_w_data,
        .height = N_CA,
        .width = N_MU * N_TA,
        .offset = N_MU * N_TA
    };
    Matrix dnn2_b = {
        .data = dnn2_b_data,
        .height = N_CA,
        .width = 1,
        .offset = 1
    };
    Matrix dnn3_w = {
        .data = dnn3_w_data,
        .height = N_OUT,
        .width = N_CA,
        .offset = N_CA
    };
    Matrix dnn3_b = {
        .data = dnn3_b_data,
        .height = N_OUT,
        .width = 1,
        .offset = 1
    };
    // 4. SVM
    Matrix svm_coef = {
        .data = svm_coef_data,
        .height = N_OUT * (N_OUT - 1) / 2,
        .width = N_MU,
        .offset = N_MU
    };
    Matrix svm_intercept = {
        .data = svm_intercept_data,
        .height = N_OUT * (N_OUT - 1) / 2,
        .width = 1,
        .offset = 1
    };

    // Initialize cluster device
    struct pi_cluster_conf cl_conf;
    pi_cluster_conf_init(&cl_conf);
    struct pi_device cluster_dev;
    pi_open_from_conf(&cluster_dev, &cl_conf);

    // Open cluster device
    if (pi_cluster_open(&cluster_dev))
        pmsis_exit(-1);

    // Execute decomposition task on the cluster
    DecompArgs decomp_args = {
        .emg = &emg,
        .mean_vec = &mean_vec,
        .white_mtx = &white_mtx,
        .sep_mtx = &sep_mtx,
        .spike_th = &spike_th
    };
    struct pi_cluster_task decomp_task;
    pi_cluster_task(&decomp_task, decomp_entry, (void *) &decomp_args);
    pi_cluster_send_task_to_cl(&cluster_dev, &decomp_task);

    // Execute classification task on the cluster
    uint8_t class = 0;
#if defined(USE_SVM)
    printf("Using SVM\n");
    SVMArgs clf_args = {
        .svm_coef = &svm_coef,
        .svm_intercept = &svm_intercept,
        .class = &class
    };
#else
    printf("Using DNN\n");
    DNNArgs clf_args = {
        .dnn1_w = &dnn1_w,
        .dnn1_b = &dnn1_b,
        .dnn2_w = &dnn2_w,
        .dnn2_b = &dnn2_b,
        .dnn3_w = &dnn3_w,
        .dnn3_b = &dnn3_b,
        .class = &class
    };
#endif
    struct pi_cluster_task clf_task;
    pi_cluster_task(&clf_task, clf_entry, (void *) &clf_args);
    pi_cluster_send_task_to_cl(&cluster_dev, &clf_task);

    // Release cluster device
    pi_cluster_close(&cluster_dev);

    switch (class) {
        case 0:
            printf("Predicted class: rest\n");
            break;
        case 1:
            printf("Predicted class: hand_open\n");
            break;
        case 2:
            printf("Predicted class: fist\n");
            break;
        case 3:
            printf("Predicted class: index\n");
            break;
        case 4:
            printf("Predicted class: ok\n");
            break;
        default:
            printf("Unexpected class\n");
    }

    pmsis_exit(0);
}

int main(int argc, char *argv[]) {
    return pmsis_kickoff((void *)run_semg_bss);
}