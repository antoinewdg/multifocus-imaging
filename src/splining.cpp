//
// Created by antoinewdg on 24/09/16.
//

#include "splining.h"

Mat_<float> generating_kernel(float a) {
    float b = 0.25f, c = 0.25f - 0.5f * a;
    Mat_<float> kernel(5, 5);
    vector<float> w = {c, b, a, b, c};
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            kernel(i, j) = w[i] * w[j];
        }
    }
    return kernel;
}

Mat_<float> expand(Mat_<float> &src, Mat_<float> &w) {

    int n = src.rows * 2 - 1;
    Mat_<float> expanded(n, n, 0.0f);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            expanded(2 * i, 2 * j) = 4 * src(i, j);
        }
    }
    return convolution_with_reflection(expanded, w);

}

Mat_<float> reduce(Mat_<float> &src, Mat_<float> &w) {
    int n = src.rows;
    Mat_<float> reduced(n / 2 + 1, n / 2 + 1);
    Mat_<float> blurred = convolution_with_reflection(src, w);
    for (int i = 0; i < reduced.rows; i++) {
        for (int j = 0; j < reduced.cols; j++) {
            reduced(i, j) = blurred(2 * i, 2 * j);
        }
    }

    return reduced;
}

Mat_<float> generate_half_mask(int n) {
    Mat_<float> mask(n, n, 0.0f);
    for (int i = 0; i < n; i++) {
        mask(i, n / 2) = 0.5f;
        for (int j = 0; j < n / 2; j++) {
            mask(i, j) = 1.0f;
        }
    }
    return mask;
}

vector<Mat_<float>> compute_gaussian_pyramid(Mat_<float> src, Mat_<float> w) {
    vector<Mat_<float>> G = {src};

    for (int n = src.rows; n > 3;) {
        n = n / 2 + 1;
        G.emplace_back(n, n);
    }

    for (int l = 1; l < G.size(); l++) {
        G[l] = reduce(G[l - 1], w);
    }

    return G;
}


vector<Mat_<float>> compute_laplacian_pyramid(Mat_<float> src, Mat_<float> w) {
    vector<Mat_<float>> G = compute_gaussian_pyramid(src, w);


    vector<Mat_<float>> L(G.size());
    L.back() = G.back().clone();

    for (int l = G.size() - 2; l >= 0; l--) {
        L[l] = G[l] - expand(G[l + 1], w);
    }


    return L;
}

Mat_<Vec3b> merge_images(Mat_<Vec3b> &im_a, Mat_<Vec3b> &im_b, Mat_<float> region) {
    vector<Mat_<float>> vec_a = separate_channels(im_a);
    vector<Mat_<float>> vec_b = separate_channels(im_b);
    vector<Mat_<float>> vec_s;
    for (int i = 0; i < 3; i++) {
        vec_s.push_back(merge_images(vec_a[i], vec_b[i], region));
    }
    return merge_channels(vec_s);
}

Mat_<float> merge_images(Mat_<float> &im_a, Mat_<float> &im_b, Mat_<float> region) {

    Mat_<float> result;
    Mat_<float> w = generating_kernel(0.3f);
    if (region.empty()) {
        region = generate_half_mask(im_a.cols);
    }

    vector<Mat_<float>> LA = compute_laplacian_pyramid(im_a, w);
    vector<Mat_<float>> LB = compute_laplacian_pyramid(im_b, w);
    vector<Mat_<float>> LS;
    vector<Mat_<float>> masks = compute_gaussian_pyramid(region, w);


    for (int l = 0; l < LA.size(); l++) {
        LS.push_back(LA[l].mul(masks[l]) + LB[l].mul(1.0f - masks[l]));
    }

    result = LS.back().clone();
    for (int l = LS.size() - 2; l >= 0; l--) {
        result = LS[l] + expand(result, w);
    }

    return result;
}