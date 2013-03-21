//
//  ramBinaryCoder.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/22/12.
//
//

#include "ramBinaryCoder.h"
#include "ramCommon.h"
#include "ramUtils.h"
#include "ramSkeleton.h"
#include "ofxException.h"

using namespace ram;
using namespace skeleton;
using namespace skeleton::coder;

//----------------------------------------------------------------------------------------
BinaryCoder::BinaryCoder() :
mIsOpened(false),
mFilePath(""),
mFileName(""),
mLastTime(0.0),
mNumFrames(0),
mRecordingFrameTime(MOTION_DATA_DEFAULT_FRAME_TIME),
mVersion(""),
mName(""),
mDate(""),
mHeaderLength(0)
{
}

//----------------------------------------------------------------------------------------
BinaryCoder::~BinaryCoder()
{
    close();
}

//----------------------------------------------------------------------------------------
void BinaryCoder::open(const string &filePath)
{
    //ofLogNotice("BinaryCoder") << "Open a file \"" + filePath + "\"";
    mBuffer = ofBufferExtendedFromFile(filePath, true); //// as binary
    const vector<string> pathes = ofSplitString(filePath, "/");
    if (!pathes.empty()) {
        mFileName = *(--pathes.end());
        ofStringReplace(mFileName, MOTION_DATA_EXT, "");
    }
    
    mFilePath =filePath;
    mIsOpened = true;
    mNumFrames = 0;
}

//----------------------------------------------------------------------------------------
void BinaryCoder::save()
{
    checkError();
    
    /// mode=binary
    bool ret = ofBufferExtendedToFile(mFilePath, mBuffer, true); //// as binary
    if (!ret)
        ofxThrowException(ofxException, "Cann't save the file! " + mFilePath);
    close();
}

//----------------------------------------------------------------------------------------
void BinaryCoder::checkError() const
{
    if (!mIsOpened)
        ofxThrowException(ofxException, "File isn't opened yet! " + mFilePath);
}

//----------------------------------------------------------------------------------------
void BinaryCoder::close()
{
    clear();
    mFilePath = "";
    mFileName = "";
    mNumFrames = 0;
    mIsOpened = false;
}

//----------------------------------------------------------------------------------------
void BinaryCoder::clear()
{
    mBuffer.clear();
}

/// encode
//----------------------------------------------------------------------------------------
void BinaryCoder::writeHeader(const Skeleton *skeleton)
{
    checkError();
    
    /// write fps
    FloatAndChar fc = { mRecordingFrameTime, };
    mBuffer.prependData(&(fc.c[0]), sizeof(mRecordingFrameTime));
    
    /// write total frames
    UInt64AndChar ic = { mNumFrames, };
    mBuffer.prependData(&(ic.c[0]), sizeof(mNumFrames));
    
    /// write date
    mDate = ofGetTimestampString("%Y/%m/%d %H:%M:%S");
    mBuffer.prependData(mDate.c_str(), mDate.length());
    Int32AndChar size = { mDate.length(), };
    mBuffer.prependData(size.c, sizeof(int32_t));
    
    /// write name
    mName = skeleton->getName();
    mBuffer.prependData(mName.c_str(), mName.length());
    size.i = mName.length();
    mBuffer.prependData(size.c, sizeof(int32_t));
    
    /// write version
    mVersion = MOTION_DATA_VERSION;
    mBuffer.prependData(mVersion.c_str(), mVersion.length());
    size.i = mVersion.length();
    mBuffer.prependData(size.c, sizeof(int32_t));
}

