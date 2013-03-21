#include "ofMain.h"
#include "MotionerApp.h"
#include "ofAppGlutWindow.h"

//----------------------------------------------------------------------------------------
int main()
{
    ofAppGlutWindow window;
    window.displayString = "rgba double depth alpha samples>=2 stencil";
    ofSetupOpenGL(&window, 1280, 768, OF_WINDOW);
	ofRunApp( new MotionerApp());
}
