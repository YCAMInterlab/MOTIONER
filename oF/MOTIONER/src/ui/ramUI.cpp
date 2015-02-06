//
//  ramUI.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/9/12.
//
//

#include "ramUI.h"

#include "ramUtils.h"
#include "ramSkeleton.h"
#include "ramSkeletonManager.h"
#include "ramInspectorUI.h"
#include "ramEvent.h"

#include "ofxUI.h"
#include "ofxException.h"
#include "ofxUITabbedCanvas.h"
#include "ofxEvent.h"


using namespace ram;

//----------------------------------------------------------------------------------------
UI::UI() :
mIsInited(false),
mWidth(200.0f),
mHeight(ofGetHeight()),
mLogger(NULL),
mDeviceIncomingPortInput(NULL),
mOscOutHostInput(NULL),
mOscOutPortInput(NULL),
mEnableOscToggle(NULL),
mPlayToggle(NULL),
mRecording(false),
mDetailedRendering(true),
mWillOpenMotionFile(false),
mLogOffset(0),
mNumLines(0),
mLogBufferSize(LOGGER_DEFAULT_BUFFER_SIZE)
{
}

//----------------------------------------------------------------------------------------
UI::~UI()
{
}

//----------------------------------------------------------------------------------------
UI &UI::getInstance()
{
    static UI instance;
    return instance;
}

