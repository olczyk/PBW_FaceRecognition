#include "garecognizer.h"

#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

enum {MALE, FEMALE};
const string ageRanges[] = {"0-12", "13-18", "19-30", "31+"};
const int LINESKIP = 7;

Ptr<FaceRecognizer> ageRecognizer, genderRecognizer;

string GARecognizer::estimateGender(Mat sample) {
    int predictedLabel = genderRecognizer->predict(sample);
    switch(predictedLabel) {
    case MALE:
        return "Male";
    case FEMALE:
        return "Female";
    default:
        return "N/A";
    }
}

string GARecognizer::estimateAgeRange(Mat sample) {
    int predictedLabel = genderRecognizer->predict(sample);
    if(predictedLabel < 0 || predictedLabel >= sizeof(ageRanges)/sizeof(string)) {
        return "N/A";
    }
    else {
        return ageRanges[predictedLabel];
    }
}

bool isFemale(string genderLabel) {
    switch(atoi(genderLabel.c_str())) {
    case 4:
    case 34:
    case 20:
    case 28:
    case 5:
    case 27:
    case 26:
    case 21:
    case 9:
        return true;
    default:
        return false;
    }
}

void trainWithCSV(string rootDir) {
    const string csvName = "gender.csv";
    std::ifstream file((rootDir + "/" + csvName).c_str(), ifstream::in);
    if (!file) {
        CV_Error(CV_StsBadArg, "No valid input file was given, please check the given filename.");
    }

    const char separator = ';';
    vector<Mat> images;
    vector<int> genderLabels, ageLabels;

    cout << "loading data..." << endl;

    string line, path, genderLabel, ageLabel;
    int lineCount = 0;
    while (getline(file, line)) {
        lineCount++;
        if(lineCount % LINESKIP != 0) continue;

        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, genderLabel);
        ageLabel = genderLabel;
        if(!path.empty() && !genderLabel.empty() && !ageLabel.empty()) {
            images.push_back(imread(rootDir + "/" + path, 0));

            int genderValue = isFemale(genderLabel) ? FEMALE : MALE;
            genderLabels.push_back(genderValue);
            ageLabels.push_back(atoi(ageLabel.c_str()));
        }
    }

    if(images.size() <= 1) {
        CV_Error(CV_StsError, "This program needs at least 2 images to work. Please add more images to your data set!");
    }

    cout << "training..." << endl;
    genderRecognizer->train(images, genderLabels);
    ageRecognizer->train(images, ageLabels);
    cout << "training complete" << endl;
}

GARecognizer::GARecognizer(string rootDir)
{
    genderRecognizer = createFisherFaceRecognizer();
    ageRecognizer = createFisherFaceRecognizer();

    trainWithCSV(rootDir);
}
