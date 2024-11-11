#include "Preprocessor.h"

cv::Mat Preprocessor::correctRotation(const cv::Mat &image)
{
    cv::Mat gray;
    if (image.channels() > 1)
    {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        gray = image;
    }

    cv::Mat binary;
    cv::threshold(gray, binary, 128, 255, cv::THRESH_BINARY_INV);

    cv::Mat edges;
    cv::Canny(binary, edges, 50, 150);

    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180, 100);

    if (lines.empty())
    {
        return image;
    }

    double angleSum = 0;
    int count = 0;
    for (const auto &line : lines)
    {
        float theta = line[1];

        if (std::abs(theta - CV_PI / 2) > CV_PI / 4)
        {
            angleSum += theta;
            count++;
        }
    }

    if (count == 0)
    {
        return image;
    }

    double averageAngle = angleSum / count;
    double rotationAngle = (averageAngle - CV_PI / 2) * 180 / CV_PI;

    cv::Mat rotationMatrix = cv::getRotationMatrix2D(cv::Point2f(image.cols / 2, image.rows / 2), rotationAngle, 1);
    cv::Mat rotatedImage;

    cv::warpAffine(image, rotatedImage, rotationMatrix, image.size(), cv::INTER_CUBIC);

    return rotatedImage;
}

cv::Mat Preprocessor::preprocess(const cv::Mat &image)
{
    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, cv::Size(3, 3), 0);

    cv::Mat contrastEnhanced;
    cv::equalizeHist(image, contrastEnhanced);

    cv::Mat preproccessedImage = correctRotation(contrastEnhanced);
    return contrastEnhanced;
}
