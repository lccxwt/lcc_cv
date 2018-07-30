#ifdef LCC_CV_EDGE_EDGE_H_
#define LCC_CV_EDGE_EDGE_H_

#include <math>
#include "common/type.h"

namespace lcc_cv {
typedef unsigned char Byte;

struct BytePair {
  Byte x;
  Byte y;
  Byte ampl;
  float theta;
}

class BaseEdge {
 public:
  BaseEdge() {}
  ~BaseEdge() {}
  virtual void Init() = 0;
  virtual void Process(const std::shared_ptr<ImageByte>& input_image,
                       std::shared_ptr<ImageByte> edge_image) = 0;
  virtual BytePair Pix3Conv(const std::shared_ptr<ImageByte>& input_image,
                            int irow,
                            int icol,
                            int ichan);
 protected:
  int kernel_x_[3][3];
  int kernel_y_[3][3];
};

BytePair BaseEdge::Pix3Conv(const std::shared_ptr<ImageByte>& input_image,
                         int irow,
                         int icol,
                         int ichan) {
  if (irow == 0 || irow == input_image->GetHeight() - 1
      || icol == 0 || icol == input_image->GetWidth() - 1) {
    BytePair grad;
    grad.x = 0; 
    grad.y = 0; 
    grad.theta = 0;
    grad.ampl = 0;
    return grad;
  }

  int sum_x = 0;
  int sum_y = 0;
  for (int krow = irow - 1; krow <= irow + 1; ++krow) {
    for (int kcol = icol - 1; kcol <= icol + 1; ++kcol) {
      sum_x += (input_image->GetData(krow, kcol, ichan)
               * kernel_x_[krow - irow + 1][kcol - icol + 1]); 
      sum_y += (input_image->GetData(krow, kcol, ichan)
               * kernel_y_[krow - irow + 1][kcol - icol + 1]); 
    }
  }
  BytePair grad;
  grad.x = (Byte)sum_x;
  grad.y = (Byte)sum_y;
  grad.theta = atan2(sum_y, sum_x) + PI;
  grad.ampl = grad.x > grad.y? grad.x : grad.y;
  return grad;
}
class SobelEdge : public BaseEdge {
 public:
  SobelEdge() {}
  ~SobelEdge() {}
  void Init();
  void Process(const std::shared_ptr<ImageByte>& input_image,
               std::shared_ptr<ImageByte> edge_image);
 private:
}
void SobelEdge::Init() {
  kernel_x_[0][0] = -1;
  kernel_x_[0][1] = 0;
  kernel_x_[0][2] = 1;
  kernel_x_[1][0] = -2;
  kernel_x_[1][1] = 0;
  kernel_x_[1][2] = 2;
  kernel_x_[2][0] = -1;
  kernel_x_[2][1] = 0;
  kernel_x_[2][2] = 1;
  kernel_y_[0][0] = 1;
  kernel_y_[0][1] = 2;
  kernel_y_[0][2] = 1;
  kernel_y_[1][0] = 0;
  kernel_y_[1][1] = 0;
  kernel_y_[1][2] = 0;
  kernel_y_[2][0] = -1;
  kernel_y_[2][1] = -2;
  kernel_y_[2][2] = -1;
}
void SobelEdge::Process(const std::shared_ptr<ImageByte>& input_image,
                        std::shared_ptr<ImageByte> edge_image) {
  int height = input_image->GetHeight();
  int width = input_image->GetWidth();
  int channel = input_image->GetChannel();
  for (int irow = 0; irow < height; ++irow) {
    for (int icol = 0; icol < width; ++icol) {
      for (int ichan = 0; ichan < channel; ++ichan) {
        BytePair grad =  Pix3Conv(input_image, irow, icol, ichan);
        edge_image->SetData(irow, icol, ichan, grad.ampl);
      }
    }
  } 
}

class CannyEdge : public BaseEdge {
 public:
  CannyEdge() {}
  ~CannyEdge() {}
  void Init();
  void Process(const std::shared_ptr<ImageByte>& input_image,
               std::shared_ptr<ImageByte> edge_image);
 private:
}

void CannyEdge::Init() {
  kernel_x_[0][0] = -1;
  kernel_x_[0][1] = 0;
  kernel_x_[0][2] = 1;
  kernel_x_[1][0] = -2;
  kernel_x_[1][1] = 0;
  kernel_x_[1][2] = 2;
  kernel_x_[2][0] = -1;
  kernel_x_[2][1] = 0;
  kernel_x_[2][2] = 1;
  kernel_y_[0][0] = 1;
  kernel_y_[0][1] = 2;
  kernel_y_[0][2] = 1;
  kernel_y_[1][0] = 0;
  kernel_y_[1][1] = 0;
  kernel_y_[1][2] = 0;
  kernel_y_[2][0] = -1;
  kernel_y_[2][1] = -2;
  kernel_y_[2][2] = -1;
}
void CannyEdge::Process(const std::shared_ptr<ImageByte>& input_image,
                        std::shared_ptr<ImageByte> edge_image) {
  std::shared_ptr<ImageByte> ampl_image;
  std::shared_ptr<ImageByte> ampl_image_2;
  std::shared_ptr<ImageByte> theta_image;
  int height = input_image->GetHeight();
  int width = input_image->GetWidth();
  int channel = input_image->GetChannel();
  for (int irow = 0; irow < height; ++irow) {
    for (int icol = 0; icol < width; ++icol) {
      for (int ichan = 0; ichan < channel; ++ichan) {
        BytePair grad =  Pix3Conv(input_image, irow, icol, ichan);
        ampl_image->SetData(irow, icol, ichan, grad.ampl);
        ampl_image_2->SetData(irow, icol, ichan, grad.ampl);
        theta_image->SetData(irow, icol, ichan, grad.theta);
      }
    }
  } 
  for (int irow = 0; irow < height; ++irow) {
    for (int icol = 0; icol < width; ++icol) {
      for (int ichan = 0; ichan < channel; ++ichan) {
        BytePair grad =  Pix3Conv(input_image, irow, icol, ichan);
        Byte ampl = ampl_image->GetData(irow, icol, ichan);
        float theta = theta_image->GetData(irow, icol, ichan); 
        float ampl1 = -1;
        float ampl2 = -1;
        if ((theta >= 0 && theta < PI / 4)
            || (theta >= PI && theta < 5 * PI / 4 && (theta -= PI))) {
          if (icol != width - 1 && irow != 0) {
            ampl1 = (1 - 4 * theta / PI) * ampl_image->GetData(irow, icol + 1, ichan)
                  + (4 * theta / PI) * ampl_image->GetData(irow - 1, icol + 1, ichan);
          }
          if (icol != 0 && irow != height - 1) {
            ampl2 = (1 - 4 * theta / PI) * ampl_image->GetData(irow, icol - 1, ichan)
                  + (4 * theta / PI) * ampl_image->GetData(irow + 1, icol - 1, ichan);
          }
        } else if ((theta >= PI / 4 && theta < PI / 2 && (theta -= PI / 4))
            || (theta >= 5 * PI / 4 && theta < 3 * PI / 2 && (theta -= 5 * PI / 4))) {
          if (icol != width - 1 && irow != 0) {
            ampl1 = (1 - 4 * theta / PI) * ampl_image->GetData(irow - 1, icol + 1, ichan)
                  + (4 * theta / PI) * ampl_image->GetData(irow - 1, icol, ichan);
          }
          if (icol != 0 && irow != height - 1) {
            ampl2 = (1 - 4 * theta / PI) * ampl_image->GetData(irow + 1, icol - 1, ichan)
                  + (4 * theta / PI) * ampl_image->GetData(irow + 1, icol, ichan);
          }
        } else if ((theta >= PI / 2 && theta < 3 * PI / 4 && (theta -= PI / 2))
            || (theta >= 3 * PI / 2 && theta < 7 * PI / 4 && (theta -= 3 * PI / 2))) {
          if (icol != 0 && irow != 0) {
            ampl1 = (1 - 4 * theta / PI) * ampl_image->GetData(irow - 1, icol, ichan)
                  + (4 * theta / PI) * ampl_image->GetData(irow - 1, icol - 1, ichan);
          }
          if (icol != width - 1 && irow != height - 1) {
            ampl2 = (1 - 4 * theta / PI) * ampl_image->GetData(irow + 1, icol, ichan)
                  + (4 * theta / PI) * ampl_image->GetData(irow + 1, icol + 1, ichan);
          }
        } else if ((theta >= 3 * PI / 4 && theta < PI && (theta -= 3 * PI / 4))
            || (theta >= 7 * PI / 4 && theta < 2 * PI && (theta -= 7 * PI / 4))) {
          if (icol != 0 && irow != 0) {
            ampl1 = (1 - 4 * theta / PI) * ampl_image->GetData(irow - 1, icol - 1, ichan)
                  + (4 * theta / PI) * ampl_image->GetData(irow, icol - 1, ichan);
          }
          if (icol != width - 1 && irow != height - 1) {
            ampl2 = (1 - 4 * theta / PI) * ampl_image->GetData(irow + 1, icol + 1, ichan)
                  + (4 * theta / PI) * ampl_image->GetData(irow, icol + 1, ichan);
          }
        }
        if (ampl < ampl1 || ampl < ampl2) {
          ampl_image_2->SetData(irow, icol, ichan, 0); 
        }
      }
    }
  } 

  BYTE low_th = 20;
  BYTE high_th = 100;
  for (int irow = 0; irow < height; ++irow) {
    for (int icol = 0; icol < width; ++icol) {
      for (int ichan = 0; ichan < channel; ++ichan) {
        if (ampl_image_2->GetData(irow, icol, ichan) > low_th) {
          ampl_image->SetData(irow, icol, ichan, 1);
        } else {
          ampl_image->SetData(irow, icol, ichan, 0);
        }
        if (ampl_image_2->GetData(irow, icol, ichan) > high_th) {
          edge_image->SetData(irow, icol, ichan, 1);
        } else {
          edge_image->SetData(irow, icol, ichan, 0);
        }
      }
    }
  } 

  for (int irow = 0; irow < height; ++irow) {
    for (int icol = 0; icol < width; ++icol) {
      for (int ichan = 0; ichan < channel; ++ichan) {
        if (edge_image->GetData(irow, icol, ichan) == 1) {
          int row_m1 = irow > 0? irow - 1 : irow;
          int row_p1 = irow < height - 1? irow + 1 : irow;
          int col_m1 = icol > 0? icol - 1 : icol;
          int col_p1 = icol < width - 1? icol + 1 : icol;
          edge_image->SetData(row_m1,
                              col_m1,
                              ichan,
                              ampl_image->GetData(row_m1, col_m1, ichan));
          edge_image->SetData(row_m1,
                              icol,
                              ichan,
                              ampl_image->GetData(row_m1, icol, ichan));
          edge_image->SetData(row_m1,
                              col_p1,
                              ichan,
                              ampl_image->GetData(row_m1, col_p1, ichan));
          edge_image->SetData(irow,
                              col_m1,
                              ichan,
                              ampl_image->GetData(irow, col_m1, ichan));
          edge_image->SetData(irow,
                              col_p1,
                              ichan,
                              ampl_image->GetData(irow, col_p1, ichan));
          edge_image->SetData(row_p1,
                              col_m1,
                              ichan,
                              ampl_image->GetData(row_p1, col_m1, ichan));
          edge_image->SetData(row_p1,
                              icol,
                              ichan,
                              ampl_image->GetData(row_p1, icol, ichan));
          edge_image->SetData(row_p1,
                              col_p1,
                              ichan,
                              ampl_image->GetData(row_p1, col_p1, ichan));
        }
      }
    }
  } 


}

}

#endif // LCC_CV_EDGE_EDGE_H_
