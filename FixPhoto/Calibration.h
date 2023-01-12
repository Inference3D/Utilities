//--------------------------------------------------
// The calibration associated with the given images
//
// @author: Wild Boar
//
// @date: 2023-01-04
//--------------------------------------------------

#pragma once

#include <iostream>
using namespace std;

#include <NVLib/FileUtils.h>

#include <opencv2/opencv.hpp>
using namespace cv;

namespace NVL_App
{
	class Calibration
	{
	private:
		Mat _camera;
		Mat _distortion;
		Size _imageSize;
	public:
		Calibration(Mat& camera, Mat& distortion, const Size& size);
		Calibration(const string& folder);

		inline Mat& GetCamera() { return _camera; }
		inline Mat& GetDistortion() { return _distortion; }
		inline Size& GetSize() { return _imageSize; }
	};
}
