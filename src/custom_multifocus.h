//
// Created by antoinewdg on 04/10/16.
//

#ifndef MULTI_FOCUS_CUSTOM_MULTIFOCUS_H
#define MULTI_FOCUS_CUSTOM_MULTIFOCUS_H

#include <opencv2/photo/photo.hpp>

#include "utils.h"
#include "splining.h"

Mat_<Vec3b> custom_multifocus(vector<Mat_<Vec3b>> &images, int t);

#endif //MULTI_FOCUS_CUSTOM_MULTIFOCUS_H