//----------------------------------------------------------------------------------------
void BinaryCoder::writeHierarchy(const Skeleton *skeleton)
{
    checkError();
    /// MOVE LOCATER HERE
    
    /// P.O.D
    FloatAndChar dat[N_JOINT_FLOATS*NUM_JOINTS];
    
    const vector<Node> &joints = skeleton->getJoints();
    
    for (int i=0; i<NUM_JOINTS; i++) {
        ofVec3f v = joints.at(i).getPosition();
        ofQuaternion q = joints.at(i).getGlobalOrientation();
        
        /// vector 3
        for (int j=0; j<ofVec3f::DIM; j++)
            dat[i*N_JOINT_FLOATS+j].f = v[j];
        
        /// quaternion xyzw
        for (int j=0; j<QUATERNION_DIM; j++)
            dat[i*N_JOINT_FLOATS+ofVec3f::DIM+j].f = q[j];
    }
    
    /// n joints * vector * quaternion * float bytes
    const size_t size = NUM_JOINTS*N_JOINT_FLOATS*sizeof(float);
    
    /// debug
    //for (int i=0; i<NUM_JOINTS; i++) {
    //    for (int j=0; j<ofVec3f::DIM; j++)
    //        cout << dat[i*N_JOINT_FLOATS+j].f << " ";
    //
    //    cout << "/";
    //
    //    /// quaternion xyzw
    //    for (int j=0; j<4; j++)
    //        cout << dat[i*N_JOINT_FLOATS+ofVec3f::DIM+j].f << " ";
    //
    //    cout << endl;
    //}
    
    mBuffer.append(&(dat[0].c[0]), size);
}

//----------------------------------------------------------------------------------------
void BinaryCoder::writeFrame(const Skeleton *skeleton)
{
    checkError();
    /// MOVE LOCATER HERE
    
    const double current = ofGetElapsedTimef();
    //const float elapsed = static_cast<float>(current-mLastTime);
    mLastTime = current;
    
    FloatAndChar dat[FRAME_HEADER_FLOATS + QUATERNION_DIM*NUM_JOINTS];
    
    /// FRAME_HEADER_FLOATS
    ///dat[0].f = elapsed;
    
    const vector<Node> &joints = skeleton->getJoints();
    
    /// get quaternion for each joints
    for (int i=0; i<NUM_JOINTS; i++) {
        ofQuaternion q = joints.at(i).getGlobalOrientation();
        
        /// offset = FRAME_HEADER_FLOATS
        for (int j=0; j<QUATERNION_DIM; j++)
            dat[FRAME_HEADER_FLOATS + i*QUATERNION_DIM + j].f = q[j];
    }
    
    const size_t size = (QUATERNION_DIM*NUM_JOINTS + FRAME_HEADER_FLOATS)*sizeof(float);
    mBuffer.append(&(dat[0].c[0]), size);
    
    mNumFrames++;
}


/// decode
//----------------------------------------------------------------------------------------
void BinaryCoder::readHeader(Skeleton *skeleton)
{
    checkError();
    mBuffer.resetLineReader();
    mHeaderLength = 0;
    
    char *buf = mBuffer.getBinaryBuffer();
    
    size_t size;
    /// version
    memcpy(&size, &buf[0], sizeof(int32_t));
    mHeaderLength += sizeof(int32_t);
    
    char version[size+1];
    memcpy(version, &buf[mHeaderLength], size);
    version[size] = NULL; /// c style str
    mVersion = string(version);
    mHeaderLength += size;
    //printf("version %s\n", mVersion.c_str()); // not crash
    
    /// name
    memcpy(&size, &buf[mHeaderLength], sizeof(int32_t));
    mHeaderLength += sizeof(int32_t);
    char name[size+1];
    memcpy(name, &buf[mHeaderLength], size);
    name[size] = NULL; /// c style str
    mName = string(name);
    mHeaderLength += size;
    
    /// name
    memcpy(&size, &buf[mHeaderLength], sizeof(int32_t));
    mHeaderLength += sizeof(int32_t);
    
    char date[size+1];
    memcpy(date, &buf[mHeaderLength], size);
    mDate[size] = NULL; /// c style str
    mDate = string(date);
    mHeaderLength += size;
    
    memcpy(&mNumFrames, &buf[mHeaderLength], sizeof(uint64_t));
    mHeaderLength += sizeof(uint64_t);
    
    memcpy(&mRecordingFrameTime, &buf[mHeaderLength], sizeof(float));
    mHeaderLength += sizeof(float);
    
    skeleton->setName(mName);

    const float secs = mNumFrames * mRecordingFrameTime;
    const unsigned int msec = static_cast<unsigned int>(fmodf(secs, 1.0f) * 1000.0f);
    const unsigned int seci = static_cast<unsigned int>(secs);
    const unsigned int sec = seci%60;
    const unsigned int min = (seci/60)%60;
    const unsigned int hou = (min/60)%12;
    
    ostringstream oss;
    oss << "Loaded a motion file" << endl <<
    "MOT version: " << mVersion << endl << // crash
    "Name: " << mName << endl <<
    "Date/Time: " << mDate << endl <<
    "Total frames: " << mNumFrames << endl <<
    "Frame time: " << mRecordingFrameTime << endl <<
    "Duration: " << hou << ":" << min << ":" << sec << "\"" << msec;

    ofLogNotice("BinaryCoder") << oss.str();
}

