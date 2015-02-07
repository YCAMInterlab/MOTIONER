//
//  ramSettings.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/8/12.
//
//

#include "ramSettings.h"
#include "ramConstants.h"
#include "ofxException.h"
#include "ramSkeleton.h"
#include "ramUtils.h"
#include "ofxException.h"

#include "ofxEvent.h"
#include "ramEvent.h"

#include "ramUtils.h"

using namespace ram;
using namespace ram::skeleton;

//----------------------------------------------------------------------------------------
const int       Settings::kErrori = -1;
const float     Settings::kErrorf = -1234.5678;
const string    Settings::kErrors = "NoValue";


/// default values for this class and xml setting
//----------------------------------------------------------------------------------------
Settings::Settings() :
mIsLoaded(false),
mDeviceHostName(""),
mName(kErrors)
{
}

//----------------------------------------------------------------------------------------
Settings::~Settings()
{
}

//----------------------------------------------------------------------------------------
void Settings::load(const string &filePath)
{
    /// load a xml file
    mFileName = filePath;
    
    ofLogNotice("Settings") << "Loading \"" + filePath + "\"...";
    
    bool bLoaded = mXml.loadFile(filePath);
    if (!bLoaded)
        ofxThrowException(ofxException, "Couldn't load a xml file \""+filePath+"\"");
    
    mName = mXml.getValue("ACTOR_NAME", kErrors);
    _checkError("ACTOR_NAME", mName);
    
    mHasTree = mXml.tagExists("TREE");
    if (mHasTree)
        ofLogNotice("Settings") << "Tree data found";
    
    /// notify loaded data
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_CHANGE_SKELETON_NAME);
    m.addStringArg(mName);
    ofxNotifyEvent(m);
    
    ostringstream oss;
    //oss << "\"" << filePath << "\" loaded" << endl
    oss << "Name: " << mName;
    //<< "Device host name: \"" << mDeviceHostName /*<< "\" Device port: " << mXBeeWiFiPort*/ ;
    ofLogNotice("Settings") << oss.str();
    mIsLoaded = true;
}

//----------------------------------------------------------------------------------------
void Settings::save(const string &fileName)
{
    if (mIsLoaded) {
        string file = "";
        if (fileName.empty())
            file = mFileName;
        else
            file = fileName;
        
        mXml.saveFile(file);
        mFileName = file;
        bool bLoaded = mXml.loadFile(file);
        if (!bLoaded)
            ofxThrowException(ofxException, "Couldn't load a xml file \""+file+"\"");
    }
}

//----------------------------------------------------------------------------------------
void Settings::loadJoints(Skeleton *skeleton)
{
    ofLogNotice("Settings") << "Loading joint settings...";
    
    bool ret = mXml.pushTag("JOINTS");
    if (!ret)
        ofxThrowException(ofxException, "JOINTS tag didn't found!");
    
    const string jointTag = "JOINT";
    const int nJoints = mXml.getNumTags(jointTag);
    
    if (!mHasTree && nJoints != NUM_JOINTS)
        ofxThrowExceptionf(ofxException, "Incorrect number of joints %d", nJoints);
    
    for (int i=0; i<nJoints; i++) {
        Node& n =skeleton->mJoints.at(i);
        
        const int id = mXml.getAttribute(jointTag, "ID", kErrori, i);
        _checkError("ID", id);
        n.id = id;
        
        const string name = mXml.getAttribute(jointTag, "NAME", kErrors, i);
        _checkError("NAME", name);
        n.name = name;
        
        // enable
        const int enable = mXml.getAttribute(jointTag, "ENABLE", kErrori, i);
        _checkError("ENABLE", enable);
        n.enable = enable;
        
        // positions
        const float px = mXml.getAttribute(jointTag, "PX", kErrorf, i);
        _checkError("PX", px);
        const float py = mXml.getAttribute(jointTag, "PY", kErrorf, i);
        _checkError("PY", py);
        const float pz = mXml.getAttribute(jointTag, "PZ", kErrorf, i);
        _checkError("PZ", pz);
        
        n.setPosition(ofVec3f(px, py, pz));
        
        // calibration
        const float rx = mXml.getAttribute(jointTag, "RX", kErrorf, i);
        _checkError("RX", rx);
        const float ry = mXml.getAttribute(jointTag, "RY", kErrorf, i);
        _checkError("RY", ry);
        const float rz = mXml.getAttribute(jointTag, "RZ", kErrorf, i);
        _checkError("RZ", rz);
        const float rw = mXml.getAttribute(jointTag, "RW", kErrorf, i);
        _checkError("RW", rw);
        
        skeleton->mBaseFrame.rotation.at(i).set(rx, ry, rz, rw);
        
        stringstream ss;
        ss << n.id << "|" << n.name << ": " << n.getPosition();
        ofLogNotice("Settings") << ss.str();
    }
    
    mXml.popTag();
    
    ofLogNotice("Settings") << "Loaded";
}

