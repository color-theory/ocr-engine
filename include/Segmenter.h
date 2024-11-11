#ifndef SEGMENTER_H
#define SEGMENTER_H

#include <opencv4/opencv2/opencv.hpp>
#include <vector>

class Segmenter
{
public:
    std::vector<cv::Mat> segment(const cv::Mat &image);

private:
    std::vector<cv::Mat> getLineSegments(const cv::Mat &image, float threshold);
    std::vector<cv::Mat> getCharacterSegments(const cv::Mat &image);
    cv::Mat cropToContentAndScale(const cv::Mat &image, float threshold);
    int getMaxGapWidth(const cv::Mat &image);
    cv::Mat centerCharacter(const cv::Mat &characterSegment);
};

#endif // SEGMENTER_H