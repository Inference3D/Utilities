//--------------------------------------------------
// Implementation code for Calibration
//
// @author: Wild Boar
//--------------------------------------------------

#include "MonoCalibration.h"
using namespace NVL_Tools;

//--------------------------------------------------
// Constructors
//--------------------------------------------------

/**
 * Main Constructor
 * @param path The path that we are loading the calibration from
 */
MonoCalibration::MonoCalibration(const string & path)
{
	auto reader = ifstream(path);
	if (!reader.is_open()) throw runtime_error("Unable to open file: " + path);

	_cameraMatrix = ReadCameraMatrix(reader);
	_distortion = ReadDistortion(reader);
	Mat pose = ReadPose(reader);
	_imageSize = ReadImageSize(reader);

	reader.close();
}

//--------------------------------------------------
// Set of reader helpers
//--------------------------------------------------

/**
 * Read the incomming camera matrix
 * @param reader the reader that we are using
 * @return The output result
 */
Mat MonoCalibration::ReadCameraMatrix(istream& reader) 
{
	Mat result = Mat_<double>(3,3); auto output = (double *) result.data; auto line = string(); auto parts = vector<string>();  

	getline(reader, line); parts.clear(); NVLib::StringUtils::Split(line, ' ', parts);
	for (auto i = 0; i < 3; i++) output[i] = NVLib::StringUtils::String2Double(parts[i]);

	getline(reader, line); parts.clear(); NVLib::StringUtils::Split(line, ' ', parts);
	for (auto i = 0; i < 3; i++) output[i + 3] = NVLib::StringUtils::String2Double(parts[i]);

	getline(reader, line); parts.clear(); NVLib::StringUtils::Split(line, ' ', parts);
	for (auto i = 0; i < 3; i++) output[i + 6] = NVLib::StringUtils::String2Double(parts[i]);

	return result;
}

/**
 * Read the associated distortion matrix
 * @param reader The reader that we are using
 * @return the output result
 */
Mat MonoCalibration::ReadDistortion(istream& reader) 
{
	Mat result = Mat_<double>(4,1); auto output = (double *) result.data; auto line = string(); auto parts = vector<string>();  

	getline(reader, line); parts.clear(); NVLib::StringUtils::Split(line, ' ', parts);
	for (auto i = 0; i < 4; i++) output[i] = NVLib::StringUtils::String2Double(parts[i]);

	return result;
}

/**
 * Read the pose from the system
 * @param reader The reader that we are using
 * @return The output result
 */
Mat MonoCalibration::ReadPose(istream& reader) 
{
	Mat result = Mat_<double>(4,3); auto output = (double *) result.data; auto line = string(); auto parts = vector<string>();  

	getline(reader, line); parts.clear(); NVLib::StringUtils::Split(line, ' ', parts);
	for (auto i = 0; i < 3; i++) output[i] = NVLib::StringUtils::String2Double(parts[i]);

	getline(reader, line); parts.clear(); NVLib::StringUtils::Split(line, ' ', parts);
	for (auto i = 0; i < 3; i++) output[i + 3] = NVLib::StringUtils::String2Double(parts[i]);

	getline(reader, line); parts.clear(); NVLib::StringUtils::Split(line, ' ', parts);
	for (auto i = 0; i < 3; i++) output[i + 6] = NVLib::StringUtils::String2Double(parts[i]);

	getline(reader, line); parts.clear(); NVLib::StringUtils::Split(line, ' ', parts);
	for (auto i = 0; i < 3; i++) output[i + 9] = NVLib::StringUtils::String2Double(parts[i]);

	return result;
}

/**
 * Read the image size that are dealing with
 * @param reader The reader that we are dealing with
 * @return The size that we are reading
*/
Size MonoCalibration::ReadImageSize(istream& reader) 
{
	int width, height; reader >> width >> height;
	return Size(width, height);
}
