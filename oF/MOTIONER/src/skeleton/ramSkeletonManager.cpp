//
//  ramSkeletonManager.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/4/13.
//
//

#include "ramSkeletonManager.h"
#include "ramSkeleton.h"
#include "ramEvent.h"
#include "ramUI.h"

using namespace ram;
using namespace ram::skeleton;

//----------------------------------------------------------------------------------------
SkeletonManager &SkeletonManager::getInstance()
{
    static SkeletonManager instance;
    return instance;
}

//----------------------------------------------------------------------------------------
SkeletonMap &SkeletonManager::getSkeletons()
{
    return mSkeletonMap;
}

//------------------------------------------------------------------------------------
void SkeletonManager::addSkeleton(const string &hostName)
{
    if (mSkeletonMap.find(hostName)==mSkeletonMap.end()) {
        SkeletonPtr skl = ram::skeleton::createSkeleton();
        
        skl->setup(hostName, DEFAULT_SKELETON_SETTINGS);
        
        mSkeletonMap[hostName] = skl; /// insert
        UI::getInstance().updateDeviceList(&mSkeletonMap);/// update ui drop down menu
        UI::getInstance().getInspector().setDevice(skl->getHostName()); /// set ui current skeleton
        
        UI::getInstance().getInspector().setSettingsFilePath(DEFAULT_SKELETON_SETTINGS);
    }
    else {
        ofxThrowException(ofxException, "Skeleton \""+hostName+"\" already exsists");
    }
}

//------------------------------------------------------------------------------------
void SkeletonManager::removeSkeleton(const string &hostName)
{
    ofLogNotice("Util") << "Remove skeleton: " << hostName;
    
    SkeletonMap &skl = mSkeletonMap;
    SkeletonMap::iterator it = find_if(skl.begin(), skl.end(), skeleton::SkeletonFinder(hostName)); /// slow
    
    if (it!=skl.end())
        skl.erase(it);
}

//------------------------------------------------------------------------------------
void SkeletonManager::updateSkeletons()
{
    SkeletonMap &skl = mSkeletonMap;
    for (SkeletonMap::iterator it = skl.begin(); it!=skl.end(); ++it)
        it->second->update();
}

//------------------------------------------------------------------------------------
void SkeletonManager::drawSkeletons()
{
    SkeletonMap &skl = mSkeletonMap;
    for (SkeletonMap::iterator it = skl.begin(); it!=skl.end(); ++it)
        it->second->draw();
}

//------------------------------------------------------------------------------------
void SkeletonManager::drawSkeletonsHUD()
{
    SkeletonMap &skl = mSkeletonMap;
    for (SkeletonMap::iterator it = skl.begin(); it!=skl.end(); ++it)
        it->second->drawHUD();
}

//------------------------------------------------------------------------------------
void SkeletonManager::drawSkeletonsPicker()
{
    SkeletonMap &skl = mSkeletonMap;
    for (SkeletonMap::iterator it = skl.begin(); it!=skl.end(); ++it)
        it->second->drawPicker();
}

//------------------------------------------------------------------------------------
ram::skeleton::SkeletonPtr SkeletonManager::getSkeleton(int index)
{
    SkeletonMap::iterator it = mSkeletonMap.begin();
    
    for (; index; ++it) {
        if (it==mSkeletonMap.end())
            ofxThrowException(ofxException, "Out of range!");
    }
    
    return it->second;
}

//------------------------------------------------------------------------------------
ram::skeleton::SkeletonPtr SkeletonManager::getSkeleton(const string &name)
{
    SkeletonMap::iterator it = mSkeletonMap.find(name);
    if (it==mSkeletonMap.end())
        ofxThrowException(ofxException, "Skeleton not found! " + name);
    return it->second;
}

//----------------------------------------------------------------------------------------
SkeletonManager::SkeletonManager()
{
    mSkeletonMap.clear();
}

//----------------------------------------------------------------------------------------
SkeletonManager::~SkeletonManager()
{
    mSkeletonMap.clear();
}