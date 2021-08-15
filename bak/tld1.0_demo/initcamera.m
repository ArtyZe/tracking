% Initialization of camera
% Try to run independently in Matlab (requires Image Acquisition toolbox)
% if it does not work, modify the first two lines

% Camera format
% source.vid = videoinput('winvideo', 1);
% source.vid = videoinput('winvideo', 1,'RGB24_320x240');
source.vid = videoinput('winvideo', 1,'YUY2_320x240');

set(source.vid,'ReturnedColorSpace','grayscale');
vidRes = get(source.vid, 'VideoResolution');
nBands = get(source.vid, 'NumberOfBands');
hImage = image( zeros(vidRes(2), vidRes(1), nBands) );
preview(source.vid, hImage);


