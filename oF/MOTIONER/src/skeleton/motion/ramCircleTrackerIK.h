//
//  ramCircleTrackerIK.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/12/13.
//
//

#ifndef __MOTIONER__ramCircleTrackerIK__
#define __MOTIONER__ramCircleTrackerIK__

#include "ofMain.h"
#include "ofxEvent.h"

namespace ram { namespace skeleton {
    
    //--------------------
    class Skeleton;
    
    //------------------------------------------------------------------------------------
    class CircleTrackerIK {
    public:
        CircleTrackerIK();
        ~CircleTrackerIK();
        
        //--------------------
        void update(Skeleton *skeleton);
        
        const ofVec3f &getPosition() const;
        int getCircleId() const;
        
        void setPosition(const ofVec3f &pos);
        void setCircleId(int id);
        
        void onMessageReceived(ofxEventMessage &m);
        
    private:
        //--------------------
        ofVec3f mPosition;
        ofVec3f mBasePosition;
        ofVec3f mHeight;
        ofVec3f mScale;
        float mRotation;
        int mCircleId;
    };
    
} }

#endif /* defined(__MOTIONER__ramCircleTrackerIK__) */
