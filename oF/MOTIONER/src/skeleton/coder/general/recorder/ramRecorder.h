//
//  ramRecorder.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#ifndef __MOTIONER__ramRecorder__
#define __MOTIONER__ramRecorder__

#include "ofMain.h"
#include "ramUtils.h"

namespace ram { namespace skeleton {
    
    class Skeleton;
    
    namespace coder {
        
        class BaseCoder;
        
        //--------------------------------------------------------------------------------
        class Recorder {
        public:            
            //--------------------
            Recorder();
            ~Recorder();
            
            //--------------------
            void open(const string &filePath, const Skeleton *skeleton);
            void close(const Skeleton *skeleton);/// write
            
            //--------------------
            void update(const Skeleton *skeleton);
            
        private:
            //--------------------
            string mFilePath;
            CoderPtr mCoder;
            bool mIsOpening;
        };
        
    }
} }

#endif /* defined(__MOTIONER__ramRecorder__) */
