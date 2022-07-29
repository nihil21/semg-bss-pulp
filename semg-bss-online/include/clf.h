/*
Copyright 2022 Mattia Orlandi

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
