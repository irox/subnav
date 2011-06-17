#include <iostream>
#include "rcs.hh"
#include "nmlPosition.h"

main() {
  NML positiondata_nml(ex_format, "position_buf1", "TestPositionServer", "../config.nml");
  nml_start();

  float lat = 37.5;
  float lng = -122.5;
  while(1) {
    POSITIONDATA_MSG pos_msg;
    pos_msg.lattitude = lat;
    pos_msg.longitude = lng;
   // lat = lat + 0.001
    lng = lng - 0.001;
    positiondata_nml.write(pos_msg);
    std::cout << "Updated position." << std::endl;
    sleep(1);
  }

  nml_cleanup();
}

