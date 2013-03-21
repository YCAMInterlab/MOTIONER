//
//  ramTypes.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/28/12.
//
//

#ifndef __MOTIONER__ramTypes__
#define __MOTIONER__ramTypes__

#include "ofMain.h"
#include "ramNode.h"
#include "ofxXmlSettings.h"

namespace ram {
    
    //------------------------------------------------------------------------------------
    namespace skeleton {
    class Skeleton;
        namespace coder {
            class BaseCoder;
        }
    }
    
    //------------------------------------------------------------------------------------
    typedef ofPtr<skeleton::Skeleton> SkeletonPtr;
    typedef ofPtr<skeleton::coder::BaseCoder> CoderPtr;
    typedef ofPtr<ofCamera> CameraPtr;
    
    //--------------------
    typedef map<string, SkeletonPtr> SkeletonMap;
    typedef map<string, ofxXmlSettings> SettingsMap;
    
    //--------------------
    typedef vector<skeleton::Node> NodeVec;
    
    //--------------------
    typedef unsigned long long frame_t;
    
    //------------------------------------------------------------------------------------
    union Int32AndChar {
        int32_t i;
        char c[4];
    };
    
    //------------------------------------------------------------------------------------
    union UInt64AndChar {
        uint64_t i;
        char c[4];
    };

    //------------------------------------------------------------------------------------
    union FloatAndByte {
        float f;
        uint8_t b[4];
    };

    //------------------------------------------------------------------------------------
    union FloatAndChar {
        float f;
        char c[4];
    };
    
}

#endif /* defined(__MOTIONER__ramTypes__) */
