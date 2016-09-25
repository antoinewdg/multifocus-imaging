#include "utils.h"
#include "splining.h"
#include "alignment.h"


vector<Mat_<Vec3b>> load_images(string name, unsigned long n, string ext) {
    vector<Mat_<Vec3b>> images(n);
    for (int i = 0; i < n; i++) {
        string filename = name + "_" + std::to_string(i) + '.' + ext;
        images[i] = cv::imread(filename);
    }

    return images;
}

int main() {

    vector<Mat_<Vec3b>> images = load_images("cuisine", 3, "jpg");
    align_a_on_b(images[0], images[1]);
    cv::imshow("0", images[0]);
    cv::waitKey();
    cv::imshow("0", images[1]);
    cv::waitKey();
    cv::imshow("0", images[2]);
    cv::waitKey();

//    Mat_<Vec3b> im_a = load_color("lena_color_tile_a.tiff");
//    Mat_<Vec3b> im_b = load_color("lena_color_tile_b.tiff");
//    Mat_<Vec3b> im_s = merge_images(im_a, im_b);
//
//    cv::imshow("a", im_s);
//    cv::waitKey();

    return 0;
}