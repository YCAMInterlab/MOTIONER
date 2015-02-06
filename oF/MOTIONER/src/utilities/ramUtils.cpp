//
//  ramUtils.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#include "ramUtils.h"
#include "ramDeviceCorrespondent.h"
#include "ramUI.h"
#include "ramConstants.h"
#include "ramSkeleton.h"
#include "ramSkeletonManager.h"
#include "ofxException.h"
#include "ramBinaryCoder.h"
#include "ramOscSender.h"
#include "ramEvent.h"
#include "ramRenderer.h"

#include "ofxEvent.h"

#include <numeric>

//----------------------------------------------------------------------------------------
namespace ram {
    
    static int sCoderType;
    static string sCoderExt;
    
    //------------------------------------------------------------------------------------
    void initialize()
    {
        sCoderType = CODER_BINARY;
        sCoderExt = MOTION_DATA_EXT;
        
        skeleton::Renderer::initialize();
    }
    
    //------------------------------------------------------------------------------------
    void determinate()
    {
    }
    
//    //------------------------------------------------------------------------------------
//    string getJointName(int index)
//    {
//        if (index==JOINT_HIPS)				return "JOINT_HIPS";
//        if (index==JOINT_ABDOMEN)			return "JOINT_ABDOMEN";
//        if (index==JOINT_CHEST)				return "JOINT_CHEST";
//        if (index==JOINT_NECK)				return "JOINT_NECK";
//        if (index==JOINT_HEAD)				return "JOINT_HEAD";
//        if (index==JOINT_LEFT_HIP)			return "JOINT_LEFT_HIP";
//        if (index==JOINT_LEFT_KNEE)			return "JOINT_LEFT_KNEE";
//        if (index==JOINT_LEFT_ANKLE)		return "JOINT_LEFT_ANKLE";
//        if (index==JOINT_LEFT_TOE)			return "JOINT_LEFT_TOE";
//        if (index==JOINT_RIGHT_HIP)			return "JOINT_RIGHT_HIP";
//        if (index==JOINT_RIGHT_KNEE)		return "JOINT_RIGHT_KNEE";
//        if (index==JOINT_RIGHT_ANKLE)		return "JOINT_RIGHT_ANKLE";
//        if (index==JOINT_RIGHT_TOE)			return "JOINT_RIGHT_TOE";
//        if (index==JOINT_LEFT_COLLAR)		return "JOINT_LEFT_COLLAR";
//        if (index==JOINT_LEFT_SHOULDER)		return "JOINT_LEFT_SHOULDER";
//        if (index==JOINT_LEFT_ELBOW)		return "JOINT_LEFT_ELBOW";
//        if (index==JOINT_LEFT_WRIST)		return "JOINT_LEFT_WRIST";
//        if (index==JOINT_LEFT_HAND) 		return "JOINT_LEFT_HAND";
//        if (index==JOINT_RIGHT_COLLAR) 		return "JOINT_RIGHT_COLLAR";
//        if (index==JOINT_RIGHT_SHOULDER) 	return "JOINT_RIGHT_SHOULDER";
//        if (index==JOINT_RIGHT_ELBOW) 		return "JOINT_RIGHT_ELBOW";
//        if (index==JOINT_RIGHT_WRIST) 		return "JOINT_RIGHT_WRIST";
//        if (index==JOINT_RIGHT_HAND)		return "JOINT_RIGHT_HAND";
//        
//        return "unknown joint index";
//    }
//    
//    //------------------------------------------------------------------------------------
//    int getJointIndex(const string &name)
//    {
//        if (name=="JOINT_HIPS")				return JOINT_HIPS;
//        if (name=="JOINT_ABDOMEN")			return JOINT_ABDOMEN;
//        if (name=="JOINT_CHEST")			return JOINT_CHEST;
//        if (name=="JOINT_NECK")				return JOINT_NECK;
//        if (name=="JOINT_HEAD")				return JOINT_HEAD;
//        if (name=="JOINT_LEFT_HIP")			return JOINT_LEFT_HIP;
//        if (name=="JOINT_LEFT_KNEE")		return JOINT_LEFT_KNEE;
//        if (name=="JOINT_LEFT_ANKLE")		return JOINT_LEFT_ANKLE;
//        if (name=="JOINT_LEFT_TOE")			return JOINT_LEFT_TOE;
//        if (name=="JOINT_RIGHT_HIP")		return JOINT_RIGHT_HIP;
//        if (name=="JOINT_RIGHT_KNEE")		return JOINT_RIGHT_KNEE;
//        if (name=="JOINT_RIGHT_ANKLE")		return JOINT_RIGHT_ANKLE;
//        if (name=="JOINT_RIGHT_TOE")		return JOINT_RIGHT_TOE;
//        if (name=="JOINT_LEFT_COLLAR")		return JOINT_LEFT_COLLAR;
//        if (name=="JOINT_LEFT_SHOULDER")	return JOINT_LEFT_SHOULDER;
//        if (name=="JOINT_LEFT_ELBOW")		return JOINT_LEFT_ELBOW;
//        if (name=="JOINT_LEFT_WRIST")		return JOINT_LEFT_WRIST;
//        if (name=="JOINT_LEFT_HAND") 		return JOINT_LEFT_HAND;
//        if (name=="JOINT_RIGHT_COLLAR") 	return JOINT_RIGHT_COLLAR;
//        if (name=="JOINT_RIGHT_SHOULDER") 	return JOINT_RIGHT_SHOULDER;
//        if (name=="JOINT_RIGHT_ELBOW") 		return JOINT_RIGHT_ELBOW;
//        if (name=="JOINT_RIGHT_WRIST") 		return JOINT_RIGHT_WRIST;
//        if (name=="JOINT_RIGHT_HAND")		return JOINT_RIGHT_HAND;
//        
//        return -1;
//    }
    
