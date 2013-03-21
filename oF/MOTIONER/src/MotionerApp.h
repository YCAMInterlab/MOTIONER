#pragma once

#include "ofMain.h"

#include "ramOscSender.h"
#ifdef DEBUG
#include "ramOscReceiver.h"
#endif

#include "ofxEvent.h"

//----------------------------------------------------------------------------------------
class MotionerApp : public ofBaseApp {
public:
    //--------------------
    MotionerApp();
    ~MotionerApp();
    
    //--------------------
    void setup();
    void exit();
    void update();
    void draw();
    
    //--------------------
    void keyPressed(int key);
    void windowResized(int w, int h);
    
    void onMessageReceived(ofxEventMessage &m);
    
    void dragEvent(ofDragInfo dragInfo);
    
private:
    /// draw
    //--------------------
    ofVboMesh mGrid;
    ofVec3f mOffset;
    
    bool mEnableDraw;
    bool mEnableUpdateSkeletons;
    
    /// network
    //--------------------
    /// output motion data
    ram::OscSender   mOscSender;
    
    ofLight mLight;
    
#ifdef DEBUG
    /// input playback data (currntly only debug usage)
    ram::OscReceiver mOscReceiver;
#endif
};
