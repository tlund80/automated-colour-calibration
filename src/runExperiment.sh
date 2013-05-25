#!/bin/bash
# By Calvin Tam
# Interative script that represents the workflow needed to perform the experiment
# At each stage you will be prompted with a message asking if you want to execute or not
# This is useful if you have already performed the step(s) and want to skip over it
BUILD_DIR="../build"

FILL_UNCLASSIFIED_EXEC="$BUILD_DIR/fillUnclassified"
APPLY_NNMC_EXEC="$BUILD_DIR/applyNnmc"
GRABCUT_EXEC="$BUILD_DIR/grabCut"
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

function askExecute {
    echo -n "Execute? ['y' to execute, Enter to skip] "
    read execute
    if [ "$execute" == "y" ]
    then
        eval "$1"
    fi
}

set -e
myecho "==== Manual Calibration ===="
echo "Open Offnao and select a frame with representative colours"
echo "Go to manual calibrationTab"
echo "Save raw image"
echo -n "Where is the original 640x480px raw image? "
read -e IMAGE_PATH
if [ ! -f "$IMAGE_PATH" ]
then
    echo "Invalid image path: $IMAGE_PATH"
    exit 1
fi
INPUT_DIR=`dirname $IMAGE_PATH`
origImage="$IMAGE_PATH"
smallOrigImage="$INPUT_DIR/small_original.png"
echo "Downsample from 640x480 to 160x120 ($origImage ---> $smallOrigImage)"
askExecute 'convert $origImage -sample 160x120 $smallOrigImage'
nnmc_manual="$INPUT_DIR/manual.nnmc"
echo -n "Calibrate and save nnmc file as " && myecho $nnmc_manual
askExecute
classifiedManualImage="$INPUT_DIR/manual.png"
echo "Apply the nnmc file ($smallOrigImage ---> $classifiedManualImage)"
askExecute '$APPLY_NNMC_EXEC $smallOrigImage $classifiedManualImage $nnmc_manual'
echo "Manual Calibration Complete"

askExecute
echo

myecho "==== Ground Truth ===="
classifiedTruthImage="$INPUT_DIR/partial_truth.png"
echo "Copy ($smallOrigImage ---> $classifiedTruthImage)"
askExecute 'cp $smallOrigImage $classifiedTruthImage'
echo -n "Go to GIMP and classify " && myecho $classifiedTruthImage 
askExecute
fillClassifiedTruthImage="$INPUT_DIR/truth.png"
echo "Fill in unclassified pixels ($classifiedTruthImage ---> $fillClassifiedTruthImage)"
askExecute '$FILL_UNCLASSIFIED_EXEC $classifiedTruthImage $fillClassifiedTruthImage'
echo "Ground Truth Complete"

askExecute
echo

# Preparation for both automatic colour calibration techniques
echo "Open $origImage in GIMP"
featureTemplate="$INPUT_DIR/features.template"
echo -n "Write feature template as " && myecho $featureTemplate
askExecute

myecho "==== Scaled Fovea + GrabCut ===="
echo "Apply feature template"
classifiedFoveaImage="$INPUT_DIR/fovea.png"
classifiedGrabcutImage="$INPUT_DIR/grabcut.png"
askExecute '
    happy="no"
    until [ "$happy" == "y" ]
    do
        echo -n "Update the feature template " && myecho $featureTemplate
        nnmc_fovea="$INPUT_DIR/fovea.nnmc"
        nnmc_grabcut="$INPUT_DIR/grabcut.nnmc"
        echo "Apply Fovea + Grabcut"
        askExecute "$GRABCUT_EXEC grabcut $origImage $featureTemplate $nnmc_grabcut &&
                    $GRABCUT_EXEC fovea $origImage $featureTemplate $nnmc_fovea"
        echo "Apply the nnmc file ($smallOrigImage ---> $classifiedFoveaImage)"
        echo "Apply the nnmc file ($smallOrigImage ---> $classifiedGrabcutImage)"
        askExecute "$APPLY_NNMC_EXEC $smallOrigImage $classifiedGrabcutImage $nnmc_grabcut &&
                    $APPLY_NNMC_EXEC $smallOrigImage $classifiedFoveaImage $nnmc_fovea"
        echo "Happy with the results? [\"y\" to finish, else redo template] "
        read happy
    done
'
echo "Automated Colour Calibration Complete"

askExecute
echo

myecho "==== Compare Errors ===="
echo "== Error Rates in % =="
echo -e "Manual\tFovea\tGrabCut"
echo -e "======\t=====\t======="
echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedManualImage`
echo -en "\t"
echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedFoveaImage`
echo -en "\t"
echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedGrabcutImage`

echo
