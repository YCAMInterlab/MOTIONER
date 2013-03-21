//
//  ramInspectorUI.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/3/13.
//
//

#include "ramInspectorUI.h"

#include "ramSkeleton.h"
#include "ramSkeletonManager.h"

#include "ofMain.h"
#include "ofxUI.h"

#include "ofxEvent.h"
#include "ramEvent.h"


using namespace ram;


//----------------------------------------------------------------------------------------
InspectorUI::InspectorUI() :

mIsInited(false),
mSettingsFilePath(""),

mSkeletonList(NULL),
mDeviceHostNameLabel(NULL),
mSettingFileNameLabel(NULL),
mSkeletonNameInput(NULL),
mShowJointNameToggle(NULL),

mJointNameLabel(NULL),
mJointName(getJointName(JOINT_HIPS)),
mDialerX(NULL),
mDialerY(NULL),
mDialerZ(NULL),

mResetPositionXSlider(NULL),
mResetPositionZSlider(NULL),
mDisableJointToggle(NULL)
{
}

//----------------------------------------------------------------------------------------
InspectorUI::~InspectorUI()
{
}

//----------------------------------------------------------------------------------------
void InspectorUI::setup()
{
    if (mIsInited) return;
    mIsInited = true;
    
    mSize.set(0.0f, 0.0f, 180.0f, ofGetHeight());
    
    ofxUICanvas *canvas = NULL;
    ofxUIWidget *widget = NULL;
    ofColor outlineColor;
    outlineColor.setHex(0x494949);
    
    /// create gui objects
    const float space = OFX_UI_GLOBAL_WIDGET_SPACING;
    const float w = mSize.width+space;
    const float h[] = {
        170.0f,
        130.0f,
        85.0f,
        60.0f,
    };
    
    mTabs.clear();
    
    for (int i=0; i<N_TABS; i++) {
        
        ofSetLogLevel(OF_LOG_FATAL_ERROR); /// shout ofxUI out
        ofxUICanvas *canvas = new ofxUICanvas(0.0f, 0.0f, w, h[i]);
        
        ofSetLogLevel(DEFAULT_LOG_LEVEL);
        canvas->setFont(GUI_FONT, false); /// aliased
        mTabs.push_back(canvas);
    }
    
    /// setup guis
    const float hw = mSize.width-OFX_UI_GLOBAL_WIDGET_SPACING;
    
    const float buttonSize = 20.0f;
    
    //const float range = 10000.0f;
    
    //--------------------------------------------------//
    //------------ TAB_GENERAL_SETTINGS ----------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_GENERAL_SETTINGS);
    canvas->setDrawOutline(true);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
    }
    
    canvas->addLabel("SKELETON:", OFX_UI_FONT_SMALL);
    
    /// TAB_GENERAL_SETTINGS - NAME label
    //--------------------------------------------------
    {
        //canvas->addSpacer(hw, 1.0f);
        //mDeviceHostNameLabel = canvas->addLabel("DEVICE IP:", OFX_UI_FONT_SMALL);
        mDeviceHostNameLabel = canvas->addLabel("NO IP FOUND", OFX_UI_FONT_SMALL);
        canvas->addSpacer(hw, 1.0f);
    }
    
    /// TAB_GENERAL_SETTINGS - Basic settings
    //--------------------------------------------------
    {
        canvas->addLabel("NAME:", OFX_UI_FONT_SMALL);
        mSkeletonNameInput = canvas->addTextInput("ACTOR_NAME",
                                                  "ENTER NAME",
                                                  hw,
                                                  0,
                                                  0,
                                                  0,
                                                  OFX_UI_FONT_SMALL);
        mSkeletonNameInput->setAutoClear(false);
        mSkeletonNameInput->setDrawOutline(true);
        mSkeletonNameInput->setColorOutline(outlineColor);
        
        widget = canvas->addLabelButton("CALIBRATE", false, hw);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
        canvas->addSpacer(hw, 1.0f);
    }
    
    /// TAB_GENERAL_SETTINGS - Skeleton managment
    //--------------------------------------------------
    {
        canvas->addLabel("FILE:", OFX_UI_FONT_SMALL);
        mSettingFileNameLabel = canvas->addLabel("", OFX_UI_FONT_MEDIUM);
        //canvas->addLabel(" ");
        addImageButton(canvas, buttonSize, "images/open.png", "LOAD", true);
        addImageButton(canvas, buttonSize, "images/save.png", "SAVE", false);
        addImageButton(canvas, buttonSize, "images/saveas.png", "SAVE AS...", false);
        //canvas->addSpacer(hw, 1.0f);
    }

    //--------------------------------------------------//
    //---------------- JOINT EDITOR --------------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_JOINT_EDITOR);
    canvas->setDrawOutline(true);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
    }
    
    /// JOINT EDITOR - JOINT NAME label
    //--------------------------------------------------
    {
        canvas->addLabel("JOINT:", OFX_UI_FONT_SMALL);
        mJointNameLabel =new ofxUILabel("NO JOINT SELECTED", OFX_UI_FONT_SMALL);
        canvas->addWidgetDown(mJointNameLabel);
    }
    
    /// JOINT EDITOR - X number dialer
    //--------------------------------------------------
    {
        canvas->addLabel("X:", OFX_UI_FONT_SMALL);
        
        mDialerX = new ofxUINumberDialer(-999.0f, 999.0f, 0.0f, 2.0f, "OFFSET X", OFX_UI_FONT_SMALL);
        widget = canvas->addWidgetRight(mDialerX);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
    }
    
    /// JOINT EDITOR - Y number dialer
    //--------------------------------------------------
    {
        canvas->addLabel("Y:");
        mDialerY = new ofxUINumberDialer(-999.0f, 999.0f, 0.0f, 2.0f, "OFFSET Y", OFX_UI_FONT_SMALL);
        widget = canvas->addWidgetRight(mDialerY);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
    }
    
    /// JOINT EDITOR - Z number dialer
    //--------------------------------------------------
    {
        canvas->addLabel("Z:");
        mDialerZ = new ofxUINumberDialer(-999.0f, 999.0f, 0.0f, 2.0f, "OFFSET Z", OFX_UI_FONT_SMALL);
        widget = canvas->addWidgetRight(mDialerZ);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
    }
    
    {
        mDisableJointToggle = canvas->addLabelToggle("DISABLE JOINT", false);
        mDisableJointToggle->setDrawOutline(true);
        mDisableJointToggle->setColorOutline(outlineColor);
    }
    
    //--------------------------------------------------
    {
        mShowJointNameToggle = addImageToggle(canvas,
                                              buttonSize,
                                              "images/show.png",
                                              "SHOW JOINT NAME",
                                              true);
        mShowJointNameToggle->setValue(false);
        //canvas->addWidgetRight(new ofxUILabel("SHOW NAMES", OFX_UI_FONT_SMALL));
    }
    
    //--------------------------------------------------//
    //-------------- POSITION EDITOR -------------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_POSITION_EDITOR);
    canvas->setDrawOutline(true);
    canvas->setDrawPadding(false);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
    }
    
    /// POSITION EDITOR - SLIDES
    //--------------------------------------------------
    {
        mResetPositionXSlider = canvas->addSlider("CENTTER X",
                                                  -1500.0f,
                                                  1500.0f,
                                                  0.0f,
                                                  hw,
                                                  10.0f);
        mResetPositionXSlider->setDrawOutline(true);
        mResetPositionXSlider->setColorOutline(outlineColor);
        
        mResetPositionZSlider = canvas->addSlider("CENTTER Z",
                                                  -1500.0f,
                                                  1500.0f,
                                                  0.0f,
                                                  hw,
                                                  10.0f);
        mResetPositionZSlider->setDrawOutline(true);
        mResetPositionZSlider->setColorOutline(outlineColor);
        
        widget = canvas->addLabelButton("RESET", false, hw);
        widget->setDrawOutline(true);
        widget->setColorOutline(outlineColor);
        
    }
    
    for (int i=0; i<N_TABS; i++) {
        mTabs.at(i)->setDrawWidgetPadding(false);
        ofAddListener(mTabs.at(i)->newGUIEvent, this, &InspectorUI::guiEvent);
    }
    
    //--------------------------------------------------//
    //----------------- TAB_OPTIONS --------------------//
    //--------------------------------------------------//
    canvas = mTabs.at(TAB_OPTIONS);
    canvas->setDrawOutline(true);
    canvas->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    {
        /// colors
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        canvas->setColorBack(b);
        canvas->setColorFill(f);
        canvas->setColorOutline(o);
        
        /// TAB_OPTIONS - options
        //--------------------------------------------------
        {
            mEnableOscOutToggle = canvas->addLabelToggle("OSC OUT", true, hw);
            mEnableOscOutToggle->setDrawOutline(true);
            mEnableOscOutToggle->setColorOutline(outlineColor);
            
            canvas->addSpacer(hw, 1.0f);
            
            widget = canvas->addLabelButton("DELETE", false, hw);
            widget->setDrawOutline(true);
            widget->setColorOutline(outlineColor);
        }
    }
    
    //--------------------------------------------------//
    //--------------------------------------------------//
    
    ofSetLogLevel(OF_LOG_FATAL_ERROR);
    mTabbedCanvas = new  ofxUITabbedCanvas();
    mTabbedCanvas->setup(GUI_FONT);
    for (int i=0; i<N_TABS; i++) {
        mTabbedCanvas->add(mTabs.at(i));
    }
    mTabbedCanvas->loadSettings(INSPECTOR_XML);
    
    mTabbedCanvas->setDrawOutline(true);
    mTabbedCanvas->setDrawBack(false);
    /// colors
    {
        ofColor b, f, o;
        b.setHex(COLOR_L, 200);
        f.setHex(COLOR_M);
        o.setHex(COLOR_HILIGHT);
        mTabbedCanvas->setColorBack(b);
        mTabbedCanvas->setColorFill(f);
        mTabbedCanvas->setColorOutline(o);
    }
    
    const float guiw = mSize.width+space;
    const float guiMgnX = 10.0f;
    const float guiMgnY = 10.0f;
    mTabbedCanvas->setPosition(ofVec2f(ofGetWidth()-guiw-guiMgnX, guiMgnY));
    //mTabbedCanvas->setVisible(false);
    ofSetLogLevel(DEFAULT_LOG_LEVEL);
    
    ofAddListener(ofxEvent(), this, &InspectorUI::onMessageReceived);
}

