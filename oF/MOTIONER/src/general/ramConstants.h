//
//  ramConstants.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#ifndef MOTIONER_ramConstants_h
#define MOTIONER_ramConstants_h

#include "ofMain.h"
#include "ofxXmlSettings.h"

#define RAM_SINGLETON_CLASS

namespace ram {
    
    //------------------------------------------------------------------------------------
    
    /// skeleton joints
    //--------------------
    enum JointType {
		JOINT_HIPS              = 0,
		JOINT_ABDOMEN           = 1,//o
		JOINT_CHEST             = 2,
		JOINT_NECK              = 3,
		JOINT_HEAD              = 4, /// end site
        
		JOINT_LEFT_HIP          = 5,
		JOINT_LEFT_KNEE         = 6,
		JOINT_LEFT_ANKLE        = 7,//o
		JOINT_LEFT_TOE          = 8, /// end site
        
		JOINT_RIGHT_HIP         = 9,
		JOINT_RIGHT_KNEE        = 10,
		JOINT_RIGHT_ANKLE       = 11,//o
		JOINT_RIGHT_TOE         = 12, /// end site
        
		JOINT_LEFT_COLLAR       = 13,
		JOINT_LEFT_SHOULDER     = 14,
		JOINT_LEFT_ELBOW        = 15,
		JOINT_LEFT_WRIST        = 16,//o
		JOINT_LEFT_HAND         = 17, /// end site
        
		JOINT_RIGHT_COLLAR      = 18,
		JOINT_RIGHT_SHOULDER    = 19,
		JOINT_RIGHT_ELBOW       = 20,
		JOINT_RIGHT_WRIST       = 21,//o
		JOINT_RIGHT_HAND        = 22, /// end site
        
		NUM_JOINTS              = 23,
	};
    
    extern const int N_END_SITE;
    extern const int END_SITE[];
    
    //------------------------------------------------------------------------------------
    
    /// record and playback
    //--------------------
    enum CoderType {
        CODER_BINARY    = 0,
        CODER_BVH       = 1,
        //NUM_CODER_TYPES,
    };
    
    //------------------------------------------------------------------------------------
    
    /// Pre allocated data length for UDP packet
    //--------------------
    extern const int    UDP_PACKET_BUFFER;
    /// XBee WiFi message size via UDP
    /// float*quat*joints + float*position + delimiter
    /// 4*4*18 + 3*4 + 4
    extern const int    UDP_MESSAGE_LENGTH;
    extern const string UDP_PACKET_DELIMITER;
    extern const int    UDP_REFRESH_RATE;
    extern const double DEVICE_TIMEOUT_DUR;
    
    //--------------------
    extern const int    FLOAT_BYTES;
    extern const int    QUATERNION_DIM;
    extern const int    IMU_DATA_DIM;
    
    //--------------------
    extern const int    DEBUG_OSC_INCOMING_PORT;
    
    //--------------------
    //extern const string SETTINGS_XML;
    
    //------------------------------------------------------------------------------------
    extern const string GUI_XML;
    extern const string INSPECTOR_XML;
    extern const string GUI_FONT;
    
    //--------------------
    extern const string SKELETON_XML_DIR;
    
    //--------------------
    extern const string DEFAULT_SKELETON_SETTINGS;
    
    //--------------------
    extern const ofLogLevel DEFAULT_LOG_LEVEL;
    
    //------------------------------------------------------------------------------------
    extern const int LOGGER_INDENT;
    
    //------------------------------------------------------------------------------------
    extern const string MOTION_DATA_EXT;
    extern const string MOTION_DATA_DIR;
    extern const string MOTION_DATA_VERSION;
    extern const float MOTION_DATA_DEFAULT_FRAME_TIME;
    
    /// for file name for motions
    //--------------------
    extern const string TIME_STAMP_FORMAT;
    extern const string FILE_NAME_WILDCARD;
    
    //------------------------------------------------------------------------------------
    extern const int COLOR_H;
    extern const int COLOR_MH;
    extern const int COLOR_M;
    extern const int COLOR_ML;
    extern const int COLOR_L;
    extern const int COLOR_BG;
    extern const int COLOR_HILIGHT;
    
    //------------------------------------------------------------------------------------
    extern const string OSC_ADDR_RDT_PLAYER;
    extern const string OSC_ADDR_CALIBRATION;
    extern const string OSC_ADDR_RESET_POSITION;
    extern const string OSC_ADDR_SAVE_SKELETON;
    extern const string OSC_ADDR_REC_START;
    extern const string OSC_ADDR_REC_STOP;
    extern const string OSC_ADDR_EXTERNAL_POSITION;
    extern const string OSC_ADDR_CIRCLE_TRACKING_COMBINED;
    extern const string OSC_ADDR_CIRCLE_TRACKING_FILTERD;
    
    extern const float kJointErrorEpsilon;
    extern const float kJointErrorThreshould;
    
}

//----------------------------------------------------------------------------------------
namespace std {
	using namespace tr1; /// std::tr1:: as std::
}

#endif
