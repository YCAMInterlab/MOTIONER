//
//  ramMesh.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/12/12.
//
//

#include "ramMesh.h"
#include "ramConstants.h"
#include "ofxGLUtils.h"

namespace ram {
    
    namespace Mesh {
        
        void drawAxis(float size)
        {
            ofPushStyle();
            // draw x axis
            ofSetColor(ofColor::red);
            ofLine(0, 0, 0, size, 0, 0);
            
            // draw y axis
            ofSetColor(ofColor::green);
            ofLine(0, 0, 0, 0, size, 0);
            
            // draw z axis
            ofSetColor(ofColor::blue);
            ofLine(0, 0, 0, 0, 0, size);
            ofPopStyle();

        }
                
        //--------------------------------------------------------------------------------
        ofVboMesh createGrid(float size, int numSlices)
        {
            ofVboMesh  mesh;
            
            mesh    .clear();
            mesh    .setMode(OF_PRIMITIVE_LINES);
            
            ofFloatColor color;
            color.set(0.2f);
            mesh.addColor   ( color );
            mesh.addVertex  ( ofVec3f(-size*10+0.5, 0.5, 0.5)       );
            mesh.addColor   ( color );
            mesh.addVertex  ( ofVec3f( size*10+0.5, 0.5, 0.5)       );
            mesh.addColor   ( color );
            mesh.addVertex  ( ofVec3f( 0.5, 0.5,  size*10.0f+0.5)       );
            mesh.addColor   ( color );
            mesh.addVertex  ( ofVec3f( 0.5, 0.5, -size*10.0f+0.5)       );
            
            for (int i=0; i<numSlices; i++)
            {
                const float h     = size;
                const float step  = size/numSlices;
                ofFloatColor color(0.6, 0.3);
                
                if (i%5==0) color.set(0.6);
                
                mesh.addColor   ( color                             );
                mesh.addVertex  ( ofVec3f(i*step+0.5, 0.5, -h+0.5)  );
                mesh.addColor   ( color                             );
                mesh.addVertex  ( ofVec3f(i*step+0.5, 0.5, h+0.5)   );
                mesh.addColor   ( color                             );
                mesh.addVertex  ( ofVec3f(-i*step, 0.5, -h+0.5)     );
                mesh.addColor   ( color                             );
                mesh.addVertex  ( ofVec3f(-i*step+0.5, 0.5, h+0.5)  );
                mesh.addColor   ( color                             );
                mesh.addVertex  ( ofVec3f(-h+0.5, 0.5, i*step+0.5)  );
                mesh.addColor   ( color                             );
                mesh.addVertex  ( ofVec3f(h+0.5, 0.5, i*step+0.5)   );
                mesh.addColor   ( color                             );
                mesh.addVertex  ( ofVec3f(-h+0.5, 0.5, -i*step+0.5) );
                mesh.addColor   ( color                             );
                mesh.addVertex  ( ofVec3f(h+0.5, 0.5, -i*step+0.5)  );
            }
            
            float length    = size;
            
            mesh.addColor       ( ofFloatColor(0.6)         );
            mesh.addVertex      ( ofVec3f(0, 0, 0)          );
            mesh.addColor       ( ofFloatColor(0.6)         );
            mesh.addVertex      ( ofVec3f(0, length, 0)     );
            
            return mesh;
        }
        
    }
    
}