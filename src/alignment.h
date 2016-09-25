//
// Created by antoinewdg on 23/09/16.
//

#ifndef MULTI_FOCUS_ALIGNMENT_H
#define MULTI_FOCUS_ALIGNMENT_H

#include <numeric>

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include "utils.h"

using cv::xfeatures2d::SURF;


//void align_images(vector<Mat_<Vec3b>> &images);

void align_a_on_b(Mat_<Vec3b> &im_a, const Mat_<Vec3b> &im_b);

#endif //MULTI_FOCUS_ALIGNMENT_H
