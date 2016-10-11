# Multi-focus imaging

Initially a simple implementation of [1] as a school project, I ended up making my
own implementation of a multi-resolution spline algorithm inspired by the techniques
used in [1].

A big part of the project relies on [2], which mostly uses techniques described in [3].

This project is written in C++ and uses the [OpenCV 3.1](http://opencv.org/opencv-3-1.html)
library for image processing routines.

## Demonstration

Demonstration of the algorithm can be found [here](demo.md).

## Building and running

To build the project:

```bash
git clone git@github.com:antoinewdg/multifocus-imaging.git
cd multifocus-imaging
mkdir bin
cd bin
cmake ..
make
```

To run it, the images you want to used must be named with the format `<prefix><index>.<extension>`.
For example, the images can be name `photo0.jpg`, `photo1.jpg` and `photo2.jpg`.
The `prefix`, the number of images and `extension` are the first 3 
arguments of the program, the 4th one being a boolean indicating 
whether the images must be realigned before processing.

For example, to use the images of the bug of this repository, after the build step 
described above:

```bash
./multi_focus ../files/sbug0 13 png 0
```

The program loads the input images and displays the resulting multi-focus images 
for different values of a parameter t.

## Deviation from the original algorithm

After implementing the algorithm described in [1], I found it had several limitations:
* it does not perform well when a part of the scene is blurry in every input image, 
the tiles used to decompose the image are visible in these parts
* it is quite complicated: recomposing the image tile by tile is not a trivial task, 
and for optimal quality we also need to perform a graph cut for every tile fusion
* it does not take advantage of the power of the multi-resolution spline: only small tiles
are merged using this technique

For this reasons, I deviated from this algorithm in two ways:
* I used multi-resolution spline to merge all the images at once
* I chose another measure of focus in the image

### Merging all the images at once

To merge all the images at once, I computed a depth map base one the measure
of focus similar to the one in [1], except that depth is computed for each pixel
instead of only computing it for each tile. This depth map is then smoothed using 
a denoising algorithm (here non local means is used, but is probably overkill). 

All the images are then merged using the depth map to build a mask for each 
image.

### Using a different measure of focus

Using the modified Laplacian described in [1] to measure focus gave some slightly 
unsatisfying results in specific cases (nothing huge but for example in the case of the
bug one "hair" move between to images, and the Laplacian counted both positions of the 
"hair" as focused). Consequently, I decided to again take advantage of the insight
given by the multi-resoltion spline, so I used the dilation by a circle of the
base of the pyramid (which corresponds to high frequencies) as a measure of focus.

The size of the circle used for dilatation is the parameter t used in the algorithm.





## Reference:

* \[1\]: [Multi-focus imaging using local focus estimation and mosaicking](https://vision.ece.ucsb.edu/sites/vision.ece.ucsb.edu/files/publications/fedorov_focus_icip2006.pdf/)
* \[2\]: [A Multiresolution Spline With Application to Image Mosaics](http://www.cs.toronto.edu/~mangas/teaching/320/assignments/a3/spline83.pdf)
* \[3\]: [The Laplacian Pyramid as a Compact Image Code](https://forums.cs.tau.ac.il/~hezy/Vision%20Seminar/pyramid83.pdf)
