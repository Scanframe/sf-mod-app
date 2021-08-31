#pragma once

/*

Purpose:
  This file contains all registerd variable id's and can be included
  for any purpose

     usage            bitnrs   combinations      mask
     ------------------------------------------------------
     file             2         4                0xC0000000L
ALL: spare            4        16                0x3C000000L
     server           4        16                0x03C00000L
     device           6        64                0x003F0000L
     ------------------------------------------------------
UT:  spare            1         2                0x00008000L
     channel          5        16                0x00007C00L
     gate nr          4        16                0x000003C0L
     parameters       6        64                0x0000003FL
     ------------------------------------------------------
EC:  spare            1         2                0x00008000L
     channel          5        16                0x00007C00L
     signal nr        2         4                0x00000300L
     parameters       8       256                0x000000FFL


*/

#define VID_FILE_MASK       0xC0000000L
#define VID_SPARE_MASK      0x3C000000L
#define VID_SERVER_MASK     0x03C00000L
#define VID_DEVICE_MASK     0x003F0000L

#define DEVICENR_SYS    0x1L  // System (not used!?)
#define DEVICENR_UT     0x2L  // Ultrasonic Acquisition
#define DEVICENR_EC     0x3L  // Eddy current acquisition
#define DEVICENR_MC     0x4L  // Motion Controller
#define DEVICENR_IPJ    0x5L  // Inspection Project and exported scan mode
#define DEVICENR_ULT    0x6L  // Ultimo 2000 pulser receiver.
#define DEVICENR_FIL    0x7L  // Filters
#define DEVICENR_SCR    0x9L  // Scripts.
#define DEVICENR_STO    0xEL  // Storage Server.
#define DEVICENR_LCU    0xFL  // Local control unit.
#define DEVICENR_WMS    0xAL  // WMS 800.
#define DEVICENR_USPC   0xBL  // USPC 2100.
#define DEVICENR_READ   0xCL  // Old file readers.
#define DEVICENR_COMM   0xDL  // Comment server.
#define DEVICENR_USIP   0xEL  // USIP20.
//------------------------------------------------------------------------------
// Script Specific parameters, the 0x90000 range.
//------------------------------------------------------------------------------
// 0x9F000   ID range for Analysis.
// 0x9E000   ID range for Acquisition.
// 0x99000   ID range for Project parameters.
//------------------------------------------------------------------------------
//
#define GET_FILE(vid)         (((vid) & VID_FILE_MASK) >> 30)
#define MAKE_VID_FILE(file)   ((file) << 30)
//
#define GET_SERVER(vid)       (((vid) & VID_SERVER_MASK) >> 22)
#define MAKE_VID_DEVICE(dev)  ((dev) << 16)
//---------------------------------------------------------------------------
//
//
#define VID_DEVICE_SYS      MAKE_VID_DEVICE(DEVICENR_SYS)
#define VID_DEVICE_UT       MAKE_VID_DEVICE(DEVICENR_UT)
#define VID_DEVICE_EC       MAKE_VID_DEVICE(DEVICENR_EC)
#define VID_DEVICE_MC       MAKE_VID_DEVICE(DEVICENR_MC)
#define VID_DEVICE_IPJ      MAKE_VID_DEVICE(DEVICENR_IPJ)
#define VID_DEVICE_ULT      MAKE_VID_DEVICE(DEVICENR_ULT)
#define VID_DEVICE_LCU      MAKE_VID_DEVICE(DEVICENR_LCU)
#define VID_DEVICE_STO      MAKE_VID_DEVICE(DEVICENR_STO)
#define VID_DEVICE_FIL      MAKE_VID_DEVICE(DEVICENR_FIL)
#define VID_DEVICE_SRC      MAKE_VID_DEVICE(DEVICENR_SCR)
#define VID_DEVICE_WMS      MAKE_VID_DEVICE(DEVICENR_WMS)
#define VID_DEVICE_USPC     MAKE_VID_DEVICE(DEVICENR_USPC)
#define VID_DEVICE_COMM     MAKE_VID_DEVICE(DEVICENR_COMM)
#define VID_DEVICE_USIP     MAKE_VID_DEVICE(DEVICENR_USIP)
//---------------------------------------------------------------------------
//
// Genral macros'
//
// Create a complete variable id from a device and id.
#define MAKE_VID(dev, id) (MAKE_VID_DEVICE(dev) | (id))
// Create a complete variable id from a server, device and id.
#define MAKE_SRV_VID(srv, dev, id) (((srv)<<22) | MAKE_VID_DEVICE(dev) | (id))
// Strip all bits to get the bare vid.
#define STRIP_VID(vid) ((vid) & ~(VID_FILE_MASK | VID_SPARE_MASK | VID_SERVER_MASK | VID_DEVICE_MASK))
//---------------------------------------------------------------------------
//
// OLD Special macros for the old buggy style ultrasonic acquisition device.
//
// Because the channel number is zero (no channel value) the
// full 10 bits can be used for the ID.
#define OLD_MAKE_UT_VID(srv, dev, id) \
  (((srv)<<22)|((dev)<<16)|(id))
