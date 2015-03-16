#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

#define USE_SimpleViewer 0

#if USE_SimpleViewer
#include "SimpleViewer.h"
#endif

using namespace std;
using namespace cv;

typedef void (*func_extract_and_match_feature_points_t)(Mat&, Mat&, vector<Point2f>&, vector<Point2f>&);

/*
OpenCV Feature&Descriptor Extractor
    "SIFT"  SIFT (nonfree module)
    "SURF"  SURF (nonfree module)
    "BRIEF" BriefDescriptorExtractor
    "BRISK" BRISK
    "ORB"   ORB
    "FREAK" FREAK
OpenCV Descriptor Matcher
    "BruteForce" (it uses L2 )
    "BruteForce-L1"
    "BruteForce-Hamming"
    "BruteForce-Hamming(2)"
    "FlannBased"
*/

#define EXTRACT_AND_MATCH_FEATURE_POINTS(Feature, OpenCV_Feature_Name,OpenCV_Matcher_Name)      \
    void extract_and_match_feature_points_##Feature (Mat& gray1, Mat& gray2,                    \
            vector<Point2f>& left_points, vector<Point2f>& right_points)                        \
    {                                                                                           \
        cout << __FUNCTION__ << "()-" << OpenCV_Matcher_Name << endl;                           \
                                                                                                \
        Ptr <FeatureDetector> detector = FeatureDetector::create(OpenCV_Feature_Name);          \
        vector<KeyPoint> KeyPoints1, KeyPoints2;                                                \
        detector->detect(gray1, KeyPoints1);                                                    \
        detector->detect(gray2, KeyPoints2);                                                    \
        cout << "  img1 " << OpenCV_Feature_Name << " features " << KeyPoints1.size() << endl   \
             << "  img2 " << OpenCV_Feature_Name << " features " << KeyPoints2.size() << endl;  \
                                                                                                \
        Mat Descriptors1, Descriptors2;                                                         \
        Ptr <DescriptorExtractor> extractor = DescriptorExtractor::create(OpenCV_Feature_Name); \
        extractor->compute(gray1, KeyPoints1, Descriptors1);                                    \
        extractor->compute(gray2, KeyPoints2, Descriptors2);                                    \
        cout << "  img1 " << OpenCV_Feature_Name << " descriptor " << Descriptors1.rows << endl \
             << "  img2 " << OpenCV_Feature_Name << " descriptor " << Descriptors2.rows << endl;\
                                                                                                \
        Ptr <DescriptorMatcher> matcher = DescriptorMatcher::create(OpenCV_Matcher_Name);       \
        vector< DMatch > matches;                                                               \
        matcher->match( Descriptors1, Descriptors2, matches );                                  \
        cout << "  matched features " << matches.size() << endl;                                \
                                                                                                \
        for (size_t i=0; i < matches.size(); i++)                                               \
        {                                                                                       \
            left_points.push_back(KeyPoints1[matches[i].queryIdx].pt);                          \
            right_points.push_back(KeyPoints2[matches[i].trainIdx].pt);                         \
        }                                                                                       \
    }

EXTRACT_AND_MATCH_FEATURE_POINTS(SIFT, "SIFT", "FlannBased");
EXTRACT_AND_MATCH_FEATURE_POINTS(SURF, "SURF", "FlannBased");
EXTRACT_AND_MATCH_FEATURE_POINTS(ORB, "ORB", "BruteForce");
EXTRACT_AND_MATCH_FEATURE_POINTS(BRISK, "BRISK", "BruteForce");

void extract_and_match_feature_points_OpticalFlow (Mat& gray1, Mat& gray2,
        vector<Point2f>& left_points, vector<Point2f>& right_points)
{
    cout << __FUNCTION__ << "()" << endl;
    /*  Find the optical flow using farneback dense algorithm
        Note that you might need to tune the parameters, especially window size.
        Smaller window size param, means more ambiguity when calculating the flow.
     */
    Mat flow_mat;
    int matchNum = 0;
    calcOpticalFlowFarneback( gray1, gray2, flow_mat, 0.5, 3, 12, 3, 5, 1.2, 0 );

    left_points.clear();
    right_points.clear();
    for ( int y = 0; y < gray1.rows; y+=6 ) {
        for ( int x = 0; x < gray1.cols; x+=6 ) {
            /* Flow is basically the delta between left and right points */
            Point2f flow = flow_mat.at<Point2f>(y, x);
            
            /*  There's no need to calculate for every single point, 
                if there's not much change, just ignore it
             */
            if( fabs(flow.x) < 0.1 && fabs(flow.y) < 0.1 )
                continue;
            matchNum++;
            left_points.push_back(  Point2f( x, y ) );
            right_points.push_back( Point2f( x + flow.x, y + flow.y ) );
        }
    }
    cout << "  matched features " << matchNum << endl;
}

/**
 * Structure from motion from 2 images, using farneback optical flow as the 'features'
 * No, this doesn't work on more than 2 cams, because that requires bundle adjustment, which
 * I'm still searching if there's an OpenCV implementation of it
  
   /home/yan/SfM-Toy-Library-master/SfMToyLib/BundleAdjuster.cpp
 */

