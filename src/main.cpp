#include <iostream>
#include <vector>
#include "utils.h"
#include "splining.h"
#include "tiling.h"
#include "alignment.h"
#include "custom_multifocus.h"


vector<Mat_<Vec3b>> load_images(string name, unsigned long n, string ext) {
    vector<Mat_<Vec3b>> images(n);
    for (int i = 0; i < n; i++) {
        std::string filename = name + std::to_string(i) + '.' + ext;
        images[i] = load_color(filename);
    }

    return images;
}


int main(int argc, char *argv[]) {

    if (argc < 5) {
        cout << "Usage: multifocus path_prefix n_images extension realign" << endl;
        cout << "Example: multifocus small_stuff_anim0 6 jpg" << endl;

        return 0;
    }
    auto images = load_images(argv[1], std::stoi(string(argv[2])), argv[3]);
    bool realign = argv[4] == "1";

    if (realign) {
        align_on_middle_image(images);
    }
    vector<Mat_<Vec3b>> results;

    for (int t = 1; t <= 7; t++) {
        auto elapsed = measure<>::execution([&results, &images, t]() {
            results.push_back(custom_multifocus(images, t));
        });
        cout << "Built for t=" << t << " in " << float(elapsed) / 1000 << "s" << endl;
//        cv::imwrite(string(argv[1]) + "_out_"+ std::to_string(t) + ".png", results.back());
    }

    for (auto r : results) {
        display_and_block(r);
    }

    return 0;
}