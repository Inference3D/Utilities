//--------------------------------------------------
// Defines a basic engine for a vanilla C++ project.
//
// @author: Wild Boar
//--------------------------------------------------

#pragma once

#include <iostream>
using namespace std;

#include <ModuleLib/LoggerBase.h>

#include <NVLib/Parameters/Parameters.h>
#include <NVLib/Model/StereoFrame.h>
#include <NVLib/Model/FeatureMatch.h>
#include <NVLib/LoadUtils.h>

#include <NVLib/StereoUtils.h>
#include <NVLib/Odometry/FastDetector.h>
#include <NVLib/Odometry/FastTracker.h>

#include "Hartley.h"

namespace NVL_Module
{
	class Runner
	{
	private:
		LoggerBase * _logger;
		NVLib::StereoFrame * _frame;

		Mat _rectifiedLeft;
		Mat _rectifiedRight;
		Mat _disparity;

	public:
		Runner(NVLib::Parameters& parameters, LoggerBase * logger);
		~Runner();
		
		void Run();

		inline Mat& GetRectifiedLeft() { return _rectifiedLeft; }
		inline Mat& GetRectifiedRight()	 { return _rectifiedRight; }
		inline Mat& GetDisparity() { return _disparity; }

	private:
		Mat ApplyH(const Mat& H, const Mat& image);
		int Get16Factor(double number);
		void SaveDisparity(Mat& disparityMap, Mat& H);

		NVLib::StereoFrame * LoadStereoFrame(NVLib::Parameters& parameters);
		
		inline LoggerBase& Log() { return *_logger; }

		/**
		 * @brief Read a string value from the parameters
		 * @param parameters The parameter collection
		 * @param key The key that we want
		 * @return string The resultant string
		 */
		inline string ReadString(NVLib::Parameters& parameters, const string& key) 
		{
			if (!parameters.Contains(key)) throw runtime_error("Required key not found: " + key);
			return parameters.Get(key);
		}

		
		/**
		 * Find the given disparity range
		 * @param matches The matches that were found
		 * @return The disparity range that was found
		 */
		inline Vec2d GetDisparityRange(Hartley& hartley, vector<NVLib::FeatureMatch>& matches) 
		{
			auto minDisparity = DBL_MAX; auto maxDisparity = -DBL_MAX;
			
			for (auto match : matches) 
			{
				auto p1 = ApplyH(hartley.GetHomography1(), match.GetPoint1());
				auto p2 = ApplyH(hartley.GetHomography2(), match.GetPoint2());

				auto xDiff = p1.x - p2.x;
				auto yDiff = p1.y - p2.y;
				auto magnitude = sqrt(xDiff * xDiff + yDiff * yDiff);        
				auto biggestComponent = abs(xDiff) >= abs(yDiff) ? xDiff : yDiff;
				auto sign = biggestComponent >= 0 ? 1 : -1;
				magnitude *= sign;
				minDisparity = magnitude < minDisparity ? magnitude : minDisparity;
				maxDisparity = magnitude > maxDisparity ? magnitude : maxDisparity;
			}

			return Vec2d(minDisparity, maxDisparity);
		}

		/**
		 * Apply a homography to the given points
		 * @param H The current homography
		 * @param point The point that we are applying
		 */
		inline Point2d ApplyH(const Mat& H, const Point2f& point)
		{
			auto hdata = (double*)H.data;

			auto X = hdata[0] * point.x + hdata[1] * point.y + hdata[2];
			auto Y = hdata[3] * point.x + hdata[4] * point.y + hdata[5];
			auto Z = hdata[6] * point.x + hdata[7] * point.y + hdata[8];

			return Point2d(X / Z, Y / Z);
		}
	};
}