//
//  ramFrame.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#ifndef __MOTIONER__ramFrame__
#define __MOTIONER__ramFrame__

#include "ofMain.h"

namespace ram { namespace skeleton { namespace coder {

    //------------------------------------------------------------------------------------
    struct Frame {
        Frame();
        string name;
        ofVec3f position;
        vector<ofQuaternion> rotation;
        vector<float> velocity;
        double time;
    };
    
} } }

#endif /* defined(__MOTIONER__ramFrame__) */
