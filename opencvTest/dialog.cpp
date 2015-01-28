#include "dialog.h"
#include "ui_dialog.h"

#include<QtCore>
#include <iostream>

#include <time.h>
#include <math.h>

#include<thread>

using namespace cv;
using namespace std;

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog)
{
    //recognizer = new GARecognizer(rootDir);

    ui->setupUi(this);

    capWebcam.open(0);

    tmrTimer = new QTimer(this);
    connect(tmrTimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));
    tmrTimer->start(20);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::processFrameAndUpdateGUI()
{
    time(&now);
    calculateFocalLength();
    capWebcam.read(matWebCamImage);

    if(matWebCamImage.empty()) return;

    //Loading the cascade
    if(!face_cascade.load(face_cascade_name))
    {
        printf("Error loading lbpcascade_frontalface.xml\n");
        return;
    }

    if(!eyes_cascade.load(eyes_cascade_name))
    {
        printf("Error loading haarcascade_eye_tree_eyeglasses.xml\n");
        return;
    }

    if(!nose_cascade.load(nose_cascade_name))
    {
        printf("Error loading haarcascade_mcs_nose.xml\n");
        return;
    }

    //Read the video stream
    if(capWebcam.isOpened())
    {
        for(;;)
        {
            capWebcam >> matWebCamImage;

            //Apply the classifier to the frame
            if(!matWebCamImage.empty())
            {
                detectAndDisplay(matWebCamImage);
            }
            else
            {
                printf("No captured frame!");
                break;
            }

            int c = waitKey(10);
            if((char)c == 'c')
            {
                break;
            }
        }
    }
}

void drawCaption(string caption, Rect face, Mat frame, int offset = 0)
{
    Point origin(face.x, face.y + face.height + offset);
    int fontFace = FONT_HERSHEY_COMPLEX;
    double fontScale = 0.6;
    Scalar textColor(0,255, 0);

    putText(frame, caption, origin, fontFace, fontScale, textColor);
}

int calculateOrientation(int left, int right, int nose)
{
    double result = 0;
    double result_d = 0;
    double angular_coefficient = 90;
    if(right < left)        // we assume that right eye should have a bigger x-value
    {
        int temp = left;
        left = right;
        right = temp;
    }
    if(nose > right || nose < left)  // if the value of nose position is strange we don't proceed any longer and return -1 as it is impossible to calculate orientation
    {
        return -1;
    }
    double alleged_middle = (left + right) / 2;
    double nose_tilt = nose - alleged_middle;
    if(nose_tilt < 0)
    {
        nose_tilt = - nose_tilt;
    }
    double orientation_denominator = right - alleged_middle;
    result_d = (nose_tilt / orientation_denominator);
    result = result_d * angular_coefficient;
    return (int) result;
}

void Dialog::saveFacePhoto(Mat inputFrame) {
    //Mat grayScaleFrame;
    vector<int> compressionParams;

    compressionParams.push_back(CV_IMWRITE_PXM_BINARY);
    compressionParams.push_back(0);

    string newFileName = fileName + std::to_string(imageSaveCounter);

    fullPath = rootDir + "/" + newFileName + extension;

    //cvtColor(inputFrame, grayScaleFrame, CV_BGR2GRAY);

    if ( cv::imwrite(fullPath, inputFrame, compressionParams) ) {
        imageSaveCounter++;
    }
}

