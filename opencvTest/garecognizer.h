#ifndef GARECOGNIZER_H
#define GARECOGNIZER_H

#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

class GARecognizer
{
public:
    GARecognizer(string rootDir);
    string estimateGender(Mat sample);
    string estimateAgeRange(Mat sample);
};

#endif // GARECOGNIZER_H