//----------------------------------------------------------------------------------------
void Settings::saveJoints(Skeleton *skeleton)
{
    bool ret = mXml.pushTag("JOINTS");
    if (!ret)
        ofxThrowException(ofxException, "JOINTS tag didn't found!");
    
    const string jointTag = "JOINT";
    const int nJoints = mXml.getNumTags(jointTag);
    
    if (!mHasTree && nJoints != NUM_JOINTS)
        ofxThrowExceptionf(ofxException, "Incorrect number of joints %d", nJoints);
    
    for (int i=0; i<nJoints; i++) {
        const Node& n = skeleton->getJoint(i);
        if (i != n.id)
            ofxThrowExceptionf(ofxException, "Joint index dosen't match %d vs %d", i, n.id);
        saveJointAttributes(skeleton, n.id);
    }
    
    mXml.popTag();
}

//----------------------------------------------------------------------------------------
void Settings::saveJoint(Skeleton *skeleton, int jointId)
{
    bool ret = mXml.pushTag("JOINTS");
    if (!ret)
        ofxThrowException(ofxException, "JOINTS tag didn't found!");
    
    saveJointAttributes(skeleton, jointId);
    
    mXml.popTag();
}

//----------------------------------------------------------------------------------------
void Settings::saveJointAttributes(Skeleton *skeleton, int jointId)
{
    const Node& n = skeleton->getJoint(jointId);
    
    int ret = 0;
    const string jointTag = "JOINT";
    
    const int id = n.id;
    ret = mXml.setAttribute(jointTag, "ID", id, n.id);
    _checkAtrribExist(ret);
    
    const string name = n.name;
    ret = mXml.setAttribute(jointTag, "NAME", name, n.id);
    _checkAtrribExist(ret);
    
    const int enable = (int)n.enable;
    ret = mXml.setAttribute(jointTag, "ENABLE", enable, n.id);
    _checkAtrribExist(ret);
    
    const ofVec3f& offset = n.getPosition();
    ret = mXml.setAttribute(jointTag, "PX", offset.x, n.id);
    _checkAtrribExist(ret);
    ret = mXml.setAttribute(jointTag, "PY", offset.y, n.id);
    _checkAtrribExist(ret);
    ret = mXml.setAttribute(jointTag, "PZ", offset.z, n.id);
    _checkAtrribExist(ret);
    
    const ofQuaternion& q = skeleton->mBaseFrame.rotation.at(n.id);
    
    ret = mXml.setAttribute(jointTag, "RX", q.x(), n.id);
    _checkAtrribExist(ret);
    ret = mXml.setAttribute(jointTag, "RY", q.y(), n.id);
    _checkAtrribExist(ret);
    ret = mXml.setAttribute(jointTag, "RZ", q.z(), n.id);
    _checkAtrribExist(ret);
    ret = mXml.setAttribute(jointTag, "RW", q.w(), n.id);
    _checkAtrribExist(ret);
}

