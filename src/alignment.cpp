//
// Created by antoinewdg on 23/09/16.
//

#include "alignment.h"

//void align_images(vector<Mat_<Vec3b>> &images) {
//    cv::Ptr<SURF> detector = SURF::create(400);
//    vector<vector<cv::KeyPoint>> keypoints_vecs(images.size());
//    vector<cv::Mat> descriptors(images.size());
//    for (int i = 0; i < images.size(); i++) {
//        detector->detectAndCompute(images[i], cv::Mat(), keypoints_vecs[i], descriptors[i]);
//    }
//
//
//    vector<vector<cv::DMatch>> matches_vecs(images.size() - 1);
//    cv::FlannBasedMatcher matcher;
//    for (int i = 0; i < images.size() - 1; i++) {
//        matcher.match(descriptors[i], descriptors.back(), matches_vecs[i]);
//    }
//    vector
//    double max_dist = 0;
//    double min_dist = std::numeric_limits<double>::max();
//    //-- Quick calculation of max and min distances between keypoints
////    for(int k = 0 ; k < )
//    for (int i = 0; i < images.size() - 1; i++) {
//        for (int j = 0; j < descriptors[i].rows; j++) {
//            double dist = matches_vecs[i][j].distance;
//            if (dist < min_dist) min_dist = dist;
//            if (dist > max_dist) max_dist = dist;
//        }
//    }
//
//
//    vector<vector< cv::DMatch >> good_matches_vec;
//    for( int j = 0; j < descriptors_object.rows; j++ )
//    { if( matches[j].distance < 3*min_dist )
//        { good_matches.push_back( matches[j]); }
//    }
//
//}

void align_a_on_b(Mat_<Vec3b> &im_a,const  Mat_<Vec3b>& im_b) {
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
    //-- Get the corners from the image_1 ( the object to be "detected" )
//    std::vector<Point2f> corners_a(4);
//    corners_a[0] = cvPoint(0, 0);
//    corners_a[1] = cvPoint(im_a.cols, 0);
//    corners_a[2] = cvPoint(im_a.cols, im_a.rows);
//    corners_a[3] = cvPoint(0, im_a.rows);
//    vector<Point2f> corners_b(4);
//    perspectiveTransform(corners_a, corners_b, H);

//    cout << H << endl;
//    cv::imshow("a", im_b);
//    cv::waitKey();
    cv::warpPerspective(im_a, im_a, H, cv::Size(1000, 1000));
//    cv::imshow("a", im_b);
//    cv::waitKey();

}