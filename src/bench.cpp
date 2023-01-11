#include <benchmark/benchmark.h>

#include <image.hpp>

struct Images {
    cv::Mat image_off;
    cv::Mat image_on;
};

Images load() {
    auto file_off = "near_off.tiff";
    auto file_on = "near_on.tiff";

    cv::Mat image_off = cv::imread(file_off, cv::IMREAD_GRAYSCALE);
    cv::Mat image_on = cv::imread(file_on, cv::IMREAD_GRAYSCALE);

    return {image_off, image_on};
}

template <typename ImageProcess>
static void BM_ImageProcess(benchmark::State &state) {

    Images images = load();

    ImageProcess ipb;
    ipb.do_gauss = static_cast<bool>(state.range(0));

    // Warmup
    ipb(images.image_off, images.image_on);

    for (auto _ : state) {
        auto res = ipb(images.image_off, images.image_on);
        benchmark::DoNotOptimize(res);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_ImageProcess<image::ImageProcessBasic>)->Arg(0)->Arg(1);
BENCHMARK(BM_ImageProcess<image::ImageProcessSubMat>)->Arg(0)->Arg(1);
BENCHMARK(BM_ImageProcess<image::ImageProcessSubMatInterp>)->Arg(0)->Arg(1);