#ifndef OCRIMAGEPROCESSOR_H
#define OCRIMAGEPROCESSOR_H

#include "Preprocessor.h"
#include "Segmenter.h"
#include <opencv4/opencv2/opencv.hpp>
#include <vector>

class OCRImageProcessor
{
public:
    OCRImageProcessor(const std::string &filePath);
    std::vector<std::vector<float>> extractCharacters();
    std::string serializeToJson(const std::vector<std::vector<float>> &data);

private:
    cv::Mat image;
    Preprocessor preprocessor;
    Segmenter segmenter;
};

#endif // OCRIMAGEPROCESSOR_H