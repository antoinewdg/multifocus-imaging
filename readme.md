# Multi-focus imaging

Implementation of [1]
as a school project. [Test](#1).

A big part of the project relies on the paper 
[A Multiresolution Spline With Application to
 Image Mosaics](http://www.cs.toronto.edu/~mangas/teaching/320/assignments/a3/spline83.pdf).


Use of the [OpenCV 3.1](http://opencv.org/opencv-3-1.html) library for image processing routines.


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

The 



## Reference:

* \[1\]: <a name="1"></a> [Multi-focus imaging using local focus estimation and mosaicking](https://vision.ece.ucsb.edu/sites/vision.ece.ucsb.edu/files/publications/fedorov_focus_icip2006.pdf/)
