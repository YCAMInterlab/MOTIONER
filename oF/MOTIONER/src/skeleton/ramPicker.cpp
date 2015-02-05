//
//  ramPicker.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/4/13.
//
//

#include "ramPicker.h"

#include "ramSkeleton.h"
#include "ramConstants.h"
#include "ramCameraManager.h"
#include "ofxEvent.h"
#include "ramEvent.h"

namespace ram {
    
    using namespace ram::skeleton;
    
    //------------------------------------------------------------------------------------
    Picker::Picker() :
    mDrawDebug(true),
    mSkeletonHostName("")
    {
        mScreenJointLoc.clear();
        mJointMouseState.clear();
        for (int i=0; i<NUM_JOINTS; i++) {
            mScreenJointLoc.push_back(ofVec3f::zero());
            mJointMouseState.push_back(MOUSE_IDLE);
        }
        
        enableMouseEventCallbacks();
    }
    
    //------------------------------------------------------------------------------------
    Picker::~Picker()
    {
        disableMouseEventCallbacks();
        mScreenJointLoc.clear();
        mJointMouseState.clear();
    }
    
    //------------------------------------------------------------------------------------
    void Picker::update(Skeleton *skeleton) const
    {
        /// to event
        CameraPtr cam = CameraManager::getInstance().getCurrentCamera();
        NodeVec &joints = skeleton->getJoints();
        
        ofVec3f min, max;
        
        min = cam->worldToScreen(joints.at(0).getGlobalPosition());
        max = min;
        
        for (size_t i=0; i<joints.size(); i++) {
            /// world position
            ofVec3f pos = joints.at(i).getGlobalPosition();
            
            GLdouble modelview[16];
            GLdouble projection[16];
            GLint viewport[4];
            
            /// use current modelview/projection matrix and viewport
            glGetIntegerv(GL_VIEWPORT, viewport);
            glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
            glGetDoublev(GL_PROJECTION_MATRIX, projection);
            
            GLdouble winX, winY, winZ;
            
            gluProject(static_cast<GLdouble>(pos.x),
                       static_cast<GLdouble>(pos.y),
                       static_cast<GLdouble>(pos.z),
                       modelview,
                       projection,
                       viewport,
                       &winX,
                       &winY,
                       &winZ);
            
            /// screen position
            ofVec3f screenLoc(winX, ofGetHeight()-winY, winZ);
            
            /// outside bounding rect for skeleton
            if (screenLoc.x<min.x) min.x = screenLoc.x;
            if (screenLoc.y<min.y) min.y = screenLoc.y;
            if (screenLoc.x>max.x) max.x = screenLoc.x;
            if (screenLoc.y>max.y) max.y = screenLoc.y;
            
            mScreenJointLoc.at(i) = screenLoc;
        }
        
        min -= 15.0f;
        max += 15.0f;
        
        mMinCoord = min;
        mMaxCoord = max;
        
        mSkeletonHostName = skeleton->getHostName();
    }
    
    //------------------------------------------------------------------------------------
    void Picker::debugDraw(Skeleton *skeleton) const
    {
        if (!mDrawDebug) return;
        
        ofxPushAll();
        ofEnableAlphaBlending();
        
        if (mSkeletonMouseState==MOUSE_IDLE) {
            ofSetColor(0, 0);
        }
        else {
            ofSetColor(100, 100, 200, 100);
            ofSetLineWidth(2.0f);
        }
        ofNoFill();
        ofRect(mMinCoord.x,
               mMinCoord.y,
               mMaxCoord.x-mMinCoord.x,
               mMaxCoord.y-mMinCoord.y);
        
        if (mSkeletonMouseState==MOUSE_DOWN) {
            ofFill();
            ofSetColor(100, 100, 200, 100);
            ofRect(mMinCoord.x,
                   mMinCoord.y,
                   mMaxCoord.x-mMinCoord.x,
                   mMaxCoord.y-mMinCoord.y);
        }
        
        for (int i=0; i<mScreenJointLoc.size(); i++) {
            if (mJointMouseState.at(i)==MOUSE_IDLE) {
                ofSetColor(0, 0);
            }
            else {
                ofSetColor(250, 50, 50, 100);
                ofSetLineWidth(2.0f);
            }
            ofNoFill();
            ofCircle(mScreenJointLoc.at(i), 15.0f);
            if (mJointMouseState.at(i)==MOUSE_DOWN) {
                ofFill();
                ofSetColor(250, 50, 50, 100);
                ofCircle(mScreenJointLoc.at(i), 15.0f);
            }
        }
        
        ofxPopAll();
        
    }
    
    //------------------------------------------------------------------------------------
    void Picker::setDrawDebug(bool bEnable)
    {
        mDrawDebug = bEnable;
    }
    
    //------------------------------------------------------------------------------------
    void Picker::toggleDrawDebug()
    {
        mDrawDebug ^= true;
    }
    
