//
//  ramDeviceCorrespondent.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/7/12.
//
//

#ifndef __MOTIONER__ramDeviceCorrespondent__
#define __MOTIONER__ramDeviceCorrespondent__

#include "ofMain.h"

#include "ramFrame.h"
#include "ramUtils.h"
#include "ramTypes.h"

#include "ofxUDPManager.h"
#include "ofxNonCopyable.h"
#include "ofxEvent.h"

namespace ram {
    
    //------------------------------------------------------------------------------------
    RAM_SINGLETON_CLASS
    class DeviceCorrespondent : public ofThread, ofxNonCopyable<DeviceCorrespondent>  {
    public:
        //--------------------
        typedef map<string, skeleton::coder::Frame> FrameMap;
        
        //--------------------
        ~DeviceCorrespondent();
        
        /// open UDP incoming port and start receiver thread
        //--------------------
        static DeviceCorrespondent &getInstance();
        
        //--------------------
        void setup();
        void update();
        void exit();
        
        /// UDP receiver thread
        /// run as fast as can
        //--------------------
        void threadedFunction();
        
        /// event
        //--------------------
        void onMessageReceived(ofxEventMessage &m);
        
        /// getter
        //--------------------
        skeleton::coder::Frame getFrame(const string& hostName);
        
        /// not calibrated
        //--------------------
        skeleton::coder::Frame getRaWFrame(const string& hostName);
        
        /// calibrate device with bind(T) pose
        //--------------------
        void calibrate(const string& hostName);
        
    private:
        /// udp receiver
        //--------------------
        struct ofxUDPManagerExtend : public ofxUDPManager {
            int	ReceiveBytes(uint8_t* pBuff, const int iSize);
        };
        
        /// singleton
        //--------------------
        DeviceCorrespondent();
        
        /// manegement udp socket
        //--------------------
        void connect(int myPort);
        void disconnect();
        
        /// the following methods are called from udp thread
        //--------------------
        void parseUdpPacket(uint8_t udpMessage[]);
        void checkTimeoutDevie();
        
        //--------------------
        ofxUDPManagerExtend mUdpReceiver;
        FrameMap            mFrameMap;
        FloatAndByte        mData[NUM_JOINTS][5];
        
        vector<string>      mRemoveSkeleton;
        
    };
}

#endif /* defined(__MOTIONER__ramDeviceCorrespondent__) */
