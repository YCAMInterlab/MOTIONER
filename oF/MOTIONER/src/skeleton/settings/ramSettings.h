//
//  ramSettings.h
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/8/12.
//
//

#ifndef __MOTIONER__ramSettings__
#define __MOTIONER__ramSettings__

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ramUtils.h"

namespace ram { namespace skeleton {
    
    class Skeleton;
    
    /// settings for each skeletons
    //------------------------------------------------------------------------------------
    class Settings {
        /// edit the skeleton directoly
        //--------------------
        friend class Skeleton;
        friend class SkeletonFinder;
        
    public:
        //--------------------
        Settings();
        ~Settings();
        
        /// load settings from shared().xmlSettings
        //--------------------
        void load(const string &filePath);
        void save(const string &filePath);
        
        /// load global positions for all joints in bind pose
        //--------------------
        void loadHierarchy(Skeleton *skeleton);
        void saveHierarchy(int nodeId, const ofVec3f &offset);
        
        /// default caribrated values
        //--------------------
        void loadCalibration(Skeleton *skeleton);
        void saveCalibration(Skeleton *skeleton);
        
        // tree structure
        //--------------------
        void loadTree(Skeleton *skeleton);
        
        void loadTreeJointTag(Skeleton *skeleton, int parent);
        
        // unuse
        //--------------------
        void loadUnuseJoints(Skeleton *skeleton);
        
        // properties
        //--------------------
        void loadProperties(Skeleton *skeleton);
        
        /// setters (also write into the xml file)
        //--------------------
        void setName(const string &name);
        
        /// gettters
        //--------------------
        inline const string &getFileName() const { return mFileName; }
        inline bool getIsLoaded() const { return mIsLoaded; }
        inline bool hasTree() const { return mHasTree; }
        
    private:
        /// xml utils
        //--------------------
        void _checkError(const string &name, int i);
        void _checkError(const string &name, float f);
        void _checkError(const string &name, const string &s);
        void _checkOffertAtrribExist(int tagId);
        
        //--------------------
        ofxXmlSettings  mXml;
        
        /// settings
        //--------------------
        string  mFileName;
        string  mName;
        string  mDeviceHostName;
        
        bool    mHasTree;
        
        bool    mIsLoaded;
        
        //--------------------
        static const int    kErrori;
        static const float  kErrorf;
        static const string kErrors;
    };
    
} }

#endif /* defined(__MOTIONER__ramSettings__) */