    //------------------------------------------------------------------------------------
    void Picker::enableMouseEventCallbacks()
    {
        ofAddListener(ofEvents().mouseMoved, this, &Picker::onMouseMoved);
        ofAddListener(ofEvents().mousePressed, this, &Picker::onMousePressed);
        ofAddListener(ofEvents().mouseReleased, this, &Picker::onMouseReleased);
        ofAddListener(ofEvents().mouseDragged, this, &Picker::onMouseDragged);
    }
    
    //------------------------------------------------------------------------------------
    void Picker::disableMouseEventCallbacks()
    {
        ofRemoveListener(ofEvents().mouseMoved, this, &Picker::onMouseMoved);
        ofRemoveListener(ofEvents().mousePressed, this, &Picker::onMousePressed);
        ofRemoveListener(ofEvents().mouseReleased, this, &Picker::onMouseReleased);
        ofRemoveListener(ofEvents().mouseDragged, this, &Picker::onMouseDragged);
    }
    
    //------------------------------------------------------------------------------------
    void Picker::mouseMoved(int x, int y)
    {
        isInsideSkeleton(x, y) ?
        (mSkeletonMouseState=MOUSE_OVER) : (mSkeletonMouseState=MOUSE_IDLE);
        
        for (int i=0; i<mJointMouseState.size(); i++) {
            isInsideJoint(i, x, y) ?
            (mJointMouseState.at(i)=MOUSE_OVER) : (mJointMouseState.at(i)=MOUSE_IDLE);
        }
    }
    
    //------------------------------------------------------------------------------------
    void Picker::mouseDragged(int x, int y, int button)
    {
        //isInsideSkeleton(x, y) ?
        //(mSkeletonMouseState=MOUSE_OVER) : (mSkeletonMouseState=MOUSE_IDLE);
        //
        //for (int i=0; i<mJointMouseState.size(); i++) {
        //    isInsideJoint(i, x, y) ?
        //(mJointMouseState.at(i)=MOUSE_OVER) : (mJointMouseState.at(i)=MOUSE_IDLE);
        //}
    }
    
    //------------------------------------------------------------------------------------
    void Picker::mousePressed(int x, int y, int button)
    {
        isInsideSkeleton(x, y) ?
        (mSkeletonMouseState=MOUSE_DOWN) : (mSkeletonMouseState=MOUSE_IDLE);
        
        for (int i=0; i<mJointMouseState.size(); i++) {
            isInsideJoint(i, x, y) ?
            (mJointMouseState.at(i)=MOUSE_DOWN) : (mJointMouseState.at(i)=MOUSE_IDLE);
        }
    }
    
    //------------------------------------------------------------------------------------
    void Picker::mouseReleased(int x, int y, int button)
    {
        /// pick a skeleton
        if(isInsideSkeleton(x, y)) {
            if (mSkeletonMouseState == MOUSE_DOWN) {
                /// notify event
                ofxEventMessage m;
                m.setAddress(event::ADDRESS_PICKED_SKELETON);
                m.addStringArg(mSkeletonHostName);
                ofxNotifyEvent(m);
            }
            mSkeletonMouseState = MOUSE_OVER;
        }
        else {
            mSkeletonMouseState = MOUSE_IDLE;
        }
        
        /// pick a joint
        for (int i=0; i<mJointMouseState.size(); i++) {
            if (isInsideJoint(i, x, y)) {
                if (mJointMouseState.at(i) == MOUSE_DOWN) {
                    /// notify event
                    ofxEventMessage m;
                    m.setAddress(event::ADDRESS_PICKED_JOINT);
                    m.addStringArg(mSkeletonHostName);
                    m.addIntArg(i);
                    ofxNotifyEvent(m);
                }
                mJointMouseState.at(i) = MOUSE_OVER;
            }
            else {
                mJointMouseState.at(i) = MOUSE_IDLE;
            }
        }
    }
    
    //------------------------------------------------------------------------------------
    void Picker::onMouseReleased(ofMouseEventArgs &e)
    {
        mouseReleased(e.x, e.y, e.button);
    }
    
    //------------------------------------------------------------------------------------
    void Picker::onMousePressed(ofMouseEventArgs &e)
    {
        mousePressed(e.x, e.y, e.button);
    }
    
    //------------------------------------------------------------------------------------
    void Picker::onMouseMoved(ofMouseEventArgs &e)
    {
        mouseMoved(e.x, e.y);
    }
    
    //------------------------------------------------------------------------------------
    void Picker::onMouseDragged(ofMouseEventArgs &e)
    {
        mouseDragged(e.x, e.y, e.button);
    }
    
    //------------------------------------------------------------------------------------
    bool Picker::isInsideSkeleton(int x, int y)
    {
        return (x>=mMinCoord.x && x<=mMaxCoord.x && y>=mMinCoord.y && y<=mMaxCoord.y);
    }
    
    //------------------------------------------------------------------------------------
    bool Picker::isInsideJoint(int jointId, int x, int y)
    {
        const ofVec2f &pos = mScreenJointLoc.at(jointId);
        const float dist = pos.distance(ofVec2f(x, y));
        return (dist<15.0f);
    }
}

