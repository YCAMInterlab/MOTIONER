//
//  ramNode.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/8/12.
//
//

#ifndef __MOTIONER__ramNode__
#define __MOTIONER__ramNode__

#include "ofMain.h"

namespace ram { namespace skeleton {
   
    //------------------------------------------------------------------------------------
    struct Node : public ofNode {
        
        Node();
        
        void update();
        //--------------------
        void customDraw();
        
        //--------------------
        string name;
        int id;
        float velocity;
        ofVec3f prevPosition;
        ofVec3f prevOrientationEuler;
        float noResponceDuration;
        
        float size;
        
        bool enable;
        
        bool error;
    };
    
    //------------------------------------------------------------------------------------
    struct NodeFinder {
        
        //--------------------
        NodeFinder(const string &name);
        NodeFinder(int id);
        
        //--------------------
        bool operator ()(const Node &n);
        
        //--------------------
        enum Mode { NAME, ID, };
        
        //--------------------
        string name;
        int id;
        Mode mode;
    };
    
} }

#endif /* defined(__MOTIONER__ramNode__) */