//----------------------------------------------------------------------------------------
void UI::setup(const ofRectangle &rect)
{
    if (mIsInited) return;
    mIsInited = true;
    
    ofAddListener(ofxEvent(), this, &UI::onNotifyEvent);
    ofLogNotice("UI") << "Initializing UI...";
    
    ofxUIWidget *widget = NULL;
    ofColor outlineColor;
    outlineColor.setHex(0x494949);
    
    //mSkeletonMap = &shared().getSkeletonMap();
    
    /// create gui objects
    const float space = OFX_UI_GLOBAL_WIDGET_SPACING;
    const float w = mWidth+space;
    const float h = ofGetHeight();
    
    ofSetLogLevel(OF_LOG_FATAL_ERROR); /// shout ofxUI out
    
    general = new ofxUICanvas(w, 0.0f, w, h);
    //general->setSnapping(false);
    
    ofSetLogLevel(DEFAULT_LOG_LEVEL);
    general->setFont(GUI_FONT, false); /// aliased
    
    //--------------------------------------------------//
    //--------------------------------------------------//
    //------------------ LOGGER ------------------------//
    //--------------------------------------------------//
    //--------------------------------------------------//
    float lx = w*NUM_PANELS;
    float ly = ofGetHeight()-200.0f;
    float lw =ofGetWidth()-lx;
    float lh = 200.0f;
    ofSetLogLevel(OF_LOG_FATAL_ERROR); /// shout ofxUI out
    mLogger =new ofxUICanvas(lx, ly, lw, lh);
    mLogger->setDrawOutline(true);
    mLogger->setDrawBack(false);
    //    mLogger->disableAppEventCallbacks();
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        mLogger->setColorBack(b);
        mLogger->setColorFill(f);
        mLogger->setColorOutline(o);
    }
    mLogger->setFont(GUI_FONT, false); /// aliased
    ofSetLogLevel(DEFAULT_LOG_LEVEL);
    //--------------------------------------------------//
    //--------------------------------------------------//
    //--------------------------------------------------//
    
    //--------------------------------------------------//
    //--------------------------------------------------//
    //------------------ Tools -------------------------//
    //--------------------------------------------------//
    //--------------------------------------------------//
    
    const float btnDim = 26.0f;
    const float btnMgn = OFX_UI_GLOBAL_WIDGET_SPACING;
    const int nButtons = 1;
    
    ofSetLogLevel(OF_LOG_FATAL_ERROR); /// shout ofxUI out
    mTools =new ofxUICanvas(mWidth*1.1f,
                            6.0f,
                            (btnDim+btnMgn)*nButtons + static_cast<float>(btnMgn),
                            (btnDim+btnMgn)*1.0f + btnMgn);
    mTools->setDrawOutline(false);
    mTools->setDrawBack(true);
    /// colors
    {
        ofColor b, f, o, oh;
        b.setHex(COLOR_L, 255);
        f.setHex(COLOR_HILIGHT);
        o.setHex(COLOR_HILIGHT);
        oh.setHex(COLOR_M);
        mTools->setColorBack(b);
        mTools->setColorFill(f);
        mTools->setColorOutlineHighlight(oh);
        
        mTools->setFont(GUI_FONT, false); /// aliased
        ofSetLogLevel(DEFAULT_LOG_LEVEL);
        
        widget = mTools->addWidgetRight(new ofxUIImageToggle(btnDim,
                                                             btnDim,
                                                             false,
                                                             "images/play.png",
                                                             "PLAY_MOTION"));
        
        widget->setColorBack(oh);
        mPlayToggle = static_cast<ofxUIImageToggle *>(widget);
        mPlayToggle->setValue(true);
        
        mTools->autoSizeToFitWidgets();
    }
    //--------------------------------------------------//
    //--------------------------------------------------//
    //--------------------------------------------------//
    
    /// setup guis
    const float hw = mWidth*0.6-OFX_UI_GLOBAL_WIDGET_SPACING;
    
    const float hh = 10.0f;
    
    
    const float buttonSize = 20.0f;
    
    /// for drop down menu
    vector<string> emptyItems;
    emptyItems.clear();
    
    //--------------------------------------------------//
    //--------------------------------------------------//
    //------------------ GENERAL -----------------------//
    //--------------------------------------------------//
    //--------------------------------------------------//
    
    //ofxUICanvas *general = mGui.at(GENERAL);
    general->setDrawOutline(true);
    const float genW = mWidth;
    const float genHw = genW-OFX_UI_GLOBAL_WIDGET_SPACING*2.0f;
    
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        general->setColorBack(b);
        general->setColorFill(f);
        general->setColorOutline(o);
    }
    
    general->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    //--------------------------------------------------
    /// GENERAL - OSC OUT
    {
        general->addLabel("OSC OUT", OFX_UI_FONT_MEDIUM);
        
        general->addLabel("HOST    :", OFX_UI_FONT_SMALL);
        widget = general->addWidgetRight(new ofxUITextInput("OSC_OUT_HOST",
                                                            "INPUT HOST",
                                                            hw,
                                                            0,
                                                            0,
                                                            0,
                                                            OFX_UI_FONT_SMALL));
        mOscOutHostInput = static_cast<ofxUITextInput *>(widget);
        mOscOutHostInput->setDrawOutline(true);
        mOscOutHostInput->setAutoClear(false);
        mOscOutHostInput->setColorOutline(outlineColor);
        
        general->addLabel("PORT    :", OFX_UI_FONT_SMALL);
        widget = general->addWidgetRight(new ofxUITextInput("OSC_OUT_PORT",
                                                            "INPUT PORT",
                                                            hw,
                                                            0,
                                                            0,
                                                            0,
                                                            OFX_UI_FONT_SMALL));
        
        general->addLabel("ENABLED :", OFX_UI_FONT_SMALL);
        
        mOscOutPortInput = static_cast<ofxUITextInput *>(widget);
        mOscOutPortInput->setDrawOutline(true);
        mOscOutPortInput->setAutoClear(false);
        mOscOutPortInput->setColorOutline(outlineColor);
        
        mEnableOscToggle = addImageToggle(general,
                                          buttonSize,
                                          "images/enable.png",
                                          "ENABLE OSC OUT",
                                          false);
    }
    
    //--------------------------------------------------
    /// GENERAL - OSC IN
    //{
    //    general->addSpacer(genHw, 1.0f);
    //
    //    general->addLabel("APP CONTROL OSC IN", OFX_UI_FONT_MEDIUM);
    //    general->addLabel("PORT:", OFX_UI_FONT_SMALL);
    //    widget = general->addWidgetDown(new ofxUITextInput("OSC_IN_PORT", "INPUT PORT", hw));
    //    mOscInHostInput = static_cast<ofxUITextInput *>(widget);
    //    mOscInHostInput->setDrawOutline(true);
    //    mOscInHostInput->setAutoClear(false);
    //    mOscInHostInput->setColorOutline(outlineColor);
    //}
    
    //--------------------------------------------------
    /// GENERAL - UDP INCOMING
    {
        general->addSpacer(genHw, 1.0f);
        
        general->addLabel("DEVICE DATA IN", OFX_UI_FONT_MEDIUM);
        
        general->addLabel("PORT    :", OFX_UI_FONT_SMALL);
        widget = general->addWidgetRight(new ofxUITextInput("DEVICE_INCOMING_UDP_PORT",
                                                            "9750",
                                                            hw,
                                                            0,
                                                            0,
                                                            0,
                                                            OFX_UI_FONT_SMALL));
        mDeviceIncomingPortInput = static_cast<ofxUITextInput *>(widget);
        mDeviceIncomingPortInput->setDrawOutline(true);
        mDeviceIncomingPortInput->setColorOutline(outlineColor);
        general->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        
        general->addLabel("AVILABLE SKELETONS:", OFX_UI_FONT_SMALL);
        mAvilableDeviceList = general->addDropDownList("CHOOSE A SKELETON", vector<string>());
        mAvilableDeviceList->setDrawOutline(true);
        mAvilableDeviceList->setDrawBack(true);
        mAvilableDeviceList->setColorOutline(outlineColor);
        mAvilableDeviceList->setAllowMultiple(false);
        mAvilableDeviceList->setColorBack(COLOR_L);
    }
    //--------------------------------------------------
    /// GENERAL - OSC IN
    //{
    //    general->addSpacer(genHw, 1.0f);
    //    general->addLabel("OSC IN", OFX_UI_FONT_LARGE);
    //    general->addLabel("PORT: 9999", OFX_UI_FONT_SMALL);
    //    general->addToggle("ENABLE", false, buttonSize, buttonSize);
    //}
    //-------------------------------------------r-------
    
    /// SKELETON
    {
        general->addSpacer(genHw, 1.0f);
        
        general->addLabel("SKELETON", OFX_UI_FONT_MEDIUM);
        
        mOrientation = general->addSlider("ORIENTATION-Y", 0.0f, 360.0f, 0.0f, genHw, hh);
        mOrientation->setColorOutline(outlineColor);
        mOrientation->setDrawOutline(true);
        
        mAutoResetDimenstion = general->addSlider("AUTO RESET", 0.0f, 10000.0f, 0.0f, genHw, hh);
        mAutoResetDimenstion->setColorOutline(outlineColor);
        mAutoResetDimenstion->setDrawOutline(true);
        
        mFixPosition = general->addToggle("FIX POSITION", false);
        mFixPosition->setColorOutline(outlineColor);
        mFixPosition->setDrawOutline(true);
    }
    //--------------------------------------------------
    
    
    /// GENERAL - RECORDER
    {
        general->addSpacer(genHw, 1.0f);
        
        general->addLabel("RECORDER", OFX_UI_FONT_MEDIUM);
        //general->addLabel("ELAPSED: 00:00:12", OFX_UI_FONT_SMALL);
        //general->addLabel("NUM FRAMES: 12345", OFX_UI_FONT_SMALL);
        //widget = addImageButton(general, buttonSize, "images/open.png", "OPEN REC FILES", true);
        
        widget = addImageButton(general, buttonSize, "images/record.png", "START REC", true);
        general->addWidgetRight(new ofxUILabel("", OFX_UI_FONT_SMALL));
        widget = addImageButton(general, buttonSize, "images/stop.png", "FINISH REC", false);
    }
    //--------------------------------------------------
    /// GENERAL - PLAYER
    {
        general->addSpacer(genHw, 1.0f);
        
        general->addLabel("PLAYER", OFX_UI_FONT_MEDIUM);
        //widget = general->addSlider("CURR TIME", 0.0f, 100.0f, 30.0f, genHw, hh);
        //widget->setDrawOutline(true);
        //widget->setColorOutline(outlineColor);
        //general->addLabel("CURR FRAMES: 12345", OFX_UI_FONT_SMALL);
        
        widget = addImageButton(general,
                                buttonSize,
                                "images/open.png",
                                "OPEN A MOTION FILE",
                                true);
        general->addWidgetRight(new ofxUILabel("", OFX_UI_FONT_SMALL));
        widget = addImageButton(general, buttonSize, "images/play.png", "PLAY ALL", false);
        general->addWidgetRight(new ofxUILabel("", OFX_UI_FONT_SMALL));
        widget = addImageButton(general, buttonSize, "images/pause.png", "PAUSE ALL", false);
        general->addWidgetRight(new ofxUILabel("", OFX_UI_FONT_SMALL));
        widget = addImageButton(general, buttonSize, "images/stop.png", "STOP ALL", false);
        general->addWidgetRight(new ofxUILabel("", OFX_UI_FONT_SMALL));
        ofxUIImageToggle *it = addImageToggle(general,
                                              buttonSize,
                                              "images/loop.png",
                                              "TOGGLE LOOP ALL",
                                              false);
        it->setValue(true);
    }
    //--------------------------------------------------
    /// GENERAL - APP SETTINGS
    {
        general->addSpacer(genHw, 1.0f);
        
        ofxUIImageToggle *it = addImageToggle(general,
                                              buttonSize,
                                              "images/show.png",
                                              "TOGGLE DETAILED RENDERING",
                                              true);
        it->setValue(true);
        
        general->addWidgetRight(new ofxUILabel("", OFX_UI_FONT_SMALL));
        
        addImageButton(general, buttonSize, "images/save.png", "SAVE SETTINGS", false);
        
        widget = general->addFPS(OFX_UI_FONT_SMALL);
        widget->setDrawOutline(false);
    }
    
    //--------------------------------------------------
    /// EXPERIMENT
    {
        general->addSpacer(genHw, 1.0f);
        
        mLowpass = general->addSlider("LOW-PASS", 0.0f, 1.0f, 0.2f, genHw, hh);
        mLowpass->setColorOutline(outlineColor);
        mLowpass->setDrawOutline(true);
        
        widget = general->addToggle("MOVE LIGHT", false);
        widget->setColorOutline(outlineColor);
        widget->setDrawOutline(true);
    }
    //--------------------------------------------------//
    //--------------------------------------------------//
    //--------------------------------------------------//
    
    
    
    general->setDrawWidgetPadding(false);
    ofAddListener(general->newGUIEvent, this, &UI::guiEvent);
    mLogger->setDrawWidgetPadding(false);
    ofAddListener(mLogger->newGUIEvent, this, &UI::guiEvent);
    mTools->setDrawPadding(false);
    ofAddListener(mTools->newGUIEvent, this, &UI::guiEvent);
    
    //----
    
    /// load a XML file for settings for GUI
    /// file name is ram::GUI_XML what decreared in ramConstants.h
    general->loadSettings(GUI_XML);
    
    if (mEnableOscToggle->getValue())
        enableOsc();
    
    //    notifyLowpassValue();
    
    //setupDeviceCorrespondent();
    
    mInspector.setup();
    
    mFont.loadFont(GUI_FONT, 32);
    
    //ofLogNotice("UI") << "Ready";
}

