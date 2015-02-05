//
//  ramSkeleton.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#include "ramSkeleton.h"
#include "ramConstants.h"
#include "ramUtils.h"
#include "ramSettings.h"
#include "ramSkeletonManager.h"
#include "ramPlayer.h"
#include "ramRecorder.h"
#include "ramDeviceCorrespondent.h"
#include "ramRenderer.h"
#include "ramPicker.h"
#include "ramEasyIK.h"
#include "ramCircleTrackerIK.h"
#include "ramEvent.h"

#include "ofxException.h"


using namespace ram;
using namespace ram::skeleton;
using namespace ram::skeleton::coder;

/// pimpl
/// modules
//----------------------------------------------------------------------------------------
struct Skeleton::Modules {
    Settings    settings;
    Player      player;
    Recorder    recorder;
    Renderer    renderer;
    Picker      picker;
    EasyIK      easyIK;
    CircleTrackerIK circleTrackerIK;
};

/// skeleton class member functions
//----------------------------------------------------------------------------------------
Skeleton::Skeleton() :
mFlags(CAPTURE | RENDER | EASY_IK | OSC_OUT),
mDrawDebug(true),
mLowpass(0.0f),
mOrientationY(0.0f),
mAutoResetDimension(0.0f),
mFixPosition(false)
{
    mJoints.clear();
    mJoints.assign(NUM_JOINTS, Node());
    
    createTree();
    
    //reportFlags();
    
    mModules = ofPtr<Modules>(new Modules()); /// pimpl
}

//----------------------------------------------------------------------------------------
Skeleton::~Skeleton()
{
    ofRemoveListener(ofxEvent(), this, &Skeleton::onMessageReceived);
}

