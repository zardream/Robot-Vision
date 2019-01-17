#include<opencv2/imgproc/imgproc.hpp>

#include<math.h>

#include <iostream>

#include <stdlib.h>

#include <opencv2/core/core.hpp> 

#include<opencv2/highgui/highgui.hpp> 

#include <opencv2/opencv.hpp>

using namespace cv;

using namespace std;

int main()

{
	double fps;
	char string[10];  // 用于存放帧率的字符串
	cv::namedWindow("Camera FPS");
	double t = 0;

	//打开第一个摄像头

	VideoCapture cap(0);

	//判断摄像头是否打开

	if (!cap.isOpened())

	{

		cout << "摄像头未成功打开" << endl;

	}






	for (;;)

	{

		//创建Mat对象

		Mat frame;


		//从cap中读取一帧存到frame中

		cap >> frame;


		//判断是否读取到

		if (frame.empty())

		{

			break;

		}

		//显示摄像头读取到的图像

		t = (double)cv::getTickCount();
		if (cv::waitKey(50) == 30) { break; }
		if (cap.isOpened())
		{
			cap >> frame;
			// getTickcount函数：返回从操作系统启动到当前所经过的毫秒数
			// getTickFrequency函数：返回每秒的计时周期数
			// t为该处代码执行所耗的时间,单位为秒,fps为其倒数
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			fps = 1.0 / t;
			sprintf_s(string, "%.2f", fps);      // 帧率保留两位小数
			std::string fpsString("FPS:");
			fpsString += string;                    // 在"FPS:"后加入帧率数值字符串
			// 将帧率信息写在输出帧上
			putText(frame, // 图像矩阵
				fpsString,                  // string型文字内容
				cv::Point(5, 20),           // 文字坐标，以左下角为原点
				cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
				0.5, // 字体大小
				cv::Scalar(0, 0, 0));       // 字体颜色
			cv::imshow("Camera FPS", frame);

			int LowH = 100;
			int HighH = 130;

			int LowS = 175;
			int HighS = 255;

			int LowV = 240;
			int HighV = 255;
			Mat src = frame;
			Mat imgHSV;
			vector<Mat> hsvSplit;
			cvtColor(src, imgHSV, COLOR_BGR2HSV);

			split(imgHSV, hsvSplit);
			equalizeHist(hsvSplit[2], hsvSplit[2]);
			merge(hsvSplit, imgHSV);
			Mat imgThresholded;

			inRange(imgHSV, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), imgThresholded);

			Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
			morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);

			morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);

		


			src = imgThresholded;
			//cvtColor(src, src, COLOR_RGB2GRAY);
			Mat matSrc = src;
			GaussianBlur(matSrc, matSrc, Size(5, 5), 0);
			vector<vector<Point> > contours;//contours的类型，双重的vector
			vector<Vec4i> hierarchy;//Vec4i是指每一个vector元素中有四个int型数据。
			//阈值
			threshold(matSrc, matSrc, 21, 255, THRESH_BINARY);
			//寻找轮廓，这里注意，findContours的输入参数要求是二值图像，二值图像的来源大致有两种，第一种用threshold，第二种用canny
			findContours(matSrc.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
			/// 计算矩
			vector<Moments> mu(contours.size());
			for (int i = 0; i < contours.size(); i++)
			{
				mu[i] = moments(contours[i], false);
			}
			///  计算中心矩:
			vector<Point2f> mc(contours.size());
			for (int i = 0; i < contours.size(); i++)
			{
				mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
			}
			/// 绘制轮廓
			Mat drawing = Mat::zeros(matSrc.size(), CV_8UC1);
			for (int i = 0; i < contours.size(); i++)
			{
				Scalar color = Scalar(255);
				drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
				circle(drawing, mc[i], 4, color, -1, 8, 0);
			}
			imshow("outImage", drawing);


			//等待300毫秒，如果按键则退出循环

			if (waitKey(300) >= 0)

			{

				break;

			}

		}




	}
}
