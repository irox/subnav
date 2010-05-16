
#include "kalman/ekfilter.hpp"


class SingleAxisAccelEKF : public Kalman::EKFilter<double,1,false,true,false> {
public:
        SingleAxisAccelEKF();

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
};

typedef SingleAxisAccelEKF::Vector Vector;
typedef SingleAxisAccelEKF::Matrix Matrix;


