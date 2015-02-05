//
//  ramConstants.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/7/12.
//
//

#include "ramConstants.h"

namespace ram {
    
    //------------------------------------------------------------------------------------
    const int N_END_SITE = 5;
    
    //--------------------
    const int END_SITE[N_END_SITE] =
    {
        JOINT_HEAD,
        JOINT_LEFT_TOE,
        JOINT_RIGHT_TOE,
        JOINT_LEFT_HAND,
        JOINT_RIGHT_HAND,
    };
    
    //------------------------------------------------------------------------------------
    const int    UDP_PACKET_BUFFER = 100000;
    const int    FLOAT_BYTES = 4;
    const int    QUATERNION_DIM = 4;
    const int    IMU_DATA_DIM = 5;
    const int    UDP_MESSAGE_LENGTH = IMU_DATA_DIM*FLOAT_BYTES*(NUM_JOINTS-N_END_SITE)+4;
    const string UDP_PACKET_DELIMITER = "abcd";
    const int    UDP_REFRESH_RATE = 120;
    
    const double DEVICE_TIMEOUT_DUR = 20.0f;
    
    //--------------------
    const int    DEBUG_OSC_INCOMING_PORT = 9000;
    
    //--------------------
    //const string SETTINGS_XML = "xml/settings.xml";
    
    //------------------------------------------------------------------------------------
    const string GUI_XML  = "xml/gui.xml";
    const string INSPECTOR_XML  = "xml/inspector.xml";
    const string GUI_FONT = "fonts/frabk.ttf";
    
    //--------------------
    const string SKELETON_XML_DIR = "xml/skeleton";
    
    //--------------------
    const string DEFAULT_SKELETON_SETTINGS = SKELETON_XML_DIR+"/default.xml";
    
    //------------------------------------------------------------------------------------
    const int LOGGER_INDENT = 30;
    
    //------------------------------------------------------------------------------------
    const string MOTION_DATA_EXT = "mot";
    const string MOTION_DATA_DIR = "motion/mot/";
    const string MOTION_DATA_VERSION = "1.1";
    const float MOTION_DATA_DEFAULT_FRAME_TIME = 1.0f/60.0f;
    
    //--------------------
    const string TIME_STAMP_FORMAT = "%Y-%m-%d_%H-%M-%S";
    const string FILE_NAME_WILDCARD = "*";
    
    //------------------------------------------------------------------------------------
    const int COLOR_H  = 0xCCCCCC;
    const int COLOR_MH = 0xAAAAAA;
    const int COLOR_M  = 0x999999;
    const int COLOR_ML = 0x555555;
    const int COLOR_L  = 0x222244;
    const int COLOR_BG = 0x332222;
    //const int COLOR_HILIGHT = 0x887766;
    //const int COLOR_HILIGHT = 0x998877;
    const int COLOR_HILIGHT = 0xAA9480;
    
    //------------------------------------------------------------------------------------
    const string OSC_ADDR_RDT_PLAYER = "/ram/skeleton";
    const string OSC_ADDR_CALIBRATION = "/ram/calibration";
    const string OSC_ADDR_RESET_POSITION = "/ram/reset_position";
    const string OSC_ADDR_SAVE_SKELETON = "/ram/save_skeleton";
    const string OSC_ADDR_REC_START = "/ram/rec/start";
    const string OSC_ADDR_REC_STOP = "/ram/rec/end";
    const string OSC_ADDR_EXTERNAL_POSITION = "/ram/external_position";
    const string OSC_ADDR_CIRCLE_TRACKING_COMBINED = "/CircleTracking/combined";
    const string OSC_ADDR_CIRCLE_TRACKING_FILTERD = "/CircleTracking/filterd";
    
    //------------------------------------------------------------------------------------
    
    const float kJointErrorEpsilon = 0.01f;
    const float kJointErrorThreshould = 3.f;
    
//#ifdef DEBUG
    const ofLogLevel DEFAULT_LOG_LEVEL = OF_LOG_NOTICE;
//#else
//    const ofLogLevel DEFAULT_LOG_LEVEL = OF_LOG_WARNING;
//#endif
}
