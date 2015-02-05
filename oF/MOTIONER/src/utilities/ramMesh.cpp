//
//  ramMesh.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/12/12.
//
//

#include "ramMesh.h"
#include "ramCommon.h"

namespace ram {
    
    namespace Mesh {
        
        void drawAxis(float size)
        {
            ofPushStyle();
            // draw x axis
            ofSetColor(200, 100, 100);
            ofLine(0, 0, 0, size, 0, 0);
            
            // draw y axis
            ofSetColor(100, 200, 100);
            ofLine(0, 0, 0, 0, size, 0);
            
            // draw z axis
            ofSetColor(100, 100, 200);
            ofLine(0, 0, 0, 0, 0, size);
            ofPopStyle();

        }
                
        //--------------------------------------------------------------------------------
        ofVboMesh createGrid(float size, int numSlices)
        {
            ofVboMesh  mesh;
            
            mesh.clear();
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            
            ofFloatColor c0(0.6f);
            ofFloatColor c1(0.3f);
            
            const float begin = - numSlices / 2 * size;
            
            for (int i=0; i<numSlices; i++) {
                for (int j=0; j<numSlices; j++) {
                    const float x0 = begin + size * i;
                    const float x1 = begin + size * i + size;
                    const float y0 = begin + size * j;
                    const float y1 = begin + size * j + size;
                    mesh.addVertex(ofVec3f(x0, 0, y0));
                    mesh.addVertex(ofVec3f(x1, 0, y0));
                    mesh.addVertex(ofVec3f(x1, 0, y1));
                    
                    mesh.addVertex(ofVec3f(x1, 0, y1));
                    mesh.addVertex(ofVec3f(x0, 0, y1));
                    mesh.addVertex(ofVec3f(x0, 0, y0));
                    
                    ofFloatColor c;
                    if (j%2 == 0) {
                        i%2 == 0 ?  c.setHsb(ofRandom(1.f), 0.05f, 0.3f) :  c.setHsb(ofRandom(1.f), 0.05f, 0.6f);
                    }
                    else {
                        i%2 == 1 ?  c.setHsb(ofRandom(1.f), 0.05f, 0.3f) :  c.setHsb(ofRandom(1.f), 0.05f, 0.6f);
                    }
                    
                    for (int k=0; k<6; k++) {
                        mesh.addNormal(ofVec3f(0.f, 1.f, 0.f));
                        mesh.addColor(c);

                    }

                }
            }
            
            return mesh;
        }
        
    }
    
}