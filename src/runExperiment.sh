#!/bin/bash
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
read IMAGE_PATH
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

myecho "==== GrabCut ===="
echo "Open $origImage in GIMP"
grabCutTemplate="$INPUT_DIR/features.template"
echo -n "Write GrabCut template as " && myecho $grabCutTemplate
askExecute
echo "Apply GrabCut template"
classifiedGrabcutImage="$INPUT_DIR/grabcut.png"
askExecute '
    happy="no"
    until [ "$happy" == "y" ]
    do
        echo -n "Update the GrabCut template " && myecho $grabCutTemplate
        askExecute
        nnmc_grabcut="$INPUT_DIR/grabcut.nnmc"
        echo "Apply Grabcut"
        askExecute "$GRABCUT_EXEC $origImage $grabCutTemplate $nnmc_grabcut"
        echo "Apply the nnmc file ($smallOrigImage ---> $classifiedGrabcutImage)"
        askExecute "$APPLY_NNMC_EXEC $smallOrigImage $classifiedGrabcutImage $nnmc_grabcut"
        echo "Happy with the grabcut results? [\"y\" to finish, else redo template] "
        read happy
    done
'
echo "Grabcut Complete"

askExecute
echo

myecho "==== Compare Errors ===="
echo "== Error Rates in % =="
echo -e "Manual\tAutomatic"
echo -e "======\t========="
echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedManualImage`
echo -en "\t"
echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedGrabcutImage`
echo
