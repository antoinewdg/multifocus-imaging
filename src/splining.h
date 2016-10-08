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

Mat_<float> expand_n_times(Mat_<float> &src, const Mat_<float> &w, int n);
/**
 * Compute the Gaussian pyramid of an image
 *
 * @param src
 * @param w kernel to be used
 * @return
 */
vector<Mat_<float>> compute_gaussian_pyramid(Mat_<float> src, Mat_<float> w,
                                             int max_height = -1);

/**
 * Compute the Laplacian pyramid of an image
 *
 * @param src
 * @param w kernel to be used
 * @return
 */
vector<Mat_<float>> compute_laplacian_pyramid(Mat_<float> src, Mat_<float> w,
                                              int max_height = -1);

/**
 * Merge two grayscale images using multi-resolution splining.
 *
 * The region parameter is a mask indicating where the left
 * image should be dominant, (1-region) is the equivalent
 * for the left image.
 *
 * @param im_a
 * @param im_b
 * @param region
 * @return
 */
Mat_<float> merge_images(Mat_<float> &im_a, Mat_<float> &im_b,
                         Mat_<float> region = Mat_<float>(),
                         int max_height = -1);

/**
 * Merge two colored images using multi-resolution splining.
 *
 * Performs the splining on each RGB component.
 * The region parameter is a mask indicating where the left
 * image should be dominant, (1-region) is the equivalent
 * for the left image.
 *
 * @param im_a
 * @param im_b
 * @param region
 * @return
 */
Mat_<Vec3b> merge_images(Mat_<Vec3b> &im_a, Mat_<Vec3b> &im_b,
                         Mat_<float> region = Mat_<float>(),
                         int max_height = -1);

Mat_<float> merge_multiple_images(vector<Mat_<float>>& images,
                                  vector<Mat_<float>>& regions);

Mat_<Vec3b> merge_multiple_images(vector<Mat_<Vec3b>>& images,
                                  vector<Mat_<float>>& regions);



Mat_<float> pad_to_next_square(Mat_<float> original);
#endif //MULTI_FOCUS_SPLINING_H