//----------------------------------------------------------------------------------------
void InspectorUI::exit()
{
    ofRemoveListener(ofxEvent(), this, &InspectorUI::onMessageReceived);
    
    if (mTabbedCanvas) {
        //mTabbedCanvas->saveSettings(INSPECTOR_XML);
        delete mTabbedCanvas;
        mTabbedCanvas = NULL;
    }
}

/// GUI event callback
//----------------------------------------------------------------------------------------
void InspectorUI::guiEvent(ofxUIEventArgs &e)
{
    checkError();
    
    OFX_BEGIN_EXCEPTION_HANDLING
    
    const string name = e.widget->getName();
    //ofLogNotice("Inspector") << name;
    //--------------------
    if(name == "ACTOR_NAME") {
        ofxUITextInput *ti = static_cast<ofxUITextInput *>(e.widget);
        
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl)
            skl->setName(ti->getTextString());
        else
            ofxThrowException(ofxException, "Skeleton not found!");
    }
    else if(name == "CALIBRATE") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl)
                skl->calibrate();
            else
                ofxThrowException(ofxException, "Skeleton not found!");
        }
    }
    else if(name == "RESET") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            ofVec3f pos(mResetPositionXSlider->getScaledValue(),
                        0.0f,
                        mResetPositionZSlider->getScaledValue());
            //ofLogNotice() << pos;
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl)
                skl->resetPosition(pos);
            else
                ofxThrowException(ofxException, "Skeleton not found!");
        }
    }
    else if(name == "LOAD") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl) {
                ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a xml file",
                                                                       false,
                                                                       "xml/skeleton/");
                if (openFileResult.bSuccess) {
                    skl->loadSettings(openFileResult.filePath);
                    setSettingsFilePath(openFileResult.filePath);
                }
            }
            else {
                ofxThrowException(ofxException, "Skeleton not found!");
            }
        }
    }
    else if(name == "SAVE") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl)
                skl->saveSettings();
            else
                ofxThrowException(ofxException, "Skeleton not found!");
        }
    }
    else if(name == "SAVE AS...") {
        ofxUIButton *button = static_cast<ofxUIButton *>(e.widget);
        if (button->getValue()) {
            shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
            if (skl) {
                ofFileDialogResult saveFileResult =
                ofSystemSaveDialog("Untitled.xml", "Save current skeleton settings");
                if (saveFileResult.bSuccess) {
                    skl->saveSettings(saveFileResult.filePath);
                    mSettingFileNameLabel->setLabel(getFileName(skl->getSettingsFileName()));
                }
            }
            else {
                ofxThrowException(ofxException, "Skeleton not found!");
            }
        }
    }
    else if (name == "SHOW JOINT NAME") {
        ofxUIToggle *toggle = static_cast<ofxUIToggle *>(e.widget);
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl)
            skl->setDrawJointName(toggle->getValue());
        else
            ofxThrowException(ofxException, "Skeleton not found!");
    }
    else if (name == "OFFSET X") {
        editOffset(static_cast<ofxUINumberDialer *>(e.widget), 0);
    }
    else if (name == "OFFSET Y") {
        editOffset(static_cast<ofxUINumberDialer *>(e.widget), 1);
    }
    else if (name == "OFFSET Z") {
        editOffset(static_cast<ofxUINumberDialer *>(e.widget), 2);
    }
    else if (name == "DISABLE JOINT") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        
        if (skl) {
            const skeleton::Node &n = skl->getJoint(mJointName);
            if (!n.name.empty()) {
                ofxUILabelToggle *lt = static_cast<ofxUILabelToggle *>(e.widget);
                skl->setDisableJoint(getJointIndex(mJointName), lt->getValue());
            }
            else {
                ofxThrowException(ofxException, "Node not found!");
            }
        }
        else {
            ofxThrowException(ofxException, "Skeleton not found!");
        }
    }
    else if (name == "DELETE") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl) {
            const string host = skl->getHostName();
            skeleton::SkeletonManager::getInstance().removeSkeleton(host);
        }
    }
    else if (name == "OSC OUT") {
        shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
        if (skl)
            skl->setEnableOscOut(static_cast<ofxUILabelToggle *>(e.widget)->getValue());
    }
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void InspectorUI::checkError()
{
    if (!mIsInited)
        ofxThrowException(ofxException, "UI dosen't inited!");
}

