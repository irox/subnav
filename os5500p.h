/* Copyright 2008 Ian Roxborough */

struct os5500_data {
  unsigned long timestamp;
  double compass;
  double pitch;
  double roll;
  double temp;
  double mag;
  double mag_x;
  double mag_y;
  double mag_z;
  double accel;
  double accel_x;
  double accel_y;
  double accel_z;
  double g;
  double g_x;
  double g_y;
  double g_z;
};

#ifdef CPP
extern "C" {
  void os5500GetXYZ(float[3]);
  void os5500GetABC(float[3]);
  void os5500Init();
}
#endif
