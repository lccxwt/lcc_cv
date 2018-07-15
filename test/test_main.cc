#include <iostream>
#include "common/type.h"
#include "common/tools.h"
#include "filter/filter.h"
#include "opencv2/opencv.hpp"

int main() {
  int width = 11;
  int height = 10;
  int channel = 3;
  lcc_cv::Image<float> image(height, width, channel);
  for (int i = 0; i < 10; ++i) {
    image.SetData(i, i + 1, 0, 3);
    image.SetData(i, i + 1, 1, 4);
    image.SetData(i, i + 1, 2, 5);
  }
  for (int cha = 0; cha < channel; ++cha) {
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        std::cout << (int)image.GetData(row, col, cha) << ", ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  cv::Mat input_image = cv::imread("/home/lichongchong/Awork/lcc_cv/data/koala.jpeg",
                        CV_LOAD_IMAGE_UNCHANGED);
  if (input_image.empty()) {
    std::cout << "wrong load image" << std::endl;
    return 0;
  }
  int channels = input_image.channels();
  int rows = input_image.rows;
  int cols = input_image.cols;
  std::cout << channels << ", " << rows << ", " << cols << std::endl;
  cv::Vec3b pix = input_image.ptr<cv::Vec3b>(rows - 5)[cols - 5];
  int blue = pix[0];

  std::shared_ptr<lcc_cv::ImageByte> image_byte(new
                                     lcc_cv::ImageByte(input_image.rows,
                                                       input_image.cols,
                                                       input_image.channels())); 
  lcc_cv::CvMat2Image(input_image, image_byte);
  std::shared_ptr<lcc_cv::ImageByte> image_byte_filter(new
                                     lcc_cv::ImageByte(input_image.rows,
                                                       input_image.cols,
                                                       input_image.channels())); 
  lcc_cv::FilterOptions filter_options;
  filter_options.kernel_size_ = 30;
  filter_options.sigma_ = 5.8;
  
  //lcc_cv::MeanFilter mean_filter;
  //mean_filter.Init(filter_options);
  //mean_filter.Process(image_byte, image_byte_filter);
  lcc_cv::GaussFilter gauss_filter;
  gauss_filter.Init(filter_options);
  gauss_filter.Process(image_byte, image_byte_filter);


  std::shared_ptr<cv::Mat> out_image(new cv::Mat(image_byte->GetHeight(),
                                                 image_byte->GetWidth(),
                                                 CV_8UC3));
  lcc_cv::Image2CvMat(image_byte_filter, out_image);
  cv::namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
  cv::imshow("MyWindow", *out_image);
  cv::waitKey(0);
  cv::destroyWindow("MyWindow");
  return 1;
}
