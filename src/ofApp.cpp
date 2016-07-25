#include "ofApp.h"

float getSlope(ofPoint pt1, ofPoint pt2) {
    return (pt2.y - pt1.y) / (pt2.x - pt1.x);
}

//--------------------------------------------------------------
void ofApp::setup(){
    width = 500;
    height = 500;
    
    radius = 0;
    breath = false;
    
    ofDirectory dir;
    dir.sort();
    dir.listDir("iva_color");
    
    loadImages(dir);
    
    setLookupTable(dir.size());
    
    outputImage.allocate(width, height, OF_IMAGE_COLOR);
    
    fbo.allocate(width, height, GL_RGBA);
}

void ofApp::loadImages(ofDirectory dir) {
    for(int index = 0; index < dir.size(); index++) {
        ofImage img;
        img.setUseTexture(false);
        img.load(dir.getPath(index));
        img.resize(width, height);
        images.push_back(img);
    }
}

void ofApp::setLookupTable(int size) {
    for(int x = 0; x < 256; x++) {
        vector<float> tableEnergies;
        for(int i = 0; i < size; i++) {
            float energy;
            float cutOff = (200 * (i/(float)size));
            if(x >= cutOff) {
                energy = x+1;
            } else {
                float m = getSlope(ofPoint(ceil(cutOff),ceil(cutOff)+1), ofPoint(0, 1));
                energy = m * (x - ceil(cutOff));
            }
            energy = MAX(0, energy); // Just to be safe!
            energy = sqrt(energy);
            tableEnergies.push_back(energy);
        }
        lookUpTable.push_back(tableEnergies);
    }
}

void ofApp::updateFbo() {
    if(breath) {
        color-=10;
        radius +=5;
    } else {
        radius-=2;
        color+=8;
    }
    color = MAX(0,MIN(255, color));
    radius = MAX(0, radius);
    fbo.begin();
    ofClear(255, 255, 255);
    ofSetColor(color);
    ofDrawCircle(breathOrigin, radius);
    fbo.end();

}

void ofApp::updateOutputImage() {
    ofPixels pixels;
    fbo.readToPixels(pixels);
    for( int i = 0; i < width; i++) {
        for( int j = 0; j < height; j++) {
            int patternBright = floor(pixels.getColor(i, j).getBrightness());
            ofPoint colorAvg(0,0,0);
            
            double totalEnergy = 0;
            for(int index = 0; index < images.size(); index++) {
                ofColor color = images[index].getColor(i, j);
                ofPoint colorPt(color.r, color.g, color.b);
                float energy = lookUpTable[patternBright][index];
                colorAvg += energy * colorPt;
                totalEnergy += energy;
            }
            colorAvg /= totalEnergy;
            ofColor pixelAvgColor(colorAvg.x, colorAvg.y, colorAvg.z);
            outputImage.setColor(i, j, pixelAvgColor);
        }
    }
    outputImage.update();
}


//--------------------------------------------------------------
void ofApp::update(){
    updateFbo();
    updateOutputImage();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(255,255,255);
    outputImage.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//    fbo.allocate(width, height, GL_RGBA);
//    fbo.clear();
    
    int clampedX = MAX(0,MIN(width,x));
    int clampedY = MAX(0,MIN(height,y));
    breathOrigin = ofPoint(clampedX, clampedY);
    radius = 10;
    color = 100;
    breath = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    breath = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
