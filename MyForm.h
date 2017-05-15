#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

#pragma once


namespace 測試視窗 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	//--
	using namespace std;
	using namespace cv;
	double gRemoveSize, gBigObj;
	int gProtectSize = 51;
	int paint_size=100;
	int scollbarMax=350;
	int scollbarMin=20;
	int DUBBLECLICK = 0;//看是雙擊模式還是一般模式
	int modeDC = 0;//看是放大模式還是一般大小模式
	int now_down_for_circle = 0;

	string imgName;
	string saveimgName;
	Mat original;
	Mat RedPic;
	Mat img;
	Mat protectRegion;

	Mat RedPicCPYforCIRCLE;
	Mat OriginCPYforCIRCLE;
	Mat tempPIC;

	//連通元件用
	Mat labelImage, stats, centroids;
	int label;
	bool * protectLabel;

	//橡皮擦用
	Mat RedPicCPY;
	Mat ProtectRegionCPY;
	Mat BinaryMapForUI;
	Mat BinaryMapForProtectMap;
	Mat BinaryMapForUPsample;
	Mat RedPicCPY_forUPSAMPLEeraser;

	//放大用
	Mat RedPicCPY_forUPSAMPLE;
	Mat ROI;
	const int UPSAMPLERATE = 4;
	int ROIxBEGIN, ROIxEND, ROIyBEGIN, ROIyEND;

	//偵測window關否
	void * WindowHandle1;
	void * PREWindowHandle1;
	
	
	
	void mymouse(int event, int x, int y, int flag, void* param)
	{

		//邊界定論
		if (x < paint_size / 2)x = paint_size / 2;
		else if (x>RedPic.cols - paint_size / 2)x = RedPic.cols - paint_size / 2;
		if (y < paint_size / 2)y = paint_size / 2;
		else if (y>RedPic.rows - paint_size / 2)y = RedPic.rows - paint_size / 2;

		static int oldx, oldy, DownX, DownY, oldDownX, oldDownY, now_down = 0, eraserDown;
		int XupsampleBEGIN = x - paint_size / 2, YupsampleBEGIN = y - paint_size / 2, XupsampleEND = x + paint_size / 2, YupsampleEND = y + paint_size / 2;

		/*左鍵雙擊放大部分*/
		if (event == CV_EVENT_LBUTTONDBLCLK && DUBBLECLICK == 1){

			//回復區域圖片
			for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
			for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
				RedPic.at<Vec3b>(i, j)[0] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[0];
				RedPic.at<Vec3b>(i, j)[1] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[1];
				RedPic.at<Vec3b>(i, j)[2] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[2];
			}//for

			modeDC = 1;
			ROIxBEGIN = x * 4 - RedPicCPY_forUPSAMPLE.cols / (UPSAMPLERATE * 2);
			ROIxEND = x * 4 + RedPicCPY_forUPSAMPLE.cols / (UPSAMPLERATE * 2);
			ROIyBEGIN = y * 4 - RedPicCPY_forUPSAMPLE.rows / (UPSAMPLERATE * 2);
			ROIyEND = y * 4 + RedPicCPY_forUPSAMPLE.rows / (UPSAMPLERATE * 2);


			//if (ROIxBEGIN < 0) ROIxBEGIN = 0;
			//else if (ROIxEND > RedPicCPY_forUPSAMPLE.cols)ROIxBEGIN = RedPicCPY_forUPSAMPLE.cols - RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE;
			//
			//if (ROIyBEGIN< 0) ROIyBEGIN = 0;
			//else if (ROIyEND > RedPicCPY_forUPSAMPLE.rows)ROIyBEGIN = RedPicCPY_forUPSAMPLE.rows - RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE;

			//--
			if (ROIyBEGIN < 0){
				ROIyBEGIN = 0;
				ROIyEND = RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1;
			}

			if (ROIyBEGIN >original.rows - RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1){
				ROIyBEGIN = original.rows - RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1;
				ROIyEND = original.rows - 1;
			}

			if (ROIxBEGIN < 0){
				ROIxBEGIN = 0;
				ROIxEND = RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1;
			}

			if (ROIxBEGIN >original.cols - RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1){
				ROIxBEGIN = original.cols - RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1;
				ROIxEND = original.cols - 1;
			}

			ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE));
			DUBBLECLICK = 0;
			imshow("RedPic", ROI);
		}
		/*右鍵雙擊縮小部分*/
		else if (event == CV_EVENT_RBUTTONDBLCLK&&DUBBLECLICK == 1){
			//拷貝回復區域圖片
			tempPIC = RedPic(Rect(x - paint_size / 2, y - paint_size / 2, paint_size, paint_size));
			RedPicCPYforCIRCLE = tempPIC.clone();
			circle(RedPic, cvPoint(x, y), paint_size / 2 - 1, cvScalar(255, 255, 255));

			//回復區域圖片
			for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
			for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
				RedPic.at<Vec3b>(i, j)[0] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[0];
				RedPic.at<Vec3b>(i, j)[1] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[1];
				RedPic.at<Vec3b>(i, j)[2] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[2];
			}//for

			modeDC = 0;
			DUBBLECLICK = 0;
		}

		//放大圖對應原圖的點
		DownX = ROIxBEGIN + x;
		DownY = ROIyBEGIN + y;


		/*右鍵框選部分*/
		if (event == CV_EVENT_RBUTTONDOWN){
			//若在一般模式下
			if (DUBBLECLICK == 0 && modeDC == 0){
				oldx = x; oldy = y;
			}
			//若在放大模式下
			else if (modeDC == 1){
				oldDownX = DownX; oldDownY = DownY;
				oldx = x; oldy = y;
			}

		}
		else if (event == CV_EVENT_RBUTTONUP){
			now_down_for_circle = 1;
			//若在一般模式下
			if (DUBBLECLICK == 0 && modeDC == 0){
				rectangle(protectRegion, Rect(cv::Point(oldx * 4, oldy * 4), cvPoint(x * 4, y * 4)), Scalar(255), -1);
				//rectangle(RedPic, Rect(cv::Point(oldx, oldy), cvPoint(x, y)), Scalar(0, 255, 0), -1);
				for (int i = oldy; i < y; i++)
				for (int j = oldx; j < x; j++){
					if ((int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>(i, j)[0] = 255;
						RedPic.at<Vec3b>(i, j)[1] = 255;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>(i, j)[0] = 0;
						RedPic.at<Vec3b>(i, j)[1] = 0;
						RedPic.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) != 255){
						RedPic.at<Vec3b>(i, j)[0] = 40;
						RedPic.at<Vec3b>(i, j)[1] = 40;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
				}//for
				//rectangle(RedPicCPY_forUPSAMPLE, Rect(cv::Point(oldx * 4, oldy * 4), cvPoint(x * 4, y * 4)), Scalar(0, 255, 0), -1);

				for (int i = oldy * 4; i < y * 4; i++)
				for (int j = oldx * 4; j < x * 4; j++){
					if ((int)protectRegion.at<uchar>(i, j) == 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 255;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 255;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) == 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 0;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 0;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) != 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 40;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 40;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 255;
					}
				}//for
			}
			//若在放大模式下
			else if (modeDC == 1){
				rectangle(protectRegion, Rect(cv::Point(oldDownX, oldDownY), cvPoint(DownX, DownY)), Scalar(255), -1);
				//rectangle(ROI, Rect(cv::Point(oldx , oldy ), cvPoint(x, y )), Scalar(0, 255, 0), -1);
				for (int i = oldy; i < y; i++)
				for (int j = oldx; j < x; j++){
					if ((int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 255){
						ROI.at<Vec3b>(i, j)[0] = 255;
						ROI.at<Vec3b>(i, j)[1] = 255;
						ROI.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 0 && (int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 255){
						ROI.at<Vec3b>(i, j)[0] = 0;
						ROI.at<Vec3b>(i, j)[1] = 0;
						ROI.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 0 && (int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) != 255){
						ROI.at<Vec3b>(i, j)[0] = 40;
						ROI.at<Vec3b>(i, j)[1] = 40;
						ROI.at<Vec3b>(i, j)[2] = 255;
					}
				}//for
				//rectangle(RedPic, Rect(cv::Point(oldDownX/4, oldDownY/4), cvPoint(DownX/4, DownY/4)), Scalar(0, 255, 0), -1);
				for (int i = oldDownY / 4; i < DownY / 4; i++)
				for (int j = oldDownX / 4; j < DownX / 4; j++){
					if ((int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>(i, j)[0] = 255;
						RedPic.at<Vec3b>(i, j)[1] = 255;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>(i, j)[0] = 0;
						RedPic.at<Vec3b>(i, j)[1] = 0;
						RedPic.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) != 255){
						RedPic.at<Vec3b>(i, j)[0] = 40;
						RedPic.at<Vec3b>(i, j)[1] = 40;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
				}//for
				oldDownX = DownX; oldDownY = DownY;
				oldx = x; oldy = y;
			}
		}

		//中鍵橡皮擦部分
		else if (event == CV_EVENT_MBUTTONDOWN){
			eraserDown = 1;
			//若在一般模式下
			if (modeDC == 0 && DUBBLECLICK == 0){
				oldx = x; oldy = y;
				//oldestx = x, oldesty = y;
			}

			//若在放大模式下
			else if (modeDC == 1){
				oldx = x; oldy = y;
				oldDownX = DownX; oldDownY = DownY;
			}
		}
		else if (event == CV_EVENT_MBUTTONUP&&DUBBLECLICK == 0){
			now_down_for_circle = 1;
			eraserDown = 0;

			//修補劃線後破損,使用放掉後補償
			for (int i = 0; i < RedPic.rows; i++)
			for (int j = 0; j < RedPic.cols; j++){

				if ((int)protectRegion.at<uchar>(i * 4, j * 4) == 0){
					if (((int)RedPic.at<Vec3b>(i, j)[0] + (int)RedPic.at<Vec3b>(i, j)[1] + (int)RedPic.at<Vec3b>(i, j)[2]) / 3 <125){
						RedPic.at<Vec3b>(i, j)[0] = 40;
						RedPic.at<Vec3b>(i, j)[1] = 40;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
					else{
						RedPic.at<Vec3b>(i, j)[0] = 125;
						RedPic.at<Vec3b>(i, j)[1] = 125;
						RedPic.at<Vec3b>(i, j)[2] = 125;
					}
				}
			}
			for (int i = 0; i < RedPicCPY_forUPSAMPLE.rows; i++)
			for (int j = 0; j < RedPicCPY_forUPSAMPLE.cols; j++){
				if ((int)protectRegion.at<uchar>(i, j) == 0){
					if (((int)RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] + (int)RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] + (int)RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2]) / 3 <125){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 40;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 40;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 255;
					}
					else{
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 125;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 125;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 125;
					}
				}
			}//for
		}
		else if (event == CV_EVENT_MOUSEMOVE && eraserDown == 1){
			//若在一般模式下
			if (DUBBLECLICK == 0 && modeDC == 0){
				line(protectRegion, cvPoint(x * 4, y * 4), cvPoint(oldx * 4, oldy * 4), cvScalar(0), paint_size * 4, 8, 0);

				/*for (int i = (y - paint_size/2)*4; i < (y + paint_size/2)*4; i++)
				for (int j = (x - paint_size/2)*4; j < (x + paint_size/2)*4; j++){
				protectRegion.at<uchar>(i, j) = 0;
				}*/

				for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
				for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
					//這裡再加個圓公式或著距離公式 小於等於半徑
					if (pow(i - y, 2) + pow(j - x, 2) <= pow(paint_size / 2, 2)){
						if (((int)RedPic.at<Vec3b>(i, j)[0] + (int)RedPic.at<Vec3b>(i, j)[1] + (int)RedPic.at<Vec3b>(i, j)[2]) / 3 <125){
							RedPic.at<Vec3b>(i, j)[0] = 40;
							RedPic.at<Vec3b>(i, j)[1] = 40;
							RedPic.at<Vec3b>(i, j)[2] = 255;
						}
						else{
							RedPic.at<Vec3b>(i, j)[0] = 125;
							RedPic.at<Vec3b>(i, j)[1] = 125;
							RedPic.at<Vec3b>(i, j)[2] = 125;
						}
					}
				}
			}


			//若在放大模式下
			if (modeDC == 1){
				line(protectRegion, cvPoint(DownX, DownY), cvPoint(oldDownX, oldDownY), cvScalar(0), paint_size, 8, 0);

				for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
				for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
					//這裡再加個圓公式或著距離公式 小於等於半徑
					if (pow(i - y, 2) + pow(j - x, 2) <= pow(paint_size / 2, 2)){
						if (((int)ROI.at<Vec3b>(i, j)[0] + (int)ROI.at<Vec3b>(i, j)[1] + (int)ROI.at<Vec3b>(i, j)[2]) / 3 <125){
							ROI.at<Vec3b>(i, j)[0] = 40;
							ROI.at<Vec3b>(i, j)[1] = 40;
							ROI.at<Vec3b>(i, j)[2] = 255;
						}
						else{
							ROI.at<Vec3b>(i, j)[0] = 125;
							ROI.at<Vec3b>(i, j)[1] = 125;
							ROI.at<Vec3b>(i, j)[2] = 125;
						}
					}
				}//end for
			}
			oldDownX = DownX; oldDownY = DownY;
			oldx = x; oldy = y;
		}


		/*為了讓游標有圓圈顯示*/
		//在一般模式下
		if (modeDC == 0 && now_down_for_circle == 0){
			//拷貝回復區域圖片
			tempPIC = RedPic(Rect(x - paint_size / 2, y - paint_size / 2, paint_size, paint_size));
			RedPicCPYforCIRCLE = tempPIC.clone();
			circle(RedPic, cvPoint(x, y), paint_size / 2 - 1, cvScalar(255, 255, 255));
		}
		/*FIXING*/
		//在放大模式下
		else if (modeDC == 1 && now_down_for_circle == 0){
			//拷貝回復區域圖片
			tempPIC = RedPicCPY_forUPSAMPLE(Rect(DownX - paint_size / 2, DownY - paint_size / 2, paint_size, paint_size));
			OriginCPYforCIRCLE = tempPIC.clone();
			circle(ROI, cvPoint(x, y), paint_size / 2 - 1, cvScalar(255, 255, 255));
		}


		/*左鍵畫線部分*/
		if (event == CV_EVENT_LBUTTONDOWN){
			//若在一般模式下
			if (modeDC == 0 && DUBBLECLICK == 0){

				oldx = x; oldy = y;
				//oldestx = x, oldesty = y;
				now_down = 1;
			}

			//若在放大模式下
			else if (modeDC == 1){
				oldx = x; oldy = y;
				oldDownX = DownX; oldDownY = DownY;
				now_down = 1;
			}
		}
		else if (event == CV_EVENT_LBUTTONUP){
			now_down_for_circle = 0;
			now_down = 0;

			//修補劃線後破損,使用放掉後補償
			//在一般模式下
			if (modeDC == 0 && DUBBLECLICK == 0){
				for (int i = 0; i <RedPic.rows; i++)
				for (int j = 0; j <RedPic.cols; j++){
					if ((int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>(i, j)[0] = 255;
						RedPic.at<Vec3b>(i, j)[1] = 255;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>(i, j)[0] = 0;
						RedPic.at<Vec3b>(i, j)[1] = 0;
						RedPic.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) != 255){
						RedPic.at<Vec3b>(i, j)[0] = 40;
						RedPic.at<Vec3b>(i, j)[1] = 40;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
				}

				for (int i = 0; i < protectRegion.rows; i++)
				for (int j = 0; j < protectRegion.cols; j++){
					if ((int)protectRegion.at<uchar>(i, j) == 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 255;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 255;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) == 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 0;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 0;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) != 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 40;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 40;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 255;
					}
				}//for
				imshow("RedPic", RedPic);
			}
			//若在放大模式下
			else if (modeDC == 1){
				//cout << "a=" << ROIyBEGIN << "," << ROIyEND << "," << ROIxBEGIN << "," << ROIxEND << endl;
				for (int i = ROIyBEGIN; i < ROIyEND; i++)
				for (int j = ROIxBEGIN; j < ROIxEND; j++){
					if ((int)protectRegion.at<uchar>(i, j) == 255){
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[0] = 255;
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[1] = 255;
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[2] = 255;
					}
					if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) == 255){
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[0] = 0;
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[1] = 0;
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[2] = 0;
					}
					if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) != 255){
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[0] = 40;
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[1] = 40;
						ROI.at<Vec3b>(i - ROIyBEGIN, j - ROIxBEGIN)[2] = 255;
					}
				}//for

				/*error sloved*/
				for (int i = 0; i < RedPic.rows; i++)
				for (int j = 0; j < RedPic.cols; j++){
					if ((int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>((i), (j))[0] = 255;
						RedPic.at<Vec3b>((i), (j))[1] = 255;
						RedPic.at<Vec3b>((i), (j))[2] = 255;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>((i), (j))[0] = 0;
						RedPic.at<Vec3b>((i), (j))[1] = 0;
						RedPic.at<Vec3b>((i), (j))[2] = 0;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) != 255){
						RedPic.at<Vec3b>((i), (j))[0] = 40;
						RedPic.at<Vec3b>((i), (j))[1] = 40;
						RedPic.at<Vec3b>((i), (j))[2] = 255;
					}
				}//for

				imshow("RedPic", ROI);
			}
		}
		else if (event == CV_EVENT_MOUSEMOVE && now_down == 1){
			now_down_for_circle = 1;
			//若在一般模式下
			if (modeDC == 0 && DUBBLECLICK == 0){
				if (XupsampleBEGIN < 0)XupsampleBEGIN = 0;
				else if (XupsampleEND>RedPic.cols)XupsampleEND = RedPic.cols;

				if (YupsampleBEGIN < 0)YupsampleBEGIN = 0;
				else if (YupsampleEND>RedPic.rows)YupsampleEND = RedPic.rows;

				line(protectRegion, cvPoint(x * 4, y * 4), cvPoint(oldx * 4, oldy * 4), cvScalar(255), paint_size * 4, 8, 0);
				//line(RedPic, cvPoint(x, y), cvPoint(oldx, oldy), cvScalar(0, 255, 0), paint_size, 8, 0);
				for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
				for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
					if ((int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>(i, j)[0] = 255;
						RedPic.at<Vec3b>(i, j)[1] = 255;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) == 255){
						RedPic.at<Vec3b>(i, j)[0] = 0;
						RedPic.at<Vec3b>(i, j)[1] = 0;
						RedPic.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i * 4, j * 4) == 0 && (int)protectRegion.at<uchar>(i * 4, j * 4) != 255){
						RedPic.at<Vec3b>(i, j)[0] = 40;
						RedPic.at<Vec3b>(i, j)[1] = 40;
						RedPic.at<Vec3b>(i, j)[2] = 255;
					}
				}//for
				//line(RedPicCPY_forUPSAMPLE, cvPoint(x * 4, y * 4), cvPoint(oldx * 4, oldy * 4), cvScalar(0, 255, 0), paint_size * 4, 8, 0);



				for (int i = YupsampleBEGIN * 4; i < YupsampleEND * 4; i++)
				for (int j = XupsampleBEGIN * 4; j < XupsampleEND * 4; j++){
					if ((int)protectRegion.at<uchar>(i, j) == 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 255;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 255;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) == 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 0;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 0;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) != 255){
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 40;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[1] = 40;
						RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[2] = 255;
					}
				}//for
				oldx = x; oldy = y;

			}
			//若在放大模式下
			if (modeDC == 1){
				if (XupsampleBEGIN < 0)XupsampleBEGIN = 0;
				else if (XupsampleEND>RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE)XupsampleEND = RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE;

				if (YupsampleBEGIN < 0)YupsampleBEGIN = 0;
				else if (YupsampleEND>RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE)YupsampleEND = RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE;

				line(protectRegion, cvPoint(DownX, DownY), cvPoint(oldDownX, oldDownY), cvScalar(255), paint_size, 8, 0);
				//line(ROI, cvPoint(x, y), cvPoint(oldx, oldy), cvScalar(0, 255, 0), paint_size*2, 8, 0);
				//line(RedPic, cvPoint(DownX / 4, DownY / 4), cvPoint(oldDownX / 4, oldDownY/4), cvScalar(0, 255, 0), paint_size/2, 8, 0);

				for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
				for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
					if ((int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 255){
						ROI.at<Vec3b>(i, j)[0] = 255;
						ROI.at<Vec3b>(i, j)[1] = 255;
						ROI.at<Vec3b>(i, j)[2] = 255;
					}
					if ((int)original.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 0 && (int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 255){
						ROI.at<Vec3b>(i, j)[0] = 0;
						ROI.at<Vec3b>(i, j)[1] = 0;
						ROI.at<Vec3b>(i, j)[2] = 0;
					}
					if ((int)original.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 0 && (int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) != 255){
						ROI.at<Vec3b>(i, j)[0] = 40;
						ROI.at<Vec3b>(i, j)[1] = 40;
						ROI.at<Vec3b>(i, j)[2] = 255;
					}
				}//for
				/*error / maybe we don't need this*/
				//for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
				//for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
				//	if ((int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 255){
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[0] = 255;
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[1] = 255;
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[2] = 255;
				//	}
				//	if ((int)original.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 0 && (int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 255){
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[0] = 0;
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[1] = 0;
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[2] = 0;
				//	}
				//	if ((int)original.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 0 && (int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) != 255){
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[0] = 40;
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[1] = 40;
				//		RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[2] = 255;
				//	}
				//}//for

				oldx = x; oldy = y;
				oldDownX = DownX; oldDownY = DownY;

			}
		}





		//顯示圖片部分
		//放大模式
		if (modeDC == 1){
			DUBBLECLICK = 0;
			imshow("RedPic", ROI);

			/*FIXING*/
			//若沒有在畫圖時
			if (modeDC == 1 && now_down_for_circle == 0){
				//回復區域圖片
				for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
				for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
					ROI.at<Vec3b>(i, j)[0] = OriginCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[0];
					ROI.at<Vec3b>(i, j)[1] = OriginCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[1];
					ROI.at<Vec3b>(i, j)[2] = OriginCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[2];
				}//for
			}
		}
		//一般模式
		else{
			DUBBLECLICK = 0;
			imshow("RedPic", RedPic);

			//若沒有在畫圖時
			if (modeDC == 0 && now_down_for_circle == 0){
				//回復區域圖片
				for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
				for (int j = XupsampleBEGIN; j < XupsampleEND; j++){
					RedPic.at<Vec3b>(i, j)[0] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[0];
					RedPic.at<Vec3b>(i, j)[1] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[1];
					RedPic.at<Vec3b>(i, j)[2] = RedPicCPYforCIRCLE.at<Vec3b>(i - YupsampleBEGIN, j - XupsampleBEGIN)[2];
				}//for
			}
		}
		now_down_for_circle = 0;
	}
	//--
	/// <summary>
	/// MyForm 的摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO:  在此加入建構函式程式碼
			//
		}

	protected:
		/// <summary>
		/// 清除任何使用中的資源。
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorker1;
	private: System::Windows::Forms::HScrollBar^  hScrollBar1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;

	protected:

	public:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器
		/// 修改這個方法的內容。
		/// </summary>
		
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->hScrollBar1 = (gcnew System::Windows::Forms::HScrollBar());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 12);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Start";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// backgroundWorker1
			// 
			this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::backgroundWorker1_DoWork);
			// 
			// hScrollBar1
			// 
			this->hScrollBar1->Location = System::Drawing::Point(216, 46);
			this->hScrollBar1->Maximum = 350;
			this->hScrollBar1->Minimum = 20;
			this->hScrollBar1->Name = L"hScrollBar1";
			this->hScrollBar1->Size = System::Drawing::Size(192, 19);
			this->hScrollBar1->TabIndex = 1;
			this->hScrollBar1->Value = 100;
			this->hScrollBar1->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &MyForm::hScrollBar1_Scroll);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(118, 53);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(33, 12);
			this->label1->TabIndex = 2;
			this->label1->Text = L"label1";
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(120, 12);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(386, 25);
			this->progressBar1->TabIndex = 3;
			this->progressBar1->Click += gcnew System::EventHandler(this, &MyForm::progressBar1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(12, 76);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 4;
			this->button2->Text = L"Help";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(13, 42);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 5;
			this->button3->Text = L"Save";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MyForm::button3_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(518, 111);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->hScrollBar1);
			this->Controls->Add(this->button1);
			this->Cursor = System::Windows::Forms::Cursors::Cross;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm";
			this->Text = L" ";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				
				// openFileDialog1->Multiselect = true;
				 if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 try{
						 Image^ image = Image::FromFile(openFileDialog1->FileName);
					 }
					 catch (...)
					 {
						 MessageBox::Show("Unable to open this file!\nPlease choose another one!", "Warning", MessageBoxButtons::OK, MessageBoxIcon::Error);
						 return;
					 }
					 using namespace Runtime::InteropServices;
					 const char* chars =
						 (const char*)(Marshal::StringToHGlobalAnsi(openFileDialog1->FileName)).ToPointer();
					 imgName = chars;
					 Marshal::FreeHGlobal(IntPtr((void*)chars));
					 //imgName = Convert::ToString( openFileDialog1->FileName);
					 backgroundWorker1->RunWorkerAsync();
				 }
	}
	private: System::Void backgroundWorker1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
				// progressBar1->Value = 30;
				// MessageBox::Show("Image archives successful!!", "Message");
				 original = imread(imgName, 0);
				 //RedPic = new cv::Mat(original.rows, original.cols, CV_8UC3, Scalar(255, 255, 255));
				 RedPic = cv::Mat(original.rows, original.cols, CV_8UC3, Scalar(125, 125, 125));

				 img = imread(imgName, 0);
				 protectRegion = ~img.clone();
				 imwrite("protectregion_original.png", protectRegion);

				 progressBar1->Value = 15;

				 img = ~img;//黑變白
				 gRemoveSize = 2*((double)80 / (6155 * 6435)) * (img.rows * img.cols);


				 int nLabels = connectedComponentsWithStats(img, labelImage, stats, centroids, 8, CV_32S);
				 progressBar1->Value = 30;

				 //begin for copy
				 for (int h = 0; h < img.rows; h++)
				 for (int w = 0; w < img.cols; w++) {
					 label = labelImage.at<int>(h, w);
					 if (stats.at<int>(label, CC_STAT_AREA) < gRemoveSize)
						 protectRegion.at<uchar>(h, w) = 0;

				 } // for
				 progressBar1->Value = 40;


				 Mat kernel = getStructuringElement(MORPH_ELLIPSE, cv::Size(gProtectSize, gProtectSize));

				 dilate(protectRegion, protectRegion, kernel, cv::Point(-1, -1), 1);
				 progressBar1->Value = 70;

				 protectLabel = new bool[nLabels];
				 for (int i = 0; i < nLabels; i++) protectLabel[i] = false;

				 //------
				 for (int h = 0; h < img.rows; h++)
				 for (int w = 0; w < img.cols; w++)
				 if (protectRegion.at<uchar>(h, w) != 0)
					 protectLabel[labelImage.at<int>(h, w)] = true;
				 progressBar1->Value = 80;

				 for (int h = 0; h < img.rows; h++)
				 for (int w = 0; w < img.cols; w++) {
					 label = labelImage.at<int>(h, w);

					 if (stats.at<int>(label, CC_STAT_AREA) < gRemoveSize && !protectLabel[label])
						 img.at<uchar>(h, w) = 0;
				 } // for
				 progressBar1->Value = 90;
				 img = ~img;

				 //---------多加部分
				 for (int i = 0; i < img.rows; i++)
				 for (int j = 0; j < img.cols; j++){
					 if ((int)protectRegion.at<uchar>(i, j) == 255){
						 RedPic.at<Vec3b>(i, j)[0] = 255;
						 RedPic.at<Vec3b>(i, j)[1] = 255;
						 RedPic.at<Vec3b>(i, j)[2] = 255;
					 }
					 if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) == 255){
						 RedPic.at<Vec3b>(i, j)[0] = 0;
						 RedPic.at<Vec3b>(i, j)[1] = 0;
						 RedPic.at<Vec3b>(i, j)[2] = 0;
					 }
					 if ((int)original.at<uchar>(i, j) == 0 && (int)protectRegion.at<uchar>(i, j) != 255){
						 RedPic.at<Vec3b>(i, j)[0] = 40;
						 RedPic.at<Vec3b>(i, j)[1] = 40;
						 RedPic.at<Vec3b>(i, j)[2] = 255;
					 }
				 }//for

				 //放大用參考圖
				 RedPicCPY_forUPSAMPLE = RedPic.clone();
				 RedPicCPY_forUPSAMPLEeraser = RedPic.clone();
				 //變更顯示圖大小
				 resize(RedPic, RedPic, cv::Size(RedPic.cols / 4, RedPic.rows / 4), INTER_CUBIC);
				 imwrite("mod4.png", RedPic);

				 //橡皮擦用之物件
				 Mat TempForBinaryMap(RedPic.rows, RedPic.cols, CV_8UC1, Scalar(0));
				 Mat TempForBinaryMap_protectVER(original.rows, original.cols, CV_8UC1, Scalar(0));
				 Mat TempForBinaryMap_UPsampleVER(original.rows, original.cols, CV_8UC1, Scalar(0));

				 BinaryMapForUI = TempForBinaryMap.clone();
				 RedPicCPY = RedPic.clone();
				 BinaryMapForProtectMap = TempForBinaryMap_protectVER.clone();
				 ProtectRegionCPY = protectRegion.clone();
				 BinaryMapForUPsample = TempForBinaryMap_UPsampleVER.clone();


				 progressBar1->Value = 100;
				 namedWindow("RedPic", 0);//參數0代表以可包含全部圖片的視窗大小開啟
				 imshow("RedPic", RedPic);
				 
				 imwrite("Protect_Out.png", protectRegion);
				 imwrite("Final_Output.png", original);

				 cvSetMouseCallback("RedPic", mymouse);

				 WindowHandle1 = cvGetWindowHandle("RedPic");
				 PREWindowHandle1 = cvGetWindowHandle("RedPic");

				 int key = 0;
				 while (1) {
					 //偵測視窗開與關
					 WindowHandle1 = cvGetWindowHandle("RedPic");
					 if (WindowHandle1 != PREWindowHandle1){
						 for (int h = 0; h < img.rows; h++)
						 for (int w = 0; w < img.cols; w++)
						 if (protectRegion.at<uchar>(h, w) != 0)
							 protectLabel[labelImage.at<int>(h, w)] = true;

						 for (int h = 0; h < img.rows; h++)
						 for (int w = 0; w < img.cols; w++) {
							 label = labelImage.at<int>(h, w);

							 if (stats.at<int>(label, CC_STAT_AREA) < gRemoveSize && !protectLabel[label])
								 original.at<uchar>(h, w) = 255;
						 } // for
						 //----
						 for (int i = 0; i < protectRegion.rows; i++)
						 for (int j = 0; j < protectRegion.cols; j++){
							 if ((int)protectRegion.at<uchar>(i, j) == 0){
								 original.at<uchar>(i, j) = 255;
							 }
						 }

						 imwrite("Protect_Out.png", protectRegion);
						 imwrite("Final_Output.png", original);
						 MessageBox::Show("Image archives successful!!", "Message");
						 break;
					 }

					 if (modeDC == 1){//若在放大模式下
						 if (key == 'w' || key == 'W'){//上
							 ROIyBEGIN = ROIyBEGIN - 500;
							 ROIyEND = ROIyEND - 500;
							 if (ROIyBEGIN < 0)ROIyBEGIN = 0;
							 if (ROIyEND <RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1)ROIyEND = RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1;
							 ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
							 imshow("RedPic", ROI);
						 }
						 if (key == 's' || key == 'S'){//下
							 ROIyBEGIN = ROIyBEGIN + 500;
							 ROIyEND = ROIyEND + 500;
							 if (ROIyBEGIN >original.rows - ROI.rows - 1)ROIyBEGIN = original.rows - ROI.rows - 1;
							 if (ROIyEND > original.rows - 1)ROIyEND = original.rows - 1;
							 ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
							 imshow("RedPic", ROI);
						 }
						 if (key == 'A' || key == 'a'){//左
							 ROIxBEGIN = ROIxBEGIN - 500;
							 ROIxEND = ROIxEND - 500;
							 if (ROIxBEGIN < 0)ROIxBEGIN = 0;
							 if (ROIxEND < RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1)ROIxEND = RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1;
							 ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
							 imshow("RedPic", ROI);
						 }
						 if (key == 'D' || key == 'd'){//右
							 ROIxBEGIN = ROIxBEGIN + 500;
							 ROIxEND = ROIxEND + 500;
							 if (ROIxBEGIN >original.cols - ROI.cols - 1)ROIxBEGIN = original.cols - ROI.cols - 1;
							 if (ROIxEND > original.cols - 1)ROIxEND = original.cols - 1;
							 ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
							 imshow("RedPic", ROI);
						 }
					 }
					 if (key != 27){
						 if (key == '2'&&paint_size <= scollbarMax - 30)
							 paint_size += 30;

						 if (key == '1'&&paint_size >= scollbarMin + 30)
							 paint_size -= 30;

						 key = waitKey(1);
						 if (key == '3') DUBBLECLICK = 1;
					 }
					 else{
						 break;
					 }
				 }//
				 cvWaitKey();
				 for (int h = 0; h < img.rows; h++)
				 for (int w = 0; w < img.cols; w++)
				 if (protectRegion.at<uchar>(h, w) != 0)
					 protectLabel[labelImage.at<int>(h, w)] = true;

				 for (int h = 0; h < img.rows; h++)
				 for (int w = 0; w < img.cols; w++) {
					 label = labelImage.at<int>(h, w);

					 if (stats.at<int>(label, CC_STAT_AREA) < gRemoveSize && !protectLabel[label])
						 original.at<uchar>(h, w) = 255;
				 } // for
				 //----

				 imwrite("Protect_Out.png", protectRegion);
				 imwrite("Final_Output.png", original);
				 MessageBox::Show("Image archives successful!!", "Message");
				 //-----
				
			
	}
