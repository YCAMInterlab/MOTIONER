//
//  ramEvent.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/5/13.
//
//

#ifndef __MOTIONER__ramEvent__
#define __MOTIONER__ramEvent__

#include <string>

namespace ram { namespace event {
    
    /// event names
    //------------------------------------------------------------------------------------
    extern const std::string ADDRESS_TOGGLE_DRAW;
    extern const std::string ADDRESS_ENABLE_OSC;
    extern const std::string ADDRESS_DISABLE_OSC;
    extern const std::string ADDRESS_SETUP_DEVICE_CORRESPONDENT;
    extern const std::string ADDRESS_PICKED_SKELETON;
    extern const std::string ADDRESS_PICKED_JOINT;
    extern const std::string ADDRESS_LOG;
    extern const std::string ADDRESS_CHANGE_SKELETON_NAME;
    extern const std::string ADDRESS_SET_CAMERA_ROTATION;
    extern const std::string ADDRESS_SET_CAMERA_TRANSLATION;
    extern const std::string ADDRESS_CIRCLE_TRACKER;
    extern const std::string ADDRESS_CIRCLE_TRACKER_ZERO;
    extern const std::string ADDRESS_CIRCLE_TRACKER_SCALE;
    extern const std::string ADDRESS_CIRCLE_TRACKER_HEIGHT;
    extern const std::string ADDRESS_CIRCLE_TRACKER_ANGLE;
    extern const std::string ADDRESS_START_RECORDING;
    extern const std::string ADDRESS_FINISH_RECORDING;
    extern const std::string ADDRESS_CALIBRATION;
    extern const std::string ADDRESS_RESET_POSITION;
    extern const std::string ADDRESS_PLAY_MOTION;
    extern const std::string ADDRESS_SAVE_SKELETON;
    extern const std::string ADDRESS_SET_LOWPASS;
    extern const std::string ADDRESS_SET_ORIENTATION;
    
    extern const std::string ADDRESS_REQUEST_GENERAL_SETTINGS;
    
    extern void requestGeneralSettings();
    
} }

#endif /* defined(__MOTIONER__ramEvent__) */
