#include <cassert>
#include <fmt/core.h>
#include <image.hpp>
namespace image {

double LinearGradientInterp::inverse_linear_interp(cv::Point2d p1,
                                                   cv::Point2d p2, double y) {
    if (p1.y == p2.y)
        return (p1.x + p2.x) / 2.0;
    return (y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x) + p1.x;
}

cv::Point2d LinearGradientInterp::operator()(const cv::Mat &image,
                                             const cv::Rect &sub_rect) {
    assert(sub_rect.width == 3 && sub_rect.height == 3 &&
           "LinearGradientInterp only defined for 3x3 subrect of image");
    assert(image.channels() == 1 &&
           "LinearGradientInterp only defined for grey scale image");
    const auto &img = image(sub_rect);
    cv::Mat sumx, sumy;
    cv::reduce(img, sumx, 0, cv::REDUCE_SUM, cv::DataType<int>::type);
    cv::reduce(img, sumy, 1, cv::REDUCE_SUM, cv::DataType<int>::type);
    auto interp = [this](const cv::Mat &v) {
        double diff[2];
        double ret;
        for (int i = 0; i != 2; i++) {
            diff[i] = (v.at<int>(i + 1) - v.at<int>(i)) / 2.0;
        }
        return inverse_linear_interp(cv::Point2d(-1 / 2.0, diff[0]),
                                     cv::Point2d(1 / 2.0, diff[1]), 0);
    };
    return cv::Point2d(interp(sumx), interp(sumy)) +
           static_cast<cv::Point2d>(sub_rect.tl());
}

cv::Point2d CentroidInterp::operator()(const cv::Mat &image,
                                       const cv::Rect &sub_rect) {
    assert(image.channels() == 1 &&
           "CentroidInterp only defined for grey scale image");
    cv::Mat img = image(sub_rect);
    cv::Mat gridx, gridy;
    cv::Mat XX, YY;
    double x, y;
    for (int i = 0; i != sub_rect.width; i++)
        gridx.push_back(i);
    for (int i = 0; i != sub_rect.height; i++)
        gridy.push_back(i);
    meshgrid(gridx, gridy, XX, YY);
    XX.convertTo(XX, img.type());
    YY.convertTo(YY, img.type());
    x = double(cv::sum(XX.mul(img))[0]) / cv::sum(img)[0];
    y = double(cv::sum(YY.mul(img))[0]) / cv::sum(img)[0];
    return cv::Point2d(x, y) + static_cast<cv::Point2d>(sub_rect.tl());
}

Point2D<int> ImageProcessBasic::operator()(const cv::Mat &image_off,
                                           const cv::Mat &image_on) {
    cv::subtract(image_on, image_off, diff_image);

    if (do_gauss) {
        cv::GaussianBlur(diff_image, diff_image,
                         cv::Size(gauss_radius, gauss_radius), 0, 0,
                         cv::BORDER_DEFAULT);
    }

    Point2D<int> p;

    cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p1);
    cv::circle(diff_image, p.p1, 20, cv::Scalar(0, 0, 0), cv::FILLED);
    cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p2);

    return p;
}

Point2D<int> ImageProcessSubMat::get_location(const cv::Mat &image_off,
                                              const cv::Mat &image_on) {

    // auto sub_image_off = image_off(sub_rect);
    // auto sub_image_on = image_on(sub_rect);

    cv::subtract(image_on, image_off, diff_image);

    if (do_gauss)
        cv::GaussianBlur(diff_image, diff_image,
                         cv::Size(gauss_radius, gauss_radius), 0, 0,
                         cv::BORDER_DEFAULT);

    Point2D<int> p;

    cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p1);
    cv::circle(diff_image, p.p1, 20, cv::Scalar(0, 0, 0), cv::FILLED);
    cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p2);

    return p;
}

Point2D<int> ImageProcessSubMat::operator()(const cv::Mat &image_off,
                                            const cv::Mat &image_on) {
    auto p = get_location(image_off(sub_rect), image_on(sub_rect));

    p.p1.x += sub_rect.x;
    p.p1.y += sub_rect.y;
    p.p2.x += sub_rect.x;
    p.p2.y += sub_rect.y;

    // if (image_on.at<int>(p.p1) < threshold || image_on.at<int>(p.p2) <
    // threshold)
    // {
    //     // if the location is too dark, try the whole image
    //     p = get_location(image_off, image_on);
    // }
    // fmt::print("sub_rect: ({}, {}), ({}, {})\n", sub_rect.x, sub_rect.y,
    // sub_rect.width, sub_rect.height);

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

Point2D<double>
ImageProcessSubMatInterp::get_location(const cv::Mat &image_off,
                                       const cv::Mat &image_on) {

    // auto sub_image_off = image_off(sub_rect);
    // auto sub_image_on = image_on(sub_rect);

    cv::subtract(image_on, image_off, diff_image);

    if (do_gauss)
        cv::GaussianBlur(diff_image, diff_image,
                         cv::Size(gauss_radius, gauss_radius), 0, 0,
                         cv::BORDER_DEFAULT);

    Point2D<int> p;
    Point2D<double> p_ret;

    cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p1);
    p_ret.p1 = interp(diff_image, p.p1);
    cv::circle(diff_image, p.p1, 20, cv::Scalar(0, 0, 0), cv::FILLED);
    cv::minMaxLoc(diff_image, nullptr, nullptr, nullptr, &p.p2);
    p_ret.p2 = interp(diff_image, p.p2);

    return p_ret;
}

Point2D<double> ImageProcessSubMatInterp::operator()(const cv::Mat &image_off,
                                                     const cv::Mat &image_on) {
    auto p = get_location(image_off(sub_rect), image_on(sub_rect));

    p.p1.x += sub_rect.x;
    p.p1.y += sub_rect.y;
    p.p2.x += sub_rect.x;
    p.p2.y += sub_rect.y;

    // if (image_on.at<int>(p.p1) < threshold || image_on.at<int>(p.p2) <
    // threshold)
    // {
    //     // if the location is too dark, try the whole image
    //     p = get_location(image_off, image_on);
    // }
    // fmt::print("sub_rect: ({}, {}), ({}, {})\n", sub_rect.x, sub_rect.y,
    // sub_rect.width, sub_rect.height);

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
