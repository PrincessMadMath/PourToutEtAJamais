// INF4710 A2016 TP3 v1.1

#include "utils.hpp"

using namespace std;
using namespace cv;


inline void testHistogramme(cv::Mat src, int N)
{
	// Our shit
	cv::Mat histog = tp3::histo(src, N);

	// Official shit

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Establish the number of bins
	int histSize = N;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	cv::waitKey(1);
}

inline void testConvolution(cv::Mat src)
{
	float filterData[9] = { -10, 0, 10,
						   -10, 0, 10,
						   -10, 0, 10 };
	cv::Mat_<float> kernel = cv::Mat(3, 3, CV_32F, filterData);
	
	cv::Mat convolutionResult = tp3::convo(src, kernel);

	cv::waitKey(1);
}


int main(int /*argc*/, char** /*argv*/) {
    try {
        cv::VideoCapture oCap("../data/TP3_video.avi");
        CV_Assert(oCap.isOpened());
        for(int i=0; i<(int)oCap.get(cv::CAP_PROP_FRAME_COUNT); ++i) {
            cv::Mat oImg;
            oCap >> oImg;
            cv::imshow("oImg",oImg);
            cv::waitKey(1);

			//testHistogramme(oImg, 4);
			testConvolution(oImg);

			std::cout << "Frame!" << "\n";
        }
    }
    catch(const cv::Exception& e) {
        std::cerr << "Caught cv::Exceptions: " << e.what() << std::endl;
    }
    catch(const std::runtime_error& e) {
        std::cerr << "Caught std::runtime_error: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Caught unhandled exception." << std::endl;
    }
    return 0;
}
