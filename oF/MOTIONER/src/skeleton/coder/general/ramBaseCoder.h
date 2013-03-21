//
//  ramBaseCoder.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/4/12.
//
//

#ifndef MOTIONER_ramBaseCoder_h
#define MOTIONER_ramBaseCoder_h

#include "ofMain.h"
#include "ramTypes.h"

namespace ram { namespace skeleton {
    
    class Skeleton;
    
    namespace coder {
        
        //--------------------------------------------------------------------------------
        struct BaseCoder {
            //--------------------
            virtual ~BaseCoder() {}
            virtual void open(const string &filePath) = 0;
            virtual void save() = 0;
            virtual void close() = 0;
            virtual void clear() = 0;
            
            /// encode
            //--------------------
            virtual void writeHeader(const Skeleton *skeleton) = 0;
            virtual void writeHierarchy(const Skeleton *skeleton) = 0;
            virtual void writeFrame(const Skeleton *skeleton) = 0;
            
            /// decode
            //--------------------
            virtual void readHeader(Skeleton *skeleton) = 0;
            virtual void readHierarchy(Skeleton *skeleton) = 0;
            virtual void readFrame(Skeleton *skeleton, frame_t frame) = 0;
            
            //--------------------
            virtual string getFileExtension() const = 0;
            
            virtual frame_t getTotalFrames() const = 0;
        };
        
    }
} }


#endif