private: System::Void hScrollBar1_Scroll(System::Object^  sender, System::Windows::Forms::ScrollEventArgs^  e) {
				 paint_size = hScrollBar1->Value;
				 label1->Text = "Paint size : " + Convert::ToString(hScrollBar1->Value);
	}
private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e) {
			 hScrollBar1->Value = 100;
			 paint_size = 100;
			 label1->Text = "Paint size : " + Convert::ToString(hScrollBar1->Value);
			// hScrollBar1->Value = 250;
}
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			 MessageBox::Show("功能表:\n-----------------------\na.滑鼠部分:\n1.滑鼠左鍵拖曳:畫出保護區\n2.滑鼠右鍵拖曳:方形框出保護區\n3.滑鼠中鍵拖曳:橡皮擦(將保護區變成非保護區)\n4.鍵盤按住'3'並雙擊滑鼠左鍵:圖片放大(此程式僅提供一倍放大，放大後請勿再放大)\n5.鍵盤按住'3'並雙擊滑鼠右鍵:圖片縮小(此程式僅提供一倍縮小，所以請在放大模式時使用)\n6.滑鼠點擊右上方關閉:儲存檔案並關閉\n-----------------------\nb.鍵盤部分:\n1.數字鍵'1':縮小畫筆/橡皮擦範圍\n2.數字鍵'2':放大畫筆/橡皮擦範圍\n3.數字鍵'3':長按並配合滑鼠雙擊有放大縮小功能\n4.鍵盤'w'鍵:放大時可將視窗上移\n5.鍵盤's'鍵:放大時可將視窗下移\n6.鍵盤'a'鍵:放大時可將視窗左移\n7.鍵盤'd'鍵:放大時可將視窗右移\n-----------------------\nc.您可能會遇到的問題:\nQ1.按了'3'也雙擊了卻沒放大/縮小:\nA1:請將您的輸入法改成英文，並且'長'按'3'", "manual");
}
private: System::Void progressBar1_Click(System::Object^  sender, System::EventArgs^  e) {
}
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			 //-----
			 for (int h = 0; h < img.rows; h++)
			 for (int w = 0; w < img.cols; w++)
			 if (protectRegion.at<uchar>(h, w) != 0)
				 protectLabel[labelImage.at<int>(h, w)] = true;

			 for (int h = 0; h < img.rows; h++)
			 for (int w = 0; w < img.cols; w++) {
				 label = labelImage.at<int>(h, w);

				 if (stats.at<int>(label, CC_STAT_AREA) < gRemoveSize && !protectLabel[label])
					 original.at<uchar>(h, w) = 255;
			 } // for
			 //----

			 imwrite("Protect_Out.png", protectRegion);
			 imwrite("Final_Output.png", original);
			 MessageBox::Show("Image archives successful!!", "Message");
}
};
}
