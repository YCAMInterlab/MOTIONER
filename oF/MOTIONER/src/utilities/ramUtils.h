//
//  ramUtils.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#ifndef __MOTIONER__ramUtils__
#define __MOTIONER__ramUtils__

#include "ofMain.h"
#include "ramCommon.h"
#include "ofxException.h"
#include "ofxOscMessage.h"

/// Macros
//----------------------------------------------------------------------------------------
#define RAM_NOTIMPL() ofLogWarning() << __FUNCTION__ << "Not implemented yet"

//----------------------------------------------------------------------------------------
namespace ram {
    
    /// Setup MOTIONERApp
    //--------------------
    void initialize();
    
    void determinate();
    
    //--------------------
//    string getJointName(int index);
//    int getJointIndex(const string &name);
    
    bool isEndSite(int nodeId);
    
    //--------------------
    CoderPtr createCurrentCoder();
    void setCoderType(int type);
    
    //--------------------
    //void openRecordingDialog();
    void startRecording();
    void finishRecording();
    
    //--------------------
    void openPlaybackFile(const string&path, const string &name);
    void startPlayback();
    void pausePlayback();
    void stopPlayback();
    void finishPlayback();
    void setLoopPlayback(bool bLoop);
    
    //--------------------
    string getLastPathCompornent(const string &path);
    string getFileExt(const string &path);
    string getFileName(const string &path);
    
    //--------------------
    string getDefaultRecordingDataFileName();
    
    /// Shadow matrix
    /// ax + by + cz + d = 0;
    //--------------------
    extern const float kGroundPlaneXUp[];
    extern const float kGroundPlaneYUp[];
    extern const float kGroundPlaneZUp[];
    
    //--------------------
    void calcShadowMatrix(const float groundplane[4],
                          const float lightpos[3],
                          float shadowMatrix[16]);
    
}

#endif /* defined(__MOTIONER__ramUtils__) */
