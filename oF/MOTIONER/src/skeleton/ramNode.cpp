//
//  ramNode.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/8/12.
//
//

#include "ramNode.h"
#include "ramMesh.h"
#include "ramCommon.h"

using namespace ram;
using namespace ram::skeleton;

Node::Node() :
size(20.0f),
enable(true),
noResponceDuration(0.f),
error(false)
{
    
}

void Node::update()
{
    const ofVec3f& orientation = getOrientationEuler();
        
    if (enable &&
        orientation.distance(prevOrientationEuler) < kJointErrorEpsilon) {
        noResponceDuration += ofGetLastFrameTime();
        if (noResponceDuration >= kJointErrorThreshould) {
            error = true;
        }
    }
    else {
        noResponceDuration = 0.f;
        error = false;
    }
    prevOrientationEuler = orientation;
}

//----------------------------------------------------------------------------------------
void Node::customDraw()
{
    ofBox(size);
    //Mesh::drawAxis(size * 0.8f);
}

//----------------------------------------------------------------------------------------
NodeFinder::NodeFinder(const string &name) : name(name), id(0), mode(NAME)
{
}

//----------------------------------------------------------------------------------------
NodeFinder::NodeFinder(int id) : id(id), name(""), mode(ID)
{
}

//----------------------------------------------------------------------------------------
bool NodeFinder::operator ()(const Node &n)
{
    switch (mode) {
        case NAME: return (n.name == name);
        case ID: return (n.id == id);
    }
}