CC_FLAGS := -Wall -Werror 
OPENCV_LIBS := `pkg-config opencv --cflags --libs`
 
houghLines: houghLines.cpp
	g++ $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)

houghCircle: houghCircle.cpp
	g++ $(CC_FLAGS) -o ../build/$@ $< $(OPENCV_LIBS)
