//
// Created by antoinewdg on 21/09/16.
//


#ifndef MULTI_FOCUS_UTILS_IMAGE_H
#define MULTI_FOCUS_UTILS_IMAGE_H

#include "utils.h"


inline cv::Mat_<float> tiled_laplacian(cv::Mat_<float> src, int tsize, int step) {
    cv::Mat_<float> l = laplacian(src);
    cv::Mat_<float> kernel = (cv::Mat_<float>(tsize, tsize));
    for (int i = 0; i < tsize * tsize; i++) {
        kernel(i) = 1.f;
    }


    cv::filter2D(l, l, CV_32F, kernel);

    int offset = (tsize - 1) / 2 + 1;
    step = offset;

    cv::Mat_<float> tiled((l.rows - offset) / step, (l.cols - offset) / step);
    cv::Mat_<float> tile;
    for (int i = 0; i < tiled.rows; i++) {
        for (int j = 0; j < tiled.cols; j++) {
            tile = src(cv::Range(step * i, std::min(step * i + tsize, l.rows)),
                       cv::Range(step * j, std::min(step * j + tsize, l.cols))).clone();
//            std::cout << i << j << std::endl;
//            cv::imshow("dd", tile);
//            cv::waitKey();
//            auto median_it = tile.begin() + ((tile.rows * tile.cols) / 2);
//            std::nth_element(tile.begin(), median_it, tile.end());

//            copy *= 255;
            if (i == 1 && j == 1) {
                cv::Mat_<uchar> copy;
                cv::Mat_<float>(tile * 255).convertTo(copy, CV_8UC1);
//                std::cout << copy << std::endl;
                cv::imshow("dd", copy);
                cv::waitKey();
                cv::imwrite("lena_tile.png", copy);
//                std::cout << "Write" << std::endl;
            }


//            tiled(i, j) = l(offset + i * step, offset + j * step) * (*median_it);
        }
    }

//    cv::Mat_<float> tile;
//    for(int i = )


    return tiled;

}

#endif //MULTI_FOCUS_UTILS_IMAGE_H
