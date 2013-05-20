CC_FLAGS := -Wall -Werror
OPENCV_LIBS := `pkg-config opencv --cflags --libs`
DEBUG := -g
 
fillUnclassified: fillUnclassified.cpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

compareError: compareError.cpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

applyNnmc: applyNnmc.cpp helpers/NNMC.cpp helpers/NNMC.hpp helpers/NNMCinline.hpp
	g++ $(DEBUG) -Wall -o ../build/$@ applyNnmc.cpp helpers/NNMC.cpp -lbz2 $(OPENCV_LIBS)

grabCut: grabCut.cpp helpers/classifier.cpp helpers/VisionDefs.hpp helpers/classifier.hpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ grabCut.cpp helpers/classifier.cpp -lbz2 $(OPENCV_LIBS)
	
thresholding: thresholding.cpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

classifyBall: classifyBall.cpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

classifyLine: classifyLine.cpp classifyLine.hpp Feature.hpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

houghLines: houghLines.cpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

houghCircle: houghCircle.cpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)
