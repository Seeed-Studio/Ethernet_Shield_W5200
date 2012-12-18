*****************************************************************
W5200 Arduino Ethernet Library
******************************************************************
How to use:

1. Install W5200 library
   Overwrite w5100.cpp, w5100.h to the "/libraries/Ethernet/utility" folder in your Arduino IDE. 

2. Using the W5200 library and evaluate existing Ethernet example.
   In the Arduino IDE, go to Files->Examples->Ethernet and open any example, compile and upload the file to Arduino board.

3. Note: libraries/Ethernet/Ethernet.h needs the following at line 10 instead of #define MAX_SOCK_NUM 4, since it doesn't #include "w5100.h" anymore:
   Thanks "Matthew Lange"

#ifdef W5200
#define MAX_SOCK_NUM 8
#else
#define MAX_SOCK_NUM 4
#endif