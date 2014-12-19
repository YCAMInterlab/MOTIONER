//
//  ramSkeleton.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#ifndef __MOTIONER__ramSkeleton__
#define __MOTIONER__ramSkeleton__

#include "ofMain.h"
#include "ramNode.h"
#include "ramFrame.h"
#include "ramUtils.h"

#include "ofxNonCopyable.h"
#include "ofxOscMessage.h"
#include "ofxEvent.h"

namespace ram { namespace skeleton {
    
    /// forward declaration
    //--------------------
    class Node;
    class Skeleton;
    
    /// smart ptr for Skelton class
    //--------------------
    typedef ofPtr<Skeleton> SkeletonPtr;
    
    /// factory
    //--------------------
    inline SkeletonPtr createSkeleton();
    
    /// Skeleton class
    //------------------------------------------------------------------------------------
    class Skeleton : public ofxNonCopyable<Skeleton> {
        /// These classes are a part of the skeleton
        //--------------------
        friend class Renderer;
        friend class Settings;
        friend class EasyIK;
        friend class VelocityIK;
        friend class SkeletonFinder;
        
    public:
        //--------------------
        Skeleton();
        virtual ~Skeleton();
        
        //--------------------
        virtual void setup(const string &hostName, const string &settingFilePath);
        
        /// update modules which a flag is set
        //--------------------
        virtual void update();
        void updateLowpass(coder::Frame frame);
        void updateRotation();
        
        /// use Renderer
        //--------------------
        virtual void draw();
        virtual void drawHUD();
        
        ///// conect to a MOTIONER Suit(device)
        //bool connect();
        ///// disconnect a MOTIONER Suit
        //void disconnect();
        
        /// acce. integration
        //--------------------
        void integrateAcceleration();
        
        /// calibration
        //--------------------
        void calibrate();
        
        /// settings
        //--------------------
        void loadSettings(const string &fileName);
        void saveSettings(const string &fileName = "");
        
        
        /// suicide
        /// safe self delete method
        /// but take care for use
        //--------------------
        void suicide();
        
        /// editor
        //--------------------
        void editHierarchy(int nodeId, const ofVec3f &offset);
        
        /// recoder
        //--------------------
        void prepareRecording(const string &filePath);
        void finishRecording();
        void startRecording(); /// can pause and restart
        void stopRecording(); /// pause
        
        /// player
        //--------------------
        void preparePlayback(const string &filePath);
        void finishPlayback();
        void startPlayback();
        void pausePlayback();
        void stopPlayback();
        void setLoopPlayback(ofLoopType state);
        
        /// util
        //--------------------
        void reportFlags();
        
        /// events
        //--------------------
        void onMessageReceived(ofxEventMessage &m);
        
        /// EXPERIMENT
        //--------------------
        void setLowpassFilterRate(float rate);
        float getLowpassFilterRate() const;
        
        /// setters
        //--------------------
        void setHostName(const string &host);
        void setName(const string &name);
        void setDrawJointName(bool bDraw);
        void resetPosition(const ofVec3f &pos = ofVec3f::zero());
        
        void setState(int state);
        void setActilveJoint(int joint);
        
        void setDrawDebug(bool bDraw);
        void toggleDrawDebug();
        
        void setCircleTrackerId(int id);
        void setEnableCircleTracker(bool bEnable);
        
        void setEnableOscOut(bool bEnable);
        
        void setDisableJoint(int joint, bool bDisable);
        
        /// getters
        //--------------------
        //const string &getXmlFileName() const;
        const string &getName() const;
        const string &getHostName() const;
        const string &getSettingsFileName() const;
        inline Node &getJoint(int jointId);
        inline const Node &getJoint(int jointId) const;
        inline const Node &getJoint(const string &name) const;
        inline NodeVec &getJoints();
        inline const NodeVec &getJoints() const;
        int getCircleTrackerId() const;
        bool getDrawJointName() const;
        ofVec3f getResetPosition() const;
        
        int getJointIndexFromName(const string& name) const;
        
