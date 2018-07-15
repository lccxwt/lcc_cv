#ifndef LCC_CV_COMMON_TOOLS_H
#define LCC_CV_COMMON_TOOLS_H
#include "opencv2/opencv.hpp"
#include "common/type.h" 

namespace lcc_cv {
void CvMat2Image (const cv::Mat& input_image,
                  std::shared_ptr<ImageByte> image) {
  int channels = input_image.channels();
  int rows = input_image.rows;
  int cols = input_image.cols;
  for (int irow = 0; irow < rows; ++irow) {
    for (int icol = 0; icol < cols; ++icol) {
      for (int ichan = 0; ichan < channels; ++ichan) {
        image->SetData(irow, icol, ichan,  
               (unsigned char)(input_image.ptr<cv::Vec3b>(irow)[icol])[ichan]);
      }
    }
  }
}


void Image2CvMat (std::shared_ptr<ImageByte>& input_image,
                  std::shared_ptr<cv::Mat> image) {
  int channels = input_image->GetChannel();
  int rows = input_image->GetHeight();
  int cols = input_image->GetWidth();
  for (int irow = 0; irow < rows; ++irow) {
    for (int icol = 0; icol < cols; ++icol) {
      for (int ichan = 0; ichan < channels; ++ichan) {
        (image->ptr<cv::Vec3b>(irow)[icol])[ichan] = input_image->GetData(
                                                 irow, icol, ichan);
      }
      if (channels < 3) {
        for (int irow = 0; irow < rows; ++irow) {
          for (int icol = 0; icol < cols; ++icol) {
             for (int ichan = channels; ichan < 3; ++ichan) {
               (image->ptr<cv::Vec3b>(irow)[icol])[ichan] = input_image->GetData(
                                                 irow, icol, 0);
             }
          }
        }
      }
    }
  }
}


} // namespace lcc_cv

#endif // LCC_CV_COMMON_TOOLS_H 

