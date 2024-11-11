#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <opencv4/opencv2/opencv.hpp>

class Preprocessor
{
public:
    cv::Mat preprocess(const cv::Mat &image);

private:
    cv::Mat correctRotation(const cv::Mat &image);
};

#endif // PREPROCESSOR_H