        inline const coder::Frame &getInitialPose() const;
        
        bool getDisableJoint(int joint) const;
        
        bool getEnableOscOut() const;
        
        /// flags for module
        //--------------------
        enum Flags {
            PLAY        = 1 << 0,
            RDTK_PLAY   = 1 << 1,
            RECORD      = 1 << 2,
            CAPTURE     = 1 << 3,
            RENDER      = 1 << 4,
            EASY_IK     = 1 << 5,
            VELOCITY_IK = 1 << 6,
            CIRCLE_TRACKER_IK   = 1 << 7,
            OSC_OUT     = 1 << 8,
        };
        
        /// states for rendering
        //--------------------
        enum States {
            STATE_SELECTED  = 0,
            STATE_IDLE,
            N_STATES,
        };
        
    private:
        //--------------------
        struct Modules; /// pimpl
        
        //--------------------
        virtual void createTree();
        inline bool isEnable(Flags flag) const;
        inline void enable(Flags flag);
        inline void disable(Flags flag);
        
        //--------------------
        NodeVec    mJoints; /// current
        
        ofPtr<Modules>  mModules; /// pimpl
        
        vector<bool>    mDisableJoints;
        
        coder::Frame    mBaseFrame;
        coder::Frame    mCurrentFrame;
        coder::Frame    mPreviousFrame;
        coder::Frame    mInitialPoseFrame;
        
        int             mFlags;
        
        bool            mDrawDebug;
        
        float           mLowpass;
        
        // adjust position
        float           mOrientationY;
        float           mAutoResetDimension;
        bool            mFixPosition;
    };
    
    
    /// Utils
    //------------------------------------------------------------------------------------
    struct SkeletonFinder {
        SkeletonFinder(const string &name);
        bool operator ()(const pair<string, SkeletonPtr> &itr);
        string name;
    };
        
    //------------------------------------------------------------------------------------
    inline bool Skeleton::getDisableJoint(int joint) const
    {
        if (joint<0 || joint>=NUM_JOINTS)
            ofxThrowException(ofxException, "joint out of index");
        return mDisableJoints.at(joint);
    }
        
    //------------------------------------------------------------------------------------
    inline Node &Skeleton::getJoint(int jointId)
    {
        // FIXME: add error handle
        return mJoints.at(jointId);
    }

    //------------------------------------------------------------------------------------
    inline const Node &Skeleton::getJoint(int jointId) const
    {
        // FIXME: add error handle
        return mJoints.at(jointId);
    }
    
    //------------------------------------------------------------------------------------
    inline const Node &Skeleton::getJoint(const string &name) const
    {
        // FIXME: add error handle
        NodeVec::const_iterator it = find_if(mJoints.begin(),
                                            mJoints.end(),
                                            NodeFinder(name));        
        if (it!=mJoints.end()) {
            return (*it);
        }
        else {
            ofxThrowException(ofxException, "joint not found!");
        }
    }
    
    //------------------------------------------------------------------------------------
    inline NodeVec &Skeleton::getJoints()
    {
        return mJoints;
    }
    
    //------------------------------------------------------------------------------------
    inline const NodeVec &Skeleton::getJoints() const
    {
        return mJoints;
    }
    
    inline const coder::Frame &Skeleton::getInitialPose() const
    {
        return mInitialPoseFrame;
    }
    
    //------------------------------------------------------------------------------------
    inline bool Skeleton::isEnable(Flags flag) const
    {
        return ((mFlags & flag)==flag);
    }
    
    //------------------------------------------------------------------------------------
    inline void Skeleton::enable(Flags flag)
    {
        mFlags |= flag; /// on
    }
    
    //------------------------------------------------------------------------------------
    inline void Skeleton::disable(Flags flag)
    {
        mFlags ^= (mFlags&flag); /// off
    }
    
    /// factory
    //------------------------------------------------------------------------------------
    inline SkeletonPtr createSkeleton()
    {
        return SkeletonPtr(new Skeleton());
    }

} }

#endif /* defined(__MOTIONER__ramSkeleton__) */
