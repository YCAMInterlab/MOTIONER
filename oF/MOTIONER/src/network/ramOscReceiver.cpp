//
//  ramOscReceiver.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 1/28/13.
//
//

#include "ramOscReceiver.h"
#include "ramSkeleton.h"
#include "ofxEvent.h"
#include "ramEvent.h"

using namespace ram;

//----------------------------------------------------------------------------------------
void OscReceiver::setup(int port)
{
    mOscReceiver.setup(port);
    mPort = port;
    mEnabled = true;
}

//----------------------------------------------------------------------------------------
void OscReceiver::update()
{
    while (mOscReceiver.hasWaitingMessages()) {
        ofxOscMessage m;
        mOscReceiver.getNextMessage(&m);
        const string address = m.getAddress();
        const string host = m.getRemoteIp();
        
        //for (int i=0; i<m.getNumArgs(); i++)
        //    const ofxOscArgType arg = m.getArgType(i);
        /// calibration
        /// from iOS controller app
        if (address==OSC_ADDR_CALIBRATION) {
            ofxEventMessage out;
            out.setAddress(event::ADDRESS_CALIBRATION);
            out.addStringArg(m.getArgAsString(0));
            ofxNotifyEvent(out);
        }
        /// reset position
        /// from iOS controller app
        else if (address==OSC_ADDR_RESET_POSITION) {
            ofxEventMessage out;
            out.setAddress(event::ADDRESS_RESET_POSITION);
            out.addStringArg(m.getArgAsString(0));
            ofxNotifyEvent(out);
        }
        /// save skeleton setting
        /// from iOS controller app
        else if (address==OSC_ADDR_SAVE_SKELETON) {
            ofxEventMessage out;
            out.setAddress(event::ADDRESS_SAVE_SKELETON);
            out.addStringArg(m.getArgAsString(0));
            ofxNotifyEvent(out);
        }
        /// start recording
        /// from iOS controller app
        else if (address==OSC_ADDR_REC_START) {
            ofxEventMessage out;
            out.setAddress(event::ADDRESS_START_RECORDING);
            //out.addStringArg(m.getArgAsString(0));
            ofxNotifyEvent(out);
        }
        /// finish recording
        /// from iOS controller app
        else if (address==OSC_ADDR_REC_STOP) {
            ofxEventMessage out;
            out.setAddress(event::ADDRESS_FINISH_RECORDING);
            //out.addStringArg(m.getArgAsString(0));
            ofxNotifyEvent(out);
        }

        else if (address==OSC_ADDR_CIRCLE_TRACKING_FILTERD) {
            ofxEventMessage out;
            out.setAddress(event::ADDRESS_CIRCLE_TRACKER);
            out.addIntArg(m.getArgAsInt32(0)); /// circle index
            out.addFloatArg(m.getArgAsFloat(1)*10.0f); /// x
            out.addFloatArg(m.getArgAsFloat(2)*10.0f); /// y
            out.addFloatArg(m.getArgAsFloat(3)*10.0f); /// z
            
            ofxNotifyEvent(out);
        }
    }
}