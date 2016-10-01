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
#include "tiling.h"


vector<Mat_<Vec3b>> load_images(string name, unsigned long n, string ext) {
    vector<Mat_<Vec3b>> images(n);
    for (int i = 0; i < n; i++) {
        std::string filename = name + std::to_string(i) + '.' + ext;
        images[i] = load_color(filename);
    }

    return images;
}

int main() {
    auto images = load_images("sbug0", 13, "png");
    for (int i = 20; i < 50; i += 6) {
        auto result = build_multifocus_images(images, i);
        display_and_block(result);
    }


    return 0;
}