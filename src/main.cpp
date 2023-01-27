#include <image.hpp>

#include <fmt/core.h>
#include <opencv2/opencv.hpp>

float get_mean(const cv::Mat &img) {
    cv::Scalar mean = cv::mean(img);
    return mean[0];
}

int main(int argc, char **argv) {

    auto file_off = "near_off.tiff";
    auto file_on = "near_on.tiff";

    cv::Mat image_off = cv::imread(file_off, cv::IMREAD_GRAYSCALE);
    cv::Mat image_on = cv::imread(file_on, cv::IMREAD_GRAYSCALE);

    // auto mean_on = get_mean(image_on);
    // auto mean_off = get_mean(image_off);

    // fmt::print("mean_on: {}\n", mean_on);
    // fmt::print("mean_off: {}\n", mean_off);

    auto ipb = image::ImageProcessSubMatInterp(image::CentroidInterp());
    ipb.do_gauss = true;

    // Warmup
    ipb(image_off, image_on);
    ipb(image_off, image_on);
    ipb(image_off, image_on);
    ipb(image_off, image_on);
    ipb(image_off, image_on);

    auto p = ipb(image_off, image_on);

    // print locations
    fmt::print("loc_1: ({}, {})\n", p.p1.x, p.p1.y);
    fmt::print("loc_2: ({}, {})\n", p.p2.x, p.p2.y);

    // draw circles around locations
    cv::circle(image_on, p.p1, 20, cv::Scalar(255), 2);
    cv::circle(image_on, p.p2, 20, cv::Scalar(255), 2);

    cv::namedWindow("Display Image diff", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image diff", image_on);
    cv::waitKey(0);

    return 0;
}