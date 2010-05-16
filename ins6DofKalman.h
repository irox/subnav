
#include "kalman/ekfilter.hpp"


class SixDofEKF : public Kalman::EKFilter<double,1,false,true,false> {
public:
        SixDofEKF();

protected:
        void makeBaseA();
        void makeBaseH();
        void makeBaseV();
        void makeBaseR();
        void makeBaseW();
        void makeBaseQ();

        void makeA();
        void makeH();
        void makeQ();
        void makeProcess();
        void makeMeasure();

  double period;
  int count;
};

typedef SixDofEKF::Vector Vector;
typedef SixDofEKF::Matrix Matrix;


