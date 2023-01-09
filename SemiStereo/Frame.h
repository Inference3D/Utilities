//--------------------------------------------------
// Represents a frame within the system
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
	class Frame
	{
	private:
		int _id;
		Mat _image;
		Mat _pose;
	public:
		Frame(int id, Mat& image, Mat& pose);
		Frame(const string& folder, int id);

		Size GetSize();

		inline int& GetId() { return _id; }
		inline Mat& GetImage() { return _image; }
		inline Mat& GetPose() { return _pose; }
	};
}
