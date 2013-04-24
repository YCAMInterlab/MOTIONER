//
//  ramDeviceCorrespondent.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/7/12.
//
//

#include "ramDeviceCorrespondent.h"

#include "ramCommon.h"
#include "ramSkeleton.h"
#include "ramSkeletonManager.h"
#include "ramEvent.h"
#include "ramUI.h"

#include "ofxException.h"
#include "ofxNetworkUtils.h"


using namespace ram;

//----------------------------------------------------------------------------------------
int	DeviceCorrespondent::ofxUDPManagerExtend::ReceiveBytes(uint8_t* pBuff, const int iSize)
{
    if (m_hSocket == INVALID_SOCKET) {
        printf("INVALID_SOCKET");
        return(SOCKET_ERROR);
    }
    
#ifndef TARGET_WIN32
    socklen_t nLen= sizeof(sockaddr);
#else
    int	nLen= sizeof(sockaddr);
#endif
    
    int	ret=0;
    
    memset(pBuff, 0, iSize);
    ret= recvfrom(m_hSocket, reinterpret_cast<char *>(pBuff), iSize, 0, (sockaddr *)&saClient, &nLen);
    
    if (ret	> 0) {
        canGetRemoteAddress= true;
    }
    else {
        //ofxNetworkCheckError();
        canGetRemoteAddress= false;
    }
    
    return ret;
}

//----------------------------------------------------------------------------------------
DeviceCorrespondent::DeviceCorrespondent()
{
    mFrameMap.clear();
    
    for (int i=0; i<NUM_JOINTS; i++) /// n joints
        for (int j=0; j<IMU_DATA_DIM; j++) /// quaternion xyzw
            mData[i][j].f = 0.0f;
}

//----------------------------------------------------------------------------------------
DeviceCorrespondent::~DeviceCorrespondent()
{
}

