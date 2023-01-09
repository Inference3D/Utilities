# Inference3d Utilities #

This repository contains a set of repositories for processing the data associated with the Inference3d project. The utilities available in this project are as follows:

* [MonoCalib](#mono_calib)
* [NVM Extractor](#nvm_extractor)
* [VideoExtract](#video_extract)
* [Hartley](#hartley)
* [MaaraX](#maarax)
* [SemiStereo](#semi_stereo) 


## <a name="mono_calib" />MonoCalib ##

This tool takes the output from _Camera Calibration Tools_ [link](http://www0.cs.ucl.ac.uk/staff/Dan.Stoyanov/calib/) for the calibration of a monocular camera and converts it into the XML file used in most of my applications.

## <a name="nvm_extractor" />NVM Extractor ##

Takes an "N-View Match" file [link](http://ccwu.me/vsfm//doc.html#nvm) and extracts the poses out of the file, for initialization of the refinement logic.

## <a name="video_extract" />VideoExtract ##

Takes a single video file (in all the formats supported by OpenCV) and extracts a set of frames from the file

## <a name="hartley" />Hartley ##

Takes a pair of uncalibrated images (reasonably textured images with left-right displacement and significant overlap) and attempts to generate a disparity map between the two images.

## <a name="maarax" />MaaraX ##

Points to a folder that contains the standard outputs of a "Maaratech" scan, and extracts that folder into an output folder where the files have been formatted in the typical style that I use for my systems.

## <a name="semi_stereo" />Semi Stereo ##

This is a tool that takes in a stereo pair + calibration details + pose estimation and then uses OpenCV's stereo rectification and SGBM stereo matching to produce a disparity map of the system. It can be used to create disparity maps, but also confirm things like pose files and calibration files.
