#ifndef LCC_CV_COMMON_TYPE_H
#define LCC_CV_COMMON_TYPE_H
#include <iostream> 
#include <cstring>
#include <memory> 

namespace lcc_cv {
const float PI = 3.1415926;
template<class T>
class Image {
 public:
  Image(int height, int width, int channel);
  ~Image() {
    delete []data_;
  }
  T GetData(int row, int col, int channel);
  bool SetData(int row, int col, int channel, T value);
  inline int GetHeight() {
    return height_;  
  }
  inline int GetWidth() {
    return width_;  
  }
  inline int GetChannel() {
    return channel_;  
  }
  inline int GetSize() {
    return size_;  
  }
  Image<T> GetBlock(int left_up_row,
                    int left_up_col,
                    int right_down_row,
                    int right_down_col);
  bool SetBlock(int left_up_row,
                int left_up_col,
                int right_down_row,
                int right_down_col,
                Image<T> block);
  bool CloneBlock(int left_up_row,
                  int left_up_col,
                  int right_down_row,
                  int right_down_col,
                  std::shared_ptr<Image<T> > other_image);
 private:
  int size_;
  int height_;
  int width_;
  int channel_;
  T* data_;
};

template<class T>
Image<T>::Image(int height, int width, int channel) {
  height_ = height;  
  width_ = width;
  channel_ = channel;
  size_ = height * width * channel;
  data_ = new T[size_];  
  memset(data_, 0, size_ * sizeof(T));
}

template<class T>
bool Image<T>::SetData(int row, int col, int channel, T value) {
  if (row < 0 || row > height_
      || col < 0 || col > width_
      || channel < 0 || channel > channel_) {
    std::cout << "exceed the region" << std::endl;
    return false;
  } else {
    data_[width_ * (channel * height_ + row) + col] = value;
  }
}

template<class T>
T Image<T>::GetData(int row, int col, int channel) {
  if (row < 0 || row > height_
      || col < 0 || col > width_
      || channel < 0 || channel > channel_) {
    std::cout << "exceed the region" << std::endl;
    return 0;
  } else {
    return data_[width_ * (channel * height_ + row) + col];
  }
}

template<class T>
Image<T> Image<T>::GetBlock(int left_up_row,
                            int left_up_col,
                            int right_down_row,
                            int right_down_col) {
  int height = right_down_row - left_up_row;
  int width = right_down_col - left_up_col;
  Image<T> block(height, width, channel_);
  for (int irow = 0; irow < height; ++irow) {
    for (int icol = 0; icol < width; ++icol) {
      for (int ichan = 0; ichan < channel_; ++ichan) {
        int data_row = irow + left_up_row;
        int data_col = icol + left_up_col;
        block.SetData(irow, icol, ichan, 
                      this -> GetData(data_row, data_col, ichan));
      }
    }
  }
  return block;
}

template<class T>
bool Image<T>::SetBlock(int left_up_row,
                        int left_up_col,
                        int right_down_row,
                        int right_down_col,
                        Image<T> block) {
  int height = right_down_row - left_up_row ;
  int width = right_down_col - left_up_col;
  if (height != block.GetHeight() 
      || width != block.GetWidth()
      || channel_ != block.GetChannel) {
    std::cout << "region doesn't match" << std::endl;
    return false;
  } else {
    for (int irow = 0; irow < height; ++irow) {
      for (int icol = 0; icol < width; ++icol) {
        for (int ichan = 0; ichan < channel_; ++ichan) {
          int data_row = irow + left_up_row;
          int data_col = icol + left_up_col;
          this -> SetData(data_row, data_col, ichan,
                          block.GetData(irow, icol, ichan));
        }
      }
    }
    return true; 
  }
}

template<class T>
bool Image<T>::CloneBlock(int left_up_row,
                         int left_up_col,
                         int right_down_row,
                         int right_down_col,
                         std::shared_ptr<Image<T> > other_image) {
  if (left_up_row >= 0 && left_up_row <= right_down_row 
      && right_down_row <= height_ && left_up_col >= 0
      && left_up_col <= right_down_col && right_down_col <= width_) {
    for (int irow = left_up_row; irow < right_down_row; ++irow) {
      for (int icol = left_up_col; icol < right_down_col; ++icol) {
        for (int ichan = 0; ichan < channel_; ++ichan) {
          other_image -> SetData(irow, icol, ichan, 
                                 this -> GetData(irow, icol, ichan));
        }
      }
    }
    return true;
  } else {
    return false;
  }
}

typedef Image<float> ImageFloat;
typedef Image<int> ImageInt;
typedef Image<unsigned char> ImageByte;

} // namespace lcc_cv






#endif // LCC_CV_COMMON_TYPE_H 

