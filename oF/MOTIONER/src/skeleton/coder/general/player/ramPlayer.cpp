//
//  ramPlayer.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#include "ramPlayer.h"
#include "ramBaseCoder.h"
#include "ramBinaryCoder.h"
#include "ramUtils.h"
#include "ofxException.h"

using namespace ram;
using namespace ram::skeleton;
using namespace ram::skeleton::coder;

const float Player::FPS = 60.0f;

//----------------------------------------------------------------------------------------
Player::Player() :
mFilePath(""),
mLoopType(OF_LOOP_NORMAL),
mLoaded(false),
mPlaying(false),
mCurrentFrame(0.0f),
mTotalFrames(0),
mPlaySpeed(1.0f)
{
    startThread();
}

//----------------------------------------------------------------------------------------
Player::~Player()
{
    stopThread();
    close();
}

//----------------------------------------------------------------------------------------
void Player::open(const string &filePath, Skeleton *skeleton)
{
    mCoder = createCurrentCoder();
    
    if (mCoder) {
        
        mCoder->open(filePath);
        mCoder->readHeader(skeleton);
        mCoder->readHierarchy(skeleton);
        
        mFilePath = filePath;
        
        const frame_t total = mCoder->getTotalFrames();
        
        lock();
        mCurrentFrame = 0.0f;
        mTotalFrames = total;
        unlock();
    }
    else {
        ofxThrowException(ofxException, "Coder didn't allocated!");
    }
}

//----------------------------------------------------------------------------------------
void Player::close()
{
    if (mCoder)
        mCoder->close();
    
    mFilePath = "";
    
    lock();
    mLoaded = false;
    mPlaying = false;
    unlock();
}

//----------------------------------------------------------------------------------------
void Player::update(Skeleton *skeleton)
{
    lock();
    const frame_t f = static_cast<frame_t>(mCurrentFrame);
    unlock();
    
    //if (ofGetFrameNum()%60==0) {
    //    ofLogNotice("Player") << "updating frame " << f;
    //}

    //cout << f << ", ";
    
    if (mCoder && mPlaying) {
        mCoder->readFrame(skeleton, f);
    }
}

//----------------------------------------------------------------------------------------
void Player::threadedFunction()
{
    while (isThreadRunning()) {
        unsigned long long begin = ofGetElapsedTimeMillis();
        
        
        if (mPlaying) {
            lock();
            
            mCurrentFrame += mPlaySpeed;
            
            if (static_cast<frame_t>(mCurrentFrame) >= mTotalFrames) {
                if (mLoopType==OF_LOOP_NORMAL) {
                    mCurrentFrame = 0.0f;
                }
                else if (mLoopType==OF_LOOP_PALINDROME) {
                    mCurrentFrame = mTotalFrames-1.0f;
                    mPlaySpeed *= -1.0f;
                }
                else {
                    mCurrentFrame = 0.0f;
                    mPlaying = false;
                }
            }
            else if (static_cast<frame_t>(mCurrentFrame) < 0.0f) {
                if (mLoopType==OF_LOOP_NORMAL) {
                    mCurrentFrame = mTotalFrames-1.0f;
                }
                else if (mLoopType==OF_LOOP_PALINDROME) {
                    mCurrentFrame = 0.0f;
                    mPlaySpeed *= -1.0f;
                }
                else {
                    mCurrentFrame = 0.0f;
                    mPlaying = false;
                }
            }
            
            unlock();
        }
        
        
        unsigned long long elapsed = ofGetElapsedTimeMillis()-begin;
        int sleepMillis = 1000/FPS-elapsed;
        if (sleepMillis>0)
            sleep(sleepMillis);
    }
}

//----------------------------------------------------------------------------------------
void Player::play()
{
    mPlaying = true;
}

//----------------------------------------------------------------------------------------
void Player::stop()
{
    mPlaying = false;
    mCurrentFrame = 0;
    if (mPlaySpeed<0.0f)
        mPlaySpeed *= -1.0f;
}

//----------------------------------------------------------------------------------------
void Player::setPaused(bool bPause)
{
    lock();
    mPlaying = !bPause;
    unlock();
}

//----------------------------------------------------------------------------------------
void Player::setPosition(float pct)
{
    if (pct<0.0f)
        pct = 0.0f;
    else if (pct>1.0f)
        pct = 1.0f;
    
    lock();
    mCurrentFrame = mTotalFrames/pct;
    unlock();
}

//----------------------------------------------------------------------------------------
void Player::setLoopState(ofLoopType state)
{
    lock();
    mLoopType = state;
    unlock();
}

//----------------------------------------------------------------------------------------
void Player::setSpeed(float speed)
{
    lock();
    mPlaySpeed = speed;
    unlock();
}

//----------------------------------------------------------------------------------------
void Player::setFrame(frame_t frame)
{
    if (frame>=mTotalFrames)
        frame = mTotalFrames-1;
    lock();
    mCurrentFrame = static_cast<float>(frame);
    unlock();
}

//----------------------------------------------------------------------------------------
float Player::getPosition()
{
    lock();
    const float pos = mTotalFrames/mCurrentFrame;
    unlock();
    return pos;
}

//----------------------------------------------------------------------------------------
ofLoopType Player::getLoopState()
{
    lock();
    const ofLoopType loopType = mLoopType;
    unlock();
    return loopType;
}

//----------------------------------------------------------------------------------------
float Player::getSpeed()
{
    lock();
    const float spd = mPlaySpeed;
    unlock();
    return spd;
}

//----------------------------------------------------------------------------------------
frame_t Player::getCurrentFrame()
{
    lock();
    const frame_t f = static_cast<frame_t>(mCurrentFrame);
    unlock();
    return f;
}

//----------------------------------------------------------------------------------------
frame_t Player::getTotalFrames()
{
    lock();
    const frame_t f = mTotalFrames;
    unlock();
    return f;
}

//----------------------------------------------------------------------------------------
bool Player::getIsLoaded()
{
    lock();
    const bool loaded = mLoaded;
    unlock();
    return loaded;
}

//----------------------------------------------------------------------------------------
bool Player::getIsPlaying()
{
    lock();
    const bool playing = mPlaying;
    unlock();
    return playing;
}









