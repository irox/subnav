/* Copyright 2008 Ian Roxborough */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/io.h>
#include <termios.h>
#include <time.h>
#include "os5500p.h"
/*#include "spatial.h"*/

int printOS5500Data(struct os5500_data *data) {

  printf("C:%f\nP:%f\nR:%f\nM:%6.2f\tMx:%6.2f\tMy:%6.2f\tMz:%6.2f\nA:%6.2f\tAx:%6.2f\tAy:%6.2f\tAz:%6.2f\nG:%6.2f\tGx:%6.2f\tGy:%6.2f\tGz:%6.2f\n",
     data->compass,
     data->pitch,
     data->roll,
     data->mag,
     data->mag_x,
     data->mag_y,
     data->mag_z,
     data->accel,
     data->accel_x,
     data->accel_y,
     data->accel_z,
     data->g,
     data->g_x,
     data->g_y,
     data->g_z);

}

/**
 *  Take a pointer to a string and a pointer to a float
 *  and populates the float with whatever strtof returns,
 *  then returns number of characters in the token.
 */
int getNextFloatToken(char * floatstr, double *number) {
  int index = 0;
  char *endnum = 0;

  //printf("first float char@%d is: %c\n", index, floatstr[index]);
  // Get point to the end of the float;
  for(;floatstr[index]>='0' &&
       floatstr[index]<='9' ||
       floatstr[index]=='.' ||
       floatstr[index]=='-' ||
       floatstr[index]=='+';
       index++) ; //printf("Next float char@%d: %c\n", index, floatstr[index]);

  endnum = floatstr + index;
  //printf("Last float char is : %c\n", floatstr[index - 1]);
  //printf("Left = %s\n", endnum);
 
  // Since I'm using strtod I could have done this with a lot
  // less code.  Oh well.  TODO: clean it up, make it faster. 
  (*number) = strtod(floatstr,NULL);

  return index; 
 }

int parseOS5500Line(char * input, struct os5500_data *data) {

  int index = 0;
  char *startnum;
  char *endnum;
 /// printf("String in: %s\n", input);

  // TODO: Generate time stamp.

  // Find start of sentence marker '$'
  for(;input[index]!=0 && input[index]!='$';index++) {
    if (input[index] == 0) {
      //return for more data.
      return -1;
    }
  }
  struct timespec t;

  clock_gettime(CLOCK_REALTIME, &t);
  data->timestamp = t.tv_nsec;

  index++;
  double dummyfloat;
  while(input[index] != '*') {
    //printf("next char@%d is: %c\n",index, input[index]); 
    // Find non-numeric character.
    switch(input[index]) {
      case 'C':
        index++;
        index = index + getNextFloatToken(input + index, &(data->compass));
        break;
      case 'P':
        index++;
        index = index + getNextFloatToken(input + index, &(data->pitch));
        break;
      case 'D':
        index++;
        index = index + getNextFloatToken(input + index, &dummyfloat);
        break;
      case 'R':
        index++;
        index = index + getNextFloatToken(input + index, &(data->roll));
        break;
      case 'T':
        index++;
        index = index + getNextFloatToken(input + index, &(data->temp));
        break;
      case 'M':
        index++;
      //  printf("Next mag char is %c\n",input[index]);
        switch(input[index]) {
          case 'x':
            index++;
            index = index + getNextFloatToken(input + index, &(data->mag_x));
            break;
          case 'y':
            index++;
            index = index + getNextFloatToken(input + index, &(data->mag_y));
            break;
          case 'z':
            index++;
            index = index + getNextFloatToken(input + index, &(data->mag_z));
            break;
          default:
            index = index + getNextFloatToken(input + index, &(data->mag));
        }
        break;
      case 'G':
        index++;
        switch(input[index]) {
          case 'x':
            index++;
            index = index + getNextFloatToken(input + index, &(data->g_x));
            break;
          case 'y':
            index++;
            index = index + getNextFloatToken(input + index, &(data->g_y));
            break;
          case 'z':
            index++;
            index = index + getNextFloatToken(input + index, &(data->g_z));
            break;
            default:
          index = index + getNextFloatToken(input + index, &(data->g));
        }
        break;
      case 'A':
        index++;
        switch(input[index]) {
          case 'x':
            index++;
            index = index + getNextFloatToken(input + index, &(data->accel_x));
            break;
          case 'y':
            index++;
            index = index + getNextFloatToken(input + index, &(data->accel_y));
            break;
          case 'z':
            index++;
            index = index + getNextFloatToken(input + index, &(data->accel_z));
            break;
            default:
          index = index + getNextFloatToken(input + index, &(data->accel));
        }
        break;
      case '$':
        // TODO: check checksum!
        return 0;
        break;
      default:
        printf("Unexpect char@%d <%c> while parsing\n",index,input[index]);
        exit(1);
    }
  }
 // Find second non-numeric character if any.
 // Find float.
 // Repeat until '*'
 //
// printf("String out: %s\n", input);

 return 0;

}

