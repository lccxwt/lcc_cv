#ifndef LCC_CV_FILTER_FILTER_H
#define LCC_CV_FILTER_FILTER_H
#include "common/type.h"
#include <vector>
#include <cmath>

namespace lcc_cv {

struct FilterOptions {
  int filter_type_;
  int kernel_size_;
  float sigma_;
};
typedef unsigned char Byte;
class Filter {
 public:
  Filter() {}
  ~Filter() {}
  virtual void Init(FilterOptions filter_options) {
    kernel_size_ = filter_options.kernel_size_;
  }
  virtual float KernelConv(const std::shared_ptr<ImageByte >& input_image,
                           int row,
                           int col,
                           int chan) = 0;
  void Process(const std::shared_ptr<ImageByte >& input_image,
               std::shared_ptr<ImageByte > filtered_image);
 protected:
  void BoundaryProcess(const std::shared_ptr<ImageByte >& input_image,
                       std::shared_ptr<ImageByte > filtered_image); 
  int kernel_size_;
};

void Filter::BoundaryProcess(const std::shared_ptr<ImageByte >& input_image,
                             std::shared_ptr<ImageByte > filtered_image) {
  int k = (kernel_size_ - 1) / 2;
  int left_up_row = 0;
  int left_up_col = 0;
  int right_down_row = k;
  int right_down_col = input_image -> GetWidth();
  input_image -> CloneBlock(left_up_row,
                            left_up_col,
                            right_down_row,
                            right_down_col,
                            filtered_image);

  left_up_row = input_image -> GetHeight() - k;
  left_up_col = 0;
  right_down_row = input_image -> GetHeight();
  right_down_col = input_image -> GetWidth();
  input_image -> CloneBlock(left_up_row,
                            left_up_col,
                            right_down_row,
                            right_down_col,
                            filtered_image);
  left_up_row = k;
  left_up_col = 0;
  right_down_row = input_image -> GetHeight() - k;
  right_down_col = k;
  input_image -> CloneBlock(left_up_row,
                            left_up_col,
                            right_down_row,
                            right_down_col,
                            filtered_image);
  left_up_row = k;
  left_up_col = input_image -> GetWidth() - k;
  right_down_row = input_image -> GetHeight() - k;
  right_down_col = input_image -> GetWidth();
  input_image -> CloneBlock(left_up_row,
                            left_up_col,
                            right_down_row,
                            right_down_col,
                            filtered_image);
                             
}

void Filter::Process(const std::shared_ptr<ImageByte>& input_image,
                         std::shared_ptr<ImageByte> filtered_image) {
  int k = (kernel_size_ - 1) / 2;
  BoundaryProcess(input_image, filtered_image);
  for (int irow = k; irow < input_image -> GetHeight() - k; ++irow) {
    for (int icol = k; icol < input_image -> GetWidth() - k; ++icol) {
      for (int ichan =0; ichan < input_image -> GetChannel(); ++ichan) {
        float conv_result = KernelConv(input_image, irow, icol, ichan);
        filtered_image -> SetData(irow, icol, ichan, static_cast<Byte>(conv_result));
      }
    }
  }
}

class MeanFilter : public Filter {
 public:
  MeanFilter() {}
  ~MeanFilter() {}
  float KernelConv(const std::shared_ptr<ImageByte >& input_image,
                   int row,
                   int col,
                   int chan);
 private:
};

float MeanFilter::KernelConv(const std::shared_ptr<ImageByte >& input_image,
                            int row,
                            int col,
                            int chan) {
  int k = (kernel_size_ - 1) / 2;
  float* row_sum =new float[kernel_size_];
  for (int irow = row - k; irow <= row + k; ++irow) {
    float sum = 0.0; 
    for (int icol = col - k; icol <= col + k; ++icol) {
      sum += input_image -> GetData(irow, icol, chan);  
    }
    row_sum[irow - row + k] = sum;
  }
  float sum = 0.0;
  for (int irow = 0; irow < kernel_size_; ++irow) {
    sum += row_sum[irow];
  }
  sum /= (kernel_size_ * kernel_size_);
  delete []row_sum;
  return sum;
}


class GaussFilter : public Filter {
 public:
  GaussFilter() {}
  ~GaussFilter() {}
  void Init(FilterOptions filter_options); 
  float KernelConv(const std::shared_ptr<ImageByte >& input_image,
                   int row,
                   int col,
                   int chan);
 private:
  float coff_;
  std::vector<float> row_kernel_;
};

void GaussFilter::Init(FilterOptions filter_options) {
  kernel_size_ = filter_options.kernel_size_;
  float sigma = filter_options.sigma_;
  row_kernel_.resize(kernel_size_);
  int k = (kernel_size_ - 1) / 2;
  for (int icol = -k; icol <= k; ++icol) {
    float temp = icol / sigma;
    row_kernel_[icol + k] = exp(-temp * temp / 2);
  }
  coff_ = 2 * PI * sigma * sigma;
}

float GaussFilter::KernelConv(const std::shared_ptr<ImageByte >& input_image,
                            int row,
                            int col,
                            int chan) {
  int k = (kernel_size_ - 1) / 2;
  float* row_sum =new float[kernel_size_];
  for (int irow = row - k; irow <= row + k; ++irow) {
    float sum = 0.0; 
    for (int icol = col - k; icol <= col + k; ++icol) {
      sum += (input_image -> GetData(irow, icol, chan) * row_kernel_[icol - col + k]);  
    }
    row_sum[irow - row + k] = sum;
  }
  float sum = 0.0;
  for (int irow = 0; irow < kernel_size_; ++irow) {
    sum += (row_sum[irow] * row_kernel_[irow]);
  }
  sum /= coff_;
  delete []row_sum;
  return sum;
}

}
#endif // LCC_CV_FILTER_FILTER_H
