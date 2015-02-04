//
//  ramPrimitive.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/12/12.
//
//

#ifndef __MOTIONER__ramPrimitive__
#define __MOTIONER__ramPrimitive__

#include "ofMain.h"

namespace ram {
    
    namespace Mesh {
        
        void drawAxis(float size);
        
        ofVboMesh createGrid(float size = 500.f, int numSlices = 20);
        
        
    }
}

#endif /* defined(__MOTIONER__ramPrimitive__) */
