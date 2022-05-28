#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>            
using namespace cv;
Mat pic,gray,dst,dst1,dst2,dst3,dst4,outimage,canny;
int threshold_value = 127;
int threshold_max = 255;
int type_value = 2;
int type_max = 4;
char output[] = "final output";
void Threshold_Demo(int ,void*);



Point2f computeIntersect(Vec4i a, Vec4i b)
{
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3],    x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

/*
    double ka,kb;
    ka = (double)(y2-y1)/(double)(x2-x1);
    kb = (double)(y4-y2)/(double)(x4-x2);

    Point2f crossPoint;
    crossPoint.x = (ka*x1-y1-kb*x3+y3)/(ka-kb);
    crossPoint.y = (ka*kb*(x1-x3)+ka*y3-kb*y1)/(ka-kb);
    return crossPoint;
*/

    if (float d = ((float)(x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4)))
    {
        Point2f pt;
        pt.x = ((x1*y2 - y1 * x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3 * x4)) / d;
        pt.y = ((x1*y2 - y1 * x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3 * x4)) / d;
        return pt;
    }
    else
        return Point2f(-1, -1);

}

int main(){
    pic = imread("/home/chen/pic/src.jpg");
    cvtColor(pic,gray,COLOR_RGB2GRAY);
    threshold(gray,dst,155,255,1);
    Mat kernel= getStructuringElement(MORPH_RECT,Size(11,11),Point(-1,-1));
    morphologyEx(dst,dst,3,kernel); // 2(开操作) 先腐蚀后膨胀  3(闭操作)先膨胀后腐蚀
    dst = ~dst;


    Canny(dst,canny,100,200,3,false);
    std::vector<Vec4i> plines;
    HoughLinesP(canny,plines,1,CV_PI/180.0,10,0,10);
    Scalar color = Scalar(0,0,255);


    Mat structureElement = getStructuringElement(MORPH_RECT,Size(9,9),Point(-1,-1));
    dilate(canny,canny,structureElement,Point(-1,-1),1);  //膨胀


    outimage.create(pic.size(),pic.type());
    pic.copyTo(outimage,dst);
/*
    for(size_t i =0 ;i<plines.size();i++){
        Vec4f hlines = plines[i];
        line(outimage,Point(hlines[0],hlines[1]),Point(hlines[2],hlines[3]),color,3);
        std::cout<<hlines[1]<<std::endl;

    }
*/
    std::vector<Point2f> corners(4);//线的交点存储  
    for (size_t i= 0; i<plines.size(); i++)
    {
        for (size_t j = i + 1; j<plines.size(); j++)
        {
            Point2f pt = computeIntersect(plines[i], plines[j]);
            if (pt.x >= 0 && pt.y >= 0)
            {
                corners.push_back(pt);
            }
        }
    }


    std::vector<Point2f> outimage_corners(4);
    outimage_corners[0]=Point(965,1106);
    outimage_corners[1]=Point(965+927-200,1106+160);
    outimage_corners[2]=Point(965+285,1106+1426);
    outimage_corners[3]=Point(965+1236,1106+1426-100);            

    std::vector<Point2f> result_corners(4);
    result_corners[0] = Point(0,0);
    result_corners[1] = Point(360,0);
    result_corners[2] = Point(0,680);
    result_corners[3] = Point(360,680);

    Mat m = getPerspectiveTransform(corners,result_corners);
    Mat resultImage;
    warpPerspective(outimage,resultImage,m,Size(360,680),INTER_LINEAR);



//    namedWindow("output",0);
//    resizeWindow("output", 500, 500); //创建一个500*500大小的窗口
//    imshow("output",pic);  

    namedWindow(output,0);
    resizeWindow(output, 500, 500); //创建一个500*500大小的窗口
    imshow(output,dst);

    namedWindow("canny",0);
    resizeWindow("canny", 500, 500); //创建一个500*500大小的窗口
    imshow("canny",canny);

   namedWindow("outimage",0);
    resizeWindow("outimage", 500, 500); //创建一个500*500大小的窗口
    imshow("outimage",outimage);
  
//    namedWindow("result",0);
//    resizeWindow("result", 500, 500); //创建一个500*500大小的窗口
//    imshow("result",resultImage);

    waitKey(0);
}
