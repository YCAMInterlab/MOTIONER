//
//  ramCommon.cpp
//  MOTIONER
//
//  Created by YoshitoONISHI on 2/5/15.
//
//

#include "ramCommon.h"

void ofxPushAll()
{
    ofPushView();
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    GLint matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE, &matrix_mode);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glMatrixMode(GL_COLOR);
    glPushMatrix();
    
    glMatrixMode(matrix_mode);
    
    ofPushStyle();
}

//----------------------------------------------------------------------------------------
void ofxPopAll()
{
    ofPopStyle();
    
    GLint matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE, &matrix_mode);
    
    glMatrixMode(GL_COLOR);
    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(matrix_mode);
    
    glPopAttrib();
    
    ofPopView();
}