# For all png files in the directory, perform the Hough Transform and display the image, just hit a button and it will cycle through
ls *.png | xargs -I {} ./houghLines {}
