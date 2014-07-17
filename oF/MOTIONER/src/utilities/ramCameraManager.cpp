//
//  ramCameraManager.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/4/13.
//
//

#include "ramCameraManager.h"
#include "ramEvent.h"

using namespace ram;

//----------------------------------------------------------------------------------------
CameraManager::CameraManager() :
mCurrCameraId(CAMERA_EASY_CAM)
{
    mCameras.clear();
    /// one easy cam
    {
        ofEasyCam *easyCam = new ofEasyCam();
        easyCam->setDistance(2300.0f);
        easyCam->disableMouseInput();
        easyCam->setFarClip(1000000.0f);
        CameraPtr ptr = CameraPtr(easyCam);
        mCameras.push_back(ptr);
    }
    
    ofAddListener(ofxEvent(), this, &CameraManager::onMessageReceived);
}

//----------------------------------------------------------------------------------------
CameraManager::~CameraManager()
{
    ofRemoveListener(ofxEvent(), this, &CameraManager::onMessageReceived);
    
    mCameras.clear();
}

//----------------------------------------------------------------------------------------
CameraPtr CameraManager::getCurrentCamera()
{
    return mCameras.at(mCurrCameraId);
}

//----------------------------------------------------------------------------------------
void CameraManager::setCurrentCamera(int index)
{
    mCurrCameraId = index;
}

//----------------------------------------------------------------------------------------
vector<CameraPtr> &CameraManager::getCameras()
{
    return mCameras;
}

//----------------------------------------------------------------------------------------
void CameraManager::setRotation(bool bRotate)
{
    ofPtr<ofEasyCam> cam = dynamic_pointer_cast<ofEasyCam>(mCameras.at(CAMERA_EASY_CAM));
    if (cam)
        bRotate ? cam->enableMouseInput() : cam->disableMouseInput();
}

//----------------------------------------------------------------------------------------
void CameraManager::setTranslation(bool bTranslate)
{
    ofPtr<ofEasyCam> cam = dynamic_pointer_cast<ofEasyCam>(mCameras.at(CAMERA_EASY_CAM));
    if (cam) {
        if (bTranslate) {
            cam->enableMouseInput();
            ofNotifyMousePressed(0, 0, OF_MOUSE_BUTTON_MIDDLE);
            //ofNotifyKeyPressed('m');
        }
        else {
            cam->disableMouseInput();
            ofNotifyMouseReleased(0, 0, OF_MOUSE_BUTTON_MIDDLE);
            //ofNotifyKeyReleased('m');
        }
    }
}

//----------------------------------------------------------------------------------------
void CameraManager::onMessageReceived(ofxEventMessage &m)
{
    const string addr = m.getAddress();
    if (addr==event::ADDRESS_SET_CAMERA_ROTATION) {
        const bool enable = static_cast<bool>(m.getArgAsInt32(0));
        setTranslation(false);
        setRotation(enable);
    }
    else if (addr==event::ADDRESS_SET_CAMERA_TRANSLATION) {
        const bool enable = static_cast<bool>(m.getArgAsInt32(0));
        setRotation(false);
        setTranslation(enable);
    }
}

