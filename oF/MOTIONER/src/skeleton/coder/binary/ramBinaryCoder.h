//
//  ramBinaryCoder.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/22/12.
//
//

#ifndef __MOTIONER__ramBinaryCoder__
#define __MOTIONER__ramBinaryCoder__

#include "ramBaseCoder.h"
#include "ofBufferExtended.h"

namespace ram { namespace skeleton { namespace coder {
    
    //------------------------------------------------------------------------------------
    class BinaryCoder : public BaseCoder {
    public:
        //--------------------
        explicit BinaryCoder();
        ~BinaryCoder();
        
        //--------------------
        void open(const string &filePath);
        void save();
        
        /// encode
        //--------------------
        void writeHierarchy(const Skeleton *skeleton);
        void writeFrame(const Skeleton *skeleton);
        
        /// decode
        //--------------------
        void readHeader(Skeleton *skeleton);
        void readHierarchy(Skeleton *skeleton);
        void readFrame(Skeleton *skeleton, frame_t frame);
        
        /// getter
        //--------------------
        string getFileExtension() const;
        string getName() const;
        string getDate() const;
        string getVersionStr() const;
        float getRecordingFrameTime() const;
        frame_t getTotalFrames() const;
        
        /// setter
        //--------------------
        void setRecordingFrameTime(float time);
        
    private:
        /// encode
        //--------------------
        /// write header when finished
        void writeHeader(const Skeleton *skeleton);
        
        //--------------------
        void checkError() const;
        void clear();
        void close();
        
        //--------------------
        ofBufferExtended mBuffer;
        bool mIsOpened;
        string mFileName;
        string mFilePath;
        double mLastTime;
        
        //--------------------
        frame_t mNumFrames;
        string mVersion;
        string mName;
        string mDate;
        float mRecordingFrameTime;
        
        size_t mHeaderLength;
        size_t mHierarchyLength;
        size_t mFrameLength;
        
        //--------------------
        static const int QUATERNION_DIM = 4;
        static const int FRAME_HEADER_FLOATS = 0;
        /// vector + quaternion
        static const int N_JOINT_FLOATS = QUATERNION_DIM + ofVec3f::DIM;
        
    };
    
} } }

#endif /* defined(__MOTIONER__ramBinaryCoder__) */
