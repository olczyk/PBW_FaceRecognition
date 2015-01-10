#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <stdio.h>

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
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;

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

private slots:
    void on_btnPauseOResume_clicked();

};

#endif // DIALOG_H
