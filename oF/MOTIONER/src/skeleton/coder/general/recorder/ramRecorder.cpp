//
//  ramRecorder.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#include "ramRecorder.h"
#include "ramBaseCoder.h"
#include "ramBinaryCoder.h"
#include "ramUtils.h"
#include "ofxException.h"

using namespace ram;
using namespace ram::skeleton;
using namespace ram::skeleton::coder;


//----------------------------------------------------------------------------------------
Recorder::Recorder() :
mFilePath(""),
mIsOpening(false)
{
}

//----------------------------------------------------------------------------------------
Recorder::~Recorder()
{
    
}

//----------------------------------------------------------------------------------------
void Recorder::open(const string &filePath, const Skeleton *skeleton)
{
    if (mIsOpening)
        close(skeleton);
    
    mCoder = createCurrentCoder();
    
    if (mCoder) {
        mCoder->open(filePath);
        mCoder->clear();
        mCoder->writeHierarchy(skeleton);
        mIsOpening = true;
    }
    else {
        ofxThrowException(ofxException, "Coder didn't allocated!");
    }
}

//----------------------------------------------------------------------------------------
void Recorder::close(const Skeleton *skeleton)
{
    if (mCoder) {
        mIsOpening = false;
        mCoder->writeHeader(skeleton);
        mCoder->save();
        mCoder->close();
    }
    else {
        ofxThrowException(ofxException, "Coder didn't allocated!");
    }
}

//----------------------------------------------------------------------------------------
void Recorder::update(const Skeleton *skeleton)
{
    if (mCoder) {
        mCoder->writeFrame(skeleton);
    }
    else {
        ofxThrowException(ofxException, "Coder didn't allocated!");
    }
}
