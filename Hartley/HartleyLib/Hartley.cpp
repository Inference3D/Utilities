//--------------------------------------------------
// Implementation code for Hartley
//
// @author: Wild Boar
//--------------------------------------------------

#include "Hartley.h"
using namespace NVL_Module;

//--------------------------------------------------
// Constructors
//--------------------------------------------------

/**
 * Main Constructor
 * @param features The list of features for the application
 * @param fmatrix The fundamental matrix
 * @param size The size of the image
 */
Hartley::Hartley(vector<NVLib::FeatureMatch> & features, const Mat& fmatrix, const Size & size)
{
    vector<Point2f> points1, points2;
    for (auto feature : features) { points1.push_back(feature.GetPoint1()); points2.push_back(feature.GetPoint2()); }
    stereoRectifyUncalibrated(points1, points2, fmatrix, size, _homography1, _homography2, 1);
}
