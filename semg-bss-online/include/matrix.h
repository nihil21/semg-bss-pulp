#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>
#include "pmsis.h"

/*
 * Data type declaration and macros
 */
typedef struct {
    float *data;
    size_t height;
    size_t width;
    size_t offset;
} Matrix;

#define MAT_DATA(m) (((Matrix *) (m))->data)
#define MAT_IDX(m, i, j) (((i) * ((m)->offset)) + (j))
#define MAT_CELL(m, i, j) ((MAT_DATA(m))[MAT_IDX(m, i, j)])

/*
 * Transfer matrices between Fabric Controller and Cluster
 */
void mat_fc2cl(const Matrix *m_fc, const Matrix *m_cl);
void mat_cl2fc(const Matrix *m_cl, const Matrix *m_fc);

/*
 * Matrix manipulation
 */
Matrix slice(const Matrix *m, size_t row_start, size_t row_stop, size_t col_start, size_t col_stop);

/* 
 * Matrix operations
 */
void add_row_h(const Matrix *m, const Matrix *r);
void add_row_w(const Matrix *m, const Matrix *r);
void sub_col_h(const Matrix *m, const Matrix *c);
void sub_col_w(const Matrix *m, const Matrix *c);

/*
 * Utils
 */
void print_mat(const Matrix *m);

#endif
