//--------------------------------------------------
// Implementation of class Frame
//
// @author: Wild Boar
//
// @date: 2023-01-04
//--------------------------------------------------

#include "Frame.h"
using namespace NVL_App;

//--------------------------------------------------
// Constructors and Terminators
//--------------------------------------------------

/**
 * @brief Custom Constructor
 * @param id The identifier associated with the frame
 * @param image The image represented by the frame
 * @param pose The pose associated with the frame
 */
Frame::Frame(int id, Mat& image, Mat& pose) : _id(id), _image(image), _pose(pose)
{
	// Extra implementation can go here
}

/**
 * @brief Custom Constructor
 * @param folder The folder that we are loading from
 * @param id The name of the identifier that we are loading from
 */
Frame::Frame(const string& folder, int id)
{
	auto imageFile = stringstream(); imageFile << "image_" << setw(4) << setfill('0') << id << ".jpg";
	auto poseFile = stringstream(); poseFile << "pose_" << setw(4) << setfill('0') << id << ".xml";

	auto imagePath = NVLib::FileUtils::PathCombine(folder, imageFile.str());
	auto posePath = NVLib::FileUtils::PathCombine(folder, poseFile.str());

	_image = imread(imagePath); if (_image.empty()) throw runtime_error("Unable to open image: " + imagePath);

	auto poseReader = FileStorage(posePath, FileStorage::FORMAT_XML | FileStorage::READ);
	if (!poseReader.isOpened()) throw runtime_error("Unable to open the pose reader: " + posePath);
	poseReader["pose"] >> _pose; 

	_id = id;
}

//--------------------------------------------------
// Image Size
//--------------------------------------------------

/**
 * @brief Retrieve the size of the image
 * @return Size Returns a Size
 */
Size Frame::GetSize()
{
	return _image.size();
}
