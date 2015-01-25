#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <stdio.h>

#include "garecognizer.h"

using namespace std;
using namespace cv;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    string face_cascade_name = "../opencvTest/lbpcascade_frontalface.xml";
    string eyes_cascade_name = "../opencvTest/haarcascade_eye_tree_eyeglasses.xml";
    string nose_cascade_name = "../opencvTest/haarcascade_mcs_nose.xml";
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    CascadeClassifier nose_cascade;

    // path to save images
        string imagePath = "/Users/piotrbienias/Documents/opencvPhotos/";
        string fileName = "example";
        string extension = ".pgm";
        string fullPath = imagePath + fileName + extension;

        // counter up to 10 photos
        int imageSaveCounter = 0;

        // time variables to make photo every 2 seconds
        time_t now;
        time_t currentTime;
        double timeConstantDiffrence = 2.0;

        // constant values based on bienias's morda to prepare distance calculation
        double focalLength;
        double faceHeight = 14.5;
        double distanceFromCamera = 50;
        double faceWidthInPixels = 300;


        string rootDir = "/home/reimu/git-projects/PBW_FaceRecognition/opencvTest";
        GARecognizer *recognizer;
private:
    Ui::Dialog *ui;

    VideoCapture capWebcam;

    Mat matWebCamImage;

    QImage qimgWebCamImage;

    QTimer* tmrTimer;

    string window_name = "Capture - Face detection";

    //RNG rng(12345);

public slots:
    void processFrameAndUpdateGUI();
    void detectAndDisplay(Mat frame);
    void saveFacePhoto(Mat inputFrame);
    void calculateFocalLength();
    double calculateDistanceFromCamera(double widthInPixels);

private slots:
    void on_btnPauseOResume_clicked();

};

#endif // DIALOG_H