    /// General utils
    //------------------------------------------------------------------------------------
    bool isEndSite(int nodeId)
    {
        for (int i=0; i<N_END_SITE; i++)
            if (nodeId == END_SITE[i])
                return true;
        return false;
    }
        
    //------------------------------------------------------------------------------------
    string getDefaultRecordingDataFileName()
    {
        const string timestamp = ofGetTimestampString(TIME_STAMP_FORMAT);
        return ("rec_"+FILE_NAME_WILDCARD+"_"+timestamp+"."+sCoderExt);
    }
    
    //------------------------------------------------------------------------------------
    CoderPtr createCurrentCoder()
    {
        CoderPtr coder;
        switch (sCoderType) {
            case CODER_BVH:
                RAM_NOTIMPL();
                break;
                
            case CODER_BINARY:
                coder = CoderPtr(new skeleton::coder::BinaryCoder());
                break;
            default:
                ofxThrowException(ofxException, "No such coder found!");
        }
        return coder;
    }
    
    //------------------------------------------------------------------------------------
    void setCoderType(int type)
    {
        sCoderType = type;
        CoderPtr coder = createCurrentCoder();
        sCoderExt = coder->getFileExtension();
    }
    
    //------------------------------------------------------------------------------------
    //void openRecordingDialog()
    //{
    //    ///add directory select
    //    const string defPath = ram::getDefaultRecordingDataFileName();
    //    const string info = "Save motions for all skeletons.\n \'*\' will replase with each skeleton's name.";
    //    ofFileDialogResult ret = ofSystemSaveDialog(defPath, info);
    //    if (ret.bSuccess) {
    //        SkeletonMap &map = Shared::getInstance().skeletonMap;
    //        
    //        if (map.empty())
    //            ofLogNotice("Recorder") << "No skeleton avairable";
    //        
    //        for (SkeletonMap::iterator it = map.begin(); it!=map.end(); ++it) {
    //            SkeletonPtr skeleton = it->second;
    //            const string name = skeleton->getName();
    //            string filePath = ret.getPath();
    //            const string escape = "__WILDCARD_ESCAPE__";
    //            ofStringReplace(filePath, "\\*", escape);
    //            ofStringReplace(filePath, "*", name);
    //            ofStringReplace(filePath, escape, "*");
    //            skeleton->prepareRecording(filePath);
    //        }
    //    }
    //}
    
    static bool bOpeningRecorder = false;
    
    //------------------------------------------------------------------------------------
    void startRecording()
    {
        if (bOpeningRecorder)
            finishRecording();
        
        ///add directory select
        SkeletonMap &map = skeleton::SkeletonManager::getInstance().getSkeletons();

        if (map.empty()) {
            ofLogNotice("Recorder") << "No skeleton avairable";
            return;
        }

        ofDirectory dir(MOTION_DATA_DIR);
        if (!dir.exists()) {
            dir.create(true);
            ofLogNotice("Recorder") << "Created new directory " << MOTION_DATA_DIR;
        }
        
        for (SkeletonMap::iterator it = map.begin(); it!=map.end(); ++it) {
            SkeletonPtr skeleton = it->second;
            const string name = skeleton->getName();
            //string filePath = ret.getPath();
            string filePath = ofToDataPath(MOTION_DATA_DIR+ram::getDefaultRecordingDataFileName());
            const string escape = "__WILDCARD_ESCAPE__";
            ofStringReplace(filePath, "\\*", escape);
            ofStringReplace(filePath, "*", name);
            ofStringReplace(filePath, escape, "*");
            skeleton->prepareRecording(filePath);
            skeleton->startRecording();
        }
    
        bOpeningRecorder = true;
    }
    
