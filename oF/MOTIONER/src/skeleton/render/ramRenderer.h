//
//  ramRenderer.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/7/12.
//
//

#ifndef __MOTIONER__ramRenderer__
#define __MOTIONER__ramRenderer__

#include "ofMain.h"
#include "ramUtils.h"

namespace ram { namespace skeleton {
    
    class Node;
    class Skeleton;
    
    //------------------------------------------------------------------------------------
    class Renderer {
        friend class Skeleton;
    public:
        //--------------------
        Renderer();
        virtual ~Renderer();
        //--------------------
        virtual void update(Skeleton *skleton);
        virtual void draw(Skeleton *skeleton) const;
        virtual void drawHUD(Skeleton *skeleton) const {}
                
        void setState(int state);
        int getState() const;
        
        void setActiveJoint(int joint);
        int getActiveJoint() const;
        
    private:
        bool mIsEnableShading;
        int mState;
        int mActiveJoint;
        
        //mutable ofLight mLight;
    };
    
} }

#endif /* defined(__MOTIONER__ramRenderer__) */