//----------------------------------------------------------------------------------------
void Skeleton::setup(const string &hostName, const string &settingFilePath)
{
    OFX_BEGIN_EXCEPTION_HANDLING
    
    loadSettings(settingFilePath);
    setHostName(hostName);
    
    ofAddListener(ofxEvent(), this, &Skeleton::onMessageReceived);
    
    event::requestGeneralSettings();
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::update()
{
    OFX_BEGIN_EXCEPTION_HANDLING
    
    if (isEnable(CAPTURE) && isEnable(PLAY))
        ofxThrowException(ofxException, "CAPTURE & PLAY cnann't use at once!");
    
    /// device
    if (isEnable(CAPTURE)) {
        const string hostName = mModules->settings.mDeviceHostName;
        if (!hostName.empty()) {
            
            updateLowpass(DeviceCorrespondent::getInstance().getFrame(hostName));
            
            /// update position (currently zero)
            // mJoints.at(JOINT_HIPS).setGlobalPosition(mCurrentFrame.position);
            
            updateRotation();
        }
    }
    
    /// player
    if (isEnable(PLAY))
        mModules->player.update(this);
    
    OFX_END_EXCEPTION_HANDLING
    
    
    OFX_BEGIN_EXCEPTION_HANDLING
    
    /// recorder
    if (isEnable(RECORD))
        mModules->recorder.update(this);
    
    /// renderer
    if (isEnable(RENDER))
        mModules->renderer.update(this);
    
    /// estimate global position
    if (isEnable(EASY_IK)) {
        mModules->easyIK.update(this);
        if (mAutoResetDimension > 0.0f && mJoints.at(JOINT_HIPS).getGlobalPosition().length() >= mAutoResetDimension) {
            resetPosition(mModules->easyIK.getOffset());
        }
    }
    else if (isEnable(CIRCLE_TRACKER_IK)) {
        mModules->circleTrackerIK.update(this);
    }
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::updateLowpass(coder::Frame frame)
{
    mPreviousFrame = mCurrentFrame;
    mCurrentFrame = frame;
    for (int i=0; i<mCurrentFrame.rotation.size(); i++) {
        mCurrentFrame.rotation.at(i).slerp(mLowpass, mCurrentFrame.rotation.at(i), mPreviousFrame.rotation.at(i));
    }
}

//----------------------------------------------------------------------------------------
void Skeleton::updateRotation()
{
    /// apply current orientaion from MOTIONER device
    for (size_t i=0; i<mJoints.size(); i++) {
        /// calibration
        const ofQuaternion &baseQuat = mBaseFrame.rotation.at(i) * ofQuaternion(90.0f, ofVec3f(0.0f, 0.0f, 1.0f));

        const ofQuaternion &currQuat = mCurrentFrame.rotation.at(i) * ofQuaternion(90.0f, ofVec3f(0.0f, 0.0f, 1.0f));
        const ofQuaternion calibratedQuat = baseQuat.inverse() * currQuat;
        
        if (mJoints.at(i).enable) {
            mJoints.at(i).setGlobalOrientation(calibratedQuat);
            mJoints.at(i).velocity = mCurrentFrame.velocity.at(i);
        }
    }
    
//    ofQuaternion base = ofQuaternion(45.0, ofVec3f(0.0, 1.0, 0.0));
//    
//    mJoints.at(JOINT_HIPS).setGlobalOrientation(base);
//    
//    mJoints.at(JOINT_LEFT_HIP).setGlobalOrientation(ofQuaternion(-70, ofVec3f(1.0, 0.0, 0.0)) * base);
//    mJoints.at(JOINT_LEFT_KNEE).setGlobalOrientation(ofQuaternion(70, ofVec3f(1.0, 0.0, 0.0)) * base);
//    
//    mJoints.at(JOINT_RIGHT_HIP).setGlobalOrientation(ofQuaternion(-70, ofVec3f(1.0, 0.0, 0.0)) * base);
//    mJoints.at(JOINT_RIGHT_KNEE).setGlobalOrientation(ofQuaternion(70, ofVec3f(1.0, 0.0, 0.0)) * base);
    
    if (mJoints.empty() == false) {
        mJoints.at(0).rotate(ofQuaternion(mOrientationY, ofVec3f(0.0, 1.0, 0.0)));
    }
    
    for (size_t i=0; i<mJoints.size(); i++) {
        mJoints.at(i).update();
    }
}

//----------------------------------------------------------------------------------------
void Skeleton::draw()
{
    OFX_BEGIN_EXCEPTION_HANDLING
    if (isEnable(RENDER)) {
        mModules->renderer.draw(this);
        mModules->picker.update(this); /// must call here
    }
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::drawHUD()
{
    OFX_BEGIN_EXCEPTION_HANDLING
    if (isEnable(RENDER)) {
        mModules->renderer.drawHUD(this);
    }
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::drawPicker()
{
    OFX_BEGIN_EXCEPTION_HANDLING
    if (isEnable(RENDER)) {
        mModules->picker.debugDraw(this);
    }
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::calibrate()
{
    mBaseFrame = mCurrentFrame;
        
    for (int i=0; i<mBaseFrame.rotation.size(); i++) {
        ofLogNotice("Skeleton") << "Caribrated node at " << i << ": " << mBaseFrame.rotation.at(i).getEuler();
    }
    
    mModules->settings.saveCalibration(this);
    
    updateRotation();
    
    mInitialPoseFrame = mCurrentFrame;
}

//----------------------------------------------------------------------------------------
void Skeleton::loadSettings(const string &fileName)
{
    OFX_BEGIN_EXCEPTION_HANDLING
    /// general settings for this skeleton
    mModules->settings.load(fileName);
    
    if (mModules->settings.hasTree()) {
        mModules->settings.loadTree(this);
    }
    else {
        createTree();
    }
    
    /// hierarchy settings for this skeleton
    mModules->settings.loadHierarchy(this);
    /// default caribrated values
    mModules->settings.loadCalibration(this);
    
    mModules->settings.loadUnuseJoints(this);
    
    mModules->settings.loadFlags(this);
    
    mModules->settings.loadColor(this);
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::saveSettings(const string &fileName)
{
    OFX_BEGIN_EXCEPTION_HANDLING
    mModules->settings.save(fileName);
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::suicide()
{
    SkeletonPtr skl = skeleton::SkeletonManager::getInstance().getSkeleton(getHostName()); /// move owner to me
    skeleton::SkeletonManager::getInstance().removeSkeleton(getHostName());
} /// delete shred_ptr

//----------------------------------------------------------------------------------------
void Skeleton::editHierarchy(int nodeId, const ofVec3f &offset)
{
    OFX_BEGIN_EXCEPTION_HANDLING
    Node &n = mJoints.at(nodeId);
    n.setPosition(offset);
    mModules->settings.saveHierarchy(nodeId, offset);
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::prepareRecording(const string &filePath)
{
    ofLogNotice("Skeleton") << "Recoeder (" << getName() << ") is opening a file \"" + filePath + "\"...";
    mModules->recorder.open(filePath, this);
}

//----------------------------------------------------------------------------------------
void Skeleton::finishRecording()
{
    mModules->recorder.close(this);
    ofLogNotice("Skeleton") << "Recoeder (" << getName() << ") finished recording";
}

//----------------------------------------------------------------------------------------
void Skeleton::startRecording()
{
    ofLogNotice("Skeleton") << "Recoeder (" << getName() << ") start recording";
    enable(RECORD);
    //reportFlags();
}

//----------------------------------------------------------------------------------------
void Skeleton::stopRecording()
{
    ofLogNotice("Skeleton") << "Recoeder (" << getName() << ") stop recording";
    disable(RECORD);
    //reportFlags();
}

//----------------------------------------------------------------------------------------
void Skeleton::preparePlayback(const string &filePath)
{
    ofLogNotice("Skeleton") << "Player (" << getName()
    << ") is opening a file \"" + filePath + "\"...";
    mModules->player.open(filePath, this);
    disable(CAPTURE);
}

//----------------------------------------------------------------------------------------
void Skeleton::finishPlayback()
{
    if (isEnable(CAPTURE) == false) {
        ofLogNotice("Skeleton") << "Player (" << getName() << ") finished playback";
        mModules->player.stop();
        //mModules->player.close();
    }
}

//----------------------------------------------------------------------------------------
void Skeleton::startPlayback()
{
    if (isEnable(CAPTURE) == false) {
        ofLogNotice("Skeleton") << "Player (" << getName() << ") start playback";
        enable(PLAY);
        mModules->player.play();
    }
}

//----------------------------------------------------------------------------------------
void Skeleton::pausePlayback()
{
    if (isEnable(CAPTURE) == false) {
        ofLogNotice("Skeleton") << "Player (" << getName() << ") pause playback";
        enable(PLAY);
        mModules->player.setPaused(true);
    }
}

//----------------------------------------------------------------------------------------
void Skeleton::stopPlayback()
{
    if (isEnable(CAPTURE) == false) {
        ofLogNotice("Skeleton") << "Player (" << getName() << ") stop playback";
        disable(PLAY);
        mModules->player.stop();
    }
}

//----------------------------------------------------------------------------------------
void Skeleton::setLoopPlayback(ofLoopType state)
{
    if (isEnable(CAPTURE) == false) {
        ofLogNotice("Skeleton") << "Player (" << getName() << ") set loop state";
        mModules->player.setLoopState(state);
    }
}

//----------------------------------------------------------------------------------------
void Skeleton::reportFlags()
{
    ofLogNotice() <<
    "PLAY:" << isEnable(PLAY) <<
    " RECORD:" << isEnable(RECORD) <<
    " CAPTURE:" << isEnable(CAPTURE) <<
    " RENDER:" << isEnable(RENDER) <<
    " EASY_IK:" << isEnable(EASY_IK) <<
    " VELOCITY_IK:" << isEnable(VELOCITY_IK) <<
    " CIRCLE_TRACKER_IK:" << isEnable(CIRCLE_TRACKER_IK) <<
    " OSC_OUT:" << isEnable(OSC_OUT);
}

//----------------------------------------------------------------------------------------
void Skeleton::onMessageReceived(ofxEventMessage &m)
{
    OFX_BEGIN_EXCEPTION_HANDLING
    
    const string addr = m.getAddress();
    if (addr==event::ADDRESS_RESET_POSITION) {
        const string name = m.getArgAsString(0);
        if (name==getName())
            resetPosition();
    }
    else if (addr==event::ADDRESS_CALIBRATION) {
        const string name = m.getArgAsString(0);
        if (name==getName())
            calibrate();
    }
    else if (addr==event::ADDRESS_SAVE_SKELETON) {
        const string name = m.getArgAsString(0);
        if (name==getName())
            saveSettings();
    }
    else if (addr==event::ADDRESS_SET_ORIENTATION) {
        mOrientationY = m.getArgAsFloat(0);
    }
    else if (addr==event::ADDRESS_SET_AUTO_RESET_DIMENSION) {
        mAutoResetDimension = m.getArgAsFloat(0);
    }
    else if (addr==event::ADDRESS_FIX_POSITION) {
        mFixPosition = m.getArgAsInt32(0);
        if (mFixPosition) {
            mModules->easyIK.setFixedAxis(true, false, true);
        }
        else {
            mModules->easyIK.setFixedAxis(false, false, false);
        }
    }
    else if (addr==event::ADDRESS_SET_LOWPASS) {
        mLowpass = m.getArgAsFloat(0);
    }
    else if (addr==event::ADDRESS_EXTERNAL_POSITION) {
        const string name = m.getArgAsString(0);
        if (name == mModules->settings.mName) {
            ofVec3f pos;
            pos.z = m.getArgAsFloat(1);
            pos.y = m.getArgAsFloat(2);
            pos.x = -m.getArgAsFloat(3);
            
            ofVec3f head = mJoints.at(JOINT_HEAD).getGlobalPosition();
            ofVec3f hips = mJoints.at(JOINT_HIPS).getGlobalPosition();
            head.y = hips.y = 0.0;
            ofVec3f sub = head - hips;
            sub.y = 0.0;
            float scale = 0.65;
//            const float camH = 10000.0f;
//            const float h = mJoints.at(JOINT_HEAD).getGlobalPosition().y;
//            if (h != 0.0f) {
//                scale = (camH - h) / camH;
//            }
           // cout << pos << " / " << scale << " / " << sub << endl;
            //pos *= scale;
            
            //mModules->easyIK.setExternalPosition(pos-sub);
            mModules->easyIK.setExternalPosition(pos*scale+sub);
        }
    }
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
//const string &Skeleton::getXmlFileName() const
//{
//    OFX_BEGIN_EXCEPTION_HANDLING
//    return mModules->settings.getFileName();
//    OFX_END_EXCEPTION_HANDLING
//    static string err("");
//    return err;
//}

//----------------------------------------------------------------------------------------
void Skeleton::createTree()
{
    OFX_BEGIN_EXCEPTION_HANDLING
    mJoints.at(JOINT_ABDOMEN).setParent(mJoints.at(JOINT_HIPS));
	{
		mJoints.at(JOINT_CHEST).setParent(mJoints.at(JOINT_ABDOMEN));
		{
			mJoints.at(JOINT_NECK).setParent(mJoints.at(JOINT_CHEST));
			{
				mJoints.at(JOINT_HEAD).setParent(mJoints.at(JOINT_NECK));
			}
		}
	}
    mJoints.at(JOINT_LEFT_COLLAR).setParent(mJoints.at(JOINT_ABDOMEN));
    {
        mJoints.at(JOINT_LEFT_SHOULDER).setParent(mJoints.at(JOINT_LEFT_COLLAR));
        //mJoints.at(JOINT_LEFT_SHOULDER).setParent(mJoin xts.at(JOINT_CHEST));
        {
            mJoints.at(JOINT_LEFT_ELBOW).setParent(mJoints.at(JOINT_LEFT_SHOULDER));
            //mJoints.at(JOINT_LEFT_ELBOW).setParent(mJoints.at(JOINT_LEFT_COLLAR));
            {
                mJoints.at(JOINT_LEFT_WRIST).setParent(mJoints.at(JOINT_LEFT_ELBOW));
                {
                    mJoints.at(JOINT_LEFT_HAND).setParent(mJoints.at(JOINT_LEFT_WRIST));
                }
            }
        }
    }
    
    mJoints.at(JOINT_RIGHT_COLLAR).setParent(mJoints.at(JOINT_ABDOMEN));
    {
        mJoints.at(JOINT_RIGHT_SHOULDER).setParent(mJoints.at(JOINT_RIGHT_COLLAR));
        //mJoints.at(JOINT_RIGHT_SHOULDER).setParent(mJoints.at(JOINT_CHEST));
        {
            mJoints.at(JOINT_RIGHT_ELBOW).setParent(mJoints.at(JOINT_RIGHT_SHOULDER));
            //mJoints.at(JOINT_RIGHT_ELBOW).setParent(mJoints.at(JOINT_RIGHT_COLLAR));
            {
                mJoints.at(JOINT_RIGHT_WRIST).setParent(mJoints.at(JOINT_RIGHT_ELBOW));
                {
                    mJoints.at(JOINT_RIGHT_HAND).setParent(mJoints.at(JOINT_RIGHT_WRIST));
                }
            }
        }
    }
    
	mJoints.at(JOINT_LEFT_HIP).setParent(mJoints.at(JOINT_HIPS));
	{
        
		mJoints.at(JOINT_LEFT_KNEE).setParent(mJoints.at(JOINT_LEFT_HIP));
		{
			mJoints.at(JOINT_LEFT_ANKLE).setParent(mJoints.at(JOINT_LEFT_KNEE));
			{
				mJoints.at(JOINT_LEFT_TOE).setParent(mJoints.at(JOINT_LEFT_ANKLE));
			}
		}
	}
    
	mJoints.at(JOINT_RIGHT_HIP).setParent(mJoints.at(JOINT_HIPS));
	{
		mJoints.at(JOINT_RIGHT_KNEE).setParent(mJoints.at(JOINT_RIGHT_HIP));
		{
			mJoints.at(JOINT_RIGHT_ANKLE).setParent(mJoints.at(JOINT_RIGHT_KNEE));
			{
				mJoints.at(JOINT_RIGHT_TOE).setParent(mJoints.at(JOINT_RIGHT_ANKLE));
			}
		}
	}
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Skeleton::setHostName(const string &host)
{
    mModules->settings.mDeviceHostName = host;
}

//----------------------------------------------------------------------------------------
void Skeleton::setName(const string &name)
{
    mModules->settings.setName(name);
}

//----------------------------------------------------------------------------------------
void Skeleton::setState(int state)
{
    mModules->renderer.setState(state);
}

//----------------------------------------------------------------------------------------
void Skeleton::setActilveJoint(int joint)
{
    mModules->renderer.setActiveJoint(joint);
}

//----------------------------------------------------------------------------------------
void Skeleton::setDrawDebug(bool bDraw)
{
    mDrawDebug = bDraw;
    mModules->picker.setDrawDebug(mDrawDebug);
}

//----------------------------------------------------------------------------------------
void Skeleton::toggleDrawDebug()
{
    mDrawDebug ^= true;
    setDrawDebug(mDrawDebug);
}

//----------------------------------------------------------------------------------------
void Skeleton::setCircleTrackerId(int id)
{
    mModules->circleTrackerIK.setCircleId(id);
}

//----------------------------------------------------------------------------------------
void Skeleton::setEnableCircleTracker(bool bEnable)
{
    if (bEnable) {
        enable(CIRCLE_TRACKER_IK);
        disable(EASY_IK);
    }
    else {
        disable(CIRCLE_TRACKER_IK);
        enable(EASY_IK);
    }
}

//----------------------------------------------------------------------------------------
void Skeleton::setEnableOscOut(bool bEnable)
{
    bEnable ? enable(OSC_OUT) : disable(OSC_OUT);
}

//----------------------------------------------------------------------------------------------
void Skeleton::setDisableJoint(int joint, bool bDisable)
{
    if (joint<0 || joint>=NUM_JOINTS)
        ofxThrowException(ofxException, "joint out of index");
    mJoints.at(joint).enable = !bDisable;
    if (bDisable) {
        if (mModules->settings.hasTree()) {
            mModules->settings.loadTree(this);
        }
        else {
            createTree();
        }
        mModules->settings.loadHierarchy(this);
        
        mModules->settings.loadFlags(this);
        
        mModules->settings.loadColor(this);
    }
}

void Skeleton::setColorR(int c)
{
    mColor.r = c;
    mModules->settings.saveColor(this);
}

void Skeleton::setColorG(int c)
{
    mColor.g = c;
    mModules->settings.saveColor(this);
}

void Skeleton::setColorB(int c)
{
    mColor.b = c;
    mModules->settings.saveColor(this);
}

//----------------------------------------------------------------------------------------
bool Skeleton::getEnableOscOut() const
{
    return isEnable(OSC_OUT);
}

//----------------------------------------------------------------------------------------
const string &Skeleton::getName() const
{
    return mModules->settings.mName;
}

//----------------------------------------------------------------------------------------
const string &Skeleton::getHostName() const
{
    return mModules->settings.mDeviceHostName;
}

const string &Skeleton::getSettingsFileName() const
{
    return mModules->settings.mFileName;
}

//----------------------------------------------------------------------------------------
int Skeleton::getCircleTrackerId() const
{
    return mModules->circleTrackerIK.getCircleId();
}

//----------------------------------------------------------------------------------------
void Skeleton::resetPosition(const ofVec3f &pos)
{
    mModules->easyIK.setPosition(pos);
}

//-----------------------------------------------------------------------------------------
ofVec3f Skeleton::getResetPosition() const
{
    return mModules->easyIK.getOffset();
}

//----------------------------------------------------------------------------------------
int Skeleton::getJointIndexFromName(const string& name) const
{
    for (int i=0; i<mJoints.size(); i++) {
        if (mJoints.at(i).name == name) return i;
    }
    ofxThrowException(ofxException, "joint id not found!");
}

//----------------------------------------------------------------------------------------
void Skeleton::setLowpassFilterRate(float rate)
{
    mLowpass = rate;
}

//----------------------------------------------------------------------------------------
float Skeleton::getLowpassFilterRate() const
{
    return mLowpass;
}

//----------------------------------------------------------------------------------------
SkeletonFinder::SkeletonFinder(const string &name) : name(name)
{
}

//----------------------------------------------------------------------------------------
bool SkeletonFinder::operator()(const pair<string, ram::skeleton::SkeletonPtr> &itr)
{
    return (name == itr.second->mModules->settings.mDeviceHostName);
}








