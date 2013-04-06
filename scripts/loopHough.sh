# For all png files in the directory, perform the Hough Transform and display the image, just hit a button and it will cycle through
if [ -d $1 ]
then
    ls "$1"/*.png | xargs -I {} ../build/houghLines {}
else
    echo Usage: $0 [IMG_DIR]
fi
