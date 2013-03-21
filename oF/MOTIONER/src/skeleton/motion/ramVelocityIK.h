//
//  ramVelocityIK.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/12/12.
//
//

#ifndef __MOTIONER__ramVelocityIK__
#define __MOTIONER__ramVelocityIK__

#include "ofMain.h"

namespace ram { namespace skeleton {
    
    //------------------------------------------------------------------------------------
    class Skeleton;
    class Node;
    
    //------------------------------------------------------------------------------------
    class VelocityIK {
    public:
        void update(Skeleton *skeleton);
    };
    
} }

#endif /* defined(__MOTIONER__ramVelocityIK__) */
