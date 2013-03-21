//
//  ramFrame.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#include "ramFrame.h"
#include "ramConstants.h"

using namespace ram;
using namespace ram::skeleton;
using namespace ram::skeleton::coder;


//----------------------------------------------------------------------------------------
Frame::Frame() : time(0.0)
{
    position.set(0.0f, 0.0f);
    rotation.clear();
    for (int i=0; i<NUM_JOINTS; i++) {
        rotation.push_back(ofQuaternion());
        velocity.push_back(0.0f);
    }
}