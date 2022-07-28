APP = semg-bss-online

APP_SRCS += src/main.c src/hand_open.c src/separator.c src/dnn.c src/svm.c src/matrix.c src/decomp.c src/clf.c src/shared_buf.c src/pulp_matmul_fp32.c src/pulp_train_utils_fp32.c

# Number of cores of the cluster to use
CORES ?= 1
APP_CFLAGS += -D CORES=$(CORES)

# Signal parameters
FS ?= 4000
WIN_LEN ?= 200
N_CH ?= 16
APP_CFLAGS += -D FS=$(FS) -D WIN_LEN=$(WIN_LEN) -D N_CH=$(N_CH)

# Decomposition parameters
FE ?= 4
N_MU ?= 19
Q ?= 32
APP_CFLAGS += -D FE=$(FE) -D N_MU=$(N_MU) -D Q=$(Q)

# Classification parameters
N_TA ?= 4
N_CA ?= 16
N_OUT ?= 5
APP_CFLAGS += -D N_TA=$(N_TA) -D N_CA=$(N_CA) -D N_OUT=$(N_OUT)
ifdef USE_SVM
APP_CFLAGS += -D USE_SVM
endif

# Optimization flags
APP_CFLAGS += -g -Os

# Math library linking
APP_LDFLAGS += -lm -flto

include $(RULES_DIR)/pmsis_rules.mk