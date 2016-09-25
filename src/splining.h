//
// Created by antoinewdg on 24/09/16.
//

#ifndef MULTI_FOCUS_SPLINING_H
#define MULTI_FOCUS_SPLINING_H


#include "utils.h"

/**
 * Build the generating kernal of parameter a
 *
 * Defined in the paper on splining.
 *
 * @param a
 * @return
 */
Mat_<float> generating_kernel(float a);

/**
 * Compute the Gaussian pyramid of an image
 *
 * @param src
 * @param w kernel to be used
 * @return
 */
vector<Mat_<float>> compute_gaussian_pyramid(Mat_<float> src, Mat_<float> w);

/**
 * Compute the Laplacian pyramid of an image
 *
 * @param src
 * @param w kernel to be used
 * @return
 */
vector<Mat_<float>> compute_laplacian_pyramid(Mat_<float> src, Mat_<float> w);

/**
 * Merge two grayscale images using multi-resolution splining.
 *
 * @param im_a
 * @param im_b
 * @return
 */
Mat_<float> merge_images(Mat_<float> &im_a, Mat_<float> &im_b);

/**
 * Merge two colored images using multi-resolution splining.
 *
 * Performs the splining on each RGB component.
 *
 * @param im_a
 * @param im_b
 * @return
 */
Mat_<Vec3b> merge_images(Mat_<Vec3b> &im_a, Mat_<Vec3b> &im_b);


#endif //MULTI_FOCUS_SPLINING_H
