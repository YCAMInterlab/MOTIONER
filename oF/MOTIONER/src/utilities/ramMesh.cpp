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
        
        void drawRuler()
        {
            const float step = 1000.0f;
            ofPoint offset(30.0f, 2.0f);
            const int n = 10;
            
            /// draw ruler
            ofxPushAll();
            ofSetHexColor(ram::COLOR_M);
            ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
            
            for (int i=1; i<=n; i++) {
                ofDrawBitmapString(ofToString( i*step, 0),
                                   ofPoint(offset.x, i*step+offset.y));
                ofDrawBitmapString(ofToString( i*step, 0),
                                   ofPoint(i*step+offset.y, offset.x));
                ofDrawBitmapString(ofToString( i*step, 0),
                                   ofPoint(offset.x, offset.x, i*step+offset.y));
                ofDrawBitmapString(ofToString(-i*step, 0),
                                   ofPoint(-i*step+offset.y, offset.x));
                ofDrawBitmapString(ofToString(-i*step, 0),
                                   ofPoint(offset.x, offset.x, -i*step+offset.y));
                if (i==n) {
                    ofDrawBitmapString("Y", ofPoint(0.0f, i*step+1000.0f));
                    ofDrawBitmapString("X", ofPoint(i*step+1000.0f, 0.0f));
                    ofDrawBitmapString("Z", ofPoint(0.0f, 0.0f, i*step+1000.0f));
                }
            }
            ofxPopAll();
        }
        
        //--------------------------------------------------------------------------------
        ofVboMesh createGrid(float size, int numSlices)
        {
            ofVboMesh  mesh;
            
            mesh    .clear();
            mesh    .setMode(OF_PRIMITIVE_LINES);
            
            for (int i=0; i<size/numSlices; i++)
            {
                float h;
                ofFloatColor color;
                
                if (i%100==0) {
                    h = 48 - i%2*2;
                    color.set(0.9f);
                }
                else if (i%10==0) {
                    h = 18 - i%2*2;
                    color.set(0.6f);
                }
                else {
                    h = 12 - i%2*2;
                    color.set(0.4f);
                }
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-h+0.5, i*10+0.5, 0.5)        );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( h+0.5, i*10+0.5, 0.5)        );
            }
            
            for (int i=0; i<size/numSlices/2; i++)
            {
                float h;
                ofFloatColor color;
                
                if (i%10==0) {
                    continue;
                }
                else {
                    h = 24 - i%2*2;
                    color.set(0.4f);
                }
                
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( i*100+0.5, 0.5, -h+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( i*100+0.5, 0.5, h+0.5)        );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-i*100+0.5, 0.5, -h+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-i*100+0.5, 0.5, h+0.5)        );
                
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-h+0.5, 0.5,  i*100+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( h+0.5, 0.5,  i*100+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-h+0.5, 0.5, -i*100+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( h+0.5, 0.5, -i*100+0.5)       );
            }
            
            
            ofFloatColor color;
            color.set(0.2f);
            for (int i=0; i<10; i++) {
                if (i%2==0) continue;
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-size*i+0.5, 0.5, -size*i+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-size*i+0.5, 0.5,  size*i+0.5)       );
                
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-size*i+0.5, 0.5,  size*i+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( size*i+0.5, 0.5,  size*i+0.5)       );
                
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( size*i+0.5, 0.5,  size*i+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( size*i+0.5, 0.5, -size*i+0.5)       );
                
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f( size*i+0.5, 0.5, -size*i+0.5)       );
                mesh.addColor   ( color );
                mesh.addVertex  ( ofVec3f(-size*i+0.5, 0.5, -size*i+0.5)       );
            }
            
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