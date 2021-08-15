TLD1.0 demo (Windows version)
=========================================================
This demo is available only for research and educational purposes. 
If you decide to use it, please cite the following paper:

@ARTICLE{Kalal2010,
  author = {Kalal, Z. and Matas, J. and Mikolajczyk, K.},
  title = {{P-N Learning: Bootstrapping Binary Classifiers by Structural Constraints}},
  journal = {CVPR},
  year = {2010}
}
                     
========================================================= 

Instalation:
 1. Microsoft Visual C++ 2008 SP1 Redistributable Package
    download and instal http://www.microsoft.com/downloads/details.aspx?familyid=a5c84275-3b97-4ab7-a40d-3802b2af5fc2&displaylang=en
 2. Download http://info.ee.surrey.ac.uk/Personal/Z.Kalal/TLD/MCRInstaller.exe
    run MCRInstaller.exe

Usage: 
 1. Define input (a or b)
    a) plug in your camera
    b) fill '/data' with image sequence, define 'init.txt' (initial bounding box position)
 2. run tld1.exe
 3. Output of the TLD is written to 'log.txt', format [frame id; left col; top row; right col; bottom row]
  

Control keys:
   n ... shows negative examples in online model (default on)
   p ... shows positive examples in online model (default on)
   i ... initialization of different target
   c ... show confidence score (default on)
   o ... show output as circle/dot/no output (default circle)
   d ... show detections (default on)
   t ... show target in top left corner (default off)
   r ... replace target with first patch (default off)
   # ... draw trajectory of target (default off)
   <space> ... saves current image
   1 ... mode without learning (fast, does not improve)
   2 ... mode with learning (slower, but can learn new appearances and discriminate)
   q ... finish application


Common errors:
============================================================

??? Error using ==> videoinput.videoinput at 328
The FORMAT specified is not supported by this device. See
IMAQHWINFO(ADAPTORNAME).
Error in ==> tmdInitSource at 12
Error in ==> tmdGo at 13
Error in ==> go at 49
}  
imaq:videoinput:noFormat

Reason:
Your camera does not support the format specified in initcamera.m

Solution:
Find what format is supported by your camera or directly try different setting in the section 'camera format' of file initcamera.m

--------------------------------------------------------------







   

