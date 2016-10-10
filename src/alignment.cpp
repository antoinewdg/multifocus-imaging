//
// Created by antoinewdg on 23/09/16.
//

#include "alignment.h"


using cv::xfeatures2d::SURF;


void align_a_on_b(Mat_<Vec3b> &im_a, const Mat_<Vec3b> &im_b) {
    //-- Step 1: Detect the keypoints and extract descriptors using SURF
    int minHessian = 400;
    cv::Ptr<SURF> detector = SURF::create(minHessian);
    std::vector<cv::KeyPoint> keypoints_a, keypoints_b;
    cv::Mat descriptors_a, descriptors_b;
    detector->detectAndCompute(im_a, cv::Mat(), keypoints_a, descriptors_a);
    detector->detectAndCompute(im_b, cv::Mat(), keypoints_b, descriptors_b);

    //-- Step 2: Matching descriptor vectors using FLANN matcher
    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors_a, descriptors_b, matches);
    double max_dist = 0;
    double min_dist = 100;
    //-- Quick calculation of max and min distances between keypoints
    for (int i = 0; i < descriptors_a.rows; i++) {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector<cv::DMatch> good_matches;
    for (int i = 0; i < descriptors_a.rows; i++) {
        if (matches[i].distance < 3 * min_dist) { good_matches.push_back(matches[i]); }
    }


    //-- Localize the object
    std::vector<Point2f> points_a;
    std::vector<Point2f> points_b;
    for (size_t i = 0; i < good_matches.size(); i++) {
        //-- Get the keypoints from the good matches
        points_a.push_back(keypoints_a[good_matches[i].queryIdx].pt);
        points_b.push_back(keypoints_b[good_matches[i].trainIdx].pt);
    }
    cv::Mat H = cv::findHomography(points_a, points_b, cv::RANSAC);
    cv::warpPerspective(im_a, im_a, H, im_b.size());

}

void align_on_middle_image(vector<Mat_<Vec3b>> &images) {
    for (int i = 0; i < images.size(); i++) {
        if (i == images.size() / 2) {
            continue;
        }
        align_a_on_b(images[i], images[images.size() / 2]);
    }
}