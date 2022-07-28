#ifndef DECOMP_H
#define DECOMP_H

#include "pmsis.h"
#include "matrix.h"

typedef struct {
    Matrix *emg;
    Matrix *mean_vec;
    Matrix *white_mtx;
    Matrix *sep_mtx;
    Matrix *spike_th;
    Matrix *firings;
} DecompArgs;

void decomp_fn(void *args);
void decomp_entry(void *args);

#endif