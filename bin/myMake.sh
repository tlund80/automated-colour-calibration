if [ $1 ]
then
	g++ -o $1 $1.cpp `pkg-config opencv --cflags --libs`
fi