    //------------------------------------------------------------------------------------
    void finishRecording()
    {
        bOpeningRecorder = false;
        
        SkeletonMap &map = skeleton::SkeletonManager::getInstance().getSkeletons();
        
        if (map.empty()) {
            ofLogNotice("Recorder") << "No skeleton avairable";
            return;
        }
        
        for (SkeletonMap::iterator it = map.begin(); it!=map.end(); ++it) {
            SkeletonPtr skeleton = it->second;
            skeleton->stopRecording();
            skeleton->finishRecording();
        }
    }
    
    //------------------------------------------------------------------------------------
    ofFileDialogResult openPlaybackDialog()
    {
        ///add directory select
        const string defPath = ram::MOTION_DATA_DIR;
        const string info = "Open motions for a skeleton.";
        return ofSystemLoadDialog(info, false, defPath);
        
    }
    
    //------------------------------------------------------------------------------------
    void openPlaybackFile(const string&path, const string &name)
    {
        const string hostName = name;
        SkeletonMap &map = skeleton::SkeletonManager::getInstance().getSkeletons();
        if (map.find(hostName) == map.end()) {
            skeleton::SkeletonManager::getInstance().addSkeleton(hostName);
            skeleton::SkeletonManager::getInstance().getSkeleton(hostName)->preparePlayback(path);
            skeleton::SkeletonManager::getInstance().getSkeleton(hostName)->startPlayback();
        }
    }
    
    //------------------------------------------------------------------------------------
    void startPlayback()
    {
        SkeletonMap &map = skeleton::SkeletonManager::getInstance().getSkeletons();
        for (SkeletonMap::iterator it = map.begin(); it!=map.end(); ++it) {
            SkeletonPtr skeleton = it->second;
            skeleton->startPlayback();
        }
    }
    
    //------------------------------------------------------------------------------------
    void pausePlayback()
    {
        SkeletonMap &map = skeleton::SkeletonManager::getInstance().getSkeletons();
        for (SkeletonMap::iterator it = map.begin(); it!=map.end(); ++it) {
            SkeletonPtr skeleton = it->second;
            skeleton->pausePlayback();
        }
    }
    
    //------------------------------------------------------------------------------------
    void stopPlayback()
    {
        SkeletonMap &map = skeleton::SkeletonManager::getInstance().getSkeletons();
        for (SkeletonMap::iterator it = map.begin(); it!=map.end(); ++it) {
            SkeletonPtr skeleton = it->second;
            skeleton->stopPlayback();
        }
    }
    
    //------------------------------------------------------------------------------------
    void setLoopPlayback(bool bLoop)
    {
        SkeletonMap &map = skeleton::SkeletonManager::getInstance().getSkeletons();
        for (SkeletonMap::iterator it = map.begin(); it!=map.end(); ++it) {
            SkeletonPtr skeleton = it->second;
            ofLoopType loop = bLoop ? OF_LOOP_NORMAL : OF_LOOP_NONE;
            skeleton->setLoopPlayback(loop);
        }
    }
    
    //------------------------------------------------------------------------------------
    string getLastPathCompornent(const string &path)
    {
        vector<string> split = ofSplitString(path, "/");
        if (!split.empty()) {
            const string fileName = *(split.end()-1);
            return fileName;
        }
        ofxThrowException(ofxException, "Empty path! "+path);
        return "";
    }
    
    //------------------------------------------------------------------------------------
    string getFileExt(const string &path)
    {
        string::size_type pos(path.rfind("."));
        if (pos != string::npos)
            return path.substr(pos+1, path.length());
        return "";
    }
    
    //------------------------------------------------------------------------------------
    string getFileName(const string &path)
    {
        string::size_type pos(path.rfind("/"));
        if (pos != string::npos)
            return path.substr(pos+1, path.length());
        return "";
    }
    
    /// shadow matrix
    //------------------------------------------------------------------------------------
    const float kGroundPlaneXUp[4] = { 1.0, 0.0, 0.0, 1.0 };
    const float kGroundPlaneYUp[4] = { 0.0, 1.0, 0.0, 1.0 };
    const float kGroundPlaneZUp[4] = { 0.0, 0.0, 1.0, 1.0 };
    
    //------------------------------------------------------------------------------------
    void calcShadowMatrix(const float groundplane[],
                          const float lightpos[],
                          float shadowMatrix[])
    {
        float dot = inner_product(groundplane, groundplane+4, lightpos, 0.f);
        
        for(int y = 0; y < 4;++y) {
            for(int x = 0; x < 4; ++x) {
                
                shadowMatrix[y*4+x] = - groundplane[y]*lightpos[x];
                
                if (x == y) shadowMatrix[y*4+x] += dot;
            }
        }
        
    }
} // namespace ram


