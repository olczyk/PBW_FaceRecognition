#include "dialog.h"
#include "ui_dialog.h"

#include<QtCore>

using namespace cv;
using namespace std;

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog)
{
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

void drawCaption(string caption, Rect face, Mat frame)
{
    Point origin(face.x, face.y + face.height);
    int fontFace = FONT_HERSHEY_COMPLEX;
    double fontScale = 0.75;
    Scalar textColor(0,255, 0);

    putText(frame, caption, origin, fontFace, fontScale, textColor);
}

void Dialog::detectAndDisplay(Mat frame)
{
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
           //Draw the face
           Point center(face.x + face.width/2, face.y + face.height/2);
           ellipse(frame, center, Size( face.width/2, face.height/2), 0, 0, 360, Scalar( 255, 0, 0 ), 2, 8, 0);

          for(Rect eye: eyes)
          {
              //Draw the eyes
              Point eye_center(face.x + eye.x + eye.width/2, face.y + eye.y + eye.height/2);
              int radius = cvRound((eye.width + eye.height)*0.25);
              circle(frame, eye_center, radius, Scalar( 255, 0, 255 ), 3, 8, 0);
          }

          string gender = "N/A";
          string caption = "gender: " + gender;
          drawCaption(caption, face, frame);
       }
    }
   //Show what you got
   //QImage qimgWebCamImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
   //ui->lblWebCamImage->setPixmap(QPixmap::fromImage(qimgWebCamImage));

   imshow( window_name, frame );
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
