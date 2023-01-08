//--------------------------------------------------
// Implementation code for Engine
//
// @author: Wild Boar
//--------------------------------------------------

#include "Runner.h"
using namespace NVL_Module;

//--------------------------------------------------
// Constructor and Terminator
//--------------------------------------------------

/**
 * @brief Main constructor
 * @param parameters 
 * @param logger 
 */
Runner::Runner(NVLib::Parameters& parameters, LoggerBase * logger) : _logger(logger) 
{
    _frame = LoadStereoFrame(parameters);
}

/**
 * @brief Main Terminator
 */
Runner::~Runner() 
{ 
    delete _frame; 
}
		
//--------------------------------------------------
// Execution Entry Point
//--------------------------------------------------

/**
 * Entry point function
 */
void Runner::Run()
{
    setUseOptimized(true);
    double t = (double)getTickCount();

    Log() << "Finding Matching Points..." << LoggerBase::End();
    auto detector = NVLib::FastDetector(5);
    auto features_1 = vector<KeyPoint>(); detector.Extract(_frame->GetLeft(), features_1);
    auto features_2 = vector<KeyPoint>(); detector.Extract(_frame->GetRight(), features_2);
    Log() << "Features Found for Left: " << features_1.size() << LoggerBase::End();
    Log() << "Features Found for Right: " << features_2.size() << LoggerBase::End();

    Log() << "Finding Feature Matches..." << LoggerBase::End();
    detector.SetFrame(_frame->GetLeft(), _frame->GetRight());
    auto indices = vector<NVLib::MatchIndices *>(); detector.Match(features_1, features_2, indices,2);

    auto matches = vector<NVLib::FeatureMatch>();   
    for (auto index : indices) 
    {
        matches.push_back(NVLib::FeatureMatch(features_1[index->GetFirstId()].pt, features_2[index->GetSecondId()].pt));
        delete index;
    }
    Log() << " Matches Found: " << matches.size() << LoggerBase::End();

    Log() << "Calculating the Fundamental Matrix..." << LoggerBase::End();
    Mat F = NVLib::StereoUtils::GetFMatrix(matches);
    Log() << F << LoggerBase::End();

    Log() << "Finding the F Error: " << LoggerBase::End();
    auto errors = vector<double>();
    for (auto& match : matches) 
    {
        auto error = NVLib::StereoUtils::GetSampsonError(F, match);
        errors.push_back(error);
    }
    
    vector<double> mean, stddev;
    meanStdDev(errors, mean, stddev);
    auto error = Vec2d(mean[0], stddev[0]);
    Log() << error[0] << " &plusmn; " << error[1] << LoggerBase::End();

    Log() << "Computing rectification Homography..." << LoggerBase::End();
    auto hartley = Hartley(matches, F, _frame->GetLeft().size());
    Log() << "Done!" << LoggerBase::End();

    Log() << "Finding the Disparity Range: " << LoggerBase::End();
    auto disparityRange = GetDisparityRange(hartley, matches);
    Log() << "Range: " << disparityRange[0] << " to " << disparityRange[1] << LoggerBase::End();

    Log() << "Warping Stereo Pair..." << LoggerBase::End();
    Mat rLeft = ApplyH(hartley.GetHomography1(), _frame->GetLeft());
    Mat rRight = ApplyH(hartley.GetHomography2(), _frame->GetRight());
    Log() << "Done!" << LoggerBase::End();

    // Saving the rectified values to the globals
    _rectifiedLeft = rLeft.clone();
    _rectifiedRight = rRight.clone();

    Log() << "Setting up stereo matching..." << LoggerBase::End();
    auto disparityStart = Get16Factor(disparityRange[0]);
    auto disparityEnd = Get16Factor(disparityRange[1]);
    auto numDisparities = disparityEnd - disparityStart;
    Log() << "Done!" << LoggerBase::End();

    Log() << "Performing Stereo Matching..." << LoggerBase::End();
    auto matcher = StereoSGBM::create(disparityStart, numDisparities, 3, 200, 2400, 1, 0, 5, 200, 2, StereoSGBM::MODE_SGBM);
    Mat disparityMap; matcher->compute(rLeft, rRight, disparityMap);
    Log() << "Done!" << LoggerBase::End();

    Log() << "Calculating disparity range..." << LoggerBase::End();
    double minValue, maxValue;  minMaxIdx(disparityMap, &minValue, &maxValue);
    Log() << "Done!" << LoggerBase::End();

    Log() << "Normalizing the disparity map" << LoggerBase::End();
    SaveDisparity(disparityMap, hartley.GetHomography1());
    Log() << "Done" << LoggerBase::End();

    t = ((double)getTickCount() - t) / getTickFrequency();
    Log() << "Time passed in seconds: " << t << LoggerBase::End();
}

//--------------------------------------------------
// Helper Methods
//--------------------------------------------------

/**
 * Derive the functionality to apply the given homography
 * @param H The homography that we are applying
 * @param image The image that we are applying the homography to
 * @return The resultant matrix
 */
Mat Runner::ApplyH(const Mat& H, const Mat& image) 
{
    Mat result; warpPerspective(image, result, H, result.size(), INTER_CUBIC);
    return result;
}

/**
 * Get the 16 factor for the given number
 * @param number The number that we are getting the factor for
 * @return The factor as an integer
 */
int Runner::Get16Factor(double number)
{
    auto sign = number >= 0 ? 1 : -1;
    auto magnitude = abs(number);
    auto factor = (int)ceil(magnitude / 16.0);
    return sign * factor * 16;
}

/**
 * Save the disparity map to disk
 * @param disparityMap The disparity map that is being saved
 * @param H The current homography
 */
void Runner::SaveDisparity(Mat& disparityMap, Mat& H)
{
    Mat result = Mat_<float>::zeros(disparityMap.size());

    auto input = (short*)disparityMap.data;
    auto output = (float *) result.data;

    for (auto row = 0; row < disparityMap.rows; row++)
    {
        for (auto column = 0; column < disparityMap.cols; column++)
        {
            auto index = column + row * disparityMap.cols;
            auto disparity = (float)input[index];
            output[index] = disparity / 16.0f;
        }
    }

    Mat wresult; warpPerspective(result, wresult, H.inv(), result.size(), INTER_NEAREST);

    _disparity = wresult;
}

//--------------------------------------------------
// Defines the loader functionality
//--------------------------------------------------

/**
 * @brief Defines the functionality for loading the stereo image from the parameters
 * 
 * @param parameters 
 * @return NVLib::StereoFrame The stereo frame that we are loading
 */
NVLib::StereoFrame * Runner::LoadStereoFrame(NVLib::Parameters& parameters) 
{
    // Retrieve the names of the files
    auto leftFile = ReadString(parameters, "left_image");
    auto rightFile = ReadString(parameters, "right_image");

    // Load the individual images
    auto leftImage = NVLib::LoadUtils::LoadImage(leftFile);
    auto rightImage = NVLib::LoadUtils::LoadImage(rightFile);

    // Perform the resize stuff
    auto maxDim = max(leftImage.cols, leftImage.rows);
    auto factor = maxDim > 1000.0 ? 1000.0 / maxDim : 1.0;

    Mat image1; resize(leftImage, image1, Size(), factor, factor);
    Mat image2; resize(rightImage, image2, Size(), factor, factor);

    // Return the loaded stereo frame
    return new NVLib::StereoFrame(image1, image2);
}