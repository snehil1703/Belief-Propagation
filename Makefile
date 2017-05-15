all : segment

segment: CImg.h segment.cpp
	g++ -Dcimg_display=0 segment.cpp -o segment -I. -O3

clean:
	rm segment
