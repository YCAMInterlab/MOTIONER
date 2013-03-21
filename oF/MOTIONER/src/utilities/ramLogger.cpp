//
//  ramLogger.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/17/12.
//
//

#include "ramLogger.h"
#include "ramConstants.h"
#include "ofxEvent.h"
#include "ramEvent.h"

using namespace ram;


//----------------------------------------------------------------------------------------
LoggerChannel::LoggerChannel() :
mLineCount(0)
{
}

//----------------------------------------------------------------------------------------
void LoggerChannel::log(ofLogLevel level,
                        const string & module,
                        const string & message)
{
    string lineSpace("");
    string lineCountStr = ofToString(mLineCount);
    for (int i=0; i<LINE_SPACING-lineCountStr.length(); i++)
        lineSpace += " ";
    
    vector<string> lines = ofSplitString(message, "\n");
    ostringstream oss;
    oss <<  mLineCount << lineSpace << " [";
    if (module!="OF")
        oss << module << ":";
    oss << ofGetLogLevelName(level) << "]";

    const size_t size = oss.str().size();
    for (size_t i=0; i<LOGGER_INDENT-size; i++)
        oss << " ";
    for (size_t i=0; i<lines.size(); i++) {
        if (i!=0)
            for (int j=0; j<LOGGER_INDENT; j++)
                oss << " ";
        oss << lines.at(i) << endl;
    }
    
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_LOG);
    m.addStringArg(oss.str());
    ofxNotifyEvent(m);
    
#ifdef DEBUG
    cout << oss.str();
#else
    if (ofGetLogLevel()>=OF_LOG_ERROR)
        cout << oss.str();
#endif
    mLineCount++;
}

//----------------------------------------------------------------------------------------
void LoggerChannel::log(ofLogLevel logLevel,
                        const string & module,
                        const char* format, ...)
{
	va_list args;
	va_start(args, format);
	log(logLevel, module, format, args);
	va_end(args);
}

//----------------------------------------------------------------------------------------
void LoggerChannel::log(ofLogLevel logLevel,
                        const string & module,
                        const char* format,
                        va_list args)
{
    string lineSpace("");
    string lineCountStr = ofToString(mLineCount);
    for (int i=0; i<LINE_SPACING-lineCountStr.length(); i++)
        lineSpace += " ";
    
    vector<string> lines = ofSplitString(ofVAArgsToString(format, args), "\n");
    ostringstream oss;
    oss << mLineCount << lineSpace << " [";
    if (module!="OF")
        oss << module << ":";
    else
        oss << "MOTIONER" << ":";
    oss << ofGetLogLevelName(logLevel) << "]";
    const size_t size = oss.str().size();
    for (size_t i=0; i<LOGGER_INDENT-size; i++)
        oss << " ";
    for (size_t i=0; i<lines.size(); i++) {
        if (i!=0)
            for (int j=0; j<LOGGER_INDENT; j++)
                oss << " ";
        oss << lines.at(i) << endl;
    }
    
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_LOG);
    m.addStringArg(oss.str());
    ofxNotifyEvent(m);
    
#ifdef DEBUG
    cout << oss.str();
#else
    if (logLevel>=OF_LOG_ERROR)
        cout << oss.str();
#endif
    
    mLineCount++;
}


