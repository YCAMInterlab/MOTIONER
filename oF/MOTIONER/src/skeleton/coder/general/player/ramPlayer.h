//
//  ramPlayer.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#ifndef __MOTIONER__ramPlayer__
#define __MOTIONER__ramPlayer__

#include "ofMain.h"
#include "ofxNonCopyable.h"
#include "ramCommon.h"

namespace ram { namespace skeleton {
    
    class Skeleton;
    
    namespace coder {
        
        class BaseCoder;
        
        //--------------------------------------------------------------------------------
        class Player : public ofxNonCopyable<Player>, public ofThread {
        public:            
            //--------------------
            Player();
            virtual ~Player();
            
            //--------------------
            void open(const string &filePath, Skeleton *skeleton);
            void close();/// write
            
            //--------------------
            void update(Skeleton *skeleton);
            void threadedFunction();
            
            /// controller
            //--------------------
            void play();
            void stop();
            
            /// accessors
            //--------------------
            void setPaused(bool bPause);
            void setPosition(float pct);
            void setLoopState(ofLoopType state);
            void setSpeed(float speed);
            void setFrame(frame_t frame);
            
            float getPosition();
            ofLoopType getLoopState();
            float getSpeed();
            frame_t getCurrentFrame();
            frame_t getTotalFrames();
            
            bool getIsLoaded();
            bool getIsPlaying();
            
            
        private: 
            //--------------------
            string mFilePath;
            CoderPtr mCoder;
            
            ofLoopType mLoopType;
            bool mLoaded;
            bool mPlaying;
            
            float mCurrentFrame;
            frame_t mTotalFrames;
            
            float mPlaySpeed;
            
            static const float FPS;
        };
        
    }
} }

#endif /* defined(__MOTIONER__ramPlayer__) */
