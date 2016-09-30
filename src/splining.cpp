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

vector<Mat_<float>> compute_gaussian_pyramid(Mat_<float> src, Mat_<float> w, int max_height) {
    vector<Mat_<float>> G = {src};
    if (max_height < 0) {
        max_height = std::numeric_limits<int>::max();
    }
    int h = 0;
    for (int n = src.rows; n > 3 && h < max_height; h++) {
        n = n / 2 + 1;
        G.emplace_back(n, n);
    }

    for (int l = 1; l < G.size(); l++) {
        G[l] = reduce(G[l - 1], w);
    }

    return G;
}


vector<Mat_<float>> compute_laplacian_pyramid(Mat_<float> src, Mat_<float> w, int max_height) {
    vector<Mat_<float>> G = compute_gaussian_pyramid(src, w, max_height);


    vector<Mat_<float>> L(G.size());
    L.back() = G.back().clone();

    for (int l = G.size() - 2; l >= 0; l--) {
        L[l] = G[l] - expand(G[l + 1], w);
    }


    return L;
}

Mat_<Vec3b> merge_images(Mat_<Vec3b> &im_a, Mat_<Vec3b> &im_b, Mat_<float> region, int max_height) {
    vector<Mat_<float>> vec_a = separate_channels(im_a);
    vector<Mat_<float>> vec_b = separate_channels(im_b);
    vector<Mat_<float>> vec_s;
    for (int i = 0; i < 3; i++) {
        vec_s.push_back(merge_images(vec_a[i], vec_b[i], region, max_height));
    }
    return merge_channels(vec_s);
}


Mat_<float> merge_square_images(Mat_<float> &im_a, Mat_<float> &im_b, Mat_<float> region, int max_height) {

    Mat_<float> result;
    Mat_<float> w = generating_kernel(0.3f);
    if (region.empty()) {
        region = generate_half_mask(im_a.cols);
    }

    vector<Mat_<float>> LA = compute_laplacian_pyramid(im_a, w, max_height);
    vector<Mat_<float>> LB = compute_laplacian_pyramid(im_b, w, max_height);
    vector<Mat_<float>> LS;
    vector<Mat_<float>> masks = compute_gaussian_pyramid(region, w, max_height);


    for (int l = 0; l < LA.size(); l++) {
        LS.push_back(LA[l].mul(masks[l]) + LB[l].mul(1.0f - masks[l]));
    }

    result = LS.back().clone();
    for (int l = LS.size() - 2; l >= 0; l--) {
        result = LS[l] + expand(result, w);
    }

    return result;
}

Mat_<float> subrect_copy(Mat_<float> &m, cv::Rect &r) {
    return m.colRange(r.x, r.x + r.width).rowRange(r.y, r.y + r.height).clone();
}

cv::Rect find_ideal_subrect(Mat_<float> &region){
    int j, left_j, right_j;
    for (j = 0; j < region.cols; j++) {
        for (int i = 0; i < region.rows; i++) {
            if (region(i, j) != 0.f) {
                break;
            }
        }
    }
    left_j = j;
    for (j = region.cols - 1; j >= 0; j--) {
        for (int i = 0; i < region.rows; i++) {
            if (region(i, j) != 1.f) {
                break;
            }
        }
    }
    right_j = j + 1;
    int n = right_j - left_j;
    return cv::Rect(left_j - (region.rows - n) / 2, 0, region.rows, region.rows);
}

Mat_<float> merge_images(Mat_<float> &im_a, Mat_<float> &im_b, Mat_<float> region, int max_height) {

    if (region.empty()) {
        region = generate_half_mask(im_a.cols);
    }
    if (im_a.cols == im_a.rows) {
        return merge_square_images(im_a, im_b, region, max_height);
    }

    // Handle the case where the image has more cols than rows.
    // In this case, the actual region has to fit in a square
    // of size rows.
    Mat_<float> result(im_a.rows, im_a.cols);
    auto r = find_ideal_subrect(region);
    Mat_<float> jm_a = subrect_copy(im_a, r);
    Mat_<float> jm_b = subrect_copy(im_b, r);
    Mat_<float> region_b = subrect_copy(region, r);

    Mat_<float> subresult = merge_square_images(jm_a, jm_b, region_b, max_height);

    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            if (j < r.x) {
                result(i, j) = im_a(i, j);
            } else if (j < r.x + r.width) {
                result(i, j) = subresult(i, j - r.x);
            } else {
                result(i, j) = im_b(i, j);
            }
        }
    }

    return result;

}
