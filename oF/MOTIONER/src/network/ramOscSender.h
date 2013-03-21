//
//  ramOscSender.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 1/17/13.
//
//

#ifndef __MOTIONER__ramOscSender__
#define __MOTIONER__ramOscSender__

#include "ofMain.h"
#include "ofxOsc.h"
#include "ramConstants.h"

namespace ram {
  
    //------------------------------------------------------------------------------------
    class OscSender {
    public:
        //--------------------
        OscSender();
        
        //--------------------
        void setup(const string &host, int port);
        void update();
        
        //--------------------
        inline const string &getHostName() const;
        inline int getPortNumber() const;
        
        inline void setEnabled(bool bEnabled);
        inline bool getEnabled() const;
        
    private:
        //--------------------
        ofxOscSender mSender;
        string mHostName;
        int mPortNumber;
        bool mInited;
        bool mEnabled;
    };
    
    //------------------------------------------------------------------------------------
    inline void OscSender::setEnabled(bool bEnabled)
    {
        mEnabled = bEnabled;
    }
    
    //------------------------------------------------------------------------------------
    inline const string &OscSender::getHostName() const
    {
        return mHostName;
    }
    
    //------------------------------------------------------------------------------------
    inline int OscSender::getPortNumber() const
    {
        return mPortNumber;
    }
    
    //------------------------------------------------------------------------------------
    inline bool OscSender::getEnabled() const
    {
        return (mEnabled && mInited);
    }
    
};

#endif /* defined(__MOTIONER__ramOscSender__) */