//
//#define MAX_GATE_VALUE 0xF
//#define NO_GATE_VALUE MAX_GATE_VALUE
//
#define OLD_MAKE_UT_GATE_VID(srv, dev, ch, gate, id) \
  (((srv)<<22)|((dev)<<16)|((ch)<<10)|((gate)<<6)|(id))
// For channel parameters the non gate value must be entered. so only 6 bits are available.
#define OLD_MAKE_UT_CHAN_VID(srv, dev, ch, id) \
  (((srv)<<22)|((dev)<<16)|((ch)<<10)|((0xF)<<6)|(id))
//---------------------------------------------------------------------------
//
// NEW Special macros for the ultrasonic acquisition device.
//
// Because the channel number is zero (no channel value) the
// full 10 bits can be used for the ID.
#define MAKE_UT_VID(srv, dev, id) \
  (((srv)<<22)|((dev)<<16)|(id))
// Because the gate number is zero (no signal value) the
// 6 bits can be used for the ID.
#define MAKE_UT_CHAN_VID(srv, dev, ch, id) \
  (((srv)<<22)|((dev)<<16)|((ch)<<10)|(id))
// Channel and gate value are now used so only 6 bits can be used for the ID.
#define MAKE_UT_GATE_VID(srv, dev, ch, gate, id) \
  (((srv)<<22)|((dev)<<16)|((ch)<<10)|((gate)<<6)|(id))
/*
// Retrieves the channel number if the variable ID.
#define GET_UT_CHANNEL(vid)  (((vid) & 0x00007C00L) >> 10)
// Retrieves the gate number if the variable ID.
#define GET_UT_GATE(vid)     (((vid) & 0x000003C0L) >>  6)
// Retrieves the gate ID number if the variable ID. (6-bits)
#define GET_UT_GATE_ID(vid)   (((vid) & 0x0000003FL))
// Retrieves the channel ID number if the variable ID. (6-bits)
#define GET_UT_CHAN_ID(vid)   (((vid) & 0x0000003FL))
// This one retrieves the server ID number. (10 bits)
#define GET_UT_ID(vid)       (((vid) & 0x000003FFL))
*/
//---------------------------------------------------------------------------
//
// Special macros for the eddy current acquisition device.
//
// Because the channel number is zero (no channel value) the
// full 10 bits can be used for the ID.
#define MAKE_EC_VID(srv, dev, id) \
  (((srv)<<22)|((dev)<<16)|(id))
// Because the signal number is zero (no signal value) the
// 8 bits can be used for the ID.
#define MAKE_EC_CHAN_VID(srv, dev, ch, id) \
  (((srv)<<22)|((dev)<<16)|((ch)<<10)|(id))
// Channel and signal value are now used so only 8 bits can be used for the ID.
#define MAKE_EC_SIGNAL_VID(srv, dev, ch, sig, id) \
  (((srv)<<22)|((dev)<<16)|((ch)<<10)|((sig)<<8)|(id))
/*
// Retrieves the channel number if the variable ID.
#define GET_EC_CHANNEL(vid)   (((vid) & 0x00007C00L) >> 10)
// Retrieves the signal ID number if the variable ID.
#define GET_EC_SIGNAL(vid)    (((vid) & 0x00000300L) >>  8)
// This one retrieves the server ID number.
#define GET_EC_SIGNAL_ID(vid) (((vid) & 0x000000FFL))
// This one retrieves the server ID number. (10 bits)
#define GET_EC_ID(vid)        (((vid) & 0x000003FFL))
*/
//---------------------------------------------------------------------------
//
// Special macros for the motion device.
//
// Special macros for the motion control device.
#define MAX_AXIS_VALUE 0xF
#define MAKE_MC_AXIS_VID(srv, dev, axis, id) \
  (((srv)<<22)|((dev)<<16)|((axis)<<10)|(id))
#define MAKE_MC_VID(srv, dev, id) \
  MAKE_MC_AXIS_VID(srv, dev, MAX_AXIS_VALUE, id)
//---------------------------------------------------------------------------
//
// Special macros for the inspection project device.
//
#define MAKE_IPJ_VID(id) ((VID_DEVICE_IPJ) | (id))

