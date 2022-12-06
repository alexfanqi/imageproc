#include <image.hpp>
#include <fmt/core.h>
namespace image
{

    Point2D ImageProcessBasic::operator()(const cv::Mat &image_off, const cv::Mat &image_on)
    {
        cv::subtract(image_on, image_off, diff_image);

        if (do_gauss)
        {
            cv::GaussianBlur(diff_image, diff_image, cv::Size(gauss_radius, gauss_radius), 0, 0, cv::BORDER_DEFAULT);
        }

        Point2D p;

        cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p1);
        cv::circle(diff_image, p.p1, 20, cv::Scalar(0, 0, 0), cv::FILLED);
        cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p2);

        return p;
    }

    Point2D ImageProcessSubMat::get_location(const cv::Mat &image_off, const cv::Mat &image_on)
    {

        // auto sub_image_off = image_off(sub_rect);
        // auto sub_image_on = image_on(sub_rect);

        cv::subtract(image_on, image_off, diff_image);

        if (do_gauss)
            cv::GaussianBlur(diff_image, diff_image, cv::Size(gauss_radius, gauss_radius), 0, 0, cv::BORDER_DEFAULT);

        Point2D p;

        cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p1);
        cv::circle(diff_image, p.p1, 20, cv::Scalar(0, 0, 0), cv::FILLED);
        cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p2);

        return p;
    }

    Point2D ImageProcessSubMat::operator()(const cv::Mat &image_off, const cv::Mat &image_on)
    {
        auto p = get_location(image_off(sub_rect), image_on(sub_rect));

        p.p1.x += sub_rect.x;
        p.p1.y += sub_rect.y;
        p.p2.x += sub_rect.x;
        p.p2.y += sub_rect.y;

        // if (image_on.at<int>(p.p1) < threshold || image_on.at<int>(p.p2) < threshold)
        // {
        //     // if the location is too dark, try the whole image
        //     p = get_location(image_off, image_on);
        // }
        // fmt::print("sub_rect: ({}, {}), ({}, {})\n", sub_rect.x, sub_rect.y, sub_rect.width, sub_rect.height);

        auto x_min = std::min(p.p1.x, p.p2.x);
        auto x_max = std::max(p.p1.x, p.p2.x);
        auto y_min = std::min(p.p1.y, p.p2.y);
        auto y_max = std::max(p.p1.y, p.p2.y);

        x_min = x_min < 20 ? 0 : x_min - margin;
        x_max = x_max > image_on.cols - 20 ? image_on.cols : x_max + margin;
        y_min = y_min < 20 ? 0 : y_min - margin;
        y_max = y_max > image_on.rows - 20 ? image_on.rows : y_max + margin;

        sub_rect = cv::Rect(x_min, y_min, x_max - x_min, y_max - y_min);

        return p;
    }

} // namespace image
