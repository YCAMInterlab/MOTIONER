//
//  ramCameraManager.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/4/13.
//
//

#ifndef __MOTIONER__ramCameraManager__
#define __MOTIONER__ramCameraManager__

#include "ofMain.h"

#include "ramCommon.h"
#include "ramUtils.h"

#include "ofxNonCopyable.h"
#include "ofxEvent.h"

namespace ram {
    
    //------------------------------------------------------------------------------------
    RAM_SINGLETON_CLASS
    class CameraManager : public ofxNonCopyable<CameraManager> {
    public:
        enum CameraType {
            CAMERA_EASY_CAM = 0,
            N_CAMERA_TYPES,
        };
        
        inline static CameraManager &getInstance();
        
        CameraPtr getCurrentCamera();
        void setCurrentCamera(int index);
        
        vector<CameraPtr> &getCameras();
        
        void setRotation(bool bRotate);
        void setTranslation(bool bTranslate);
        
        void keyPressed(int key);
        void keyReleased(int key);
        
    private:
        CameraManager();
        ~CameraManager();
        
        void onMessageReceived(ofxEventMessage &m);
        
        vector<CameraPtr> mCameras;
        int mCurrCameraId;
    };
    
    //------------------------------------------------------------------------------------
    inline CameraManager &CameraManager::getInstance()
    {
        static CameraManager instance;
        return instance;
    }

}

#endif /* defined(__MOTIONER__ramCameraManager__) */