//----------------------------------------------------------------------------------------
void BinaryCoder::readHierarchy(Skeleton *skeleton)
{
    checkError();
    
    char *buf = mBuffer.getBinaryBuffer();
    mHierarchyLength = N_JOINT_FLOATS * NUM_JOINTS * sizeof(float);
    
    char data[mHierarchyLength];
    
    memcpy(data, buf+mHeaderLength, mHierarchyLength);
    
    /// read binary datachar* as float* (both types are P.O.D)
    const float *f = reinterpret_cast<const float *>(data);
    //const int nFloats = N_JOINT_FLOATS*NUM_JOINTS;
    //for (int i=0; i<nFloats; i++) {
    //    cout << f[i] << " ";
    //}
    //cout << endl;
    for (int i=0; i<NUM_JOINTS; i++) {
        const int loc = i*N_JOINT_FLOATS;
        skeleton->getJoint(i).setPosition(ofVec3f(f[loc+0],
                                                  f[loc+1],
                                                  f[loc+2]));
        skeleton->getJoint(i).setGlobalOrientation(ofQuaternion(f[loc+3],
                                                                f[loc+4],
                                                                f[loc+5],
                                                                f[loc+6]));
    }
}

//----------------------------------------------------------------------------------------
void BinaryCoder::readFrame(Skeleton *skeleton, frame_t frame)
{
    checkError();
    
    char *buf = mBuffer.getBinaryBuffer();
    const size_t headerLen = mHeaderLength + mHierarchyLength;
    mFrameLength = (FRAME_HEADER_FLOATS + QUATERNION_DIM*NUM_JOINTS)*sizeof(float);
    
    const frame_t head = static_cast<frame_t>(headerLen) + static_cast<frame_t>(mFrameLength)*frame;
    
    char data[mFrameLength];
    
    memcpy(data, buf+head, mFrameLength);
    
    const float *f = reinterpret_cast<const float *>(data);
    
    for (int i=0; i<NUM_JOINTS; i++) {
        const int loc = i*QUATERNION_DIM;
        skeleton->getJoint(i).setGlobalOrientation(ofQuaternion(f[loc+0],
                                                                f[loc+1],
                                                                f[loc+2],
                                                                f[loc+3]));
    }
    
    /// ease motion with time between frames
}

//----------------------------------------------------------------------------------------
string BinaryCoder::getFileExtension() const
{
    return MOTION_DATA_EXT;
}

//----------------------------------------------------------------------------------------
string BinaryCoder::getName() const
{
    return mName;
}

//----------------------------------------------------------------------------------------
string BinaryCoder::getDate() const
{
    return mDate;
}

//----------------------------------------------------------------------------------------
string BinaryCoder::getVersionStr() const
{
    return mVersion;
}

//----------------------------------------------------------------------------------------
float BinaryCoder::getRecordingFrameTime() const
{
    return mRecordingFrameTime;
}

//----------------------------------------------------------------------------------------
frame_t BinaryCoder::getTotalFrames() const
{
    return mNumFrames;
}

//----------------------------------------------------------------------------------------
void BinaryCoder::setRecordingFrameTime(float time)
{
    mRecordingFrameTime = time;
}



