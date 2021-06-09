# Offline phase: data collection and model training


## Setting up environment

* The device used for training must be rooted.
* Install [Termux](https://termux.com/) environment on Android device.
* Install cmake and python within Termux environment.
* Storage permission needs to be manually granted to Termux app.

Besides, a functional MATLAB and python environment is required for data processing.

## Experiment procedure

Data are first collected on Android device. When finished, the `output` directory
is copied onto workstation for further processing.

### Data collection

The whole `autotest` directory should be placed in Android Termux environment in advance.

1. Manually obtain coordinate for all relavant keys from the keyboard you are currently testing. Use the coordinate information to fill in `virtual_keyboard_location.txt`.
2. Adjust `APP_LOC` variable in `botprogram.py` to provide the coordinate of text input field in your App.
3. In the termux environment, run `cmake ; make` to compile the GPU PC collecting program.
4. Use `su` to switch to root user, then run `./botprogram.py findmatch` to collect data for key-counter correlation. The output file `analyze.txt` provides key-counter correlation information, while `deviceinfo.txt` records device setup.
6. Use `su` to switch to root user, then run `./botprogram.py randomtype` to collect data for classification model.

### Data processing
7. Copy the entire `output` directory out of the Android device. Place it in the same directory as `get_parameter.m` file.
8. Execute `get_parameter.m` with MATLAB to get the threshold parameter.
9. Execute `get_modelfile.py` to get a model file ready to be inserted into the App of online phase. Please refer to the documentation of the online phase for its usage.
