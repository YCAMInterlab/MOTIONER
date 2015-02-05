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
#include "ramSimpleShadow.h"

//----------------------------------------------------------------------------------------
MotionerApp::MotionerApp() :
mEnableDraw(true),
mEnableUpdateSkeletons(true)
{
    mOffset.set(0.0f, -1000.0f, 0.0f);
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
    
    /// osc
    //--------------------
    mOscReceiver.setup(ram::DEBUG_OSC_INCOMING_PORT);
    
#ifdef DEBUG
    
    /// add test skeleton (DEBUG
    //--------------------
    ram::skeleton::SkeletonManager::getInstance().addSkeleton("192.168.2.5");
    ram::skeleton::SkeletonManager::getInstance().addSkeleton("192.168.2.6");
    
#endif
    
    mShadow = ofPtr<ramSimpleShadow>(new ramSimpleShadow());
    mShadow->setup();
    
    windowResized(ofGetWidth(), ofGetHeight());
    
    ofSetEscapeQuitsApp(false);
    
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
    
    if (mOscReceiver.getEnabled())
        mOscReceiver.update();
    
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
        
        ofColor c2(230, 230, 250);
        ofColor c3(100, 100, 150);
        
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
        
        ofTranslate(mOffset);
        
        const float time = ofGetElapsedTimef();
        
        { // update lights
            ofSetSmoothLighting(true);
            
            mLightW.setPosition(-2500.f, 8000.f, 4000.f);
            mLightW.setDiffuseColor(ofFloatColor(0.3f, 0.3f, 0.3f));
            mLightW.setAmbientColor(ofFloatColor(0.1f, 0.1f, 0.1f));
            
            mLightR.setPosition(-3000.f * ::cosf(time * 1.15f),
                                4000.f * ::sinf(time * 0.24f) + 4000.f,
                                5000.f * ::cosf(time * 0.5f));
            mLightR.setPointLight();
            mLightR.setAttenuation(0.f, 0.00015f);
            mLightR.setDiffuseColor(ofFloatColor(0.3f, 0.15f, 0.15f));
            mLightR.setAmbientColor(ofFloatColor(0.05f, 0.025f, 0.025f));
            
            mLightG.setPosition( 6000.f * ::cosf(time * 1.51f),
                                3000.f * ::sinf(time * 0.42f) + 3000.f,
                                -4000.f * ::cosf(time * 0.6f));
            mLightG.setPointLight();
            mLightG.setAttenuation(0.f, 0.00015f);
            mLightG.setDiffuseColor(ofFloatColor(0.15f, 0.3f, 0.15f));
            mLightG.setAmbientColor(ofFloatColor(0.025f, 0.05f, 0.025f));
            
            mLightB.setPosition( 3500.f * ::cosf(time * 0.81f),
                                2500.f * ::sinf(time * 0.16f) + 2500.f,
                                -2000.f * ::cosf(time * 0.5f));
            mLightB.setPointLight();
            mLightB.setAttenuation(0.f, 0.00015f);
            
            mLightB.setDiffuseColor(ofFloatColor(0.15f, 0.15f, 0.3f));
            mLightB.setAmbientColor(ofFloatColor(0.025f, 0.025f, 0.05f));
        }
        
        ofEnableLighting();
        mLightW.enable();
        mLightR.enable();
        mLightG.enable();
        mLightB.enable();
        
        glEnable(GL_DEPTH_TEST);
        ofPushMatrix();
        ofTranslate(0.f, -2.f); // bias
        mGrid.draw();
        ofPopMatrix();
        
        ram::skeleton::SkeletonManager::getInstance().drawSkeletons();
        ofPopStyle();
        
        mLightB.disable();
        mLightR.disable();
        mLightG.disable();
        mLightW.disable();
        ofDisableLighting();
        
        ofPushMatrix();
        ofEnableAlphaBlending();
        glEnable(GL_DEPTH_TEST);
        ofMatrix4x4 modelview;
        glGetFloatv(GL_MODELVIEW, modelview.getPtr());
        
        mShadow->setLightPosition(mLightW.getGlobalPosition());
        mShadow->begin(modelview);
        ram::skeleton::SkeletonManager::getInstance().drawSkeletons();
        mShadow->end();
        ofPopMatrix();
        
        cam->end();
    }
    ofxPopAll();
    
    ofxPushAll();
    {
        ram::skeleton::SkeletonManager::getInstance().drawSkeletonsHUD();
        ram::skeleton::SkeletonManager::getInstance().drawSkeletonsPicker();
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












