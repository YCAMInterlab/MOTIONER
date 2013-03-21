//
//  ramOscReceiver.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 1/28/13.
//
//

#ifndef __MOTIONER__ramOscReceiver__
#define __MOTIONER__ramOscReceiver__

#include "ofxOscReceiver.h"

namespace ram {
    
    //------------------------------------------------------------------------------------
    class OscReceiver {
    public:
        //--------------------
        void setup(int port);
        void update();
        
        //--------------------
        inline int getPortNumber() const;
        inline bool getEnabled() const;
        
    private:
        //--------------------
        ofxOscReceiver mOscReceiver;
        int mPort;
        bool mEnabled;
        
    };
    
    //------------------------------------------------------------------------------------
    inline bool OscReceiver::getEnabled() const
    {
        return mEnabled;
    }
    
    //------------------------------------------------------------------------------------
    inline int OscReceiver::getPortNumber() const
    {
        return mPort;
    }
};

#endif /* defined(__MOTIONER__ramOscReceiver__) */
