//--------------------------------------------------
// Implementation of class Calibration
//
// @author: Wild Boar
//
// @date: 2023-01-04
//--------------------------------------------------

#include "Calibration.h"
using namespace NVL_App;

//--------------------------------------------------
// Constructors and Terminators
//--------------------------------------------------

/**
 * @brief Custom Constructor
 * @param camera The camera matrix
 * @param distortion The distortion matrix
 * @param size The size of the images that this calibration describes
 */
Calibration::Calibration(Mat& camera, Mat& distortion, const Size& size) : _camera(camera), _distortion(distortion), _imageSize(size)
{
	// Extra Implementation
}

/**
 * @brief Custom Constructor
 * @param folder The folder that we are loading the calibration from
 */
Calibration::Calibration(const string& folder)
{
	auto path = NVLib::FileUtils::PathCombine(folder, "Calibration.xml");
	auto reader = FileStorage(path, FileStorage::FORMAT_XML | FileStorage::READ);
	if (!reader.isOpened()) throw runtime_error("Unable to open: " + path);
	reader["camera"] >> _camera; reader["distortion"] >> _distortion; reader["image_size"] >> _imageSize;
	reader.release();
}