//----------------------------------------------------------------------------------------
ofxUIImageButton *UI::addImageButton(ofxUICanvas *canvas,
                                     int dimension,
                                     const string &path,
                                     const string &name,
                                     bool bNewLine)
{
    ofxUIWidget *widget = NULL;
    if (!bNewLine)
        widget = canvas->addWidgetRight(new ofxUIImageButton(dimension,
                                                             dimension,
                                                             false,
                                                             path,
                                                             name));
    else
        widget = canvas->addWidgetDown(new ofxUIImageButton(dimension,
                                                            dimension,
                                                            false,
                                                            path,
                                                            name));
    ofColor deactive, outline;
    deactive.setHex(COLOR_M);
    outline.setHex(COLOR_HILIGHT);
    widget->setColorBack(deactive);
    widget->setDrawOutline(false);
    //widget->setColorOutline(outline);
    return static_cast<ofxUIImageButton *>(widget);
}

//----------------------------------------------------------------------------------------
ofxUIImageToggle *UI::addImageToggle(ofxUICanvas *canvas,
                                     int dimension,
                                     const string &path,
                                     const string &name,
                                     bool bNewLine)
{
    ofxUIWidget *widget = NULL;
    if (!bNewLine)
        widget = canvas->addWidgetRight(new ofxUIImageToggle(dimension,
                                                             dimension,
                                                             false,
                                                             path,
                                                             name));
    else
        widget = canvas->addWidgetDown(new ofxUIImageToggle(dimension,
                                                            dimension,
                                                            false,
                                                            path,
                                                            name));
    ofColor deactive, hilight;
    deactive.setHex(COLOR_ML);
    hilight.setHex(COLOR_H);
    widget->setColorBack(deactive);
    widget->setDrawOutline(false);
    widget->setColorFill(hilight);
    widget->setColorOutlineHighlight(COLOR_M);
    return static_cast<ofxUIImageToggle *>(widget);
}

