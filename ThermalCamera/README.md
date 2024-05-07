# This directory contains:

1. An interactive python notebook "rotate_images.ipynb" that is used to augment the data
2. Another python notebook "training_and_test.ipynb" that is used for just what you think.

## Data augmentation

Pretty simple image manipulations if you ask me. For each training example in the unaugmented set:
1. Produce a new image by reflecting over the vertical axis
2. Produce a new image by reflecting over the horizontal axis
3. Produce a new image by reflecting over the vertical axis, then the horizontal axis

Here's a little problem that, if you figure out how to solve it, you should now have access to more data then you will ever need.

Our data contains tuples of images that are not exactly perfect - here's why:
The focal length, lens settings, and other paramters that can vary from camera to camera, are not the same between the thermal and Jetson Nano cameras, resulting in a slightly different field of view. The scence that the thermal camera captures is a subset of that captured by the Jetson Nano. Therefore, the mapping that the image to image translation model learns also should clip off a certain percentage of the NoIR/Optical photos.

In the ideal case (same field of view for every camera), you would be able to stack all three photos (thermal, NoIR, Optical) on top of each other and you would be looking at the same scene in all of them. Now consider slicing the stack into a KxN grid. There are (KN)! many permutations of this grid. Enumerate through these, and add them to the train set.


## The model

The input of the image to image translation model consists of the NoIR, Optical, and image subtraction stacked on top of each other in that order. The model is an implementation of pix2pix, which is a GAN trained to generate 


## Training the model


How the discriminator works:
Again, each training example contains an input 512x640x9 imaage

Let X denote the set of all 512x640x9 images. Let Y denote the set of all 512x640x3 images.

The discirminator network is a function mapping from X x Y to 



Signs the model has converged:


## Testing the model

Test statistics:
In order to get a grasp of how well the output of the thermal imaging model corresponds to the true thermal image, I restricted most of my analysis to using percent differences.

To be clear, the percent difference that I compute is the sum of the magnitude of percent differences for each test example. It suffices to say that this is an upper bound for the magnitude of true population percent difference.


## What has been tested

Email clukan@mail.bradley.edu for access to a spreadsheet containing (percent difference, model parameters) tuples.
