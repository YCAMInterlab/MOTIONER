//
//  ramOscSender.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 1/17/13.
//
//

#include "ramOscSender.h"
#include "ramSkeleton.h"
#include "ramSkeletonManager.h"

using namespace ram;


//----------------------------------------------------------------------------------------
OscSender::OscSender() :
mHostName(""),
mPortNumber(0),
mInited(false),
mEnabled(false)
{
}

//----------------------------------------------------------------------------------------
void OscSender::setup(const string &host, int port)
{
    mSender.setup(host, port);
    mHostName = host;
    mPortNumber = port;
    mInited = true;
}

//----------------------------------------------------------------------------------------
void OscSender::update()
{
    if (!mInited || !mEnabled) return;
    
    SkeletonMap &skeletons = skeleton::SkeletonManager::getInstance().getSkeletons();
    
    SkeletonMap::iterator it = skeletons.begin();
    for (; it!=skeletons.end(); ++it) {
        SkeletonPtr skeleton = it->second;
        
        if (skeleton->getEnableOscOut() == false)
            continue;
        
        //ofLogNotice() << skeleton->getName() << " " << ofGetFrameNum();
        
        /// create ram specified osc message
        ofxOscMessage msg;
        
        msg.setAddress(OSC_ADDR_RDT_PLAYER);
        
        msg.addStringArg(skeleton->getName());
        msg.addIntArg(skeleton->getJoints().size());
        
        const NodeVec &joints = skeleton->getJoints();
        
        for (size_t i=0; i<joints.size(); i++) {
            
            const skeleton::Node &joint = joints.at(i);
            //const string &name = joint.name;
            const ofVec3f posision = joint.getGlobalPosition();
            const ofQuaternion orientation = joint.getGlobalOrientation();
            float angle;
            ofVec3f axis;
            orientation.getRotate(angle, axis);
            
            msg.addStringArg(joint.name); /// -> will remove
            
            msg.addFloatArg(posision.x/10.0f); /// mm to cm
            msg.addFloatArg(posision.y/10.0f); /// mm to cm
            msg.addFloatArg(posision.z/10.0f); /// mm to cm
            msg.addFloatArg(angle);
            msg.addFloatArg(axis.x);
            msg.addFloatArg(axis.y);
            msg.addFloatArg(axis.z);
        }
        
        msg.addFloatArg(ofGetElapsedTimef());
        
        mSender.sendMessage(msg);
    }
}

