//
// Created by antoinewdg on 24/09/16.
//

#include "splining.h"

typedef vector<Mat_<float>> Pyramid;

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

Mat_<float> expand(Mat_<float> &src, const Mat_<float> &w) {

    int n = src.rows * 2 - 1;
    Mat_<float> expanded(n, n, 0.0f);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            expanded(2 * i, 2 * j) = 4 * src(i, j);
        }
    }
    return convolution_with_reflection(expanded, w);

}

Mat_<float> expand_n_times(Mat_<float> &src, const Mat_<float> &w, int n) {
    Mat_<float> r = src;
    for (int i = 0; i < n; i++) {
        r = expand(r, w);
    }
    return r;
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

Mat_<float> generate_half_mask(int rows, int cols) {
    Mat_<float> mask(rows, cols, 0.0f);
    for (int i = 0; i < rows; i++) {
        mask(i, cols / 2) = 0.5f;
        for (int j = 0; j < cols / 2; j++) {
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
        region = generate_half_mask(im_a.rows, im_a.cols);
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


cv::Rect find_ideal_subrect(Mat_<float> &region) {
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

Mat_<float> pad_to_next_square(Mat_<float> original) {
    int n = 1;
    int m = std::max(original.rows, original.cols);
    while (n + 1 < m) {
        n *= 2;
    }
    n += 1;
    Mat_<float> padded(n, n, 0.f);
    original.copyTo(padded(cv::Rect(0, 0, original.cols, original.rows)));
    return padded;
}

Mat_<float> merge_images(Mat_<float> &im_a, Mat_<float> &im_b, Mat_<float> region, int max_height) {

    if (region.empty()) {
        region = generate_half_mask(im_a.rows, im_a.cols);
    }
    auto padded_a = pad_to_next_square(im_a);
    auto padded_b = pad_to_next_square(im_b);
    auto padded_region = pad_to_next_square(region);
    auto padded_merge = merge_square_images(padded_a, padded_b, padded_region, max_height);

    Mat_<float> merged(im_a.rows, im_a.cols);
    padded_merge(cv::Rect(0, 0, im_a.cols, im_a.rows)).copyTo(merged);

    return merged;
}

Mat_<float> merge_multiple_images(vector<Mat_<float>> &images,
                                  vector<Mat_<float>> &regions) {
    vector<Mat_<float>> padded_images, padded_regions;
    vector<Pyramid> image_pyramids, region_pyramids;
    Mat_<float> w = generating_kernel(0.3f);
//
//    for (auto r : regions) {
//        display_and_block(r);
//    }
//    cout << regions.size() << " " << images.size() << endl;


    for (int i = 0; i < images.size(); i++) {
        padded_images.push_back(pad_to_next_square(images[i]));
        padded_regions.push_back(pad_to_next_square(regions[i]));
//        display_and_block(regions[i]);
//        display_and_block(padded_regions[i]);
        image_pyramids.push_back(compute_laplacian_pyramid(padded_images[i], w));
        region_pyramids.push_back(compute_gaussian_pyramid(padded_regions[i], w));
    }

    Pyramid merge_pyramid;
    for (int l = 0; l < region_pyramids[0].size(); l++) {
        merge_pyramid.emplace_back(region_pyramids[0][l].rows,
                                   region_pyramids[0][l].cols, 0.f);
        for (int i = 0; i < region_pyramids.size(); i++) {
            merge_pyramid[l] += region_pyramids[i][l].mul(image_pyramids[i][l]);
        }
    }

//    for(auto p : merge_pyramid){
//        display_and_block(p);
//    }


    Mat_<float> padded_merge = merge_pyramid.back().clone(),
            merge(images[0].rows, images[1].cols);

    for (int l = merge_pyramid.size() - 2; l >= 0; l--) {
        padded_merge = merge_pyramid[l] + expand(padded_merge, w);
    }

    return padded_merge(cv::Rect(0, 0, images[0].cols, images[0].rows)).clone();


}


Mat_<Vec3b> merge_multiple_images(vector<Mat_<Vec3b>> &images,
                                  vector<Mat_<float>> &regions) {
    vector<vector<Mat_<float>>> separated(3);
    for (auto im : images) {
        auto channels = separate_channels(im);
        separated[0].push_back(channels[0]);
        separated[1].push_back(channels[1]);
        separated[2].push_back(channels[2]);
    }
    vector<Mat_<float>> separated_merge;
    separated_merge.push_back(merge_multiple_images(separated[0], regions));
    separated_merge.push_back(merge_multiple_images(separated[1], regions));
    separated_merge.push_back(merge_multiple_images(separated[2], regions));
//    display_and_block(separated_merge.back());

    return merge_channels(separated_merge);

}