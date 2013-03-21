//
//  ramEasyIK.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/12/12.
//
//

#ifndef __MOTIONER__ramEasyIK__
#define __MOTIONER__ramEasyIK__

#include "ofMain.h"
#include "ramConstants.h"

namespace ram { namespace skeleton {
    
    //--------------------
    class Skeleton;
    class Node;
    
    //------------------------------------------------------------------------------------
    class EasyIK {
    public:
        EasyIK();
        
        //--------------------
        void update(Skeleton *skeleton);
        inline const ofVec3f &getPosition() const;
        inline const ofVec3f &getOffset() const;
        inline void setPosition(const ofVec3f &pos);
        
    private:
        enum Axis {
            AXIS_X = 0,
            AXIS_Y = 1,
            AXIS_Z = 2,
            NUM_AXIES,
        };
        
        /// NOTICE: this function is not working yet.
        /// target = A joint which we want to limit a degree of freedom
        /// corrector = A referance joint for limit a degree of freedom (e.g. target.euler.y = corrector.euler.y)
        /// axis = ofVec3f axis. 0 = x, 1 = y, 2 = z
        /// TODO: Will not use euler angles
        /// Because it cause error sometime
        ofQuaternion getLimiedAngle(Skeleton *skeleton,
                                    JointType target,
                                    JointType corrector,
                                    Axis axis);
        
        //--------------------
        
        ofVec3f mBottomOrigin;
        ofVec3f mPrevBottomPos;
        ofVec3f mPosition;
        ofVec3f mLastPosition;
        Node*   mPrevNode;
        
        float   mLowpass;
        ofVec3f mOffset;
    };
    
    //------------------------------------------------------------------------------------
    inline const ofVec3f &EasyIK::getPosition() const
    {
        return mPosition;
    }
    
    //------------------------------------------------------------------------------------
    inline const ofVec3f &EasyIK::getOffset() const
    {
        return mOffset;
    }
    
    //------------------------------------------------------------------------------------
    inline void EasyIK::setPosition(const ofVec3f &pos)
    {
        //ofLogNotice() << pos;
        mBottomOrigin = pos;
        mPosition = pos;
        mPrevBottomPos = pos;
        mPrevNode = NULL;
        mOffset = pos;
    }
    
} }

#endif /* defined(__MOTIONER__ramEasyIK__) */
