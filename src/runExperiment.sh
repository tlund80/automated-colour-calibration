#!/bin/bash
# In progress atm....

#FILL_UNCLASSIFIED_EXEC="./fillUnclassified"
#APPLY_NNMC_EXEC="./applyNnmc"
#COMPARE_ERROR_EXEC="./compareError"

# Highlights the arguments with a sharp green
function myecho {
    echo -ne '\033[32;1m'
    echo -n $@
    echo -e '\033[0m'
}

myecho "==== Manual Calibration ===="
echo "Open Offnao and select a frame"
echo "Go to manual calibrationTab"
echo "Save raw image"
echo -n "Where is the original 640x480px raw image? "
read origImage
smallOutputImage="small_`basename $origImage`"
echo "Downsampling from 640x480 to 160x120 ($origImage ---> $smallOutputImage)? "
#convert $origImage -sample 160x120 $smallOutputImage
echo "Calibrate and save nnmc file"
echo -n "Where is the nnmc file? "
read nnmc_manual
classifiedManualImage="classified_manual_`basename $origImage`"
echo "Applying the nnmc file ($smallOutputImage ---> $classifiedManualImage)"
#$APPLY_NNMC_EXEC $smallOutputImage $classifiedManualImage $nnmc_manual

echo

myecho "==== Ground Truth ===="
echo -n "Go to GIMP and classify " && myecho $smallOutputImage 
echo -n "Where is the image you just classified? "
read truthImage
classifiedTruthImage="classified_truth_`basename $origImage`"
echo "Filling in unclassified pixels ($truthImage-> $classifiedTruthImage)"
#$FILL_UNCLASSIFIED_EXEC $truthImage 

echo

myecho "==== GrabCut ===="
echo "Open $origImage in GIMP"
echo "Write GrabCut template"
echo "Applying Grabcut"
#$GRABCUT_EXEC $origImage
echo -n "Where is the nnmc that was just produced? "
read nnmc_grabcut
classifiedGrabcutImage="classified_grabcut_`basename $origImage`"
#$APPLY_NNMC_EXEC $smallOutputImage sified_grabcut_`basename $origImage` $nnmc_grabcut_

echo

myecho "==== Compare Errors ===="
#echo -n `$COMPARE_ERROR_EXEC $classifiedTruthImage $classifiedManualImage`
#echo -n "\t"
#echo -n `$COMPARE_ERROR_EXEC $classifiedTruthImage $classifiedGrabcutImage`