//----------------------------------------------------------------------------------------
void UI::exit()
{
    mInspector.exit();
    
    checkError();
    
    general->saveSettings(GUI_XML);
    
    ofRemoveListener(general->newGUIEvent, this, &UI::guiEvent);
    ofRemoveListener(mLogger->newGUIEvent, this, &UI::guiEvent);
    ofRemoveListener(mTools->newGUIEvent, this, &UI::guiEvent);
    
    if (general) {
        general->disable();
        delete general;
        general = NULL;
    }
    
    if (mLogger) {
        mLogger->disable();
        delete mLogger;
        mLogger = NULL;
    }
    
    ofRemoveListener(ofxEvent(), this, &UI::onNotifyEvent);
}

//----------------------------------------------------------------------------------------
void UI::update()
{
    if (mWillOpenMotionFile) {
        if (mFileDialogResult.bSuccess)
            openPlaybackFile(mFileDialogResult.getPath(),
                             mFileDialogResult.getName());
        mWillOpenMotionFile = false;
    }
    
    const float y = general->getRect()->getY();
    const float h = general->getRect()->getHeight();
    
    if (h<=ofGetHeight()-1.0f) {
        general->getRect()->setY(0.0f);
    }
    else {
        if (general->getRect()->getY() > 0.0f)
            general->getRect()->setY(0.0f);
        else if (y < -h+ofGetHeight())
            general->getRect()->setY(-h+ofGetHeight());
    }
}

