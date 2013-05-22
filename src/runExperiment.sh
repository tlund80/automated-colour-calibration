#!/bin/bash
BUILD_DIR="../build"

FILL_UNCLASSIFIED_EXEC="$BUILD_DIR/fillUnclassified"
APPLY_NNMC_EXEC="$BUILD_DIR/applyNnmc"
COMPARE_ERROR_EXEC="$BUILD_DIR/compareError"


# Highlights the arguments with a sharp green
function myecho {
    echo -ne '\033[32;1m'
    echo -n $@
    echo -e '\033[0m'
}

function confirm {
    echo -n "Done with the previous? [Enter to continue, Ctrl-C to exit] "
    read done
}

myecho "==== Manual Calibration ===="
echo "Open Offnao and select a frame with representative colours"
echo "Go to manual calibrationTab"
echo "Save raw image"
echo -n "Where is the original 640x480px raw image? "
read IMAGE_PATH
INPUT_DIR=`dirname $IMAGE_PATH`
origImage="$IMAGE_PATH"
smallOrigImage="$INPUT_DIR/small_original.png"
echo "Downsampling from 640x480 to 160x120 ($origImage ---> $smallOrigImage)"
convert $origImage -sample 160x120 $smallOrigImage
nnmc_manual="$INPUT_DIR/manual.nnmc"
echo "Calibrate and save nnmc file as $nnmc_manual"
confirm
classifiedManualImage="$INPUT_DIR/manual.png"
echo "Applying the nnmc file ($smallOrigImage ---> $classifiedManualImage)"
eval $APPLY_NNMC_EXEC $smallOrigImage $classifiedManualImage $nnmc_manual
echo "Manual Calibration Complete"

echo
confirm

myecho "==== Ground Truth ===="
classifiedTruthImage="$INPUT_DIR/partial_truth.png"
echo "Copying ($smallOrigImage ---> $classifiedTruthImage)"
cp $smallOrigImage $classifiedTruthImage
echo -n "Go to GIMP and classify " && myecho $classifiedTruthImage 
confirm
fillClassifiedTruthImage="$INPUT_DIR/truth.png"
echo "Filling in unclassified pixels ($classifiedTruthImage ---> $fillClassifiedTruthImage)"
eval $FILL_UNCLASSIFIED_EXEC $classifiedTruthImage $fillClassifiedTruthImage
echo "Ground Truth Complete"

echo
confirm

myecho "==== GrabCut ===="
echo "Open $origImage in GIMP"
grabCutTemplate="$INPUT_DIR/features.template"
echo "Write GrabCut template as $grabCutTemplate"
confirm
nnmc_grabcut="$INPUT_DIR/grabcut.nnmc"
echo "Applying Grabcut"
eval $GRABCUT_EXEC $origImage $grabCutTemplate $nnmc_grabcut
classifiedGrabcutImage="$INPUT_DIR/grabcut.png"
echo "Applying the nnmc file ($smallOrigImage ---> $classifiedGrabcutImage)"
eval $APPLY_NNMC_EXEC $smallOrigImage $classifiedGrabcutImage $nnmc_grabcut
echo "Grabcut Complete"

echo
confirm

myecho "==== Compare Errors ===="
echo -e "Manual\tAutomatic"
echo -e "======\t========="
echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedManualImage`
echo -n "\t"
echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedGrabcutImage`

