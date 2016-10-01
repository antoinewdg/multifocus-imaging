#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "utils.h"
#include "splining.h"

using cv::Vec3b;

std::vector<cv::Mat_<Vec3b>> load_images(std::string name, unsigned long n, std::string ext) {
    std::vector<cv::Mat_<Vec3b>> images(n);
    for (int i = 0; i < n; i++) {
        std::string filename = name + "_" + std::to_string(i) + '.' + ext;
        images[i] = cv::imread(filename);
    }

    return images;
}

int main() {

    Mat_<Vec3b> im_a = load_color("sbug03.png");
    Mat_<Vec3b> im_b = load_color("sbug03b.png");
    Mat_<float> region = load_grayscale("mask.png");
    display_and_block(im_a);
    display_and_block(im_b);
    display_and_block(merge_images(im_a, im_b, region));

    return 0;
}