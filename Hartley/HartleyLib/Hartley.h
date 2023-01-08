//--------------------------------------------------
// Wrapper for the Hartely rectification algorithm as per OpenCV
//
// @author: Wild Boar
//--------------------------------------------------

#pragma once

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <NVLib/Model/FeatureMatch.h>

namespace NVL_Module
{
	class Hartley
	{
	private:
		Mat _homography1;
		Mat _homography2;
	public:
		Hartley(vector<NVLib::FeatureMatch> & features, const Mat& fmatrix, const Size & size);

		inline Mat& GetHomography1() { return _homography1; }
		inline Mat& GetHomography2() { return _homography2; }
	};
}