//----------------------------------------------------------------------------------------
void UI::draw()
{
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate(5.0f, 17.f);
    ofTranslate(mLogger->getRect()->x, mLogger->getRect()->y);
    ofSetHexColor(COLOR_M);
    
    const float lineH = kBitmapStringFontSize + kBitmapStringPadding;
    float y =  -(mNumLines + 1) * lineH + mLogger->getRect()->height;
    y += mLogOffset * lineH;
    
    for (int i=0; i<mLog.size(); i++) {
        const string& line = mLog.at(i);
        const int nLines = ofSplitString(line, "\n").size() - 1;
        ofPushMatrix();
        ofTranslate(0.f, y);
        if (y >= -lineH && y < mLogger->getRect()->height)
            ofDrawBitmapString(line, ofVec3f::zero());
        y += nLines * lineH;
        ofPopMatrix();
    }
    
    ofPopStyle();
    ofPopMatrix();
}

//----------------------------------------------------------------------------------------
void UI::drawHUD()
{
    /// aleart
    ofxPushAll();
    
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofEnableAlphaBlending();
    
    //ofRect(x, y, 180.0f, 60.0f);
    if (mRecording) {
        const float x = (ofGetWidth()-mWidth)/2.0f + mWidth;
        const float y = (ofGetHeight()-200.0f)/2.0f;
        
        const string str = "RECORDING...";
        
        const float spd = 0.8f;
        const float t = fmod(ofGetElapsedTimef(), spd);
        
        if (t<spd/2.0f) {
            ofSetColor(255, 20, 20, 130);
            mFont.drawString(str,
                             x-mFont.stringWidth(str)/2.0f,
                             y-mFont.stringHeight(str)/2.0f);
        }
        ofSetColor(255, 255, 255, 130);
        ofDrawBitmapString("elapsed time(sec): "
                           + ofToString(ofGetElapsedTimef()-mRecordBegin, 2),
                           x-mFont.stringWidth(str)/2.0f+3.5f,
                           y+mFont.stringHeight(str)/2.0f);
    }
    ofxPopAll();
}

