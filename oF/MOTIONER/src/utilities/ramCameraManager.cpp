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
        easyCam->setFarClip(1000000.0f);
        easyCam->setFov(80.f);
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
void CameraManager::onMessageReceived(ofxEventMessage &m)
{
}

