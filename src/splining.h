//
// Created by antoinewdg on 24/09/16.
//

#ifndef MULTI_FOCUS_SPLINING_H
#define MULTI_FOCUS_SPLINING_H


#include "utils.h"


Mat_<float> generating_kernel(float a);

vector<Mat_<float>> compute_laplacian_pyramid(Mat_<float> src, Mat_<float> w);

Mat_<float> merge_images(Mat_<float> &im_a, Mat_<float> &im_b);

Mat_<Vec3b> merge_images(Mat_<Vec3b> &im_a, Mat_<Vec3b> &im_b);

vector<Mat_<float>> compute_gaussian_pyramid(Mat_<float> src, Mat_<float> w);

#endif //MULTI_FOCUS_SPLINING_H
