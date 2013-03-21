//
//  ramPicker.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/4/13.
//
//

#ifndef __MOTIONER__ramPicker__
#define __MOTIONER__ramPicker__

#include "ofMain.h"
#include "ramUtils.h"

namespace ram { namespace skeleton {
    
    class Node;
    class Skeleton;
    
    //------------------------------------------------------------------------------------
    class Picker {
        friend class Skeleton;
    public:
        //--------------------
        Picker();
        virtual ~Picker();
        
        /// ** tricky **
        /// this class should update inside of rendering scope
        /// because this class calcurate screen coordinate from current modelview/projection matrix
        /// and viewport
        ///
        /// marked as const for other rendering methods
        /// this method will modify mutable members
        //--------------------
        virtual void update(Skeleton *skeleton) const;
        
        //--------------------
        virtual void debugDraw(Skeleton *skeleton) const;
        
        //--------------------
        void setDrawDebug(bool bEnable);
        void toggleDrawDebug();
        
        //--------------------
        void enableMouseEventCallbacks();
        void disableMouseEventCallbacks();
        
        //--------------------
        virtual void mouseMoved(int x, int y);
        virtual void mouseDragged(int x, int y, int button);
        virtual void mousePressed(int x, int y, int button);
        virtual void mouseReleased(int x, int y, int button);
        
    private:
        //--------------------
        enum MouseState {
            MOUSE_IDLE,
            MOUSE_OVER,
            MOUSE_DOWN,
        };
        
        //--------------------        
        void onMouseMoved(ofMouseEventArgs &e);
        void onMouseDragged(ofMouseEventArgs &e);
        void onMousePressed(ofMouseEventArgs &e);
        void onMouseReleased(ofMouseEventArgs &e);
        
        bool isInsideSkeleton(int x, int y);
        bool isInsideJoint(int jointId, int x, int y);
        
        bool mDrawDebug;
        
        mutable vector<ofVec3f> mScreenJointLoc;
        mutable ofVec3f mMinCoord;
        mutable ofVec3f mMaxCoord;
       
        mutable string mSkeletonHostName;
        
        MouseState mSkeletonMouseState;
        vector<MouseState> mJointMouseState;
    };
    
} }


#endif /* defined(__MOTIONER__ramPicker__) */
