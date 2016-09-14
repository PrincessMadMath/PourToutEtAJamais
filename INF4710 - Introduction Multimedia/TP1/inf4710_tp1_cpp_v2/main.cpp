
#include "tp1.hpp"

using ImagePathFlag = std::pair<std::string,int>; // first = image path, second = cv::imread flag

void ghettoTestEncode();

int main(int /*argc*/, char** /*argv*/) {
    try {
        // note: by default, imread always returns 3-ch images unless the cv::IMREAD_GRAYSCALE flag is set (here we hardcode it based on prior knowledge)
        const std::vector<ImagePathFlag> vsTestImages = {
            {"data/test1.png",cv::IMREAD_GRAYSCALE},
            {"data/test2.png",cv::IMREAD_GRAYSCALE},
            {"data/test3.png",cv::IMREAD_GRAYSCALE},
            {"data/test4.png",cv::IMREAD_COLOR},
            {"data/test5.png",cv::IMREAD_COLOR},
            {"data/test6.png",cv::IMREAD_COLOR},
            {"data/test7.png",cv::IMREAD_COLOR},
            {"data/test8.jpg",cv::IMREAD_COLOR},
            {"data/test9.bmp",cv::IMREAD_COLOR},
            {"data/test10.bmp",cv::IMREAD_COLOR},
        };
        for(const ImagePathFlag& oImagePathFlag : vsTestImages) {
            cv::Mat oInputImg = cv::imread(oImagePathFlag.first,oImagePathFlag.second);
            if(oInputImg.empty())
                CV_Error_(-1,("Could not load image at '%s', check local paths",oImagePathFlag.first.c_str()));

			
            // ... @@@@ TODO (make sure decoding also provides the original image!)
            
        }
		ghettoTestEncode();
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

void ghettoTestEncode()
{
	// Vecteur a tester
	static const int arr[] = { 0, 0, 1, 0, 1, 0, 2, 1, 0, 2, 1, 0, 2, 1, 2, 4, 0, 1, 0, 1, 0, 2, 1, 0, 0};
	std::vector<uint8_t> vSignal(arr, arr + sizeof(arr) / sizeof(arr[0]));

	// Slide property
	size_t n1 = 9;
	size_t N = 18;

	std::vector<LZ77Code> encodeSignal = lz77_encode(vSignal, N, n1);

	std::cout << "************** Encoded signal ************** \n";
	for (int index = 0; index < encodeSignal.size(); ++index)
	{
		std::cout << encodeSignal[index].to_string();
	}

	std::cout << "\n\n************** Decoded signal ************** \n";
	// Decode
	std::vector<uint8_t> decodeSignal = lz77_decode(encodeSignal, N, n1);

	for (int index = 0; index < decodeSignal.size(); ++index)
	{
		std::cout << std::to_string(decodeSignal[index]) << ", " ;
	}

}