//----------------------------------------------------------------------------------------
void InspectorUI::addSkeletonFromXML(const string &fileName)
{
    skeleton::SkeletonManager::getInstance().addSkeleton(fileName);
}

//----------------------------------------------------------------------------------------
void InspectorUI::performSelectListItems(ofxUIDropDownList *ddList,
                                         void (InspectorUI::*func)(const string &))
{
    checkError();
    
    vector<ofxUIWidget *> &selected = ddList->getSelected();
    for(size_t i = 0; i < selected.size(); i++) {
        //cout << "SELECTED: " << selected[i]->getName() << endl;
        const string &name = selected.at(i)->getName();
        (this->*func)(name);
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::setDevice(const string &hostName)
{
    checkError();
    
    SkeletonMap *skeletonMap = &skeleton::SkeletonManager::getInstance().getSkeletons();
    
    for (SkeletonMap::iterator it=skeletonMap->begin(); it!=skeletonMap->end(); ++it)
        it->second->setState(skeleton::Skeleton::STATE_IDLE);
    
    SkeletonMap::iterator result = skeletonMap->find(hostName);
    if (result == skeletonMap->end()) {
        ofLogWarning("UI") << "No such skelton!";
        return;
    }
    mSkeleton = result->second; /// mSkeleton is weak_ptr
    shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
    if (skl) {
        /// set host name
        mDeviceHostNameLabel->setLabel(skl->getHostName());
        
        /// set name
        mSkeletonNameInput->setTextString(skl->getName());
        
        mSettingFileNameLabel->setLabel(getFileName(skl->getSettingsFileName()));
        
        mShowJointNameToggle->setValue(skl->getDrawJointName());
        
        mResetPositionXSlider->setValue(skl->getResetPosition().x);
        mResetPositionZSlider->setValue(skl->getResetPosition().z);
        
        mEnableOscOutToggle->setValue(skl->getEnableOscOut());
        
        skl->setActilveJoint(getJointIndex(mJointName));
        mDisableJointToggle->setValue(skl->getDisableJoint(getJointIndex(mJointName)));
        
        /// mark as selected
        skl->setState(skeleton::Skeleton::STATE_SELECTED);
    }
    else {
        ofxThrowException(ofxException, "Skeleton not found!");
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::setSettingsFilePath(const string &path)
{
    mSettingsFilePath = path;
    
    if (mSettingFileNameLabel) {
        const string name = getLastPathCompornent(path);
        mSettingFileNameLabel->setLabel(name);
    }
    else {
        ofxThrowException(ofxException, "mSettingFileNameLabel isn't allocated!");
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::setJoint(int index)
{
    setJoint(getJointName(index));
}

//----------------------------------------------------------------------------------------
void InspectorUI::setJoint(const string &name)
{
    checkError();
    
    mJointNameLabel->setLabel(name);
    mJointName = name;
    shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
    
    if (skl) {
        const skeleton::Node &n = skl->getJoint(name);
        skl->setActilveJoint(getJointIndex(name));
        mDisableJointToggle->setValue(skl->getDisableJoint(getJointIndex(name)));
        
        if (!n.name.empty()) {
            //ofLogNotice() << n.name << ": " << n.getPosition();//debug
            mDialerX->setValue(n.getPosition().x);
            mDialerY->setValue(n.getPosition().y);
            mDialerZ->setValue(n.getPosition().z);
        }
        else {
            ofxThrowException(ofxException, "Node not found!");
        }
    }
    else {
        ofxThrowException(ofxException, "Skeleton not found!");
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::setName(const string &name)
{
    mSkeletonNameInput->setTextString(name);
}

//----------------------------------------------------------------------------------------
void InspectorUI::editOffset(ofxUINumberDialer *nd, int vec3fIndex)
{
    checkError();
    
    const bool inRange = ofInRange((float)vec3fIndex, 0.0f, (float)ofVec3f::DIM);
    if (!inRange)
        ofxThrowException(ofxException, "vec3fIndex out of range!");
    
    const float f = nd->getValue();
    
    shared_ptr<skeleton::Skeleton> skl = mSkeleton.lock();
    
    if (skl) {
        const skeleton::Node &n = skl->getJoint(mJointName);
        
        if (!n.name.empty()) {
            ofVec3f vec = n.getPosition();
            vec[vec3fIndex] = f;
            skl->editHierarchy(n.id, vec);
        }
        else {
            ofxThrowException(ofxException, "Node not found!");
        }
    }
    else {
        ofxThrowException(ofxException, "Skeleton not found!");
    }
}

//----------------------------------------------------------------------------------------
void InspectorUI::onMessageReceived(ofxEventMessage &m)
{
    const string addr = m.getAddress();
    if (addr==event::ADDRESS_PICKED_SKELETON) {
        setDevice(m.getArgAsString(0));
    }
    else if (addr==event::ADDRESS_PICKED_JOINT) {
        setDevice(m.getArgAsString(0));
        setJoint(m.getArgAsInt32(1));
    }
    else if (addr==event::ADDRESS_CHANGE_SKELETON_NAME) {
        setName(m.getArgAsString(0));
    }
}

//----------------------------------------------------------------------------------------
ofxUIImageButton *InspectorUI::addImageButton(ofxUICanvas *canvas,
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
ofxUIImageToggle *InspectorUI::addImageToggle(ofxUICanvas *canvas,
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