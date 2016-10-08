//
// Created by antoinewdg on 29/09/16.
//

#include "tiling.h"

vector<vector<cv::Rect>> get_tiles(cv::Mat &src, int tile_size) {

    int step = tile_size / 2;
    vector<vector<cv::Rect>> tiles;
    for (int i = 0; i * step + tile_size <= src.rows; i++) {
        tiles.emplace_back();
        for (int j = 0; j * step + tile_size <= src.cols; j++) {
            tiles[i].emplace_back(j * step, i * step, tile_size, tile_size);
        }

    }
    return tiles;

}

float average_over_rect(Mat_<float> m, cv::Rect r) {
    float value = 0.f;
    for (int y = r.y; y < r.height + r.y; y++) {
        for (int x = r.x; x < r.width + r.x; x++) {
            value += m(y, x);
        }
    }

    return value / (r.height * r.width);
}

Mat_<float> tiled_laplacian(cv::Mat_<float> &laplacian, vector<vector<cv::Rect>> &tiles) {
    Mat_<float> result(tiles.size(), tiles[0].size(), 0.f);
    for (int i = 0; i < tiles.size(); i++) {
        for (int j = 0; j < tiles[i].size(); j++) {
            result(i, j) = average_over_rect(laplacian, tiles[i][j]);
        }
    }

    return result;
}

Mat_<uchar> compute_depth_map(vector<Mat_<float>> tiled_laplacians) {
    Mat_<uchar> depth_map(tiled_laplacians[0].rows, tiled_laplacians[0].cols);
    for (int i = 0; i < depth_map.rows; i++) {
        for (int j = 0; j < depth_map.cols; j++) {
            float max_value = -1.f;
            for (int k = 0; k < tiled_laplacians.size(); k++) {
                if (tiled_laplacians[k](i, j) > max_value) {
                    depth_map(i, j) = uchar(k);
                    max_value = tiled_laplacians[k](i, j);
                }
            }
        }
    }

    return depth_map;
}


Mat_<Vec3b> build_from_tiles(vector<vector<cv::Rect>> &tiles,
                             vector<Mat_<Vec3b>> &images,
                             Mat_<uchar> depth_map, int i0, int j0) {
    Mat_<Vec3b> result(images[0].rows, images[1].cols);
    for (int i = i0; i < tiles.size(); i += 2) {
        for (int j = j0; j < tiles[i].size(); j += 2) {
            auto t = tiles[i][j];
            int k = depth_map(i, j);
            images[k](t).copyTo(result(t));
        }
    }

    return result;
}

Mat_<float> generate_horizontal_mask(int rows, int cols, int tsize) {
    Mat_<float> mask(rows, cols, 1.0);
    int step = tsize / 2;
    for (int i = 0; i < rows; i++) {
        for (int j = step; j < cols - step; j++) {
            int x = (j - step) % tsize;
            mask(i, j) = float(std::abs(x - step)) / step;
        }
    }
    return mask;
}


Mat_<float> generate_vertical_mask(int rows, int cols, int tsize) {
    Mat_<float> mask(rows, cols, 1.0);
    int step = tsize / 2;
    for (int i = step; i < rows - step; i++) {
        for (int j = 0; j < cols; j++) {
            int x = (i - step) % tsize;
            mask(i, j) = float(std::abs(x - step)) / step;
        }
    }
    return mask;
}

Mat_<Vec3b> add_padding(Mat_<Vec3b> &src, int tsize) {
    Mat_<Vec3b> padded(src.rows + 2 * tsize, src.cols + 2 * tsize, Vec3b(0, 0, 0));
    src.copyTo(padded(cv::Rect(tsize, tsize, src.cols, src.rows)));
    return padded;
}

Mat_<Vec3b> remove_padding(Mat_<Vec3b> &src, int tsize) {
    Mat_<Vec3b> result(src.rows - 2 * tsize, src.cols - 2 * tsize);
    src(cv::Rect(tsize, tsize, result.cols, result.rows)).copyTo(result);
    return result;
}

Mat_<Vec3b> tiling_multifocus(vector<Mat_<Vec3b>> &raw_images, int tsize) {
    vector<Mat_<Vec3b>> images(raw_images.size());
    for (int i = 0; i < images.size(); i++) {
        images[i] = add_padding(raw_images[i], tsize);
    }

    vector<Mat_<float>> grayscales, laplacians, tiled_laplacians;
    vector<vector<cv::Rect>> tiles = get_tiles(images[0], tsize);
    for (auto im : images) {
        grayscales.push_back(to_grayscale(im));
        laplacians.push_back(laplacian(grayscales.back()));
        tiled_laplacians.push_back(tiled_laplacian(laplacians.back(), tiles));
    }

    Mat_<uchar> depth_map = compute_depth_map(tiled_laplacians);

    auto over_00 = build_from_tiles(tiles, images, depth_map, 0, 0);
    auto over_01 = build_from_tiles(tiles, images, depth_map, 0, 1);
    auto over_10 = build_from_tiles(tiles, images, depth_map, 1, 0);
    auto over_11 = build_from_tiles(tiles, images, depth_map, 1, 1);
    auto mask = generate_horizontal_mask(over_00.rows, over_00.cols, tsize);

    auto rows_0 = merge_images(over_00, over_01, mask);
    auto rows_1 = merge_images(over_10, over_11, mask);

    mask = generate_vertical_mask(over_00.rows, over_00.cols, tsize);
    auto result = merge_images(rows_0, rows_1, mask);

    return remove_padding(result, tsize);
}