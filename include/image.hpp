#pragma once

#include <opencv2/opencv.hpp>

namespace image
{

    struct Point2D {
        cv::Point p1;
        cv::Point p2;
    };

    struct ImageProcessBasic
    {
        bool do_gauss = false;
        int gauss_radius = 21;


        cv::Mat diff_image;

        Point2D operator()(const cv::Mat& image_off, const cv::Mat& image_on);
    };

    struct ImageProcessSubMat
    {
        bool do_gauss = false;
        int gauss_radius = 21;
        std::size_t margin = 20;
        int threshold = 10;

        cv::Mat diff_image;

        cv::Rect_<int> sub_rect{cv::Point_<int>{0, 0}, cv::Point_<int>{1440, 1080}};

        Point2D operator()(const cv::Mat& image_off, const cv::Mat& image_on);

        Point2D get_location(const cv::Mat& image_off, const cv::Mat& image_on);
    };

} // namespace image
