#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <QDebug>
#include <QThread>

using namespace cv;
using namespace std;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()


/** @function Dilation */
void Dilation( Mat src, Mat dst, int dilation_size = 3, int dilation_type = MORPH_CROSS){
//  int dilation_type = MORPH_CROSS;
//  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
//  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
//  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

  Mat element = getStructuringElement( dilation_type,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
  /// Apply the dilation operation
  dilate( src, dst, element );
  //imshow( "Dilation Demo", dst );
}

int main(int argc, char **argv)
{
    // Read video
    VideoCapture video("test2.mp4");
    // VideoCapture video("rtmp://stream.lesohranitel.ru:1935/static/aqua_axis1");
    // VideoCapture video("rtmp://stream.lesohranitel.ru:1935/static/aqua_axis2");
    // Exit if video is not opened
    if(!video.isOpened())
    {
        cout << "Could not read video file" << endl;
        return 1;
    }

    // Read first frame
    Mat frame, resizedFrame;
    bool ok = video.read(frame);

    cv::resize(frame, resizedFrame, cv::Size(), 0.1,0.1, INTER_LINEAR);
    Ptr<BackgroundSubtractor> bgsubtractor;
    Mat  bgmaskTemp ;

    if (bgmaskTemp.data == 0)
           bgmaskTemp = resizedFrame;
    bgsubtractor = createBackgroundSubtractorMOG2(200,70,true);


    // Define initial boundibg box
    //Rect2d bbox(770, 23, 90, 60);


    //Rect2d finish(0, 999, frame.size().width, 80);
    // Uncomment the line below to select a different bounding box
    // bbox = selectROI(frame, false);

    // Display bounding box.
    //rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );

    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5),Point( 1, 1) );

    /* Параметризация детектора движения
     * {"motionDetector": {
     *    "bgsubtractor" : {"history": 200, "threshold": 70},
     *    "resize": 10,
     *    "beforeFilter": [],
     *    "afterFilter": [
     *      {"type": "medianBlur", "params": {"size": 5}},
     *      {"type": "threshold"},
     *      {"type": "Dilation"},
     *      {"type": "Canny"}
     *    ]
     * }}
     *
     */


    while(video.read(frame))
    {
        // Start timer
        double timer = (double)getTickCount();
        //QThread::msleep(50);

        //cv::resize(frame, resizedFrame, cv::Size(), 0.1,0.1, INTER_AREA);   // SLOW and GOOD
        cv::resize(frame, resizedFrame, cv::Size(), 0.1,0.1, INTER_LINEAR);   // FAST and BAD (x2-4)

        // Evеньшаем шум
        //morphologyEx( resizedFrame, resizedFrame, cv::MORPH_OPEN, element );
        //morphologyEx( resizedFrame, resizedFrame, cv::MORPH_CLOSE, element);
        // Вычитаем фон
        bgsubtractor->apply(resizedFrame ,bgmaskTemp);

        // фильтруем мелкий шум
        cv::medianBlur(bgmaskTemp, bgmaskTemp, 5);
        cv::threshold(bgmaskTemp, bgmaskTemp, 0, 255, cv::THRESH_BINARY);

        // сращиваем светлые пятна
        Dilation(bgmaskTemp, bgmaskTemp, 7, MORPH_RECT);

        //Ищем контуры
        Mat findCounter;
        bgmaskTemp.copyTo(findCounter);
        cv::Canny(bgmaskTemp, findCounter, 0, 100, 5);
        std::vector<std::vector<cv::Point> > contours;
        findContours(findCounter, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        for(int i = 0; i< contours.size(); i++ ){
            Rect boundRect = boundingRect( Mat(contours[i]) );
            // qDebug() << boundRect.width << "x" << boundRect.height;
            // rectangle(findCounter, boundRect, Scalar( 255, 255, 255 ));
        }
        imshow("COUNTURS", findCounter);

        // Calculate Frames per second (FPS)
        float fps = getTickFrequency() / ((double)getTickCount() - timer);

        // Display FPS on frame
        // putText(frame, "FPS : " + SSTR(int(fps)), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);
        qDebug() << "FPS: " << fps;
        // Exit if ESC pressed.
        int k = waitKey(1);
        if(k == 27)
        {
            break;
        }


    }
}

