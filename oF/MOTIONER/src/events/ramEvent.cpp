//
//  ramEvent.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/5/13.
//
//

#include "ramEvent.h"
#include "ofxEvent.h"

namespace ram { namespace event {
    
    //------------------------------------------------------------------------------------
    const std::string ADDRESS_TOGGLE_DRAW                   = "ADRRESS_TOGGLE_DRAW";
    const std::string ADDRESS_ENABLE_OSC                    = "ADDRESS_ENABLE_OSC";
    const std::string ADDRESS_DISABLE_OSC                   = "ADDRESS_DISABLE_OSC";
    const std::string ADDRESS_SETUP_DEVICE_CORRESPONDENT    = "ADDRESS_SETUP_DEVICE_CORRESPONDENT";
    const std::string ADDRESS_PICKED_SKELETON               = "ADDRESS_PICKED_SKELETON";
    const std::string ADDRESS_PICKED_JOINT                  = "ADDRESS_PICKED_JOINT";
    const std::string ADDRESS_LOG                           = "ADDRESS_LOG";
    const std::string ADDRESS_CHANGE_SKELETON_NAME          = "ADDRESS_CHANGE_SKELETON_NAME";
    const std::string ADDRESS_SET_CAMERA_ROTATION           = "ADDRESS_SET_CAMERA_ROTATION";
    const std::string ADDRESS_SET_CAMERA_TRANSLATION        = "ADDRESS_SET_CAMERA_TRANSLATION";
    
    const std::string ADDRESS_CIRCLE_TRACKER                = "ADDRESS_CIRCLE_TRACKER";
    const std::string ADDRESS_CIRCLE_TRACKER_ZERO           = "ADDRESS_CIRCLE_TRACKER_ZERO";
    const std::string ADDRESS_CIRCLE_TRACKER_SCALE          = "ADDRESS_CIRCLE_TRACKER_SCALE";
    const std::string ADDRESS_CIRCLE_TRACKER_HEIGHT         = "ADDRESS_CIRCLE_TRACKER_HEIGHT";
    const std::string ADDRESS_CIRCLE_TRACKER_ANGLE          = "ADDRESS_CIRCLE_TRACKER_ANGLE";
    
    const std::string ADDRESS_START_RECORDING               = "ADDRESS_START_RECORDING";
    const std::string ADDRESS_FINISH_RECORDING              = "ADDRESS_FINISH_RECORDING";
    
    const std::string ADDRESS_CALIBRATION                   = "ADDRESS_CALIBRATION";
    const std::string ADDRESS_RESET_POSITION                = "ADDRESS_RESET_POSITION";
    const std::string ADDRESS_SAVE_SKELETON                 = "ADDRESS_SAVE_SKELETON";
    
    const std::string ADDRESS_PLAY_MOTION                   = "ADDRESS_PLAY_MOTION";
    
    const std::string ADDRESS_SET_LOWPASS                   = "ADDRESS_SET_LOWPASS";
    const std::string ADDRESS_SET_ORIENTATION               = "ADDRESS_SET_ORIENTATION";
    const std::string ADDRESS_REQUEST_GENERAL_SETTINGS      = "ADDRESS_REQUEST_GENERAL_SETTINGS";
    
    void requestGeneralSettings()
    {
        ofxEventMessage m;
        m.setAddress(event::ADDRESS_REQUEST_GENERAL_SETTINGS);
        ofxNotifyEvent(m);
    }
    
} }