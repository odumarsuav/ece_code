#!/bin/bash

opencv_createsamples -img positives/image1.png -bg negatives/negatives.info -num 30 -info overlay1.info -bgcolor 255 -bgthresh 8 -w 48 -h 48
opencv_createsamples -img positives/image2.png -bg negatives/negatives.info -num 30 -info overlay2.info -bgcolor 255 -bgthresh 8 -w 48 -h 48
opencv_createsamples -img positives/image3.png -bg negatives/negatives.info -num 30 -info overlay3.info -bgcolor 255 -bgthresh 8 -w 48 -h 48
opencv_createsamples -img positives/image4.png -bg negatives/negatives.info -num 30 -info overlay4.info -bgcolor 255 -bgthresh 8 -w 48 -h 48

cat overlay*.info > overlays.info

opencv_createsamples -info positives/positive_rectangles.info -bg negatives/negatives.info -vec overlays.vec -num 100 -w 48 -h 48

opencv_traincascade -data soda_cascade -vec overlays.info -bg negatives/negatives.info -numStages 10 -w 48 -h 48


