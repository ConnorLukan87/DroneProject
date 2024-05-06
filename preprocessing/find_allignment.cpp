//
// Created by connputer on 12/3/23.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>


std::string DATA_PATH = "/home/connputer/CS/DroneProject/";
std::string image_path = DATA_PATH + "images/good_images3/";

int main()
{
    cv::Mat noIR, optical;
    noIR = cv::imread(image_path + "unfiltered/NoIR_110.jpg");
    optical = cv::imread(image_path + "filtered/Optical_110.jpg");

    cv::Mat NoIR_edges, Optical_edges;
    cv::Canny(noIR, NoIR_edges, 10, 40);
    cv::Canny(optical, Optical_edges, 10, 40);

    cv::namedWindow("dot product", cv::WINDOW_NORMAL);
    cv::resizeWindow("dot product", cv::Size(1520, 2016));
    cv::namedWindow("optical", cv::WINDOW_NORMAL);
    cv::resizeWindow("optical", cv::Size(1520, 2016));
    cv::namedWindow("noIR", cv::WINDOW_NORMAL);
    cv::resizeWindow("noIR", cv::Size(1520, 2016));

    cv::destroyWindow("optical");
    cv::destroyWindow("noIR");
    // take some off the top, and from the right
    cv::cuda::GpuMat noIR_gpu(NoIR_edges);
    cv::cuda::GpuMat optical_gpu(Optical_edges);

    int max_shift_r = 0, max_shift_c = 0, max_dot = 0;

    bool done = false;
    for (int shift_c = 1; shift_c < 150; shift_c++)
    {
        std::cout << "Column shift: " << shift_c << std::endl;
        for (int shift_r = 1; shift_r < 150; shift_r++)
        {
            cv::cuda::GpuMat cropped_NoIR(noIR_gpu, cv::Rect(0, shift_r, 4032-shift_c, 3040-shift_r));
            cv::cuda::GpuMat cropped_Optical(optical_gpu, cv::Rect(shift_c, 0, 4032-shift_c, 3040-shift_r));

            cv::cuda::GpuMat almost_dot_gpu;

            cv::cuda::multiply(cropped_NoIR, cropped_Optical, almost_dot_gpu);

            cv::Mat cpu_almost_dot;
            almost_dot_gpu.download(cpu_almost_dot);
            int dot = cv::sum(cpu_almost_dot)[0];
            if (dot > max_dot)
            {
                max_shift_c = shift_c;
                max_shift_r = shift_r;
                max_dot = dot;
                std::cout << "Max dot: " << max_dot << std::endl;
                cv::imshow("dot product", cpu_almost_dot);
                int q = cv::waitKey(1000);
            }
        }
    }

    std::cout << "Column shift: " << max_shift_c << std::endl;
    std::cout << "Row shift: " << max_shift_r << std::endl;
    std::cout << "Dot product at the shift: " << max_dot << std::endl;
    cv::destroyAllWindows();

    return 0;
}
