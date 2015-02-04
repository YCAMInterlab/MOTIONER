//
//  ramRenderer.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/7/12.
//
//

#include "ramRenderer.h"
#include "ramSkeleton.h"
#include "ramConstants.h"

using namespace ram;
using namespace ram::skeleton;

//----------------------------------------------------------------------------------------
static void billboard()
{
	ofMatrix4x4 m;
	glGetFloatv(GL_MODELVIEW_MATRIX, m.getPtr());
	
	ofVec3f s = m.getScale();
	
	m(0, 0) = s.x;
	m(0, 1) = 0;
	m(0, 2) = 0;
	
	m(1, 0) = 0;
	m(1, 1) = s.y;
	m(1, 2) = 0;
	
	m(2, 0) = 0;
	m(2, 1) = 0;
	m(2, 2) = s.z;
	
	glLoadMatrixf(m.getPtr());
}

//----------------------------------------------------------------------------------------
Renderer::Renderer() :
mIsEnableShading(true),
mState(0),
mActiveJoint(JOINT_HIPS)
{
}

//----------------------------------------------------------------------------------------
Renderer::~Renderer()
{
    
}

//----------------------------------------------------------------------------------------
void Renderer::update(Skeleton *skleton)
{
    
}

//----------------------------------------------------------------------------------------
void Renderer::draw(Skeleton *skeleton) const
{
    ofxPushAll();
        
    vector<Node> &joints = skeleton->mJoints;
    
    ofNoFill();
    
    ofColor color;
    
    (mState==Skeleton::STATE_SELECTED) ? color.setHex(COLOR_H) : color.setHex(COLOR_M);    
    
    for (size_t i=0; i<joints.size(); i++) {
        
        ofSetColor(color);
        
        ofSetLineWidth(1.f);
        Node &n = joints.at(i);
        n.size = 60.0f;
        
        if (mIsEnableShading) {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            ofPushStyle();
            ofDisableAlphaBlending();
            glEnable(GL_DEPTH_TEST);
            ofEnableLighting();
            ofFill();
        }
        n.draw();
        if (mIsEnableShading) {
            ofDisableLighting();
            ofEnableAlphaBlending();
            ofPopStyle();
            glPopAttrib();
        }
        if (i==mActiveJoint && mState==Skeleton::STATE_SELECTED) {
            ofxPushAll();
            ofMultMatrix(n.getGlobalTransformMatrix());
            ofSetColor(255, 50, 50, 90);
            ofFill();
            billboard();
            ofCircle(ofVec3f::zero(), 50.0f);
            ofxPopAll();
        }
        
        /// velocity debug
        //ofSetColor(color);
        //ofBox(n.getGlobalPosition(), n.velocity * 10);
        
        (mState==Skeleton::STATE_SELECTED) ? ofSetLineWidth(2.0f) : ofSetLineWidth(1.0f);
        ofSetColor(color);
        if (!n.getParent()) continue;
        
         if (mIsEnableShading) {
             ofPushStyle();
             ofDisableLighting();
             glPushAttrib(GL_ALL_ATTRIB_BITS);
             glEnable(GL_DEPTH_TEST);
         }
        ofLine(n.getGlobalPosition(), n.getParent()->getGlobalPosition());
         if (mIsEnableShading) {
             glPopAttrib();
             ofPopStyle();
         }
    }
    
    ofPushMatrix();
    ofSetHexColor(COLOR_HILIGHT);
    ofMultMatrix(joints.at(JOINT_HEAD).getGlobalTransformMatrix());
    ofDrawBitmapString(skeleton->getHostName()+"\n"+skeleton->getName(),
                       ofPoint(0.0f, 100.0f));
    ofPopMatrix();
    
    ofxPopAll();
}

//----------------------------------------------------------------------------------------
void Renderer::setState(int state)
{
    //ofLogNotice("Renderer") << "state:" << state;
    mState = state;
}

//----------------------------------------------------------------------------------------
int Renderer::getState() const
{
    return mState;
}

//----------------------------------------------------------------------------------------
void Renderer::setActiveJoint(int joint)
{
    mActiveJoint = joint;
}
//----------------------------------------------------------------------------------------
int Renderer::getActiveJoint() const
{
    return mActiveJoint;
}
