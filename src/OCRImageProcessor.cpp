#include "OCRImageProcessor.h"
#include <vector>
#include <string>
#include <chrono>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

std::string OCRImageProcessor::serializeToJson(const std::vector<std::vector<float>> &data)
{
    json jsonObject;
    jsonObject["images"] = data;
    return jsonObject.dump();
}

OCRImageProcessor::OCRImageProcessor(const std::string &filePath)
{
    image = cv::imread(filePath, cv::IMREAD_GRAYSCALE);
    if (image.empty())
    {
        throw std::runtime_error("Failed to load image from file path: " + filePath);
    }
}

std::vector<std::vector<float>> OCRImageProcessor::extractCharacters()
{
    // cv::Mat preprocessedImage = preprocessor.preprocess(image);
    std::vector<cv::Mat> characterMats = segmenter.segment(image);
    std::vector<std::vector<float>> characters;
    for (const cv::Mat &mat : characterMats)
    {
        cv::Mat convertedMat;
        mat.convertTo(convertedMat, CV_32F, 1.0 / 255);

        std::vector<float> flatArray(convertedMat.isContinuous() ? convertedMat.begin<float>() : convertedMat.clone().begin<float>(),
                                     convertedMat.isContinuous() ? convertedMat.end<float>() : convertedMat.clone().end<float>());

        characters.push_back(std::move(flatArray));
    }
    return characters;
}