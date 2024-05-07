# This folder contains:
1. A program "find_allignment.cpp" that finds the amount of rows and columns to shift such that the NoIR and Optical images are alligned.
2. A program "label_data.cpp" that lines up images, subtracts them, and saves them to appropriate folders

## Find allignment

$\textbf{Prerequisites:}$
You're going to need an NVIDIA GPU, OpenCV with OpenCV Contrib for this one. This is due to the fact that we need to iterate over a 4k image numerous times, and is simply impractical to compute on the CPU.

$\textit{Remarks:}$
1. You shouldn't need to use this ever again, but if a new holder for the cameras was warranted, then you would need to run this to find the new allignment.
2. Make sure to change the DATA_PATH variable to the directory containing the C++ file on your computer.

$\textbf{What it is doing:}$
In order to understand what will happen when you run the code, consider the following facts:
1. We can detect edges in a photo using Canny edge detection. In an edge detected photo, all pixels containing an edge will be set to 255, and all pixels without an edge set to 0.
2. Two images under different filters share the same edges.
3. If two images that were taken from different perspectives share some region R, then the detected edges in region R will be the same always.

It follows that, for any way to crop the NoIR and Optical images' edge maps to some region R', the dot product between the edge maps reaches a maximum exactly when R'=R, the overlapping region.

This program finds the "shift" the images must undergo in order to share exactly the same region, then prints it out. It does this in a manner similar to sliding one image over the other, checking the dot product along the way.

## Label data

$\textbf{Prerequisites:}$
OpenCV is the only additional software needed.

$\textit{Remarks:}$
1. Make sure to change the directories to those that fit your computer
2. If the "shift" in the NoIR/Optical images ever changes due to reasons discussed above, then one must change the "process_and_save" method.
3. This program heavily relies on the file name formatting defined in the DataCollection program as it is right now.

$\textbf{Why this program:}$
We have footage from three different cameras, each yielding three different videos of the same thing under a different filter. We need to rip these videos apart into training examples, where each training example has a NoIR, Optical, image subtraction, and thermal frame of the same scene.

A logical first step would be to realize that if we find the frames in each video that were captured at the same time, we can just resume from there and recover all of the training examples at once. There are a couple of pitfalls to this:
1. Due to the design choice of saving once every minute in the data collection program, we would at the very least have to find the same frame taken at the same time for every minute of video we have.
2. Divergence from containing the same scene. Even though I solved the different frame rate problem (Thermal camera: 25fps, Jetson Nano: 30fps. Solve by taking 5 frames of thermal for every 6 frames of Jetson Nano), It has been realized that the true time of capture between thermal and Jetson nano video will diverge relatively quickly.

This is where you either decide "I shall try to pick up on the pattern in how the videos diverge" so as to only deal with the first problem, or bite the bullet and make a program that allows you to just allign each frame of every video by hand. The first of which may be worth looking into (given this project is continued), but I chose the second. 

The person who chops up the video has three windows pop up, one for each video type, and uses the keys {d, c, e} to move one frame forward of their choice.
Key bindings:
1. 'd' to go to the next thermal frame
2. 'c' to go to the next NoIR frame
3. 'e' to go to the next Optical frame

Once the images shown contain the same thing, the user presses 'q' on the keyboard to save a short burst of frames to the train set.