//----------------------------------------------------------------------------------------
void UI::keyPressed(int key)
{
    const float lineH = kBitmapStringFontSize + kBitmapStringPadding;
    const int nLinesDisplay = mLogger->getRect()->getHeight() / lineH;
    switch (key) {
        case OF_KEY_DOWN:
            mLogOffset++;
            if (mLogOffset > mNumLines - nLinesDisplay)
                mLogOffset = mNumLines - nLinesDisplay;
            break;
        case OF_KEY_UP:
            mLogOffset--;
            if (mLogOffset < 0) mLogOffset = 0;
            break;
        default: /*do nothing*/ break;
    }
}

//----------------------------------------------------------------------------------------
void UI::resize()
{
    if (!general || !mLogger) return;
    if (!general) return;
    /// create gui objects
    const float genW = mWidth;
    const float genH = (ofGetHeight()>1250.0f) ? ofGetHeight() : 1250.0f;
    
    const float logx = genW;
    const float logy = ofGetHeight()-200.0f;
    const float logw =ofGetWidth()-logx;
    const float logh = 201.0f;
    
    general->getRect()->set(0.5f, 1.0f, genW-0.5f, genH-1.0f);
    
    mLogger->getRect()->set(logx, logy, logw, logh);
    
    ofxUITabbedCanvas* inspector = mInspector.getTabbedCanvas();
    if (inspector) {
        float y;
        mDetailedRendering ? y = 10.f : y = 490.f;
        inspector->setPosition(ofGetWidth()-inspector->getRect()->width-10.0f, y);
    }
    
    mView.x = genW;
    mView.y = 0.0f;
    mView.width = logw;
    mView.height = ofGetHeight()-logh;
    
    if (ofGetWidth()<genW)
        ofSetWindowShape(genW, ofGetHeight());
}

//----------------------------------------------------------------------------------------
void UI::checkError()
{
    if (!mIsInited)
        ofxThrowException(ofxException, "UI dosen't inited!");
}

