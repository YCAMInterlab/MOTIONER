//
//  ramSimpleShadow.h
//  MOTIONER
//
//  Created by YoshitoONISHI on 2/4/15.
//
//

#ifndef __MOTIONER__ramSimpleShadow__
#define __MOTIONER__ramSimpleShadow__

#include "ofMain.h"

class ramSimpleShadow
{
public:
    
    void setup();
    void setLightPosition(ofVec3f pos);
    void setShadowColor(ofFloatColor color);
    
    void begin(const ofMatrix4x4& modelview);
    void end();
    
    void setEnable(bool v) { enable = v; }
    bool getEnable() const { return enable; }
    
    void setShadowAlpha(float alpha);
    
protected:
    
    ofMatrix4x4 shadow_matrix;
    ofShader shader;
    
    bool enable;
    ofFloatColor shadow_color;
    
};

#endif /* defined(__MOTIONER__ramSimpleShadow__) */
