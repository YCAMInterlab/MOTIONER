//
//  ramNode.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/8/12.
//
//

#include "ramNode.h"
#include "ramMesh.h"

using namespace ram;
using namespace ram::skeleton;

Node::Node() :
size(20.0f)
{
    
}

//----------------------------------------------------------------------------------------
void Node::customDraw()
{
    ofBox(size);
    Mesh::drawAxis(size);
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