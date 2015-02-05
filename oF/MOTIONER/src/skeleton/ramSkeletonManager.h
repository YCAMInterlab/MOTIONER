//
//  ramSkeletonManager.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/4/13.
//
//

#ifndef __MOTIONER__ramSkeletonManager__
#define __MOTIONER__ramSkeletonManager__

#include "ramCommon.h"
#include "ramUtils.h"
#include "ofxNonCopyable.h"

namespace ram { namespace skeleton {
    
    class SkeletonManager : public ofxNonCopyable<SkeletonManager> {
    public:
        static SkeletonManager &getInstance();
        
        //--------------------
        SkeletonMap &getSkeletons();
        
        //--------------------
        void addSkeleton(const string &hostName);
        void removeSkeleton(const string &hostName);
        
        //--------------------
        void updateSkeletons();
        void drawSkeletons();
        void drawSkeletonsHUD();
        void drawSkeletonsPicker();
        
        //--------------------
        SkeletonPtr getSkeleton(int index);
        SkeletonPtr getSkeleton(const string &name);
        
    private:
        SkeletonManager();
        ~SkeletonManager();
        
        /// all skeletons
        //--------------------
        SkeletonMap mSkeletonMap;
    };
    
} }

#endif /* defined(__MOTIONER__ramSkeletonManager__) */
