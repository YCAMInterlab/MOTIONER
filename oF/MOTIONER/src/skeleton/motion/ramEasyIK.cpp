//
//  ramEasyIK.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/12/12.
//
//

#include "ramEasyIK.h"
#include "ramSkeleton.h"
#include "ramConstants.h"

using namespace ram;
using namespace ram::skeleton;

EasyIK::EasyIK() :
mLowpass(0.0f),
mFixX(false),
mFixY(false),
mFixZ(false),
mExternalPosition(0.0f, 0.0f, 0.0f)
{
}

/// NOTICE: not work yet!
//----------------------------------------------------------------------------------------
ofQuaternion EasyIK::getLimiedAngle(Skeleton *skeleton,
                                    JointType target,
                                    JointType corrector,
                                    Axis axis)
{
    ofVec3f axisVector;
    switch (axis) {
        case AXIS_X: axisVector.set(1.0f, 0.0f, 0.0f); break;
        case AXIS_Y: axisVector.set(0.0f, 1.0f, 0.0f); break;
        case AXIS_Z: axisVector.set(0.0f, 0.0f, 1.0f); break;
            
        default:
            ofxThrowException(ofxException, "Incorrect axis");
    }
    
    //const coder::Frame initial = skeleton->getInitialPose();
    //const float offset = initial.rotation.at(corrector).getEuler()[axis] - initial.rotation.at(target).getEuler()[axis];
    
    const ofQuaternion invert(-skeleton->getJoint(target).getOrientationEuler()[axis], axisVector);
    const ofQuaternion correct(skeleton->getJoint(corrector).getOrientationEuler()[axis], axisVector);
    const ofQuaternion result = skeleton->getJoint(target).getOrientationQuat() * invert * correct;
    
    return result;
}

//----------------------------------------------------------------------------------------
void EasyIK::setFixedAxis(bool fixX, bool fixY, bool fixZ)
{
    mFixX = fixX;
    mFixY = fixY;
    mFixZ = fixZ;
}

//----------------------------------------------------------------------------------------
void EasyIK::update(Skeleton *skeleton)
{
    vector<Node> &joints = skeleton->mJoints;
    
    /// experiment
    //ofQuaternion limitedElbowL = getLimiedAngle(joints.at(JOINT_LEFT_ELBOW).getGlobalOrientation(),
    //                                           joints.at(JOINT_LEFT_SHOULDER).getGlobalOrientation(),
    //                                           AXIS_Y); /// limit Y axis
    //joints.at(JOINT_LEFT_SHOULDER).setGlobalOrientation(limitedElbowL);
    //
    //ofQuaternion limitedElbowR = getLimiedAngle(joints.at(JOINT_RIGHT_ELBOW).getGlobalOrientation(),
    //                                           joints.at(JOINT_RIGHT_SHOULDER).getGlobalOrientation(),
    //                                           AXIS_Y); /// limit Y axis
    //joints.at(JOINT_RIGHT_SHOULDER).setGlobalOrientation(limitedElbowR);
    //
    /// experiment
    //ofQuaternion limitedKneeL = getLimiedAngle(joints.at(JOINT_LEFT_KNEE).getGlobalOrientation(),
    //                                            joints.at(JOINT_LEFT_HIP).getGlobalOrientation(),
    //                                            AXIS_Y); /// limit Y axis
    //joints.at(JOINT_LEFT_KNEE).setGlobalOrientation(limitedKneeL);
    //
    //ofQuaternion limitedKneeR = getLimiedAngle(joints.at(JOINT_RIGHT_KNEE).getGlobalOrientation(),
    //                                            joints.at(JOINT_RIGHT_HIP).getGlobalOrientation(),
    //                                            AXIS_Y); /// limit Y axis
    //joints.at(JOINT_RIGHT_KNEE).setGlobalOrientation(limitedKneeR);
    
    /// remove ankles magnetometer distortion
    /// limit Y axis
    //ofQuaternion limitedAnkleL = getLimiedAngle(skeleton, JOINT_LEFT_ANKLE, JOINT_LEFT_KNEE, AXIS_Y);
    //joints.at(JOINT_LEFT_ANKLE).setGlobalOrientation(limitedAnkleL);
    //
    //ofQuaternion limitedAnkleR = getLimiedAngle(skeleton, JOINT_RIGHT_ANKLE, JOINT_RIGHT_KNEE, AXIS_Y);
    //joints.at(JOINT_RIGHT_ANKLE).setGlobalOrientation(limitedAnkleR);
    
    joints.at(JOINT_HIPS).setGlobalPosition(ofVec3f());
    
    Node *btm = &joints.at(JOINT_HIPS);
    
    for (size_t i=0; i<joints.size(); i++) {
        /// for ankles distortion
        //if (i==JOINT_LEFT_TOE || i==JOINT_RIGHT_TOE)
        //    continue;
        
        //if (i==JOINT_LEFT_ANKLE || i==JOINT_RIGHT_ANKLE)
        //    continue;
        
        /// remove fingers
        if (i==JOINT_LEFT_HAND || i== JOINT_RIGHT_HAND)
            continue;
        
        if (joints.at(i).getGlobalPosition().y < btm->getGlobalPosition().y)
            btm = &joints.at(i);
    }
    
    if (mPrevNode && mPrevNode != btm) {
        mBottomOrigin = mPosition;
        mPrevBottomPos = btm->getGlobalPosition();
    }
    
    mPosition.x = mBottomOrigin.x + (btm->getGlobalPosition().x - mPrevBottomPos.x);
    mPosition.y = btm->getGlobalPosition().y;
    mPosition.z = mBottomOrigin.z + (btm->getGlobalPosition().z - mPrevBottomPos.z);
    
    mPrevNode = btm;
    
    mLastPosition = mPosition;
    /// lowpassed
    const ofVec3f pos = -(mPosition*(1.0f-mLowpass) + mLastPosition*mLowpass);
    /// normal
    //ofVec3f pos = -mPosition;
    
    ofVec3f result = pos+mOffset;
    
    if (mFixX) result.x = 0.0;
    if (mFixY) result.y = 0.0;
    if (mFixZ) result.z = 0.0;
    
    //cout << result << endl;
    
    result.x += mExternalPosition.x;
    result.z += mExternalPosition.z;
    
    joints.at(JOINT_HIPS).setGlobalPosition(result);
}











