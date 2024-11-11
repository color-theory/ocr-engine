#include "OCRExport.h"
#include "OCRImageProcessor.h"
#include <string>
#include <cstring>

#include <iostream>

extern "C"
{
    const char *process_image_file(const char *inputFile)
    {
        OCRImageProcessor processor(inputFile);
        std::vector<std::vector<float>> processedData = processor.extractCharacters();
        std::string serializedData = processor.serializeToJson(processedData);

        char *outputData = new char[serializedData.size() + 1];
        std::strcpy(outputData, serializedData.c_str());
        return outputData;
    }

    void free_output(const char *outputData)
    {
        delete[] outputData;
    }
}
