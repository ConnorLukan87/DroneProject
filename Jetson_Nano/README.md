# In this directory, there exists:
1. A "record.cpp" file that once compiled, is typically run on boot by a system service on the Jetson Nano.
2. A "record_n_seconds.sh" shell script that is iteratively executed by the record C++ program.
3. A "record.service" system unit file. This is for recording on boot

## How to set it up

In the home directory on the Jetson Nano, make a folder called "DataCollection":

cd ~ && mkdir DataCollection

Move the "record.cpp" file and "record_n_seconds.sh" file into the "DataCollection" directory:

mv /path_to_this_repo/Jetson_Nano/record.cpp DataCollection/

mv /path_to_this_repo/Jetson_Nano/record_n_seconds.sh DataCollection/

Move into the "DataCollection" directory:

cd DataCollection

Compile the "record.cpp" file:

g++ record.cpp -o out -lpthread -lposix

## Optional: Setting up recording on boot

Start out by moving the "record.service" file into the proper directory:

sudo mv /path_to_this_repo/Jetson_Nano/record.service /etc/systemd/system/

Now enable it:

sudo systemctl enable record.service

And start it:

sudo systemctl start record.service

To stop the service:

sudo systemctl stop record.service

## Helpful tips:

1. A quick way to test if the cameras are recording is to run the command:

nvgstcapture-1.0 --sensor-id=ID

Where "ID" is the sensor id you would like to sample from.

2. Make sure to check how much disk space is on the Nano before recording. Should you run out of disk space, the program will keep on recording and likely corrupt the operating system.

3. Make sure the cameras are connected to the MIPI-CSI ports as defined by the program. Failure to do so will result in wrong filenames after collecting data.
