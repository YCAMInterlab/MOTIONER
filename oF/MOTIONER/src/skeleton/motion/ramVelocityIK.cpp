//
//  ramVelocityIK.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/12/12.
//
//

#include "ramVelocityIK.h"
#include "ramSkeleton.h"
#include "ramConstants.h"

using namespace ram;
using namespace ram::skeleton;

//----------------------------------------------------------------------------------------
void VelocityIK::update(Skeleton *skeleton)
{
    vector<Node> &joints = skeleton->mJoints;
    
    const float shape = .001; // this parameter needs some testing
    ofVec3f displacement;
    float weightSum = 0;
    for (size_t i=0; i<joints.size(); i++)
    {
        Node& cur = joints.at(i);
        ofVec3f curPosition = cur.getGlobalPosition();
        ofVec3f prevPosition = cur.prevPosition;
        // uncomment the next line to only use feet
        //if(i == JOINT_LEFT_ANKLE || i == JOINT_RIGHT_ANKLE)
        {
            if(prevPosition != ofVec3f()) 
            {
                float velocityMeasured = cur.velocity;
                float weight = shape / (shape + velocityMeasured);
                weight = ofClamp(weight, 1, 0);
                ofVec3f curDisplacement = curPosition - prevPosition;
                displacement += weight * curDisplacement;
                weightSum += weight;
            }
        }
        cur.prevPosition = curPosition;
    }
    
    if(weightSum > 0) 
    {
        displacement /= weightSum;
    }
    
    // accumulate displacement
    joints.at(JOINT_HIPS).setGlobalPosition(joints.at(JOINT_HIPS).getGlobalPosition() - displacement);
}
