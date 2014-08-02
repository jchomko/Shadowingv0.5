#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(FRAMERATE);
    // Setup Variables
    setupVariables();
    // Looks for masks inside of the Masks folder
    setupMasks();
    // Setup the GUI
    setupGUI();
    // Setup Custom openCV Class
    openCV.setup(CAM_WIDTH,CAM_HEIGHT,FRAMERATE);
}
//--------------------------------------------------------------
void ofApp::update()
{
    // Set Window Title
    string title = "Shadowing Stage 0.5: " + ofToString(ofGetTimestampString("%H:%M:%S  %d/%m/%Y"));
    ofSetWindowTitle(title);
    //--------------------------------------------------------------
    // If we have %i buffers in the memory then release one
    if (buffers.size() >= howManyBuffersToStore)
    {
        buffers.erase(buffers.begin(), buffers.begin()+1);
        //buffers.pop_back();
    }
    //--------------------------------------------------------------
    
    // Custom CV mechanisms
    int mode = 0;
    if (mode == 0)
    {
        // Subtraction Plus Brightness and Contrast Settings
        openCV.subtractionLoop(learnBackground, bProgressiveLearning,fProgressiveRate,bMirrorH,bMirrorV,threshold,fBlur,iMinBlobSize, iMaxBlobSize,iMaxBlobNum,bFillHoles,bUseApprox);
        // Do Blob Assembly
        openCV.readAndWriteBlobData(backColor,shadowColor);
    }
    else if(mode == 1)
    {
        // Subtraction Plus Brightness and Contrast Settings
        openCV.subtractionLoop(learnBackground, bProgressiveLearning,fProgressiveRate,bMirrorH,bMirrorV,threshold,fBlur,iMinBlobSize, iMaxBlobSize,iMaxBlobNum,bFillHoles,bUseApprox,brightness,contrast);
        // Do Blob Assembly
        openCV.readAndWriteBlobData(backColor,shadowColor);
    }
    else if(mode == 2)
    {
        // Jonathan Chomko's CV Subtraction
        openCV.subtractionLoop(learnBackground,bMirrorH,bMirrorV,threshold,fBlur,iMinBlobSize, iMaxBlobSize,iMaxBlobNum,bFillHoles,bUseApprox,0,0);
    }
    else
    {
        // Nothing Error space
    }

    // If blob detected Start Recording
    if(openCV.isSomeoneThere())
    {
        startRecording = true;
        hasBeenPushedFlag = false;
    }
    else
    {
        startRecording = false;
        if (hasBeenPushedFlag == false)
        {
            if (videoImage.size() >= 30)// && !blobPath.empty())
            {
                ofLog(OF_LOG_NOTICE, "Buffers Ok");
                videoBuffer b;
                b.getNewImages(videoImage);
                //b.getPath(blobPath);
                buffers.push_front(b);
                videoImage.clear();
                //blobPath.clear();
                hasBeenPushedFlag = true;
            }
            else if(videoImage.size() < 30)
            {
                ofLog(OF_LOG_NOTICE, "Buffer Not Big Enough");
                videoImage.clear();
                //blobPath.clear();
                hasBeenPushedFlag = true;
            }
            else
            {
                ofLog(OF_LOG_NOTICE,"Nope you shouldn't be here Video Size sections");
            }
        }
        else
        {
            //Do nothing
        }
    }
    
    if(startRecording == true)
    {
        // If new frame
        if (openCV.newFrame())
        {
            // Capture Data according to %i number
            if (ofGetFrameNum() % 1 == 0)
            {
                // Capture the CV image
                videoImage.push_back(openCV.getRecordPixels());
                blobPath.push_back(openCV.getBlobPath());
            }
        }
    }
    else if (!startRecording)
    {
        
    
    }
    // Update the buffer progressors
    if (!buffers.empty())
    {
        for (int i = 0; i < buffers.size(); i++)
        {
            buffers[i].update();
        }
    }
    
}
//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0, 0, 0);
    
    ofPushStyle();
    // These are the different playmodes See the Functions for their descriptions
    if (playbackMode == 0)
    {
        playOneBufferTriggered();
    }
    else if (playbackMode == 1)
    {
        playLiveImageWithBufferTriggered();
    }
    else if (playbackMode == 2)
    {
        playAllBuffersLayered();
    }
    else if (playbackMode == 3)
    {
        playBuffersWithNoOneThere();
    }
    else if (playbackMode == 4)
    {
        playAllBuffersSequentiallyTriggered();
    }
    else
    {
        // Nothing
    }
    ofPopStyle();

    if (drawMask)
    {
        ofEnableAlphaBlending();
        ofSetColor(255, 255);
        masks[whichMask].draw(0,0,ofGetWidth(),ofGetHeight());
        ofDisableAlphaBlending();
    }
    
    // Draw all the CV Stuff
    if(drawCV == true)
    {
        openCV.draw();
    }
    // Draw only the Live input in Grayscale
    if(drawLiveImage == true)
    {
        openCV.drawLive();
    }
    ofPushStyle();
    // Show the previous Buffers
    if (showPreviousBuffers)
    {
        // Draw the Mini Buffers
        if (!buffers.empty())
        {
            for (int i = 0; i < buffers.size(); i++)
            {
                buffers[i].drawMini(0, 0+(i*240/4));
            }
        }
    }
    ofPopStyle();
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 'h')
    {
        guiCV->toggleVisible();
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
//--------------------------------------------------------------
void ofApp::setupVariables()
{
    howManyBuffersToStore = 5;
    progress = 0;
    whichMask = 1;
    whichBufferAreWePlaying = 0;
    ofClamp(whichBufferAreWePlaying, 0, 5);
    hasBeenPushedFlag = true;
    learnBackground = true;
}
//--------------------------------------------------------------
void ofApp::setupMasks()
{
    // Look inside of the Masks folder
    ofDirectory maskDirectory;
    int nFiles = maskDirectory.listDir("Masks");
    
    // Not sure why I've sorted them?
    maskDirectory.sort();
    
    if(nFiles)
    {
        ofLog(OF_LOG_ERROR, "Found Mask Folder");
        for(int i = 0; i< maskDirectory.numFiles(); i++)
        {
            string filePath = maskDirectory.getPath(i);
            masks[i].loadImage(filePath);
        }
    }
    else
    {
        ofLog(OF_LOG_ERROR, "Can't Find Mask Folder");
    }
}
//--------------------------------------------------------------
void ofApp::exit()
{
    guiCV->saveSettings("GUI/CV.xml");
    delete guiCV;
    openCV.releaseCamera();
}
//--------------------------------------------------------------
//* Here are all the Buffer Playback functions
//--------------------------------------------------------------
void ofApp::playOneBufferTriggered()
{
    // If Someone walks through the Camera view play the last persons buffer
    if(openCV.isSomeoneThere()) // if So
    {
        if (!buffers.empty())
        {
            buffers[0].start();
        }
    }
    else
    {
        
    }
    
    // Draw the first buffer in the Vector
    if (!buffers.empty())
    {
        buffers[0].draw();
    }
}
//--------------------------------------------------------------
void ofApp::playLiveImageWithBufferTriggered()
{
    // If Someone walks through the Camera view play the last persons buffer
    if(openCV.isSomeoneThere()) // if So
    {
        if (!buffers.empty())
        {
            buffers[0].start();
        }
    }
   
    // Draw the first buffer in the Vector
    if (!buffers.empty())
    {
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        buffers[0].draw();
        openCV.drawLiveShadow();
        ofDisableBlendMode();
    }
}
//--------------------------------------------------------------
void ofApp::playAllBuffersLayered()
{
    if (!buffers.empty())
    {
        for (int i = 0; i < buffers.size(); i++)
        {
            buffers[i].start();
        }
    }
    
    // Update the buffer progressors
    if (!buffers.empty())
    {
        for (int i = 0; i < buffers.size(); i++)
        {
            if (buffers[i].isFinished())
            {
                buffers[i].start();
            }
        }
    }
    
    if (!buffers.empty())
    {
        for (int i = 0; i < buffers.size(); i++)
        {
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            buffers[i].draw();
            ofDisableBlendMode();
        }
    }
}
//--------------------------------------------------------------
void ofApp::playAllBuffersSequentiallyTriggered()
{
    // Draw the first buffer in the Vector
    if (!buffers.empty())
    {
        buffers[whichBufferAreWePlaying].start();
        buffers[whichBufferAreWePlaying].draw();
        
        if (buffers.size() > 2)
        {
            if (buffers[whichBufferAreWePlaying].isFinished())
            {
                whichBufferAreWePlaying++;
                buffers[whichBufferAreWePlaying].start();
            }
        }
        if (whichBufferAreWePlaying >= buffers.size())
        {
            // Go back to the start and Await my instructions
            whichBufferAreWePlaying = 0;
            buffers[whichBufferAreWePlaying].start();
        }
    }
    
    
}
//--------------------------------------------------------------
void ofApp::playBuffersWithNoOneThere()
{
    // Check if the buffers are live. Start and Draw the first element in the vector.
    // When the buffer has finished playing the iterate to the next buffer
    if (!buffers.empty())
    {
        buffers[whichBufferAreWePlaying].start();
        buffers[whichBufferAreWePlaying].draw();
        
        if (buffers.size() > 2)
        {
            if (buffers[whichBufferAreWePlaying].isFinished())
            {
                whichBufferAreWePlaying++;
                buffers[whichBufferAreWePlaying].start();
            }
        }
        if (whichBufferAreWePlaying >= buffers.size())
        {
            // Go back to the start and Await my instructions
            whichBufferAreWePlaying = 0;
            buffers[whichBufferAreWePlaying].start();
        }
    }
}
//--------------------------------------------------------------
//* Here are all the Buffer Playback functions
//--------------------------------------------------------------
void ofApp::setupGUI()
{
    colorSampler = new ofImage();
    colorSampler->loadImage("GUI/colorSamplerImage.png");
    
    guiCV = new ofxUICanvas(0,0,500,600);
    guiCV->setTheme(OFX_UI_THEME_HACKER);
    guiCV->addWidgetDown(new ofxUILabel("Shadowing Stage 0.5", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabelToggle("Fullscreen",true,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabelToggle("Draw CV",true,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUILabelToggle("Show Buffers",true,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabelToggle("Draw Live",false,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUILabelToggle("Show Data",true,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Playback Mode", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, 6,1, 0, "PLAYBACK_MODE", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Number of Bufferss", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, 15,5, 0, "BUFFER_NUMBER", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabelButton("Learn Background",false,255,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabelToggle("Mirror H",false,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUILabelToggle("Mirror V",false,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Threshold", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, 255, 80, 0, "THRESHOLD", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Min Blob Size", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, (CAM_WIDTH*CAM_HEIGHT)/3, 20, 1, "MIN_BLOB_SIZE", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Max Blob Size", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, (CAM_WIDTH*CAM_HEIGHT), (CAM_WIDTH*CAM_HEIGHT)/3, 1, "MAX_BLOB_SIZE", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Max Num Blob", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, 10, 2, 0, "MAX_BLOB_NUM", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabelToggle("Fill Holes",false,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUILabelToggle("Use Approximation",false,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Blur", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, 100, 1, 1, "BLUR", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Brighness", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, 100, 1, 2, "BrightnessV", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Contrast", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, 100, 1, 2, "ContrastV", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabelToggle("Progressive Background",false,255,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Progression Rate", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0.00f, 1.00f, 0.01f, 4, "PROGRESSIVE_RATE", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("BGL","Background Color", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUIImageSampler(255/2, 255/2, colorSampler, "Background_Color"));
    guiCV->addWidgetEastOf(new ofxUILabel("Shadow Color", OFX_UI_FONT_MEDIUM),"BGL");
    guiCV->addWidgetEastOf(new ofxUIImageSampler(255/2, 255/2, colorSampler, "Shadow_Color"),"Background_Color");
    guiCV->addWidgetDown(new ofxUILabelToggle("Use Mask",true,255/2,30,OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetDown(new ofxUILabel("Mask Number", OFX_UI_FONT_MEDIUM));
    guiCV->addWidgetRight(new ofxUINumberDialer(0, 5, 1, 0, "Mask_No", OFX_UI_FONT_MEDIUM));
    guiCV->autoSizeToFitWidgets();
    ofAddListener(guiCV->newGUIEvent,this, &ofApp::guiEvent);
    guiCV->loadSettings("GUI/CV.xml");
}
//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "Progressive Background")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        bProgressiveLearning = toggle->getValue();
    }
    else if(e.getName() == "Use Mask")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        drawMask = toggle->getValue();
    }
    else if(e.getName() == "PLAYBACK_MODE")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        playbackMode = dial->getValue();
    }
    else if(e.getName() == "Mask_No")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        whichMask = dial->getValue();
    }
    else if(e.getName() == "Learn Background")
    {
        ofxUILabelButton * button = (ofxUILabelButton *) e.widget;
        learnBackground = button->getValue();
    }
    else if(e.getName() == "PROGRESSIVE_RATE")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        fProgressiveRate = dial->getValue();
    }
    else if(e.getName() == "BUFFER_NUMBER")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        howManyBuffersToStore = dial->getValue();
    }
    else if(e.getName() == "BrightnessV")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        brightness = dial->getValue();
    }
    else if(e.getName() == "ContrastV")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        contrast = dial->getValue();
    }
    else if(e.getName() == "Fullscreen")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        ofSetFullscreen(toggle->getValue());
    }
    else if(e.getName() == "Show Buffers")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        showPreviousBuffers = toggle->getValue();
    }
    else if(e.getName() == "THRESHOLD")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        threshold = dial->getValue();
    }
    else if(e.getName() == "MIN_BLOB_SIZE")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        iMinBlobSize = dial->getValue();
    }
    else if(e.getName() == "MAX_BLOB_SIZE")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        iMaxBlobSize = dial->getValue();
    }
    else if(e.getName() == "MAX_BLOB_NUM")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        iMaxBlobNum = dial->getValue();
    }
    else if (e.getName() == "Mirror H")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        bMirrorH = toggle->getValue();
    }
    else if (e.getName() == "Mirror V")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        bMirrorV = toggle->getValue();
    }
    else if (e.getName() == "Use Approximation")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        bUseApprox = toggle->getValue();
    }
    else if (e.getName() == "Fill Holes")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        bFillHoles = toggle->getValue();
    }
    else if(e.getName() == "BLUR")
    {
        ofxUINumberDialer * dial = (ofxUINumberDialer *) e.widget;
        fBlur = dial->getValue();
    }
    else if (e.getName() == "Draw CV")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        drawCV = toggle->getValue();
    }
    else if (e.getName() == "Draw Live")
    {
        ofxUILabelToggle * toggle = (ofxUILabelToggle *) e.widget;
        drawLiveImage = toggle->getValue();
    }
    else if(e.widget->getName() == "Background_Color")
    {
        ofxUIImageSampler *sampler = (ofxUIImageSampler *) e.widget;
        backColor = sampler->getColor();
    }
    else if(e.widget->getName() == "Shadow_Color")
    {
        ofxUIImageSampler *sampler = (ofxUIImageSampler *) e.widget;
        shadowColor = sampler->getColor();
    }
}
