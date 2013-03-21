//
//  ramCircleTrackerIK.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/12/13.
//
//

#include "ramCircleTrackerIK.h"
#include "ramSkeleton.h"
#include "ramEvent.h"

using namespace ram::skeleton;

//----------------------------------------------------------------------------------------
CircleTrackerIK::CircleTrackerIK() :
mCircleId(0),
mRotation(180.0f)
{
    ofAddListener(ofxEvent(), this, &CircleTrackerIK::onMessageReceived);
    mPosition.zero();
    mBasePosition.zero();
    mScale.set(-0.1f, 0.1f, 0.1f);
    mHeight.set(0.0f, 700.0f, 0.0f);
}

//----------------------------------------------------------------------------------------
CircleTrackerIK::~CircleTrackerIK()
{
    ofRemoveListener(ofxEvent(), this, &CircleTrackerIK::onMessageReceived);
}

//----------------------------------------------------------------------------------------
void CircleTrackerIK::update(Skeleton *skeleton)
{
    const ofVec3f pos = ((mPosition-mBasePosition)*mScale+mHeight).rotate(mRotation, ofVec3f(0.0f, 1.0f, 0.0f));
    skeleton->getJoint(JOINT_HIPS).setGlobalPosition(pos);
}

//----------------------------------------------------------------------------------------
const ofVec3f &CircleTrackerIK::getPosition() const
{
    return mPosition;
}

//----------------------------------------------------------------------------------------
int CircleTrackerIK::getCircleId() const
{
    return mCircleId;
}

//----------------------------------------------------------------------------------------
void CircleTrackerIK::setPosition(const ofVec3f &pos)
{
    mPosition = pos;
}

//----------------------------------------------------------------------------------------
void CircleTrackerIK::setCircleId(int id)
{
    mCircleId = id;
}

//----------------------------------------------------------------------------------------
void CircleTrackerIK::onMessageReceived(ofxEventMessage &m)
{
    if (m.getAddress()==event::ADDRESS_CIRCLE_TRACKER) {
        const int circleId = m.getArgAsInt32(0);
        if (circleId==mCircleId)
            mPosition.set(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3));
    }
    else if (m.getAddress()==event::ADDRESS_CIRCLE_TRACKER_ZERO) {
        const int circleId = m.getArgAsInt32(0);
        if (circleId==mCircleId)
            mBasePosition = mPosition;
    }
    else if (m.getAddress()==event::ADDRESS_CIRCLE_TRACKER_SCALE) {
        const int circleId = m.getArgAsInt32(0);
        if (circleId==mCircleId)
            mScale.set(-m.getArgAsFloat(1),m.getArgAsFloat(1),m.getArgAsFloat(1));
    }
    else if (m.getAddress()==event::ADDRESS_CIRCLE_TRACKER_HEIGHT) {
        const int circleId = m.getArgAsInt32(0);
        if (circleId==mCircleId)
            mHeight.y = m.getArgAsFloat(1);
    }
    else if (m.getAddress()==event::ADDRESS_CIRCLE_TRACKER_ANGLE) {
        const int circleId = m.getArgAsInt32(0);
        if (circleId==mCircleId)
            mRotation = m.getArgAsFloat(1);
    }
}



