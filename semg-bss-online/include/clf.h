#ifndef CLF_H
#define CLF_H

#include "pmsis.h"
#include "matrix.h"

typedef struct {
    Matrix *firings;
    Matrix *dnn1_w;
    Matrix *dnn1_b;
    Matrix *dnn2_w;
    Matrix *dnn2_b;
    Matrix *dnn3_w;
    Matrix *dnn3_b;
    uint8_t *class;
} DNNArgs;

typedef struct {
    Matrix *firings;
    Matrix *svm_coef;
    Matrix *svm_intercept;
    uint8_t *class;
} SVMArgs;

void clf_entry(void *args);

#endif