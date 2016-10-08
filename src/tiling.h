//
// Created by antoinewdg on 29/09/16.
//

#ifndef MULTI_FOCUS_TILING_H
#define MULTI_FOCUS_TILING_H

#include "utils.h"
#include "splining.h"

vector<vector<cv::Rect>> get_tiles(cv::Mat &src, int tile_size) ;

Mat_<float> tiled_laplacian(cv::Mat_<float> &laplacian, vector<vector<cv::Rect>>& tiles);

Mat_<uchar> compute_depth_map(vector<Mat_<float>> tiled_laplacians);

Mat_<Vec3b> tiling_multifocus(vector<Mat_<Vec3b>> &images, int tsize = 9);
#endif //MULTI_FOCUS_TILING_H
