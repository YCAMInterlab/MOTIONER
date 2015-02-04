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
        ofPushStyle();
        ofFill();
        
        ofSetColor(color);
        
        ofSetLineWidth(1.f);
        Node &n = joints.at(i);
        isEndSite(i) || i == 0 ? n.size = 110.f : n.size = 80.f;
        
        n.draw();

        //if (i==mActiveJoint && mState==Skeleton::STATE_SELECTED) {
        //ofxPushAll();
        //ofMultMatrix(n.getGlobalTransformMatrix());
        //float t = ::fmodf(ofGetElapsedTimef() * 1.f, 1.f);
        //
        //ofSetColor(255, 50, 50, 90 + t * 50.f);
        //ofFill();
        //billboard();
        //ofCircle(ofVec3f::zero(), 50.0f + t * 80.f);
        //ofxPopAll();
        //}
        
        ofSetColor(color);
        if (!n.getParent()) continue;
        
        const ofVec3f v0 = n.getGlobalPosition();
        const ofVec3f v1 = n.getParent()->getGlobalPosition();
        
        ofPushStyle();
        ofSetLineWidth(2.f);
        ofSetColor(255, 255, 255, 255);
        ofLine(v0, v1);
        ofPopStyle();
        
        int d = v0.distance(v1);
        const int s = 60;
        if (d%s < s /10 || d%s > s - s / 10)
            d += s / 5;
        const int repeat = d/s;
        for (int j=0; j<repeat; j++) {
            const float t = j/(float)repeat;
            ofSetColor(140, 140, 140);
            ofPushMatrix();
            ofTranslate(v0.interpolated(v1, t));
            ofDrawBox(ofVec3f::zero(), s*0.8f);
            ofPopMatrix();
        }
        
         ofPopStyle();
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
