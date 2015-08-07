#include "ofApp.h"

void ofApp::setup(){
    ofSetWindowShape(1600, 1000);
    manager.setup();
    manager.toggleDebugUI();
    setupAudioUnits();
    setupTimeline();

    playing = false;
    note = 60;
}

void ofApp::setupAudioUnits() {
    manager.add(&chain, "tal-one", ofColor::blue);
    chain.link(&noiseMaker)
         .toMixer();
    manager.loadPresets(&chain);
}

void ofApp::setupTimeline() {
    timeline.setup();
    timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setDurationInSeconds(10);
    timeline.setOffset(ofVec2f(10, 450));
    timeline.setWidth(ofGetWidth() - 400);
    timeline.addBangs("bangs");
    timeline.addCurves("lfo rate", ofRange(0.03, 0.2));
    timeline.addCurves("filter cutoff", ofRange(0, 1));
    timeline.addSwitches("switches");
    timeline.addFlags("flags");
    timeline.addLFO("lfo", ofRange(0, 0.2));
    ofAddListener(timeline.events().bangFired, this, &ofApp::bang);
}

void ofApp::bang(ofxTLBangEventArgs &args) {
    if(args.flag == "") {
        chain.midi()->sendNoteOn(1, note);
    } else {
        chain.midi()->sendNoteOn(1, ofToInt(args.flag));
    }
}

void ofApp::togglePlaying() {
    playing = !playing;
    if(!playing) {
        chain.midi()->sendNoteOff(1, note);
    }
}

void ofApp::update(){
    noiseMaker.set(TALNoiseMaker_lfo2rate, timeline.getValue("lfo rate"));
    noiseMaker.set(TALNoiseMaker_cutoff, timeline.getValue("filter cutoff"));
}

void ofApp::draw(){
    manager.draw();
    timeline.draw();
}

void ofApp::exit() {
    manager.exit();
}

void ofApp::keyPressed(int key){
    if(key == ' ') {
        togglePlaying();
    } else if(key == '[') {
        togglePlaying();
        note--;
        togglePlaying();
    } else if(key == ']') {
        togglePlaying();
        note++;
        togglePlaying();
    } else {
        manager.keyPressed(key);
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