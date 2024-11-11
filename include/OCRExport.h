#ifndef OCR_EXPORT_H
#define OCR_EXPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

    const char *process_image_file(const char *inputFile);
    void free_output(const char *outputData);

#ifdef __cplusplus
}
#endif

#endif // OCR_EXPORT_H
