/* Copyright Ian Roxborough. */
#ifndef NMLPOSITION_H
#define NMLPOSITION_H
#include "rcs.hh"

/* Unique id number */
#define POSITIONDATA_MSG_TYPE 201

/* Message structure for vehicle position. */
struct POSITIONDATA_MSG: public NMLmsg 
{
  POSITIONDATA_MSG();

  /* Each new type needs to overload the update function. */
  void update(CMS *cms); 

  /* Data in this new message format. */
  float lattitude;
  float longitude;
  float altitude;
  float velocity;
  float heading;
  float pitch;
  float roll;
};

/* Declare the NML Format function. */
int ex_format(NMLTYPE type, void *buf, CMS *cms);

extern char *TEST_CFG;

#endif /* End of NMLPOSITION_H */

