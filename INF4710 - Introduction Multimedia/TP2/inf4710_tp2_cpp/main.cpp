
// INF4710 A2016 TP2 v1

#include "tp2.h"

// one function per header to simplify grading (bad habit though)
#include "conv_rgb2ycbcr.h"
#include "conv_ycbcr2rgb.h"
#include "dct.h"
#include "dct_inv.h"
#include "decoup.h"
#include "decoup_inv.h"
#include "huff.h"
#include "huff_inv.h"
#include "quantif.h"
#include "quantif_inv.h"
#include "zigzag.h"
#include "zigzag_inv.h"
#include <opencv\highgui.h>

#define USE_SUBSAMPLING 1
#define USE_QUANT_QUALITY 10

int main(int /*argc*/, char** /*argv*/) {

	std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
	std::cout.precision(2);

    try {
        const std::vector<std::string> vsTestImagePaths = {
            {"data/airplane.png"},
            {"data/baboon.png"},
			{ "data/cameraman.tif" },
			{ "data/lena.png" },
            {"data/logo.tif"},
			{ "data/logo_noise.tif" },
            {"data/peppers.png"},
        };
        for(const std::string& sTestImagePath : vsTestImagePaths) {
            const cv::Mat oInput = cv::imread(sTestImagePath);
            if(oInput.empty() || oInput.type()!=CV_8UC3)
                CV_Error_(-1,("Could not load image at '%s', check local paths",sTestImagePath.c_str()));

			std::cout << "\n ***************************************** \n\n";

            // COMPRESSION
            cv::Mat_<uchar> Y,Cb,Cr;
            conv_rgb2ycbcr(oInput,USE_SUBSAMPLING,Y,Cb,Cr);
            const std::vector<cv::Mat_<uchar>> vBlocks_Y = decoup(Y);
            const std::vector<cv::Mat_<uchar>> vBlocks_Cb = decoup(Cb);
            const std::vector<cv::Mat_<uchar>> vBlocks_Cr = decoup(Cr);

			/* Test de-conversion */
			cv::Mat image_unconvert;
			conv_ycbcr2rgb(Y, Cb, Cr, USE_SUBSAMPLING, image_unconvert);
			cv::Mat diff;
			cv::absdiff(oInput, image_unconvert, diff);


            std::vector<cv::Mat_<uchar>> vBlocks;
            vBlocks.insert(vBlocks.end(),vBlocks_Y.begin(),vBlocks_Y.end());
            vBlocks.insert(vBlocks.end(),vBlocks_Cb.begin(),vBlocks_Cb.end());
            vBlocks.insert(vBlocks.end(),vBlocks_Cr.begin(),vBlocks_Cr.end());
            std::vector<cv::Mat_<float>> vDCTBlocks(vBlocks.size());

			/* Test block - unblock*/
			//const cv::Mat_<uchar> test = decoup_inv(vBlocks, Y.size());

			for (size_t b = 0; b < vBlocks.size(); ++b)
			{
				vDCTBlocks[b] = dct(vBlocks[b]);

				/* Test i_dct*/
				cv::Mat_<uchar> original = vBlocks[b];
				cv::Mat_<float> dct = vDCTBlocks[b];
				cv::Mat_<uchar> inverse = dct_inv(vDCTBlocks[b]);			
			}
			
			// Quantification
            std::vector<cv::Mat_<short>> vQuantifDCTBlocks(vDCTBlocks.size());
            for(size_t b=0; b<vDCTBlocks.size(); ++b)
                vQuantifDCTBlocks[b] = quantif(vDCTBlocks[b],USE_QUANT_QUALITY);

            std::vector<std::array<short,8*8>> vInlinedBlocks(vQuantifDCTBlocks.size());
			for (size_t b = 0; b < vQuantifDCTBlocks.size(); ++b)
			{
				vInlinedBlocks[b] = zigzag(vQuantifDCTBlocks[b]);

				// Test zigzag ...
				/*
				cv::Mat_<short> original = vQuantifDCTBlocks[b];
				std::array<short, 8 * 8> arr = vInlinedBlocks[b];
				cv::Mat_<short> inverse = zigzag_inv(vInlinedBlocks[b]);
				*/
			}
                
            const HuffOutput<short> oCode = huff(vInlinedBlocks);

            // @@@@ TODO: check compression rate here...
			cv::Size s = oInput.size();
			int nbPixel = s.height * s.width;

			// Size in bits

			double size_before = 8 * nbPixel * oInput.channels();
			double size_after_color = 8 * (Y.size().area() + Cb.size().area() + Cr.size().area());
			double size_after_dct = 8 * 8 * 8 * vInlinedBlocks.size();
			double size_after_pipeline= oCode.string.size();

			double compressionRate_after_color = 1 - (size_after_color / size_before);
			double compressionRate_after_dct = 1 - (size_after_dct / size_after_color);
			double compressionRate_afer_pipeline = 1 - (size_after_pipeline / size_before);

			/*
			double compressionRate_after_color = size_before / size_after_color;
			double compressionRate_after_dct = size_after_color / size_after_dct;
			double compressionRate_afer_pipeline = size_before /size_after_pipeline;
			*/

			std::cout << "Images: " << sTestImagePath << "\n";

			std::cout << "Size before color   : " << size_before/ (1000.0 * 8.0) << " ko\n";
			std::cout << "Size after color    : " << size_after_color/ (1000.0 * 8.0) << " ko\n";
			std::cout << "Size after dct      : " << size_after_dct/ (1000.0 * 8.0) << " ko\n";
			std::cout << "Size after pipeline : " << size_after_pipeline/ (1000.0 * 8.0) << " ko\n";
			
			std::cout << "Compression rate couleur seulement   : " << compressionRate_after_color << "%\n";
			std::cout << "Compression rate dct(+q+z) seulement : " << compressionRate_after_dct << "%\n";
			std::cout << "Compression rate fin pipeline        : " << compressionRate_afer_pipeline << "%\n";

            // DECOMPRESSION
            const std::vector<std::array<short,8*8>> vInlinedBlocks_decompr = huff_inv<8*8>(oCode);


			// Comment to test dct
            std::vector<cv::Mat_<short>> vQuantifDCTBlocks_decompr(vInlinedBlocks_decompr.size());
            for(size_t b=0; b<vInlinedBlocks_decompr.size(); ++b)
                vQuantifDCTBlocks_decompr[b] = zigzag_inv(vInlinedBlocks_decompr[b]);

			// Uncomment to test dct
			//std::vector<cv::Mat_<short>> vQuantifDCTBlocks_decompr(vInlinedBlocks.size());
			//for (size_t b = 0; b<vInlinedBlocks.size(); ++b)
			//	vQuantifDCTBlocks_decompr[b] = zigzag_inv(vInlinedBlocks[b]);
			
			// Comment to test dct
            std::vector<cv::Mat_<float>> vDCTBlocks_decompr(vQuantifDCTBlocks_decompr.size());
            for(size_t b=0; b<vQuantifDCTBlocks_decompr.size(); ++b)
                vDCTBlocks_decompr[b] = quantif_inv(vQuantifDCTBlocks_decompr[b],USE_QUANT_QUALITY);

			// Uncomment to test dct
			//std::vector<cv::Mat_<float>> vDCTBlocks_decompr(vQuantifDCTBlocks_decompr.size());
			//for (size_t b = 0; b<vQuantifDCTBlocks_decompr.size(); ++b)
			//	vDCTBlocks_decompr[b] = quantif_inv(vQuantifDCTBlocks_decompr[b], USE_QUANT_QUALITY);

			// Commment to test quantification
            std::vector<cv::Mat_<uchar>> vBlocks_decompr(vDCTBlocks_decompr.size());
			for (size_t b = 0; b<vDCTBlocks_decompr.size(); ++b)
				vBlocks_decompr[b] = dct_inv(vDCTBlocks_decompr[b]);

			// Uncomment to test quantification inverse
			//std::vector<cv::Mat_<uchar>> vBlocks_decompr(vDCTBlocks.size());
   //         for(size_t b=0; b<vDCTBlocks.size(); ++b)
   //             vBlocks_decompr[b] = dct_inv(vDCTBlocks[b]);

            const std::vector<cv::Mat_<uchar>> vBlocks_Y_decompr(vBlocks_decompr.begin(),vBlocks_decompr.begin()+vBlocks_Y.size());
            const std::vector<cv::Mat_<uchar>> vBlocks_Cb_decompr(vBlocks_decompr.begin()+vBlocks_Y.size(),vBlocks_decompr.begin()+vBlocks_Y.size()+vBlocks_Cb.size());
            const std::vector<cv::Mat_<uchar>> vBlocks_Cr_decompr(vBlocks_decompr.begin()+vBlocks_Y.size()+vBlocks_Cb.size(),vBlocks_decompr.end());
            const cv::Mat_<uchar> Y_decompr = decoup_inv(vBlocks_Y_decompr,Y.size());
            const cv::Mat_<uchar> Cb_decompr = decoup_inv(vBlocks_Cb_decompr,Cb.size());
            const cv::Mat_<uchar> Cr_decompr = decoup_inv(vBlocks_Cr_decompr,Cr.size());
            cv::Mat oInput_decompr;
            conv_ycbcr2rgb(Y_decompr,Cb_decompr,Cr_decompr,USE_SUBSAMPLING,oInput_decompr);

            cv::Mat oDisplay;
            cv::hconcat(oInput,oInput_decompr,oDisplay);
            cv::Mat oDiff;
            cv::absdiff(oInput,oInput_decompr,oDiff);
            cv::hconcat(oDisplay,oDiff,oDisplay);
            cv::imshow(sTestImagePath.substr(sTestImagePath.find_last_of("/\\")+1),oDisplay);
            cv::waitKey(1);
        }
        std::cout << "all done; press any key on a window to quit..." << std::endl;
        cv::waitKey(0);
        return 0;
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
    return 1;
}


