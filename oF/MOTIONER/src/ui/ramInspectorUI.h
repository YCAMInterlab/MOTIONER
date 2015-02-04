//
//  ramInspectorUI.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/3/13.
//
//

#ifndef __MOTIONER__ramInspectorUI__
#define __MOTIONER__ramInspectorUI__

#include "ofxUITabbedCanvas.h"
#include "ramCommon.h"
#include "ramUtils.h"
#include "ofMain.h"
#include "ofxEvent.h"

class ofxUICanvas;

namespace ram {
    
    /// Each skeleton setting inspector
    class InspectorUI {
    public:
        //--------------------
        enum Tab {
            TAB_GENERAL_SETTINGS    = 0,
            TAB_JOINT_EDITOR,
            TAB_POSITION_EDITOR,
            TAB_OPTIONS,
            N_TABS,
        };
        
        //--------------------
        InspectorUI();
        ~InspectorUI();
        
        //--------------------
        void setup();
        void exit();
        
        /// NOT IMPLEMENTED YET
        /// when target skeleton was changed,
        /// just call this method for refresh inspector contents
        //--------------------
        void refresh(const string &hostName,
                     const string &skeletonName,
                     const string &settingPath) {
            RAM_NOTIMPL();
            /// refresh skeleton ptr
        }
        
        //--------------------
        void setDevice(const string &hostName);
        void setName(const string &name);
        void setJoint(int index);
        void setSettingsFilePath(const string &path);
        
        ofxUITabbedCanvas* getTabbedCanvas() { return mTabbedCanvas; }

        
    private:
        /// GUI event callback
        //--------------------
        void guiEvent(ofxUIEventArgs &e);
        
        //--------------------
        void checkError();
        
        void addSkeletonFromXML(const string &fileName);
        void setJoint(const string &name);
        
        /// do something with name which is selected item of drop down list
        //--------------------
        void performSelectListItems(ofxUIDropDownList *ddList,
                                    void (InspectorUI::*func)(const string &));
        
        /// edit position vec3f for a node of current skeleton
        //--------------------
        void editOffset(ofxUINumberDialer *nd, int vec3fIndex);
        
        //--------------------
        void onMessageReceived(ofxEventMessage &m);
        
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
        
        //--------------------
        vector<ofxUICanvas *>   mTabs;
        ofxUITabbedCanvas   *mTabbedCanvas;
        
        //--------------------
        string              mSettingsFilePath;
        
        //--------------------
        bool                mIsInited;
        
        //--------------------
        ofRectangle         mSize;
        
        /// skeletons
        //--------------------
        weak_ptr<skeleton::Skeleton>  mSkeleton;
        
        //--------------------
        ofxUIDropDownList   *mSkeletonList;
        ofxUILabel          *mDeviceHostNameLabel;
        ofxUILabel          *mSettingFileNameLabel;
                
        //--------------------
        ofxUITextInput      *mSkeletonNameInput;
        
        /// joint editor
        //--------------------
        ofxUILabel          *mJointNameLabel;
        string              mJointName;
        ofxUINumberDialer   *mDialerX;
        ofxUINumberDialer   *mDialerY;
        ofxUINumberDialer   *mDialerZ;
        
        ofxUISlider         *mResetPositionXSlider;
        ofxUISlider         *mResetPositionZSlider;
        
        ofxUILabelToggle    *mEnableOscOutToggle;
        
        ofxUILabelToggle    *mDisableJointToggle;
        
    };
    
}

#endif /* defined(__MOTIONER__ramInspectorUI__) */
