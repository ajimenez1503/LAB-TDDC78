# LAB-TDDC78
Laboratory Work in Programming of Parallel Computers TDDC78.

## LAB1 Image Filter.
Two simple image transformation algorithms. The transformations are from an input image to an output image.
  * Thresholding Filter:
    The filter computes the average intensity of image and the result is an image containing only black and white pixels. White for those pixels in the input image that are lighter than the threshold and black for those pixels in the input image that are darker than the threshold.

  * Averaging Filter:
    The value for a pixel (x, y) in the output image is the normalized weighted sum of all the pixels in a
rectangle in the input image centred around (x, y).
### Build the project:
 * Install: `sudo apt-get install libcr-dev mpich2 mpich2-doc`
 * Compile: `make`
 * Run: `bash run.sh`
