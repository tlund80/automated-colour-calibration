CC_FLAGS := -Wall -Werror 
OPENCV_LIBS := `pkg-config opencv --cflags --libs`
DEBUG := -g
 
applyNnmc: applyNnmc.cpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ $<

grabCut: grabCut.cpp classifier.cpp VisionDefs.hpp classifier.hpp
	g++ $(DEBUG) $(CC_FLAGS) -o ../build/$@ grabCut.cpp classifier.cpp -lbz2 $(OPENCV_LIBS)
	
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
