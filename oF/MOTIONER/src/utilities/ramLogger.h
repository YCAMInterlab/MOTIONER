//
//  ramLogger.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/17/12.
//
//

#ifndef __MOTIONER__ramLogger__
#define __MOTIONER__ramLogger__

#include "ofMain.h"

namespace ram {
    
    //------------------------------------------------------------------------------------
    class LoggerChannel : public ofBaseLoggerChannel {
    public:
        LoggerChannel();
        void log(ofLogLevel level, const string & module, const string & message);
        void log(ofLogLevel logLevel, const string & module, const char* format, ...);
        void log(ofLogLevel logLevel, const string & module, const char* format, va_list args);
    private:
        static const int LINE_SPACING = 5;
        unsigned long long mLineCount;
    };
    
}

#endif /* defined(__MOTIONER__ramLogger__) */
