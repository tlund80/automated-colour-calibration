#!/bin/bash
#FILL_UNCLASSIFIED_EXEC="../build/fillUnclassified"
#APPLY_NNMC_EXEC="../build/applyNnmc"
#COMPARE_ERROR_EXEC="../build/compareError"

# Highlights the arguments with a sharp green
function myecho {
    echo -ne '\033[32;1m'
    echo -n $@
    echo -e '\033[0m'
}

myecho "==== Manual Calibration ===="
echo "Open Offnao and select a frame with representative colours"
echo "Go to manual calibrationTab"
echo "Save raw image"
echo -n "Where is the original 640x480px raw image? "
read origImage
smallOrigImage="small_`basename $origImage`"
echo "Downsampling from 640x480 to 160x120 ($origImage ---> $smallOrigImage)"
#convert $origImage -sample 160x120 $smallOrigImage
echo "Calibrate and save nnmc file"
echo -n "Where is the nnmc file? "
read nnmc_manual
classifiedManualImage="classified_manual_`basename $origImage`"
echo "Applying the nnmc file ($smallOrigImage ---> $classifiedManualImage)"
#eval $APPLY_NNMC_EXEC $smallOrigImage $classifiedManualImage $nnmc_manual
echo "Manual Calibration Complete"

echo

myecho "==== Ground Truth ===="
classifiedTruthImage="classified_truth_`basename $origImage`"
echo "Copying ($smallOrigImage ---> $classifiedTruthImage)"
cp $smallOrigImage $classifiedTruthImage
echo -n "Go to GIMP and classify " && myecho $classifiedTruthImage 
echo -n "Finished classifying? "
read confirm
fillClassifiedTruthImage="filled_$classifiedTruthImage"
echo "Filling in unclassified pixels ($classifiedTruthImage ---> $fillClassifiedTruthImage)"
#eval $FILL_UNCLASSIFIED_EXEC $classifiedTruthImage $fillClassifiedTruthImage
echo "Ground Truth Complete"

echo

myecho "==== GrabCut ===="
echo "Open $origImage in GIMP"
echo "Write GrabCut template"
echo "Applying Grabcut"
nnmc_grabcut="grabcut.nnmc"
#eval $GRABCUT_EXEC $origImage $nnmc_grabcut
classifiedGrabcutImage="classified_grabcut_`basename $origImage`"
echo "Applying the nnmc file ($smallOrigImage ---> $classifiedGrabcutImage)"
#eval $APPLY_NNMC_EXEC $smallOrigImage $classifiedGrabcutImage $nnmc_grabcut
echo "Grabcut Complete"

echo

myecho "==== Compare Errors ===="
echo -e "Manual\tAutomatic"
echo -e "======\t========="
#echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedManualImage`
#echo -n "\t"
#echo -n `eval $COMPARE_ERROR_EXEC $fillClassifiedTruthImage $classifiedGrabcutImage`

