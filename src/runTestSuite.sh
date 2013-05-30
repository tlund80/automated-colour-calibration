#!/bin/bash
# By Calvin Tam
# Interative script that given the directory storing the grabCut and manual nnmc files, applies them to the test image and compares to the truth image for error rates
# At each stage you will be prompted with a message asking if you want to execute or not
# This is useful if you have already performed the step(s) and want to skip over it
BUILD_DIR="../build"

FILL_UNCLASSIFIED_EXEC="$BUILD_DIR/fillUnclassified"
APPLY_NNMC_EXEC="$BUILD_DIR/applyNnmc"
COMPARE_ERROR_EXEC="$BUILD_DIR/compareError"


# Highlights the arguments with a sharp green
function myecho {
    echo -ne '\033[32;1m'
    echo -n $@
    echo -ne '\033[0m'
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
echo -n "Where is the manual.nnmc/grabcut.nnmc/fovea.nnmc file? "
read -e NNMC_PATH
if [ ! -f "$NNMC_PATH" ]
then
    echo "Invalid nnmc path: $NNMC_PATH"
    exit 1
fi
INPUT_DIR=`dirname $NNMC_PATH`
nnmc_fovea="$INPUT_DIR/fovea.nnmc"
nnmc_grabcut="$INPUT_DIR/grabcut.nnmc"
nnmc_manual="$INPUT_DIR/manual.nnmc"
myecho "==== Run Test Suite ===="
echo
# Test in descending order
for test_dir in $(find "../images/test" -type d -regextype posix-egrep -regex ".*/[0-9]+$" | sort -r)
do
    echo "Running test: $test_dir"
    
    # Downsample the original test image
    smallOrigImage="$test_dir/small_original.png"
    
    # Only downsample if not done so already
    if [ ! -f "$smallOrigImage" ]
    then
        origImage="$test_dir/original.png"
        echo "Downsample from 640x480 to 160x120 ($origImage ---> $smallOrigImage)"
        convert "$origImage" -sample 160x120 "$smallOrigImage"
    fi

    # Apply the manual.nnmc
    classifiedManualImage="$test_dir/manual.png"
    echo "Apply the nnmc file ($smallOrigImage ---> $classifiedManualImage)"
    eval "$APPLY_NNMC_EXEC $smallOrigImage $classifiedManualImage $nnmc_manual"

    # Apply the fovea.nnmc
    classifiedFoveaImage="$test_dir/fovea.png"
    echo "Apply the nnmc file ($smallOrigImage ---> $classifiedFoveaImage)"
    eval "$APPLY_NNMC_EXEC $smallOrigImage $classifiedFoveaImage $nnmc_fovea"
    
    # Apply the grabcut.nnmc
    classifiedGrabcutImage="$test_dir/grabcut.png"
    echo "Apply the nnmc file ($smallOrigImage ---> $classifiedGrabcutImage)"
    eval "$APPLY_NNMC_EXEC $smallOrigImage $classifiedGrabcutImage $nnmc_grabcut"

    fillClassifiedTruthImage="$test_dir/truth.png"
    if [ ! -f "$fillClassifiedTruthImage" ]
    then
        classifiedTruthImage="$test_dir/partial_truth.png"
        if [ ! -f "$classifiedTruthImage" ]
        then
            echo "Copy ($smallOrigImage ---> $classifiedTruthImage)"
            cp $smallOrigImage $classifiedTruthImage
        fi
        echo -n "Go to GIMP and classify " && myecho $classifiedTruthImage && echo 
        askExecute
        echo "Fill in unclassified pixels ($classifiedTruthImage ---> $fillClassifiedTruthImage)"
        askExecute '$FILL_UNCLASSIFIED_EXEC $classifiedTruthImage $fillClassifiedTruthImage'
    fi

    myecho "==== Compare Errors ===="
    echo
    echo "== Error Rates in % =="
    echo -e "Manual\tFovea\tGrabCut"
    echo -e "======\t=====\t======="
    echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedManualImage`
    echo -en "\t"
    echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedFoveaImage`
    echo -en "\t"
    echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedGrabcutImage`

    echo
done
