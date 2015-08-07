#include "ofApp.h"

void ofApp::setup(){
    ofSetWindowShape(500, 500);
    gif.setup(ofGetTimestampString("out/%Y-%m-%d"), 12);
}

void ofApp::update(){

}

void ofApp::draw(){
    gif.begin();
    {
        ofBackground(ofColor::black);
        ofRect(ofGetFrameNum() * 5, ofGetFrameNum() * 5, gif.width * 0.2, gif.height * 0.2);
    }
    gif.endAndCaptureFrame();
    gif.draw();
}

void ofApp::keyPressed(int key){

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