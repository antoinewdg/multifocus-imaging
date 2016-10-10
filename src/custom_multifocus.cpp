//
// Created by antoinewdg on 04/10/16.
//

#include "custom_multifocus.h"

Mat_<float> dilate(Mat_<float> a, int t) {
    auto elt = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * t + 1, 2 * t + 1));
    Mat_<float> r;
    cv::dilate(a, r, elt, cv::Point(-1, -1), 1, cv::BORDER_REFLECT101);

    return r;
}

Mat_<uchar> build_depth_map(vector<Mat_<Vec3b>> images, int t) {
    vector<Mat_<float>> focus_measures;
    for (auto im : images) {
        auto pyramid = compute_laplacian_pyramid(to_grayscale(im), generating_kernel(0.3f));
        focus_measures.push_back(dilate(pyramid[0], t));
    }

    Mat_<uchar> depth_map(images[0].rows, images[0].cols);
    for (int i = 0; i < depth_map.rows; i++) {
        for (int j = 0; j < depth_map.cols; j++) {
            float max = -10;
            for (int k = 0; k < images.size(); k++) {
                if (focus_measures[k](i, j) > max) {
                    max = focus_measures[k](i, j);
                    depth_map(i, j) = uchar(k);
                }
            }
        }
    }

    cv::fastNlMeansDenoising(depth_map, depth_map);
    return depth_map;
}

vector<Mat_<float>> build_masks(unsigned long n, Mat_<uchar> depth_map) {
    vector<Mat_<float>> masks;
    for (int i = 0; i < n; i++) {
        Mat_<uchar> d = depth_map == i;
        masks.emplace_back();
        d.convertTo(masks.back(), CV_32F, 1.f / 255);
    }


    return masks;
}

Mat_<Vec3b> custom_multifocus(vector<Mat_<Vec3b>> &images, int t) {

    auto depth_map = build_depth_map(images, t);
    auto masks = build_masks(images.size(), depth_map);
    Mat_<Vec3b> merged = merge_multiple_images(images, masks);


    return merged;

}