Mat sfm( Mat& img1, Mat& img2, Mat& cam_matrix, Mat& dist_coeff,
         func_extract_and_match_feature_points_t extract_and_match_feature_points )
{
    Mat gray1, gray2;
    cvtColor( img1, gray1, CV_BGR2GRAY );
    cvtColor( img2, gray2, CV_BGR2GRAY );

    vector<Point2f> left_points, right_points;
    extract_and_match_feature_points (gray1, gray2, left_points, right_points);

    /* Undistort the points based on intrinsic params and dist coeff */
    undistortPoints( left_points, left_points, cam_matrix, dist_coeff );
    undistortPoints( right_points, right_points, cam_matrix, dist_coeff );

    /* Try to find essential matrix from the points */
    Mat fundamental = findFundamentalMat( left_points, right_points, FM_RANSAC, 3.0, 0.99 );
    Mat essential   = cam_matrix.t() * fundamental * cam_matrix;

    /* Find the projection matrix between those two images */
    SVD svd( essential );
    static const Mat W = (Mat_<double>(3, 3) <<
                             0, -1, 0,
                             1, 0, 0,
                             0, 0, 1);

    static const Mat W_inv = W.inv();

    Mat_<double> R1 = svd.u * W * svd.vt;
    Mat_<double> T1 = svd.u.col( 2 );

    Mat_<double> R2 = svd.u * W_inv * svd.vt;
    Mat_<double> T2 = svd.u.col( 2 );

    static const Mat P1 = Mat::eye(3, 4, CV_64FC1 );
    Mat P2 =( Mat_<double>(3, 4) <<
             R1(0, 0), R1(0, 1), R1(0, 2), T1(0),
             R1(1, 0), R1(1, 1), R1(1, 2), T1(1),
             R1(2, 0), R1(2, 1), R1(2, 2), T1(2));

    Mat Pj1 =( Mat_<double>(3, 4) <<
              R2(0, 0), R2(0, 1), R2(0, 2), T2(0),
              R2(1, 0), R2(1, 1), R2(1, 2), T2(1),
              R2(2, 0), R2(2, 1), R2(2, 2), T2(2));
    Mat Pj2 =( Mat_<double>(3, 4) <<
              R1(0, 0), R1(0, 1), R1(0, 2), T1(0),
              R1(1, 0), R1(1, 1), R1(1, 2), T1(1),
              R1(2, 0), R1(2, 1), R1(2, 2), T1(2));
    cout << "***Projection Matrix Pj1 =" << endl << Pj1 << endl;
    cout << "***Projection Matrix Pj2 =" << endl << Pj2 << endl;

    /*  Triangulate the points to find the 3D homogenous points in the world space
        Note that each column of the 'out' matrix corresponds to the 3d homogenous point
     */
    Mat out;
    triangulatePoints( P1, P2, left_points, right_points, out );

    /* Since it's homogenous (x, y, z, w) coord, divide by w to get (x, y, z, 1) */
    vector<Mat> splitted;
    splitted.push_back(out.row(0) / out.row(3));
    splitted.push_back(out.row(1) / out.row(3));
    splitted.push_back(out.row(2) / out.row(3));

    merge( splitted, out );

    return out;
}

int main(int argc, const char * argv[])
{
    char filename[64];
    if (argc < 3)
        return 0;

    snprintf(filename, 64, "%s.jpg", argv[1]);
    Mat img1 = imread( filename );
    snprintf(filename, 64, "%s.jpg", argv[2]);
    Mat img2 = imread( filename );

    Mat cam_matrix = (Mat_<double>(3, 3) <<
                      6624.070862,           0, 1008.853968,
                      0, 6624.995786, 1132.158299,
                      0,           0, 1);

    Mat dist_coeff = (Mat_<double>(1, 5) << -0.159685, 0.037437, -0.000708, -0.000551, 0.000000);

    /* do SfM */
    Mat out = sfm( img1, img2, cam_matrix, dist_coeff, extract_and_match_feature_points_OpticalFlow );

    /* Convert the matrix of homogenous coords to Vec3f representation */
    vector<Vec3f> points;
    for ( int i = 0; i < out.cols; i++ ) {
        Mat m = out.col( i );
        float x = m.at<float>(0);
        float y = m.at<float>(1);
        float z = m.at<float>(2);
        
        points.push_back( Vec3f(x, y, z) );
    }

#if 1
    /*  This is a silly hack to shift the image to the origin coord (0, 0, 0) 
        by applying K-mean cluster (in this case, 1 cluster), to get the cluster center ...
     */
    Mat labels, center;
    kmeans( out.t(), 1, labels, TermCriteria( CV_TERMCRIT_ITER, 1000, 1e-5), 1, KMEANS_RANDOM_CENTERS, center );

    /*  ... and shift all the points based on the cluster center */
    for( size_t i = 0; i < points.size(); i++ ) {
        points[i][0] -= center.at<float>(0, 0);
        points[i][1] -= center.at<float>(0, 1);
        points[i][2] -= center.at<float>(0, 2);
    }
#endif

    /* output */
    cout << points.size() << " points in point cloud" << endl;
    fstream fout;
    snprintf(filename, 64, "%s_%s.xyz", argv[1], argv[2]);
    fout.open(filename, fstream::out);
    for( size_t i = 0; i < points.size(); i++ ) {
        fout << points[i][0] << " " << points[i][1] << " " << points[i][2] << endl;
    }
    fout.close();
    cout << filename << " saved." << endl;

#if USE_SimpleViewer
    SimpleViewer viewer;
    if( viewer.init( "Simple Point Cloud Viewer", 1066, 600 ) ) {
        /* Pass the coordinates to our simple viewer */
        viewer.setVertexData( points );
        viewer.run();
    }
#endif
    return 0;
}

