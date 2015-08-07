#include "ofApp.h"

/* This draws the animation */
void ofApp::setupAnim() {
    timeline.addBangs("bangs");
    timeline.addCurves("curves", ofRange(0, width));
    timeline.addSwitches("switches");
    timeline.addFlags("flags");
    timeline.addLFO("lfo", ofRange(0, height));
    timeline.addColors("colors");
    size = ofRandom(20, 200);
    text = "";
    ofAddListener(timeline.events().bangFired, this, &ofApp::bang);
}

void ofApp::updateAnim(){
    
}

void ofApp::drawAnim() {
    ofBackground(ofColor::black);
    ofSetColor(timeline.getColor("colors"));

    ofPoint position;
    position.x = timeline.getValue("curves");
    position.y = timeline.getValue("lfo");

    if(timeline.isSwitchOn("switches")){
        ofRect(position.x, position.y, size, size);
    } else {
        ofCircle(position, size);
    }
    
    ofSetColor(ofColor::white);
    ofDrawBitmapString(text, 20, 20);
}

void ofApp::bang(ofxTLBangEventArgs &args) {
    if(args.flag == "") {
        size = ofRandom(20, 200);
    } else {
        text = args.flag;
    }
}

/* This handles the setup and GIF rendering, etc */
void ofApp::setup(){
    //Gif render values
    slowMode = false;
    paused = false;
    fullscreen = false;
    width = 500, height = 500, padding = 10;
    duration = 0.2, colors = 256, numFrames = 50;
    saveOnFrame = -1;

    filename = ofGetTimestampString("%Y-%m-%d") + ".gif";
    framerate = slowMode ? 5 : 10;
    halfWidth = width * 0.5;
    halfHeight = height * 0.5;

    //Init
    ofSetFrameRate(framerate);
    ofSetWindowShape(1600, 1000);
    ofSetWindowPosition(0, 0);
    gifEncoder.setup(width, height, duration, colors);
    renderingNow = false;
    renderMessage = "";
    
    fbo.allocate(width, height, GL_RGB);
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
    
    //Timeline values
    timeline.setup();
    //timeline.setWorkingFolder("Timeline");
    //timeline.loadTracksFromFolder("Timeline");
    //https://github.com/YCAMInterlab/ofxTimeline/issues/105
    timeline.setFrameBased(true);
    timeline.setDurationInFrames(numFrames);
    timeline.setWidth(ofGetWidth() - width - 100);
    showTimeline = true;

    //Begin patch
    setupAnim();
}

void ofApp::update(){
    ofSetFullscreen(fullscreen);
    timeline.setWidth(ofGetWidth() - width - padding);
    updateAnim();
}

void ofApp::draw(){

    fbo.begin();
    drawAnim();
    fbo.end();

    if(!renderingNow && !paused) {
        captureFrame();
    }

    //Draw FBO
    ofPushMatrix();
    ofPushStyle();
    {
        ofTranslate(ofGetWidth() - width - 4, 4);
        ofBackground(ofColor::black);
        ofSetColor(ofColor::white);
        ofNoFill();
        ofSetLineWidth(2);
        ofRect(1, 1, width+2, height+2);
        ofTranslate(2, 2);
        fbo.draw(0, 0);
    }
    ofPopStyle();
    ofPopMatrix();

    //Draw timeline
    if(showTimeline) {
        timeline.draw();
    }

    ofSetColor(ofColor::white);
    ofDrawBitmapString(
        "Recording to frame #" +
        ofToString(saveOnFrame) +
        " at " +
        ofToString(ofGetFrameRate()) +
        "fps...\nCurrent frame: " +
        ofToString(ofGetFrameNum()) +
        "\n" + renderMessage,
        20, height - 50);
}

void ofApp::captureFrame() {
    fbo.readToPixels(pixels);
    gifEncoder.addFrame(pixels.getPixels(), width, height,
                        pixels.getBitsPerPixel(), duration);
    
    if(ofGetFrameNum() == saveOnFrame) {
        renderGif();
    }
}

void ofApp::renderGif() {
    if(!renderingNow) {
        renderingNow = true;
        gifEncoder.save(filename);
        renderMessage = "Now rendering";
    }
}

void ofApp::exit(){
    gifEncoder.exit();
}

void ofApp::keyPressed(int key){
    if(key == 't') {
        showTimeline = !showTimeline;
    } else if(key == 'f') {
        fullscreen = !fullscreen;
    }
}

void ofApp::keyReleased(int key){
    
}

void ofApp::mouseMoved(int x, int y ){
    
}

void ofApp::mouseDragged(int x, int y, int button){
    
}

void ofApp::mousePressed(int x, int y, int button){
    
}

void ofApp::mouseReleased(int x, int y, int button){
    
}

void ofApp::windowResized(int w, int h){
    
}

void ofApp::gotMessage(ofMessage msg){
    
}

void ofApp::dragEvent(ofDragInfo dragInfo){
    
}