int openOS5500(char * filename, int baud) {
  int fd;
  struct termios newtio;

  fd = open(filename,O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return fd;
    }

    // flush serial port
    int status = tcflush(fd, TCIFLUSH);
    if (status < 0) {
      perror("tcflush");
      close(fd);
      return -1;
    }
                                            
    /* get current terminal state */
    tcgetattr(fd,&newtio);

    // set to raw terminal type
    newtio.c_cflag = baud | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNBRK;
     newtio.c_oflag = 0;    
    //                                                                         
    //                                                                             // control parameters
    newtio.c_cc[VMIN]     = 1;   // block for at least one charater 
  // set its new attrigutes
  status = tcsetattr(fd,TCSANOW,&newtio);
  if (status < 0) {
    perror("tcsetattr");
    close(fd);
    fd = -1;
  }

/*  logfd = open("/tmp/os5500.log",O_RDWR | O_NOCTTY);
  if (logfd < 0) {
    perror("open");
        return logfd;
    }
*/

  return fd;
}

int openOS5500file(char * filename) {
  int fd;
  fd = open(filename,O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return fd;
    }

  return fd;
}

int readOS5500Line(int fd, char *buffer,int buffSize) {

  char * bufPtr;
  int readSize = 0;

  // Try and get a complete sentense to parse.
  for (bufPtr=buffer; 
       bufPtr[0] != '$' && bufPtr != buffer + buffSize;
       )  {
    while( read(fd,bufPtr,1) == 0) {};
  }


  for (;
       bufPtr[0] != '*' && bufPtr != buffer + buffSize;
       ) {
    bufPtr++;
    while( read(fd,bufPtr,1) == 0){} ;
  }

  // read checksum
  bufPtr++;
  read(fd,bufPtr++,1);
  read(fd,bufPtr++,1);
  read(fd,bufPtr++,1);
  bufPtr = '\0';

}

static int fd = -1;
static int logfd = -1;

void os5500Init(char *name) {

  if (name == NULL) {
    fd = openOS5500("/dev/ttyUSB0",B19200);
  } else {
    fd = openOS5500file(name);
  }

  /* TODO: Make it "not work" rather than exit. */
  if (fd == -1) {
    printf("Error openning device\n");
    exit(1);
  }

}


void os5500GetABCnXYZ(float hpr[3], float xyz[3]) {
    int i;
    char buffer[255];
    struct os5500_data data;
    char *bufPtr = buffer;

    for(i=0;i<201;buffer[i++]=0);

    readOS5500Line(fd, buffer, 200);

    parseOS5500Line(buffer,&data);


    hpr[0] = data.compass;
    hpr[1] = data.pitch;
    hpr[2] = data.roll;

  xyz[0] = data.accel_x;
  xyz[1] = data.accel_y;
  xyz[2] = data.accel_z;

}


