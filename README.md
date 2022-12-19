# sEMG BSS on PULP
Blind source separation of sEMG signals on PULP platform (inference only). This code has been used in the paper [Mattia Orlandi et al., sEMG Neural Spikes Reconstruction for Gesture Recognition on a Low-Power Multicore Processor. In: Biomedical Circuits and Systems](https://dx.doi.org/10.1109/BioCAS54905.2022.9948617).

## Usage
First of all, use `docker-compose run --rm pulp` to run the container and open a shell.
Then, move to the directory containing the project (i.e., `~/semg-bss-online`) and compile it using `make clean all`; you can set the following variables:

- `NUM_CORES`: number of cores to use (from 1 to 8);
- `FS`: sampling frequency of the sEMG signal;
- `WIN_LEN`: length (in ms) of the sEMG signal;
- `N_CH`: number of sEMG channels;
- `FE`: extension factor used by the BSS algorithm;
- `N_MU`: number of motor units discovered during the training phase;
- `Q`: number of time steps processed at a time;
- `N_TA`: number of hidden neurons in the temporal aggregation layer;
- `N_CA`: number of hidden neurons in the channel aggregation layer;
- `N_OUT`: number of output classes;
- `USE_SVM`: whether to use the Support Vector Machine or the Multilayer Perceptron.

Finally, the program can be run with `make run`.
