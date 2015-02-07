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


namespace ram { namespace skeleton {
    
    class Skeleton;
    class Node;
    
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
        
        /// load joint attributes
        //--------------------
        void loadJoints(Skeleton *skeleton);
        void saveJoints(Skeleton *skeleton);
        void saveJoint(Skeleton *skeleton, int jointId);
        
        // tree structure
        //--------------------
        void loadTree(Skeleton *skeleton);
        void loadTreeJointTag(Skeleton *skeleton, int parent);
        
        // properties
        //--------------------
        void loadFlags(Skeleton *skeleton);
        
        void loadColor(Skeleton *skeleton);
        void saveColor(Skeleton *skeleton);
        
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
        void _checkAtrribExist(int tagId);
        
        void saveJointAttributes(Skeleton *skeleton, int jointId);
        
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
