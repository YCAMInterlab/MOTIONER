#include "MotionerApp.h"

#include "ramConstants.h"
#include "ramSkeletonManager.h"
#include "ramUtils.h"
#include "ramSkeleton.h"
#include "ramMesh.h"
#include "ramUI.h"
#include "ramBinaryCoder.h"
#include "ramLogger.h"
#include "ramCameraManager.h"
#include "ramEvent.h"
#include "ramDeviceCorrespondent.h"
#include "ofxSimpleKalmanFilter.h"

//----------------------------------------------------------------------------------------
MotionerApp::MotionerApp() :
mEnableDraw(true),
mEnableUpdateSkeletons(true)
{
    mOffset.set(0.0f, -600.0f, 0.0f);
}

//----------------------------------------------------------------------------------------
MotionerApp::~MotionerApp()
{
}

//----------------------------------------------------------------------------------------
void MotionerApp::setup()
{
    OFX_BEGIN_EXCEPTION_HANDLING
    
    /// setup oF
    //--------------------
    ofSetFrameRate(60);
    ofSetVerticalSync(false);
    ofPtr<ofBaseLoggerChannel> logger = ofPtr<ofBaseLoggerChannel>(new ram::LoggerChannel());
    ofSetLoggerChannel(logger);
    ofSetLogLevel(ram::DEFAULT_LOG_LEVEL);
    
    ofAddListener(ofxEvent(), this, &MotionerApp::onMessageReceived);
    
    /// initialize Motioner modules
    //--------------------
    ram::initialize();
    ram::UI::getInstance().setup();
    ram::DeviceCorrespondent::getInstance().setup();
        
    ofLogNotice() << "Initializing 3D graphics...";
    
    /// display settings
    /// primitives
    //--------------------
    mGrid = ram::Mesh::createGrid();
    
#ifdef DEBUG
    /// osc
    //--------------------
    mOscReceiver.setup(ram::DEBUG_OSC_INCOMING_PORT);
    
    /// add test skeleton (DEBUG
    //--------------------
    //ram::addSkeleton("192.168.2.5");
    //ram::getSkeleton(0)->setName("Bob");
    //ram::addSkeleton("192.168.2.4");
    
#endif
    
    mLight.setGlobalPosition(-1000.0, 3000.0, 1000.0);
    
    windowResized(ofGetWidth(), ofGetHeight());
    
    ofLogNotice() << "Setup finished!";
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void MotionerApp::exit()
{
    ofPtr<ofBaseLoggerChannel> logger =
    ofPtr<ofBaseLoggerChannel>(new ofConsoleLoggerChannel());
    ofSetLoggerChannel(logger);
    
    ofLogNotice() << "Closing app...";
    //ram::UI::getInstance().exit();
    ram::DeviceCorrespondent::getInstance().exit();
    ram::determinate();
}

//----------------------------------------------------------------------------------------
void MotionerApp::update()
{
    OFX_BEGIN_EXCEPTION_HANDLING
    
    /// skeleton
    //--------------------
    if (mEnableUpdateSkeletons)
        ram::skeleton::SkeletonManager::getInstance().updateSkeletons();
    
    /// osc
    //--------------------
    if (mOscSender.getEnabled())
        mOscSender.update();
    
#ifdef DEBUG
    if (mOscReceiver.getEnabled())
        mOscReceiver.update();
#endif
    
    /// UI
    //--------------------
    ram::UI::getInstance().update();
    
    /// Device
    //--------------------
    ram::DeviceCorrespondent::getInstance().update();
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void MotionerApp::draw()
{
    OFX_BEGIN_EXCEPTION_HANDLING
    
    if (!mEnableDraw) return;
    
    ofColor c1;
    c1.setHex(ram::COLOR_BG);
    
    /// to event
    ofRectangle viewRect = ram::UI::getInstance().getViewRect();
    
    //--------------------
    ofxPushAll();
    {
        ofBackground(c1);
    }
    ofxPopAll();
    
    //--------------------
    ofxPushAll();
    {
        ram::UI::getInstance().draw();
    }
    ofxPopAll();
    
    //--------------------
    ofxPushAll();
    {
        ofViewport(viewRect);
        
        ofVec3f v(c1.r, c1.g, c1.b);
        v *= 1.3;
        ofColor c3(v.x, v.y, v.z);
        v*=0.2;
        ofColor c2(v.x, v.y, v.z);
        
        ofBackgroundGradient(c3, c2);
    }
    ofxPopAll();
    
    //--------------------
    ofxPushAll();
    {
        ofSetColor(ofColor::white);
        /// to event
        ram::CameraPtr cam = ram::CameraManager::getInstance().getCurrentCamera();
        cam->begin(viewRect);
        
        glDisable(GL_DEPTH_TEST);
        glShadeModel(GL_SMOOTH);
        
        ofEnableSmoothing();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        
        ofTranslate(mOffset);
        
        ofDrawAxis(200.0f);
        mGrid.draw();
        
        ram::Mesh::drawRuler();
        
        mLight.enable();
        ram::skeleton::SkeletonManager::getInstance().drawSkeletons();
        mLight.disable();
        
                
        cam->end();
    }
    ofxPopAll();
    
    ofxPushAll();
    {
        ram::skeleton::SkeletonManager::getInstance().drawSkeletonsHUD();
        ram::UI::getInstance().drawHUD();
    }
    ofxPopAll();
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void MotionerApp::keyPressed(int key)
{
    //--------------------
    ram::UI::getInstance().keyPressed(key);
    
    switch (key) {
        case '1':
        {
            ofxEventMessage m;
            m.setAddress(ram::event::ADDRESS_SET_CAMERA_ROTATION);
            m.addIntArg(static_cast<int>(true));
            ofxNotifyEvent(m);
        }
            break;
        case '2':
        {
            ofxEventMessage m;
            m.setAddress(ram::event::ADDRESS_SET_CAMERA_TRANSLATION);
            m.addIntArg(static_cast<int>(true));
            ofxNotifyEvent(m);
        }
            break;
        case '3':
        {
            ofxEventMessage m;
            m.setAddress(ram::event::ADDRESS_SET_CAMERA_TRANSLATION);
            m.addIntArg(static_cast<int>(false));
            ofxNotifyEvent(m);
            m.setAddress(ram::event::ADDRESS_SET_CAMERA_ROTATION);
            m.addIntArg(static_cast<int>(false));
            ofxNotifyEvent(m);
        }
            break;
        case ' ':
        {
            mEnableUpdateSkeletons ^= true;
            ofxEventMessage m;
            m.setAddress(ram::event::ADDRESS_PLAY_MOTION);
            m.addIntArg(static_cast<int>(mEnableUpdateSkeletons));
            ofxNotifyEvent(m);
        }
            break;
    }
}

//----------------------------------------------------------------------------------------
void MotionerApp::windowResized(int w, int h)
{
    //--------------------
    ofSetWindowTitle("MOTIONER");
    
    //if (ofGetWidth()>ofGetScreenWidth())
    //    ofSetWindowShape(ofGetScreenWidth(), ofGetHeight());
    //
    //if (ofGetHeight()>ofGetScreenHeight())
    //    ofSetWindowShape(ofGetWidth(), ofGetScreenHeight());
    
    //--------------------
    ram::UI::getInstance().resize();
}

//----------------------------------------------------------------------------------------
void MotionerApp::onMessageReceived(ofxEventMessage &m)
{
    const string addr = m.getAddress();
    if (addr==ram::event::ADDRESS_TOGGLE_DRAW) {
        mEnableDraw = m.getArgAsInt32(0);
    }
    else if (addr==ram::event::ADDRESS_ENABLE_OSC) {
        mOscSender.setup(m.getArgAsString(0), m.getArgAsInt32(1));
        mOscSender.setEnabled(true);
    }
    else if (addr==ram::event::ADDRESS_DISABLE_OSC) {
        mOscSender.setEnabled(false);
    }
    else if (addr==ram::event::ADDRESS_PLAY_MOTION) {
        mEnableUpdateSkeletons = static_cast<bool>(m.getArgAsInt32(0));
    }
}

//----------------------------------------------------------------------------------------
void MotionerApp::dragEvent(ofDragInfo dragInfo)
{
    vector<string> files = dragInfo.files;
    
    for (int i=0; i<files.size(); i++) {
        /// get extension
        const string path = files.at(i);
        /// get name
        if (ram::getFileExt(path) == ram::MOTION_DATA_EXT) {
            const string name = ram::getFileName(path);
            ram::openPlaybackFile(path, name);
        }
        else {
            ofLogWarning() << "Unknown file was dragged:" <<  ram::getFileName(path);
        }
    }
}












