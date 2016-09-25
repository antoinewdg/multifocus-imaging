# Multi-focus imaging

Implementation of the paper 
[Multi-focus imaging using local focus estimation and mosaicking](https://vision.ece.ucsb.edu/sites/vision.ece.ucsb.edu/files/publications/fedorov_focus_icip2006.pdf)
as a school project.

A big part of the project relies on the paper 
[A Multiresolution Spline With Application to
 Image Mosaics](http://www.cs.toronto.edu/~mangas/teaching/320/assignments/a3/spline83.pdf).


Use of the [OpenCV 3.1](http://opencv.org/opencv-3-1.html) library for image processing routines.


## Building

```bash
git clone git@github.com:antoinewdg/multifocus-imaging.git
cd multifocus-imaging
mkdir bin
cd bin
cmake ..
make
```