double calculateDistance(Point p1, Point p2) {
    double distanceSquared = (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
    return sqrt(distanceSquared);
}


// calculating Focal Length of the camera
void Dialog::calculateFocalLength() {
    focalLength = faceWidthInPixels * distanceFromCamera / faceHeight;
}

// calculating the distance of morda from the camera
double Dialog::calculateDistanceFromCamera(double widthInPixels) {
    return faceHeight * focalLength / widthInPixels;
}

void Dialog::detectAndDisplay(Mat frame)
{
    vector<Rect> faces;
    Mat frame_gray;

    Point nose_center;
    Point eye_center;
    int left_eye;
    int right_eye;
    int nose_position;
    vector<Rect> eyes;
    vector<Rect> noses;

    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    //Detect faces
    face_cascade.detectMultiScale(frame_gray, faces, 1.05, 1, 0, Size(80, 80),Size(240, 240));

    //for(size_t i = 0; i < faces.size(); i++)
    for(Rect face: faces)
    {
        Mat faceROI = frame_gray(face);

        //Draw the face
        Mat frameOut = frame(Rect(face.x + 0.15 * face.width, face.y + 0.15 * face.height, 0.7 * face.width, 0.8 * face.height));

        // resizing image
        Size resizedImageSize(168, 192);
        Mat resizedMat;
        cvtColor(frameOut, frameOut, CV_RGB2GRAY);
        cv::resize(frameOut, resizedMat, resizedImageSize);

        time(&currentTime);

        double seconds = difftime(currentTime, now);
        //cout << seconds;
        if ( imageSaveCounter < 10 && ( seconds > (double) imageSaveCounter * timeConstantDiffrence ) ) {
            // uncomment below line to make 10 photos
            if(fileName.length() > 0) saveFacePhoto(resizedMat);
            imshow("output", resizedMat);
        }
        //           Mat imageToShow = imread("/Users/piotrbienias/Documents/opencvPhotos/example1.pgm", CV_LOAD_IMAGE_COLOR);
        //           imshow("image", imageToShow);

        //In each face, detect eyes
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30));
        nose_cascade.detectMultiScale(faceROI, noses, 1.1, 6, 0, Size(40,40));
        if( eyes.size() == 2)
        {
            Point leftTop(face.x, face.y);
            Point bottomRight(face.x + face.width, face.y + face.height);
            rectangle(frame,leftTop,bottomRight,Scalar( 255, 0, 0 ));
            int flag = 1;
            for(Rect eye: eyes)
            {
                //Draw the eyes
                eye_center = Point(face.x + eye.x + eye.width/2, face.y + eye.y + eye.height/2);
                int radius = cvRound((eye.width + eye.height)*0.25);
                circle(frame, eye_center, radius, Scalar( 0, 0, 255 ), 3, 8, 0);
                if(flag == 1)   // for orientation calculation
                {
                    left_eye = eye_center.x;
                    flag++;
                }
                else
                {
                    right_eye = eye_center.x;
                }

            }

            for(Rect nose: noses)
            {
                nose_center = Point(face.x + nose.x + nose.width/2,face.y + nose.y + nose.height/2);
                nose_position = nose_center.x;
                int nose_radius = cvRound((nose.width + nose.height)*0.2);
                circle(frame, nose_center,nose_radius, Scalar(0,255,0),3,8,0);
            }



            int offset = 20;
            int count = sizeof(captions) / sizeof(*captions);

            for (int i = 0; i < count; i++) {
                drawCaption(captions[i], face, frame, i * offset);
            }

            // string orientation_output_string = "Orientation: " + orientation_string + "degrees."; // '°' -> how to make this sign a string and output it on the screen ?

        }

        vector<Rect> faces;
        Mat frame_gray;

        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
        equalizeHist(frame_gray, frame_gray);

        //Detect faces
        face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, Size(80, 80));

        //for(size_t i = 0; i < faces.size(); i++)
        for(Rect face: faces)
        {
            Mat faceROI = frame_gray(face);
            vector<Rect> eyes;

            //In each face, detect eyes
            eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30));
            if( eyes.size() == 2)
            {


                Point center(face.x + face.width/2, face.y + face.height/2);
            //    ellipse(frame, center, Size( face.width/2, face.height/2), 0, 0, 360, Scalar( 255, 0, 0 ), 2, 8, 0);

                Point onePoint(face.x, face.y);
                Point twoPoint(face.x + face.width, face.y + face.height);
            //    rectangle(frame, onePoint, twoPoint, Scalar(0, 0, 0), 1, 8, 0);

                double faceWidthInPixels = twoPoint.x - onePoint.x;
                double distanceFromCamera = calculateDistanceFromCamera(faceWidthInPixels);

                onePoint.y = onePoint.y - 20;
                //putText(frame, "Distance: " + std::to_string((int) distanceFromCamera), onePoint, FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 0));

                Point eyeCenters[2];
                for(size_t j = 0; j < eyes.size(); j++)
                {
                    //Draw the eyes
                    Point eye_center(face.x + eyes[j].x + eyes[j].width/2, face.y + eyes[j].y + eyes[j].height/2);
                    eyeCenters[j] = eye_center;
                    int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
            //        circle(frame, eye_center, radius, Scalar( 255, 0, 255 ), 3, 8, 0);
                }
                // calculation of distance between eye centers, maybe will be useful, but not now ;)
                         // line(frame, eyeCenters[0], eyeCenters[1], Scalar(255, 255, 255), 1, 8, 0);
                double distance = calculateDistance( eyeCenters[1], eyeCenters[0] );
                eyeCenters[0].x = eyeCenters[0].x + distance / 4;



            }
        }
        //Show what you got
        //QImage qimgWebCamImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        //ui->lblWebCamImage->setPixmap(QPixmap::fromImage(qimgWebCamImage));

        imshow( window_name, frame );
    }
}

void Dialog::on_btnPauseOResume_clicked()
{
    if(tmrTimer->isActive() == true)
    {
        tmrTimer->stop();
        ui->btnPauseOResume->setText("Resume");
    }
    else
    {
        tmrTimer->start(20);
        ui->btnPauseOResume->setText("Pause");
    }
}
