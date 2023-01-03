//--------------------------------------------------
// A structure for storing monocular calibration results (camera matrix + distortion matrix)
//
// @author: Wild Boar
//--------------------------------------------------

#pragma once

#include <fstream>
#include <iostream>
using namespace std;

#include <NVLib/StringUtils.h>

#include <opencv2/opencv.hpp>
using namespace cv;

namespace NVL_Tools
{
	class MonoCalibration
	{
	private:
		Mat _cameraMatrix;
		Mat _distortion;
		vector<Mat> _poses;
		Size _imageSize;
	public:
		MonoCalibration(const string & path);

		inline Mat& GetCameraMatrix() { return _cameraMatrix; }
		inline Mat& GetDistortion() { return _distortion; }
		inline Size& GetImageSize() { return _imageSize; }
	private:
		Mat ReadCameraMatrix(istream& reader);
		Mat ReadDistortion(istream& reader);
		Mat ReadPose(istream& reader);
		Size ReadImageSize(istream& reader);
	};
}