//----------------------------------------------------------------------------------------
DeviceCorrespondent &DeviceCorrespondent::getInstance()
{
    static DeviceCorrespondent instance;
    return instance;
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::threadedFunction()
{
    ofLogNotice("Device") << "Start UDP thread";
    
    while (isThreadRunning()) {
        
        //const unsigned long long frameBegin = ofGetElapsedTimeMillis();
        
        uint8_t *udpMessage = new uint8_t[UDP_PACKET_BUFFER];
        
        const ssize_t size =  mUdpReceiver.ReceiveBytes(udpMessage, UDP_PACKET_BUFFER);
        
        if (size == SOCKET_ERROR) {
            /// no host found
            //ostringstream oss;
            //oss << __FUNCTION__ << " Couldn't receive message";
            delete [] udpMessage;
            continue;
        }
        
        /// parse udp packet
        /// make quaternion from incoming data
        if (size == UDP_MESSAGE_LENGTH) { /// correct data length
            
            /// parser
            parseUdpPacket(udpMessage);
        }
        
        /// currentlly disabled for workshop and shooting
        //checkTimeoutDevie();
        
        /// this thread should run as fast as possible
        
        //const unsigned long long frameTime = ofGetElapsedTimeMillis()-frameBegin;
        //
        //const unsigned long long sleepTime = 1000/UDP_REFRESH_RATE-frameTime;
        //if (sleepTime>0)
        //    sleep(static_cast<int>(sleepTime));
        
        delete [] udpMessage;
    }
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::parseUdpPacket(uint8_t udpMessage[])
{
    char ip[255];
    mUdpReceiver.GetRemoteAddr(ip);
    const string hostName = ip;
    
    const char eolData[] =
    {
        udpMessage[UDP_MESSAGE_LENGTH-4],
        udpMessage[UDP_MESSAGE_LENGTH-3],
        udpMessage[UDP_MESSAGE_LENGTH-2],
        udpMessage[UDP_MESSAGE_LENGTH-1],
        NULL
    };
    
    const string eol = eolData;
    if (eol == UDP_PACKET_DELIMITER) { /// correct data
        skeleton::coder::Frame frame;
        
        //for (int i=0; i<18; i++) {
        //    for (int j=0; j<6; j++) {
        //        FloatAndByte f;
        //        for (int k=0; k<4; k++) {
        //            const int idx = k + j*4 + i*6*4;
        //            f.b[k] = udpMessage[idx];
        //        }
        //        cout << f.f << ", ";
        //    }
        //    cout << endl;
        //}
        //cout << endl;
        
        int nEndSite = 0;
        
        /// skip endsite becase MOTIONER app skeleton has 23 joints but MOTIONER device has 18 IMUs
        for (int i=0; i<NUM_JOINTS; i++) { /// n joints
            if (isEndSite(i)) {
                nEndSite++;
                continue; /// skip end site
            }
            
            //if (ofGetFrameNum()%60==0)
            //cout << i << ": ";
            
            for (int j=0; j<IMU_DATA_DIM; j++) { /// quaternion xyzw + velocity
                for (int k=0; k<FLOAT_BYTES; k++) { /// float bytes
                    /// i*4*5 + i*4 + k
                    const int idx =
                    (i-nEndSite)*IMU_DATA_DIM*FLOAT_BYTES + j*FLOAT_BYTES+k;
                    mData[i][j].b[k] = udpMessage[idx];
                }
                //if (ofGetFrameNum()%60==0)
                //    cout << mData[i][j].f << ",";
            }
            //if (ofGetFrameNum()%60==0)
            //    cout << endl;
            
            
            /// get velocity
            if (mData[i][4].f!=0.0f) {
                //cout << i << ":" << mData[i][4].f << endl;
                frame.velocity.at(i) = mData[i][4].f;
            }
            
            bool isNan = false;
            
            for (int k=0; k<QUATERNION_DIM; k++) {
                if (!isnan(mData[i][k].f)) {
                    //cout << mData[i][k].f << ", ";
                }
                else {
                    isNan = true;
                    //ofLogWarning("Device") << "Node:" << i << " Quaternion:" << k << " is nan";
                }
            }
            
            /// get quaternion
            if (!isNan) {
                
                //ofQuaternion x(ofRadToDeg(mData[i][0].f), ofVec3f(1.0f, 0.0f, 0.0f));
                //ofQuaternion y(ofRadToDeg(mData[i][1].f), ofVec3f(0.0f, 1.0f, 0.0f));
                //ofQuaternion z(ofRadToDeg(mData[i][2].f), ofVec3f(0.0f, 0.0f, 1.0f));
                //ofQuaternion q = x*y*z;
                
                /// format FreeIMU axsis system to OpenGL axis system
                //frame.rotation.at(i) =
                //q.conj() *
                ////ofQuaternion(180.0f, ofVec3f(0.0f, 1.0f, 0.0f)) * //reverse
                //ofQuaternion(90.0f, ofVec3f(0.0f, 0.0f, 1.0f));
                
                ofQuaternion q(mData[i][2].f, mData[i][1].f, mData[i][0].f, mData[i][3].f);
                frame.rotation.at(i) = q *
                ofQuaternion(90.0f, ofVec3f(0.0f, 0.0f, 1.0f));
            }
        }
        frame.time = ofGetElapsedTimef();
        
        //cout << endl;
        
        lock();
        /// create new Frame and Skeleton
        SkeletonMap &skeletons = skeleton::SkeletonManager::getInstance().getSkeletons();
        
        if (skeletons.find(hostName)==skeletons.end()) {
            ofLogNotice("Device") << "New device \"" << hostName << "\" available";
            skeleton::SkeletonManager::getInstance().addSkeleton(hostName);
        }
        
        /// set current frame to mFrameMap
        mFrameMap[hostName] = frame; /// insert or replace
        unlock();
    }
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::checkTimeoutDevie()
{
    lock();
    /// timeout device
    const double currentTime = ofGetElapsedTimef();
    FrameMap::iterator frameMapIt = mFrameMap.begin();
    while (frameMapIt!=mFrameMap.end()) {
        if (currentTime >= frameMapIt->second.time + DEVICE_TIMEOUT_DUR) {
            ofLogNotice("Device") << "\"" << frameMapIt->first << "\" timeout";
            mRemoveSkeleton.push_back(frameMapIt->first);
            mFrameMap.erase(frameMapIt++);
        }
        else {
            ++frameMapIt;
        }
    }
    unlock();
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::onMessageReceived(ofxEventMessage &m)
{
    if (m.getAddress()==event::ADDRESS_SETUP_DEVICE_CORRESPONDENT) {
        connect(m.getArgAsInt32(0));
    }
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::setup()
{
    ofLogNotice("Device") << "Initializing DeviceCorrespondent...";
    ofAddListener(ofxEvent(), this, &DeviceCorrespondent::onMessageReceived);
    connect(UI::getInstance().getDeviceIncomingUdpPort());
    
    mRemoveSkeleton.clear();
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::update()
{
    lock();
    for (int i=0; i<mRemoveSkeleton.size(); i++)
        skeleton::SkeletonManager::getInstance().removeSkeleton(mRemoveSkeleton.at(i));
    
    if (mRemoveSkeleton.empty()==false)
        mRemoveSkeleton.clear();
    unlock();
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::exit()
{
    disconnect();
    /// bug: can not lock Poco::Mutex
    //ofRemoveListener(ofxEvent(), this, &DeviceCorrespondent::onMessageReceived);
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::connect(int myPort)
{
    if (isThreadRunning()) {
        stopThread();
        //ofSleepMillis(100);
    }
    
    mUdpReceiver.Close();
    mUdpReceiver.Create();
    bool success;
    success = mUdpReceiver.Bind(myPort);
    if (!success)
        ofxThrowExceptionf(ofxException, "Couldn't bind to port %d", myPort);
    
    ofLogNotice("Device")<< "Successfully bound to port: " << myPort;
    mUdpReceiver.SetNonBlocking(true);
    
    startThread();
}

//----------------------------------------------------------------------------------------
void DeviceCorrespondent::disconnect()
{
    ofLogNotice("Device") << "Closing port...";
    stopThread();
    ofSleepMillis(500);
    mUdpReceiver.Close();
}

//----------------------------------------------------------------------------------------
skeleton::coder::Frame DeviceCorrespondent::getFrame(const string& hostName)
{
    skeleton::coder::Frame f;
    bool found = false;
    
    lock();
    
    FrameMap::iterator it = mFrameMap.find(hostName);
    
    if (it!=mFrameMap.end()) {
        found = true;
        f = it->second;
    }
    
    unlock();
    
    if (found) {
        return f;
    }
    else {
        //ofxThrowExceptionf(ofxException, "Host %s didn't find!", hostName.c_str());
        static skeleton::coder::Frame defaultFrame;
        return defaultFrame; /// not found
    }
}
