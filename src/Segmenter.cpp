#include "Segmenter.h"
#include <iostream>

cv::Mat Segmenter::cropToContentAndScale(const cv::Mat &image, float threshold)
{
    cv::Mat columnProjection;
    cv::reduce(image, columnProjection, 0, cv::REDUCE_SUM, CV_32S);

    int startCol = 0;
    for (int i = 0; i < columnProjection.cols; ++i)
    {
        if (columnProjection.at<int>(0, i) < 255 * image.rows)
        {
            startCol = i;
            break;
        }
    }
    int endCol = columnProjection.cols - 1;
    for (int i = columnProjection.cols - 1; i >= 0; --i)
    {
        if (columnProjection.at<int>(0, i) < 255 * image.rows)
        {
            endCol = i;
            break;
        }
    }

    if (endCol > startCol)
    {
        cv::Mat croppedImage = image(cv::Range::all(), cv::Range(startCol, endCol + 1)).clone();
        int targetHeight = 50;
        int targetWidth = static_cast<int>(croppedImage.cols * (static_cast<double>(targetHeight) / croppedImage.rows));

        cv::Mat scaledImage;
        cv::resize(croppedImage, scaledImage, cv::Size((targetWidth > 0) ? targetWidth : 1, targetHeight));
        return scaledImage;
    }
    else
    {
        return cv::Mat();
    }
}

std::vector<cv::Mat> Segmenter::segment(const cv::Mat &image)
{
    cv::Mat binaryImage;
    float threshold = cv::threshold(image, binaryImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    std::vector<cv::Mat> lineSegments = getLineSegments(binaryImage, threshold);
    std::vector<cv::Mat> characterMats;
    for (const cv::Mat &lineSegment : lineSegments)
    {
        cv::Mat croppedLine = cropToContentAndScale(lineSegment, threshold);
        std::vector<cv::Mat> characterSegments = getCharacterSegments(croppedLine);
        characterMats.insert(characterMats.end(), characterSegments.begin(), characterSegments.end());
    }

    return characterMats;
}

std::vector<cv::Mat> Segmenter::getLineSegments(const cv::Mat &image, float threshold)
{

    if (image.channels() > 1)
    {
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    }
    int bgValue = static_cast<int>(image.at<uchar>(0, 0));

    if (bgValue < threshold)
    {
        bgValue = 255 - bgValue;
        cv::bitwise_not(image, image);
    }
    cv::Mat rowProjection;
    cv::reduce(image, rowProjection, 1, cv::REDUCE_SUM, CV_32S);

    std::vector<cv::Mat> lines;
    int lineStart = 0;
    int rowThreshold = bgValue * image.cols;
    bool wasLine = (rowProjection.at<int>(0) < rowThreshold) ? true : false;
    for (int i = 0; i < rowProjection.rows; ++i)
    {
        bool isLine = (rowProjection.at<int>(i) < rowThreshold) ? true : false;

        if (wasLine && !isLine)
        {

            cv::Mat croppedImage = image(cv::Range(lineStart - 1, i + 1), cv::Range::all()).clone();
            lines.emplace_back(croppedImage);
            lineStart = i;
        }
        else if (!wasLine && isLine)
        {
            lineStart = i;
        }
        wasLine = isLine;
    }

    if (wasLine)
    {
        cv::Mat croppedImage = image(cv::Range(lineStart, rowProjection.rows), cv::Range::all()).clone();
        lines.emplace_back(croppedImage);
    }

    return lines;
}

int Segmenter::getMaxGapWidth(const cv::Mat &columnProjection)
{
    std::vector<int> gapWidths;
    int currentGapWidth = 0;
    bool inGap = false;

    double meanProjectionValue = cv::mean(columnProjection)[0];
    double dynamicThreshold = meanProjectionValue;

    for (int i = 0; i < columnProjection.cols; ++i)
    {
        float value = columnProjection.at<int>(0, i);

        // Check if this column is part of a gap using the dynamic threshold
        if (value < dynamicThreshold)
        {
            if (inGap)
            {
                currentGapWidth++;
            }
            else
            {
                inGap = true;
                currentGapWidth = 1;
            }
        }
        else if (inGap)
        {
            if (currentGapWidth > 1)
            {
                gapWidths.push_back(currentGapWidth);
            }
            inGap = false;
            currentGapWidth = 0;
        }
    }

    if (inGap && currentGapWidth > 1)
    {
        gapWidths.push_back(currentGapWidth);
    }

    std::sort(gapWidths.begin(), gapWidths.end());
    int percentileIndex = static_cast<int>(std::floor(gapWidths.size() * 0.75));
    int dynamicMaxGapWidth = (gapWidths.empty()) ? 1 : gapWidths[std::min(percentileIndex, static_cast<int>(gapWidths.size()) - 1)];

    return dynamicMaxGapWidth;
}

cv::Mat Segmenter::centerCharacter(const cv::Mat &characterSegment)
{
    int targetSize = 50;
    if (characterSegment.empty())
    {
        return cv::Mat::ones(targetSize, targetSize, CV_8UC1) * 255;
    }
    cv::Mat resizedSegment = characterSegment;

    if (characterSegment.rows > targetSize || characterSegment.cols > targetSize)
    {
        double scale = std::min(
            static_cast<double>(targetSize) / characterSegment.cols,
            static_cast<double>(targetSize) / characterSegment.rows);
        cv::resize(characterSegment, resizedSegment, cv::Size(), scale, scale, cv::INTER_AREA);
    }

    int xOffset = (targetSize - resizedSegment.cols) / 2;
    int yOffset = (targetSize - resizedSegment.rows) / 2;

    cv::Mat output = cv::Mat::ones(targetSize, targetSize, resizedSegment.type()) * 255;
    resizedSegment.copyTo(output(cv::Rect(xOffset, yOffset, resizedSegment.cols, resizedSegment.rows)));
    return output;
}

std::vector<cv::Mat> Segmenter::getCharacterSegments(const cv::Mat &image)
{
    cv::Mat binaryImage;
    cv::threshold(image, binaryImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    cv::Mat columnProjection;
    cv::reduce(binaryImage, columnProjection, 0, cv::REDUCE_SUM, CV_32S);

    int maxGapWidth = getMaxGapWidth(columnProjection);
    float rowThreshold = 255 * image.rows;
    std::vector<cv::Mat> segments;
    int segmentStart = 0;
    int gapWidth = 0;
    bool wasCharacter = columnProjection.at<int>(0, 0) < rowThreshold;

    for (int i = 0; i < columnProjection.cols; i++)
    {
        bool isCharacter = columnProjection.at<int>(0, i) < rowThreshold;
        if (wasCharacter)
        {
            if (!isCharacter)
            {
                cv::Mat croppedImage = binaryImage(cv::Range::all(), cv::Range(segmentStart, i)).clone();
                cv::Mat fittedImage = centerCharacter(croppedImage);

                segments.push_back(fittedImage);
                segmentStart = i;
                gapWidth = 1;
            }
        }
        else
        {
            if (isCharacter)
            {
                if (gapWidth >= maxGapWidth)
                {
                    segments.push_back(centerCharacter(cv::Mat()));
                }
                segmentStart = i;
                gapWidth = 0;
            }
            else
            {
                gapWidth++;
            }
        }

        wasCharacter = isCharacter;
    }

    if (wasCharacter && segmentStart != 0)
    {
        cv::Mat characterSegment = image(cv::Range::all(), cv::Range(segmentStart, columnProjection.cols)).clone();
        cv::Mat fittedImage = centerCharacter(characterSegment);
        segments.push_back(fittedImage);
    }

    return segments;
}