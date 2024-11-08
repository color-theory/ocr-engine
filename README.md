# ocr-engine
An engine to facilitate OCR with OpenCV

This should take an image with text in it, rotate it to the correct orientation (wihin reason), segment the image into lines and characters, and then return an array of flattened images to be passed on to a model for predictions.