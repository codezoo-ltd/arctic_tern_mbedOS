#ifndef __MAIN_H
#define __MAIN_H 

#include "mbed.h"
#include "rtos.h"
#include "mbed_events.h"

#include "mbed-trace/mbed_trace.h"  // trace header file

#include "sensor_hub.h"

#include "GPS.h"
#include "MDM.h"

//------------------------------------------------------------------------------------
//! Set your secret SIM pin here (e.g. "1234"). Check your SIM manual.
#define SIMPIN      NULL
/*! The APN of your network operator SIM, sometimes it is "internet" check your 
    contract with the network operator. You can also try to look-up your settings in 
    google: https://www.google.de/search?q=APN+list */
#define APN         "web.sktelecom.com"
//! Set the user name for your APN, or NULL if not needed
#define USERNAME    NULL
//! Set the password for your APN, or NULL if not needed
#define PASSWORD    NULL 
//------------------------------------------------------------------------------------

#endif /* __MAIN_H */

