/* Copyright Ian Roxborough. */
#ifndef NMLNAV_H
#define NMLNAV_H
#include "rcs.hh"

/* Unique id number */
#define NAVDATA_MSG_TYPE 101

/* Define the new message structure */
struct NAVDATA_MSG: public NMLmsg 
{
  NAVDATA_MSG();

  /* Each new type needs to overload the update function. */
  void update(CMS *cms); 

  /* Data in this new message format. */
  float xAccel;
  float yAccel;
  float zAccel;
  float xVelocity;
  float yVelocity;
  float zVelocity;
  float xDistance;
  float yDistance;
  float zDistance;
  float heading;
  float pitch;
  float roll;
};

/* Declare the NML Format function. */
int ex_format(NMLTYPE type, void *buf, CMS *cms);

extern char *TEST_CFG;

#endif /* End of NMLNAV_H */

