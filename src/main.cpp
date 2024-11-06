#include <opencv4/opencv2/opencv.hpp>
#include <iostream>

int main()
{
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    cv::Mat image = cv::Mat::zeros(500, 500, CV_8UC1);
    cv::circle(image, cv::Point(250, 250), 100, cv::Scalar(255), -1);

    if (cv::imwrite("test_image.png", image))
    {
        std::cout << "Image saved successfully as 'test_image.png'" << std::endl;
    }
    else
    {
        std::cerr << "Failed to save the image." << std::endl;
        return 1;
    }

    return 0;
}