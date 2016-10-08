//
// Created by antoinewdg on 04/10/16.
//

#include "custom_multifocus.h"

Mat_<float> windowed_max(Mat_<float> a, int t) {
    Mat_<float> r(a.rows, a.cols);
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.cols; j++) {
            r(i, j) = -20.f;
            int kmin = std::max(i - t, 0);
            int kmax = std::min(i + t, a.rows);
            int lmin = std::max(j - t, 0);
            int lmax = std::min(j + t, a.cols);
            for (int k = kmin; k < kmax; k++) {
                for (int l = lmin; l < lmax; l++) {
                    if (a(k, l) > r(i, j)) {
                        r(i, j) = a(k, l);
                    }
                }
            }
        }
    }

    return r;
}

Mat_<uchar> build_depth_map(vector<Mat_<Vec3b>> images, int t) {
    vector<Mat_<float>> focus_measures;
    cout << "Buidling measures " << endl;
    for (auto im : images) {
        auto padded = pad_to_next_square(to_grayscale(im));
        auto pyramid = compute_laplacian_pyramid(padded, generating_kernel(0.3f));
        focus_measures.push_back(windowed_max(pyramid[0], t));
    }

    cout << "Creating the depth map" << endl;
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
    cout << "Denoising" << endl;
    cv::fastNlMeansDenoising(depth_map, depth_map);
    cout << "Done" << endl;
    return depth_map;
}

vector<Mat_<float>> build_masks(unsigned long n, Mat_<uchar> depth_map) {
    vector<Mat_<float>> masks;
    cout << n << endl;
    for (int i = 0; i < n; i++) {
        Mat_<uchar> d = depth_map == i;
        masks.emplace_back();
        d.convertTo(masks.back(), CV_32F, 1.f / 255);
    }


    return masks;
}

Mat_<Vec3b> custom_multifocus(vector<Mat_<Vec3b>> &images, int t) {

    cout << "Building depth map" << endl;
    auto depth_map = build_depth_map(images, t);
    auto masks = build_masks(images.size(), depth_map);
    Mat_<Vec3b> merged = merge_multiple_images(images, masks);



    return merged;

}