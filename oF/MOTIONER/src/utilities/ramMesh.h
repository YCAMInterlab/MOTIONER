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
        
        //--------------------------------------------------------------------------------
        void drawRuler();
        
        ofVboMesh createGrid(float size = 6000.0f, int numSlices = 3000.0f/100.0f);
        
        
    }
}

#endif /* defined(__MOTIONER__ramPrimitive__) */
