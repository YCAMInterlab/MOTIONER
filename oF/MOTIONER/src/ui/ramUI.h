//
//  ramUI.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/9/12.
//
//

#ifndef __MOTIONER__ramUI__
#define __MOTIONER__ramUI__

#include "ofMain.h"

#include "ramCommon.h"
#include "ramInspectorUI.h"

#include "ofxNonCopyable.h"
#include "ofxEvent.h"

class ofxUICanvas;
class ofxUIScrollableCanvas;
class ofxUIEventArgs;
class ofxUIMovingGraph;
class ofxUIDropDownList;
class ofxUILabel;
class ofxUINumberDialer;
class ofxUITextInput;
class ofxUITextArea;
class ofxUIToggle;
class ofxUITabbedCanvas;
class ofxUIImageToggle;

namespace ram {
    
    /// forward declaration
    //--------------------
    class Node;
    class UI;
        
    /// singleton UI class
    //------------------------------------------------------------------------------------
    RAM_SINGLETON_CLASS
    class UI : public ofxNonCopyable<UI> {
    public:
        //--------------------
        static UI &getInstance();
        //--------------------
        void setup(const ofRectangle &rect = ofGetWindowRect());
        void exit();
        ~UI();
        
        //--------------------
        void update();
        void draw();
        void drawHUD();
        void keyPressed(int key);
        void resize();
        
        //--------------------
        inline const ofRectangle &getViewRect() const { return mView; }
        
        //--------------------
        inline float getWidth() const { return mWidth; }
        
        inline InspectorUI &getInspector() { return mInspector; }
        
        void updateDeviceList(SkeletonMap *skeletonMap);
        
        int getDeviceIncomingUdpPort() const;
        
    private:
        //--------------------
        enum panel {
            GENERAL             = 0,
            SKELETON_SELECTOR   = 1,
            SKELETON            = 2,
            JOINT_EDITOR        = 3,
            POSITION_EDITOR     = 4,
            NUM_PANELS,
        };
        
        
        //--------------------
        UI();
        
        //--------------------
        void checkError();
        
        /// GUI event callback
        //--------------------
        void guiEvent(ofxUIEventArgs &e);
        
        void notifySkeletonOrientation();
        void notifyAutoResetDimension();
        void notifyFixPosition();

#ifdef DEBUG
        /// experiment
        //--------------------
        void notifyLowpassValue() const;
        /// logger
        //--------------------
        void addLog(const string &log);
        float getLogHeight() const;
#endif
        
        void enableOsc();
        void disableOsc();
        
        void setupDeviceCorrespondent();
        
        /// do something with name which is selected item of drop down list
        //--------------------
        void performSelectListItems(ofxUIDropDownList *ddList, void (UI::*func)(const string &));
        
        void selectDevice(const string &hostName);
        
        void onNotifyEvent(ofxEventMessage &m);
        
        ofxUIImageButton *addImageButton(ofxUICanvas *canvas,
                                         int dimension,
                                         const string &path,
                                         const string &name,
                                         bool bNewLine);
        
        ofxUIImageToggle *addImageToggle(ofxUICanvas *canvas,
                                         int dimension,
                                         const string &path,
                                         const string &name,
                                         bool bNewLine);
        
        /// guis
        //--------------------
        ofxUICanvas         *general;
        //vector<ofxUICanvas *>   mGui;
        
        //ofxUITabbedCanvas   *mTabbedInspector;
        InspectorUI         mInspector;
        ofxUICanvas         *mTools;
        
        float               mWidth;
        float               mHeight;
        
        /// App settings
        //--------------------
        ofVec3f             mBackground;
        
        /// general device settings
        //--------------------
        ofxUITextInput      *mDeviceIncomingPortInput;
        
        ofxUIDropDownList   *mAvilableDeviceList;
        
        /// osc settings
        //--------------------
        ofxUITextInput      *mOscOutHostInput;
        ofxUITextInput      *mOscOutPortInput;
        ofxUIImageToggle    *mEnableOscToggle;
        
        ofxUILabel          *mPlayerTimeLabel;
        
        ofxUITextInput      *mOscInHostInput;

        
        /// tools
        //ofxUIImageToggle    *mRotationToggle;
        //ofxUIImageToggle    *mTranslationToggle;
        ofxUIImageToggle    *mPlayToggle;
        
        /// orientation
        //--------------------
        ofxUISlider         *mOrientation;
        ofxUISlider         *mAutoResetDimenstion;
        ofxUIToggle         *mFixPosition;
        
        /// UI params
        //--------------------
        bool                mIsInited;
        
        bool                mDetailedRendering;
        
        ofRectangle         mView;
        
        ofRectangle         mStoredWindowRect;
    
        /// alert
        bool                mRecording;
        float               mRecordBegin;
        
        //--------------------
        static const int    kBitmapStringPadding = 4;
        static const int    kBitmapStringFontSize = 8;
        
        ofTrueTypeFont      mFont;
        
#ifdef DEBUG
        /// log
        //--------------------
        ofxUICanvas         *mLogger;
        string              mLoggerStr;
        float               mLoggerY;
        
        /// experiment
        //--------------------
        ofxUISlider         *mLowpass;
#endif
        
        float               mPlayerStartTime;
        bool                mPlaying;
        bool                mWillreset;
    };

}

#endif /* defined(__MOTIONER__ramUI__) */
