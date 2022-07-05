#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(0);
	ofEnableDepthTest();

	this->radius = 30;

	this->box2d.init();
	this->box2d.setGravity(0, 0);
	this->box2d.createBounds(30, 30, ofGetWidth() - 60, ofGetHeight() - 60);
	this->box2d.setFPS(60);
	this->box2d.registerGrabbing();

	for (int i = 0; i < 40; i++) {

		auto circle = make_shared<ofxBox2dCircle>();
		circle->setPhysics(0.5, 0.63, 0.1);
		circle->setup(this->box2d.getWorld(), ofRandom(30, ofGetWidth() - 30), ofRandom(30, ofGetHeight() - 30), this->radius);
		this->circles.push_back(circle);
		this->rotation_list.push_back(glm::vec3(ofRandom(360), ofRandom(360), ofRandom(360)));
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	for (int i = 0; i < this->circles.size(); i++) {

		for (int j = i + 1; j < this->circles.size(); j++) {

			float distance = this->circles[i]->getPosition().distance(this->circles[j]->getPosition());
			if (distance < this->radius * 4) {

				this->circles[i]->addForce(this->circles[i]->getPosition() - this->circles[j]->getPosition(), ofMap(distance, this->radius, this->radius * 4, 1.8, 0.002));
				this->circles[j]->addForce(this->circles[j]->getPosition() - this->circles[i]->getPosition(), ofMap(distance, this->radius, this->radius * 4, 1.8, 0.002));
			}
		}
	}

	this->box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	auto threshold = 110;

	ofColor color;
	ofSetLineWidth(2);

	vector<glm::vec2> locations;
	for (int i = 0; i < this->circles.size(); i++) {

		glm::vec2 location = this->circles[i]->getPosition();
		locations.push_back(location);

		ofPushMatrix();
		ofTranslate(location.x, location.y);

		this->rotation_list[i].x += this->circles[i]->getVelocity().length() * 3;
		this->rotation_list[i].y += this->circles[i]->getVelocity().length() * 3;
		this->rotation_list[i].z += this->circles[i]->getVelocity().length() * 3;

		ofRotateZ(this->rotation_list[i].z);
		ofRotateY(this->rotation_list[i].y);
		ofRotateX(this->rotation_list[i].x);

		auto hue = ofMap(i, 0, this->circles.size(), 0, 255);
		color.setHsb(hue, 255, 255);
		ofSetColor(color);

		ofDrawCircle(glm::vec2(), this->radius * 0.5);

		ofPopMatrix();
	}

	ofSetColor(255);
	ofSetLineWidth(0.5);

	for (auto& location : locations) {

		int word_index = 0;
		for (auto& other : locations) {

			if (location == other) { continue; }

			auto distance = glm::distance(location, other);
			if (distance < threshold) {

				auto direction_rad = std::atan2(other.y - location.y, other.x - location.x);
				auto direction = direction_rad * RAD_TO_DEG;
				auto width = ofMap(distance, 0, threshold, 360, 0);

				ofPushMatrix();
				ofTranslate(location);

				ofNoFill();
				ofBeginShape();
				for (auto deg = direction - width * 0.5; deg <= direction + width * 0.5; deg++) {

					ofVertex(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD));
				}
				ofEndShape();

				ofDrawLine(glm::vec2(radius * cos(direction * DEG_TO_RAD), radius * sin(direction * DEG_TO_RAD)),
					(other - location) + glm::vec2(radius * cos((180 + direction) * DEG_TO_RAD), radius * sin((180 + direction) * DEG_TO_RAD)));

				ofPopMatrix();
			}
		}
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}