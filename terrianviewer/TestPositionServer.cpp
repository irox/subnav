#include <iostream>
#include <stdio.h>
#include "rcs.hh"
#include "nmlPosition.h"

main() {
  NML positiondata_nml(ex_format, "position_buf1", "TestPositionServer", "../config.nml");
  nml_start();

  float lat = 37.5;
  float lng = -122.5;
  float lat2 = 38.0;
  float lng2 = -122.5;

  int count = 0;
  while(1) {
    count++;
    if (count % 2 ==1) {
      POSITIONDATA_MSG pos_msg;
      pos_msg.lattitude = lat;
      pos_msg.longitude = lng;
      sprintf(pos_msg.name, "test_vessel");
     // lat = lat + 0.001
      lng = lng - 0.001;
      positiondata_nml.write(pos_msg);
    } else {

      POSITIONDATA_MSG pos_msg2;
      lng2 = lng2 - 0.001;
      pos_msg2.lattitude = lat2;
      pos_msg2.longitude = lng2;
      sprintf(pos_msg2.name, "test_vessel2");
      positiondata_nml.write(pos_msg2);
    }
    std::cout << "Updated position." << std::endl;
    sleep(1);
  }

  nml_cleanup();
}

