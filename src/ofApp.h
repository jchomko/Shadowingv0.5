#pragma once

#include "ofMain.h"
#include "OCV.h"
#include "ofxOpenCV.h"
#include "ofxSimpleTimer.h"
#include "ofxUI.h"
#include "VideoBufferStorage.h"

#define CAM_WIDTH 320
#define CAM_HEIGHT 240
#define FRAMERATE 30



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void setupGUI();
        void guiEvent(ofxUIEventArgs &e);
        void exit();
    
        CV openCV;
    
        std::deque<videoBuffer> buffers;

        vector<ofImage> videoImage;
        vector<ofVec2f> blobPath;
        ofImage masks[6];
        vector <string> maskString;
    
        ofxUICanvas *guiCV;
        ofImage *colorSampler;
        
        bool startRecording;
        bool learnBackground;
        bool bProgressiveLearning;
        int  threshold;
        float fProgressiveRate;
        bool bUseApprox;
        bool bFillHoles;
        int iMinBlobSize;
        int iMaxBlobSize;
        int iMaxBlobNum;
        float brightness,contrast;
        bool bMirrorH;
        bool bMirrorV;
        float fBlur;
        bool drawCV;
        bool drawLiveImage;
        int progress;
        bool hasBeenPushedFlag;
        bool showPreviousBuffers;
        bool drawMask;
        int whichMask;
    
        ofColor backColor, shadowColor;
        int whichBufferAreWePlaying;
};