//----------------------------------------------------------------------------------------
void Settings::loadTree(Skeleton *skeleton)
{
    OFX_BEGIN_EXCEPTION_HANDLING
    
    skeleton->mJoints.clear();
    skeleton->mJoints.assign(NUM_JOINTS, Node());
    
    if (mXml.tagExists("TREE")) {
        
        mXml.pushTag("TREE");
        
        for (int i=0; i<mXml.getNumTags("JOINT"); i++) {
            
            const int parent = mXml.getAttribute("JOINT", "ID", 0, i);
            
            mXml.pushTag("JOINT", i);
            
            loadTreeJointTag(skeleton, parent);
            
            mXml.popTag();
            
        }
        
        mXml.popTag();
        
    }
    else {
        ofxThrowException(ofxException, "Tree tag didn't find");
    }
    
    
    OFX_END_EXCEPTION_HANDLING
}

//----------------------------------------------------------------------------------------
void Settings::loadTreeJointTag(Skeleton *skeleton, int parent)
{
    for (int i=0; i<mXml.getNumTags("JOINT"); i++) {
        
        int id = mXml.getAttribute("JOINT", "ID", 0, i);
        
        ofLogNotice("Settings") << "child: " << id << " >>> " << "parent: " << parent;
        skeleton->mJoints.at(id).setParent(skeleton->mJoints.at(parent));
        
        mXml.pushTag("JOINT", i);
        
        loadTreeJointTag(skeleton, id);
        
        mXml.popTag();
    }
}


//----------------------------------------------------------------------------------------
void Settings::loadFlags(Skeleton *skeleton)
{
    if (mXml.tagExists("USE_IK")) {
        bool useIK = (bool)mXml.getValue("USE_IK", 1);
        useIK ? skeleton->enable(Skeleton::EASY_IK) : skeleton->disable(Skeleton::EASY_IK);
    }
    
}

//----------------------------------------------------------------------------------------
void Settings::loadColor(Skeleton *skeleton)
{
    if (mXml.tagExists("COLOR")) {
        mXml.pushTag("COLOR");
        const int r = mXml.getValue("R", 255);
        const int g = mXml.getValue("G", 255);
        const int b = mXml.getValue("B", 255);
        skeleton->setColorR(r);
        skeleton->setColorG(g);
        skeleton->setColorB(b);
        mXml.popTag();
    }
}

//----------------------------------------------------------------------------------------
void Settings::saveColor(Skeleton *skeleton)
{
    mXml.pushTag("COLOR");
    mXml.setValue("R", skeleton->getColor().r);
    mXml.setValue("G", skeleton->getColor().g);
    mXml.setValue("B", skeleton->getColor().b);
    mXml.popTag();
}

//----------------------------------------------------------------------------------------
void Settings::setName(const string &name)
{
    mName = name;
    mXml.setValue("ACTOR_NAME", mName);
}

/// xml utils
//----------------------------------------------------------------------------------------
void Settings::_checkError(const string &name, int i)
{
    if (i==kErrori)
        ofxThrowException(ofxException, "Couldn't load a setting which is tagged "+name);
}

//----------------------------------------------------------------------------------------
void Settings::_checkError(const string &name, float f)
{
    if (f==kErrorf)
        ofxThrowException(ofxException, "Couldn't load a setting which is tagged "+name);
}

//----------------------------------------------------------------------------------------
void Settings::_checkError(const string &name, const string &s)
{
    if (s == kErrors)
        ofxThrowException(ofxException, "Couldn't load a setting which is tagged "+name);
}

//----------------------------------------------------------------------------------------
void Settings::_checkAtrribExist(int tagId)
{
    if (tagId < 0 || tagId >= mXml.getNumTags("JOINT"))
        ofxThrowException(ofxException, "No such tag or attribute found!");
}


