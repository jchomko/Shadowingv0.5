#pragma once

#include "ofMain.h"
#include "OCV.h"
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
    
        void exit();
        void setupVariables();
    
        // OPEN CV
        CV openCV;
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
        bool erode;
        bool dilate;
        vector<ofVec2f> blobPath;
    
        // GUI
        void setupGUI();
        void guiEvent(ofxUIEventArgs &e);
        ofxUICanvas *guiCV;
        ofImage *colorSampler;
        
        // Masks
        void setupMasks();
        bool drawMask;
        int whichMask;
        ofImage masks[6];
        vector <string> maskString;
    
        ofColor backColor, shadowColor;
    
        // Misc
        int progress;
    
        // Video Buffer Stuff
        void playAllBuffersSequentiallyTriggered();
        void playOneBufferTriggered();
        void playLiveImageWithBufferTriggered();
        void playAllBuffersLayered();
        void playBuffersWithNoOneThere();
    
        std::deque<videoBuffer> buffers;
        vector<ofImage> videoImage;
    
        int playbackMode;
        int howManyBuffersToStore;
        bool showPreviousBuffers;
        int whichBufferAreWePlaying;
        bool showMultipleBuffers;
        bool startRecording;
        bool drawCV;
        bool drawLiveImage;
        bool hasBeenPushedFlag;
    
    
        // Debug Stuff
        void drawData();
        bool canDrawData;
        // Dev and Iteration Two
        // ofxHttpUtils httpPost;
    
    
    
};