//----------------------------------------------------------------------------------------
void UI::guiEvent(ofxUIEventArgs &e)
{
    checkError();
    
    OFX_BEGIN_EXCEPTION_HANDLING
    
    const string name = e.widget->getName();
    
    if (name == "TOGGLE DETAILED RENDERING") {
        ofxUIToggle *toggle = static_cast<ofxUIToggle *>(e.widget);
        mDetailedRendering = toggle->getValue();
        
        ofxEventMessage m;
        m.setAddress(event::ADDRESS_TOGGLE_DRAW);
        m.addIntArg(static_cast<int>(mDetailedRendering));
        ofxNotifyEvent(m);
        mLogger->setVisible(mDetailedRendering);
        
        if (!mDetailedRendering) {
            mStoredWindowRect = ofGetWindowRect();
            ofSetWindowShape(mWidth, 768.0f);
        }
        else if (mStoredWindowRect.width>0 && mStoredWindowRect.height>0) {
            //cout << mStoredWindowRect.width << "/" << mStoredWindowRect.height << endl;
            ofSetWindowShape(mStoredWindowRect.width, mStoredWindowRect.height);
        }
    }
    else if(name == "SAVE SETTINGS") {
        if (static_cast<ofxUIButton *>(e.widget)->getValue())
            general->saveSettings("xml/gui.xml");
    }
    //else if(name == "OPEN REC FILES") {
    //    if (static_cast<ofxUIButton *>(e.widget)->getValue())
    //        openRecordingDialog();
    //}
    else if(name == "START REC") {
        if (static_cast<ofxUIButton *>(e.widget)->getValue()) {
            ofxEventMessage m;
            m.setAddress(event::ADDRESS_START_RECORDING);
            ofxNotifyEvent(m);
        }
    }
    else if(name == "FINISH REC") {
        if (static_cast<ofxUIButton *>(e.widget)->getValue()) {
            ofxEventMessage m;
            m.setAddress(event::ADDRESS_FINISH_RECORDING);
            ofxNotifyEvent(m);
        }
    }
    else if(name == "DEVICE_INCOMING_UDP_PORT") {
        if (static_cast<ofxUITextInput *>(e.widget)->getTriggerType()==OFX_UI_TEXTINPUT_ON_ENTER)
            setupDeviceCorrespondent();
    }
    else if (name == "CHOOSE A SKELETON") {
        performSelectListItems(static_cast<ofxUIDropDownList *>(e.widget), &UI::selectDevice);
    }
    else if(name == "OSC_OUT_HOST") {
        if (mEnableOscToggle->getValue() &&
            static_cast<ofxUITextInput *>(e.widget)->getTriggerType()==OFX_UI_TEXTINPUT_ON_ENTER)
            enableOsc();
    }
    else if(name == "OSC_OUT_PORT") {
        if (mEnableOscToggle->getValue() &&
            static_cast<ofxUITextInput *>(e.widget)->getTriggerType()==OFX_UI_TEXTINPUT_ON_ENTER)
            enableOsc();
    }
    else if (name == "ENABLE OSC OUT") {
        static_cast<ofxUIToggle *>(e.widget)->getValue() ? enableOsc() : disableOsc();
    }
    else if (name == "PLAY_MOTION") {
        ofxEventMessage m ;
        m.setAddress(event::ADDRESS_PLAY_MOTION);
        m.addIntArg(static_cast<ofxUIImageToggle *>(e.widget)->getValue());
        ofxNotifyEvent(m);
    }
    else if (name == "ORIENTATION-Y") {
        notifySkeletonOrientation();
    }
    else if (name == "AUTO RESET") {
        notifyAutoResetDimension();
    }
    else if (name == "FIX POSITION") {
        notifyFixPosition();
    }
    else if (name == "OPEN A MOTION FILE") {
        if (static_cast<ofxUIButton *>(e.widget)->getValue()) {
            mFileDialogResult = openPlaybackDialog();
            mWillOpenMotionFile = true;
        }
    }
    else if (name == "PLAY ALL") {
        if (static_cast<ofxUIButton *>(e.widget)->getValue()) {
            startPlayback();
        }
    }
    else if (name == "PAUSE ALL") {
        if (static_cast<ofxUIButton *>(e.widget)->getValue()) {
            pausePlayback();
        }
    }
    else if (name == "STOP ALL") {
        if (static_cast<ofxUIButton *>(e.widget)->getValue()) {
            stopPlayback();
        }
    }
    else if (name == "TOGGLE LOOP ALL") {
        setLoopPlayback(static_cast<ofxUIToggle *>(e.widget)->getValue());
    }
    else if (name == "LOW-PASS") {
        notifyLowpassValue();
    }
    else if (name == "MOVE LIGHT") {
        ofxEventMessage m;
        m.setAddress(event::ADDRESS_MOVING_LIGHT);
        const int b = (int)((ofxUIToggle *)e.widget)->getValue();
        m.addIntArg(b);
        ofxNotifyEvent(m);
    }
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void UI::addLog(const string &log)
{
    const int nLines = ofSplitString(log, "\n").size() - 1;
    mNumLines += nLines;
    mLog.push_back(log);
    
    if (mLog.empty() == false) {
        while (mLog.size() >= mLogBufferSize) {
            const string& s = *mLog.begin();
            const int nLines = ofSplitString(s, "\n").size() - 1;
            mNumLines -= nLines;
            mLog.pop_front();
        }
    }
}

//----------------------------------------------------------------------------------------
void UI::updateDeviceList(SkeletonMap *skeletonMap)
{
    checkError();
    
    mAvilableDeviceList->clearToggles();
    
    for (SkeletonMap::iterator it = skeletonMap->begin(); it!=skeletonMap->end(); ++it) {
        ofSetLogLevel(OF_LOG_ERROR); /// shut ofxUI waring up...
        mAvilableDeviceList->addToggle(it->second->getHostName());
        ofSetLogLevel(DEFAULT_LOG_LEVEL);
    }
}

//----------------------------------------------------------------------------------------
void UI::performSelectListItems(ofxUIDropDownList *ddList, void (UI::*func)(const string &))
{
    checkError();
    
    vector<ofxUIWidget *> &selected = ddList->getSelected();
    for(size_t i = 0; i < selected.size(); i++) {
        ofLogNotice("UI") << "Slelected: " << selected[i]->getName();
        const string &name = selected.at(i)->getName();
        (this->*func)(name);
    }
}

//----------------------------------------------------------------------------------------
void UI::selectDevice(const string &hostName)
{
    mInspector.setDevice(hostName);
}

//----------------------------------------------------------------------------------------
void UI::onNotifyEvent(ofxEventMessage &m)
{
    const string addr = m.getAddress();
    if (addr==event::ADDRESS_LOG) {
        addLog(m.getArgAsString(0));
    }
    else if (addr==event::ADDRESS_START_RECORDING) {
        if (skeleton::SkeletonManager::getInstance().getSkeletons().empty()==false) {
            if (!mRecording) {
                mRecordBegin = ofGetElapsedTimef();
            }
            mRecording = true;
            startRecording();
        }
    }
    else if (addr==event::ADDRESS_FINISH_RECORDING) {
        if (skeleton::SkeletonManager::getInstance().getSkeletons().empty()==false) {
            if (mRecording) {
                finishRecording();
                mRecording = false;
            }
        }
    }
    else if (addr==event::ADDRESS_PLAY_MOTION) {
        const bool p = static_cast<bool>(m.getArgAsInt32(0));
        mPlayToggle->setValue(p);
        //ofLogNotice() << mPlayToggle->getValue();
    }
    else if (addr==event::ADDRESS_REQUEST_GENERAL_SETTINGS) {
        notifySkeletonOrientation();
        notifyAutoResetDimension();
        notifyFixPosition();
        notifyLowpassValue();
    }
}

//----------------------------------------------------------------------------------------
void UI::enableOsc()
{
    ofLogNotice("UI") << "Enable OSC out\n"
    << "host:" << mOscOutHostInput->getTextString()
    << " port:" << mOscOutPortInput->getTextString();
    
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_ENABLE_OSC);
    m.addStringArg(mOscOutHostInput->getTextString());
    m.addIntArg(ofToInt(mOscOutPortInput->getTextString()));
    ofxNotifyEvent(m);
}

