CC_FLAGS := -Wall -Werror 
OPENCV_LIBS := `pkg-config opencv --cflags --libs`
 
classifyBall: classifyBall.cpp
	g++ $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

classifyLine: classifyLine.cpp classifyLine.hpp Feature.hpp
	g++ $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

houghLines: houghLines.cpp
	g++ $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

houghCircle: houghCircle.cpp
	g++ $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)
