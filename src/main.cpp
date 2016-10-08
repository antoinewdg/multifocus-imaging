#include <iostream>
#include <vector>
#include "utils.h"
#include "splining.h"
#include "tiling.h"
#include "custom_multifocus.h"


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
    vector<Mat_<Vec3b>> results;

    for(int t = 1 ; t <=7 ; t++){
        auto elapsed = measure<>::execution([&results, &images, t](){
            results.push_back(custom_multifocus(images,t));
        });
        cout << "Built for t=" << t << " in "<< float(elapsed)/1000 << "s" << endl;
    }

    for(auto r : results){
        display_and_block(r);
    }
    for (int i = 2; i < 50; i += 6) {
        auto result = tiling_multifocus(images, i);
        cv::imwrite(string("out/tiling_") + std::to_string(i) + ".png", result);
    }


    return 0;
}