//----------------------------------------------------------------------------------------
void UI::disableOsc()
{
    ofLogNotice("UI") << "Disable OSC";
    
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_DISABLE_OSC);
    ofxNotifyEvent(m);
}

//----------------------------------------------------------------------------------------
void UI::setupDeviceCorrespondent()
{
    const int port = ofToInt(mDeviceIncomingPortInput->getTextString());
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_SETUP_DEVICE_CORRESPONDENT);
    m.addIntArg(port);
    ofxNotifyEvent(m);
}

//----------------------------------------------------------------------------------------
int UI::getDeviceIncomingUdpPort() const
{
    return ofToInt(mDeviceIncomingPortInput->getTextString());
}

//----------------------------------------------------------------------------------------
void UI::notifySkeletonOrientation()
{
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_SET_ORIENTATION);
    m.addFloatArg(mOrientation->getScaledValue());
    ofxNotifyEvent(m);
}

//----------------------------------------------------------------------------------------
void UI::notifyAutoResetDimension()
{
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_SET_AUTO_RESET_DIMENSION);
    m.addFloatArg(mAutoResetDimenstion->getScaledValue());
    ofxNotifyEvent(m);
}

//----------------------------------------------------------------------------------------
void UI::notifyFixPosition()
{
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_FIX_POSITION);
    m.addIntArg(mFixPosition->getValue());
    ofxNotifyEvent(m);
}

//----------------------------------------------------------------------------------------
void UI::notifyLowpassValue() const
{
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_SET_LOWPASS);
    m.addFloatArg(mLowpass->getScaledValue());
    ofxNotifyEvent(m);
}