void os5500GetABC(float hpr[3]) {
    // Clear buffer.
    int i;
    char buffer[255];
    struct os5500_data data;
    char *bufPtr = buffer;  

    for(i=0;i<201;buffer[i++]=0);
    
    readOS5500Line(fd, buffer, 200);
    //printf("Read: <%s>\n",buffer);
    parseOS5500Line(buffer,&data);
//    printOS5500Data(&data);

    hpr[0] = data.compass;
    hpr[1] = data.pitch;
    hpr[2] = data.roll; 

}

void os5500GetXYZ(float hpr[3]) {
  int i;
  char buffer[255];
  struct os5500_data data;
  char *bufPtr = buffer;

  for(i=0;i<201;buffer[i++]=0);
  readOS5500Line(fd, buffer, 200);
 // printf("Read: <%s>\n",buffer);
  parseOS5500Line(buffer,&data);
//  printOS5500Data(&data);

  hpr[0] = data.accel_x;
  hpr[1] = data.accel_y;
  hpr[2] = data.accel_z;

}
/*
 int main() {

  double running_x = 0.0;
  double running_y = 0.0;
  double running_z = 0.0;

  double dist_x = 0.0;
  double dist_y = 0.0;
  double dist_z = 0.0;

  double velocity_x = 0.0;
  double velocity_y = 0.0;
  double velocity_z = 0.0;

  double accel_x = 0.0;
  double accel_y = 0.0;
  double accel_z = 0.0;


  long last_t = 0;
  struct timespec t;

  clock_gettime(CLOCK_REALTIME, &t);
  last_t = t.tv_nsec;


  char buffer[255];
  struct os5500_data data;
  char *bufPtr = buffer;  
// open test data file.
  int fd = -1;
//  fd = open("c_sample.dat",O_RDONLY);
  fd = openOS5500("/dev/ttyUSB2",B19200);

  if (fd == -1) {
    printf("Error openning device\n");
    exit(1);
  }

  while(1) {
    // Clear buffer.
    int i;
    for(i=0;i<201;buffer[i++]=0);
    readOS5500Line(fd, buffer, 200);
    printf("Read: <%s>\n",buffer);
    parseOS5500Line(buffer,&data);

    printOS5500Data(&data);

    double delta_t = (data.timestamp - last_t)/1000000000.0;
    if (delta_t > 3.0) {
      delta_t = delta_t - 3.3;
    }

    dist_x = velocity_x * delta_t + 0.5 * data.accel_x * 10 * delta_t * delta_t;
    dist_y = velocity_y * delta_t + 0.5 * data.accel_y * 10 * delta_t * delta_t;
    dist_z = velocity_z * delta_t + 0.5 * data.accel_z * 10 * delta_t * delta_t;

    velocity_x = velocity_x + data.accel_x * 10.0 * delta_t;
    velocity_y = velocity_y + data.accel_y * 10.0 * delta_t;
    velocity_z = velocity_z + data.accel_z * 10.0 * delta_t;

    accel_x = getXFromSpherical(data.accel_x, data.compass, data.pitch);
    accel_y = getYFromSpherical(data.accel_y, data.compass, data.roll - 180.0);
    accel_z = getZFromSpherical(data.accel_z, data.compass, data.roll - 180.0);

    running_x = running_x + getXFromSpherical(dist_x, data.compass, data.roll);
    running_y = running_y + getYFromSpherical(dist_y, data.compass, data.roll);
    running_z = running_z + getZFromSpherical(dist_z, data.compass, data.roll);

  //  dist_x = dist_x + 0.5 * data.accel_x * 10.0 * delta_t * delta_t;
    printf("Running x = %f (%f, %f)  y = %f  z = %f  at time %f\n", running_x, dist_x, velocity_x, running_y, running_z, delta_t);
    last_t = data.timestamp;
    printf("Adjusted x y z acceleration: %f  %f  %f\n", accel_x, accel_y, accel_z);
  }
}
*/
