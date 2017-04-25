#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

double gRemoveSize, gBigObj;
int gProtectSize = 51;
int paint_size = 100;

int DUBBLECLICK =0;

string imgName = "a5.png";
string temp = imgName;
Mat original;
Mat RedPic;
Mat img;
Mat protectRegion;

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
int modeDC = 0;
const int UPSAMPLERATE = 4;
int ROIxBEGIN, ROIxEND, ROIyBEGIN, ROIyEND;
//int ROIxBEGINforMOVE;//放大時移動圖片用
//int ROIyBEGINforMOVE;

void mymouse(int event, int x, int y, int flag, void* param)
{
	static int oldx, oldy, DownX, DownY, oldDownX, oldDownY,now_down = 0, eraserDown;
	int XupsampleBEGIN = x - 50, YupsampleBEGIN = y - 50, XupsampleEND = x+50,YupsampleEND=y+50;

	//左鍵雙擊放大部分
	if (event == CV_EVENT_LBUTTONDBLCLK && DUBBLECLICK==1){
		
		modeDC = 1;
		ROIxBEGIN = x * 4 - RedPicCPY_forUPSAMPLE.cols / (UPSAMPLERATE*2);
		ROIxEND = x * 4 + RedPicCPY_forUPSAMPLE.cols / (UPSAMPLERATE * 2);
		ROIyBEGIN = y * 4 - RedPicCPY_forUPSAMPLE.rows / (UPSAMPLERATE * 2);
		ROIyEND = y * 4 + RedPicCPY_forUPSAMPLE.rows / (UPSAMPLERATE * 2);


		if (ROIxBEGIN < 0) ROIxBEGIN = 0;
		else if (ROIxEND > RedPicCPY_forUPSAMPLE.cols)ROIxBEGIN = RedPicCPY_forUPSAMPLE.cols-RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1;

		if (ROIyBEGIN< 0) ROIyBEGIN = 0;
		else if (ROIyEND > RedPicCPY_forUPSAMPLE.rows - 1)ROIyBEGIN = RedPicCPY_forUPSAMPLE.rows - RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1;

		ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
		DUBBLECLICK = 0;
		imshow("RedPic", ROI);
	}
	//右鍵雙擊縮小部分
	else if(event == CV_EVENT_RBUTTONDBLCLK&&DUBBLECLICK == 1){
		modeDC = 0;
		DUBBLECLICK = 0;
	}

	//左鍵畫線部分
	else if(event == CV_EVENT_LBUTTONDOWN){
		//若在一般模式下
		if (modeDC == 0 && DUBBLECLICK == 0){
			circle(RedPic, cvPoint(x, y), paint_size/2 , cvScalar(0, 0, 255));
			circle(RedPicCPY_forUPSAMPLE, cvPoint(x*4, y*4), paint_size * 2, cvScalar(0, 0, 255));
			oldx = x; oldy = y;
			now_down = 1;
		}

		//若在放大模式下
		else if (modeDC == 1){
			DownX = ROIxBEGIN + x;
			DownY = ROIyBEGIN + y;
			circle(ROI, cvPoint(x, y), paint_size*2 , cvScalar(0, 0, 255));
			circle(RedPic, cvPoint(DownX/4, DownY/4), paint_size/2, cvScalar(0, 0, 255));
			oldx = x; oldy = y;
			oldDownX = DownX; oldDownY = DownY;
			now_down = 1;
		}
	}
	else if(event == CV_EVENT_LBUTTONUP){
		now_down = 0;
	}
	else if(event == CV_EVENT_MOUSEMOVE && now_down == 1){
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
				if ((int)protectRegion.at<uchar>(i*4, j*4) == 255){
					RedPic.at<Vec3b>(i, j)[0] = 255;
					RedPic.at<Vec3b>(i, j)[1] = 255;
					RedPic.at<Vec3b>(i, j)[2] = 255;
				}
				if ((int)original.at<uchar>(i*4, j*4) == 0 && (int)protectRegion.at<uchar>(i*4, j*4) == 255){
					RedPic.at<Vec3b>(i, j)[0] = 0;
					RedPic.at<Vec3b>(i, j)[1] = 0;
					RedPic.at<Vec3b>(i, j)[2] = 0;
				}
				if ((int)original.at<uchar>(i*4, j*4) == 0 && (int)protectRegion.at<uchar>(i*4, j*4) != 255){
					RedPic.at<Vec3b>(i, j)[0] = 40;
					RedPic.at<Vec3b>(i, j)[1] = 40;
					RedPic.at<Vec3b>(i, j)[2] = 255;
				}
			}//for
			//line(RedPicCPY_forUPSAMPLE, cvPoint(x * 4, y * 4), cvPoint(oldx * 4, oldy * 4), cvScalar(0, 255, 0), paint_size * 4, 8, 0);

		

			for (int i = YupsampleBEGIN * 4; i < YupsampleEND * 4; i++)
			for (int j = XupsampleBEGIN * 4; j < XupsampleEND * 4; j++){
				if ((int)protectRegion.at<uchar>(i , j ) == 255){
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j)[0] = 255;
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i , j)[1] = 255;
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i , j)[2] = 255;
				}
				if ((int)original.at<uchar>(i , j ) == 0 && (int)protectRegion.at<uchar>(i , j ) == 255){
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i , j)[0] = 0;
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i , j)[1] = 0;
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i , j)[2] = 0;
				}
				if ((int)original.at<uchar>(i , j ) == 0 && (int)protectRegion.at<uchar>(i , j ) != 255){
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i , j)[0] = 40;
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i , j)[1] = 40;
					RedPicCPY_forUPSAMPLE.at<Vec3b>(i , j)[2] = 255;
				}
			}//for
			oldx = x; oldy = y;
			
		}
		//若在放大模式下
		if (modeDC == 1){
			DownX = ROIxBEGIN + x;
			DownY = ROIyBEGIN + y;

			if (XupsampleBEGIN < 0)XupsampleBEGIN = 0;
			else if (XupsampleEND>ROI.cols)XupsampleEND = ROI.cols;

			if (YupsampleBEGIN < 0)YupsampleBEGIN = 0;
			else if (YupsampleEND>ROI.rows)YupsampleEND = ROI.rows;

			line(protectRegion, cvPoint(DownX, DownY), cvPoint(oldDownX, oldDownY), cvScalar(255), paint_size, 8, 0);
			//line(ROI, cvPoint(x, y), cvPoint(oldx, oldy), cvScalar(0, 255, 0), paint_size*2, 8, 0);
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
			//line(RedPic, cvPoint(DownX / 4, DownY / 4), cvPoint(oldDownX / 4, oldDownY/4), cvScalar(0, 255, 0), paint_size/2, 8, 0);
			for (int i = YupsampleBEGIN; i < YupsampleEND; i++)
			for (int j = XupsampleBEGIN; j <XupsampleEND; j++){
				if ((int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 255){
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[0] = 255;
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[1] = 255;
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[2] = 255;
				}
				if ((int)original.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 0 && (int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 255){
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[0] = 0;
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[1] = 0;
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[2] = 0;
				}
				if ((int)original.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) == 0 && (int)protectRegion.at<uchar>(i + ROIyBEGIN, j + ROIxBEGIN) != 255){
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[0] = 40;
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[1] = 40;
					RedPic.at<Vec3b>((i + ROIyBEGIN) / UPSAMPLERATE, (j + ROIxBEGIN) / UPSAMPLERATE)[2] = 255;
				}
			}//for
			oldx = x; oldy = y;
			oldDownX = DownX; oldDownY = DownY;

		}
	}

	//右鍵框選部分
	else if (event == CV_EVENT_RBUTTONDOWN){
		//若在一般模式下
		if (DUBBLECLICK == 0 && modeDC == 0){
			oldx = x; oldy = y;
		}
		//若在放大模式下
		else if (modeDC == 1){
			DownX = ROIxBEGIN + x;
			DownY = ROIyBEGIN + y;
			oldDownX = DownX; oldDownY = DownY;
			oldx = x; oldy = y;
		}

	}
	else if (event == CV_EVENT_RBUTTONUP){
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
			DownX = ROIxBEGIN + x;
			DownY = ROIyBEGIN + y;

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
		if (DUBBLECLICK == 0 && modeDC == 0){
			oldx = x; oldy = y;
			
		}
		//若在放大模式下
		else if (modeDC == 1){
			DownX = ROIxBEGIN + x;
			DownY = ROIyBEGIN + y;
			oldDownX = DownX; oldDownY = DownY;
			oldx = x; oldy = y;
		}
	}
	else if (event == CV_EVENT_MBUTTONUP&&DUBBLECLICK == 0){
		eraserDown = 0;
		//把binaryMap歸零
		Mat TempForBinaryMap(RedPic.rows, RedPic.cols, CV_8UC1, Scalar(0));
		Mat TempForBinaryMap_protectVER(original.rows, original.cols, CV_8UC1, Scalar(0));
		Mat TempForBinaryMap_Upsample(original.rows, original.cols, CV_8UC1, Scalar(0));
		BinaryMapForUI = TempForBinaryMap.clone();
		BinaryMapForProtectMap = TempForBinaryMap_protectVER.clone();
		BinaryMapForUPsample = TempForBinaryMap_Upsample.clone();
	}
	else if (event == CV_EVENT_MOUSEMOVE && eraserDown == 1){
		//若在一般模式下
		if (DUBBLECLICK == 0 && modeDC == 0){
			line(BinaryMapForUI, cvPoint(x, y), cvPoint(oldx, oldy), cvScalar(1), paint_size, 8, 0);
			line(BinaryMapForProtectMap, cvPoint(x * 4, y * 4), cvPoint(oldx * 4, oldy * 4), cvScalar(1), paint_size * 4, 8, 0);
			line(BinaryMapForUPsample, cvPoint(x * 4, y * 4), cvPoint(oldx * 4, oldy * 4), cvScalar(1), paint_size * 4, 8, 0);
			oldx = x; oldy = y;
		}

		//若在放大模式下
		if (modeDC == 1){
			DownX = ROIxBEGIN + x;
			DownY = ROIyBEGIN + y;

			line(BinaryMapForUPsample, cvPoint(DownX, DownY), cvPoint(oldDownX, oldDownY), cvScalar(1), paint_size * 2, 8, 0);
			line(BinaryMapForUI, cvPoint(DownX / 4, DownY / 4), cvPoint(oldDownX / 4, oldDownY / 4), cvScalar(1), paint_size/2, 8, 0);
			line(BinaryMapForProtectMap, cvPoint(DownX, DownY), cvPoint(oldDownX, oldDownY), cvScalar(1), paint_size * 2, 8, 0);

			oldDownX = DownX; oldDownY = DownY;
			oldx = x; oldy = y;
		}


		for (int i = 0; i < BinaryMapForProtectMap.rows; i++)
		for (int j = 0; j < BinaryMapForProtectMap.cols; j++){
			if ((int)BinaryMapForProtectMap.at<uchar>(i, j) == 1)//保護區圖的橡皮擦回復
				protectRegion.at<uchar>(i, j) = ProtectRegionCPY.at<uchar>(i, j);

			if ((int)BinaryMapForUI.at<uchar>(i / 4, j / 4) == 1)//介面區圖的橡皮擦回復
				RedPic.at<Vec3b>(i / 4, j / 4) = RedPicCPY.at<Vec3b>(i / 4, j / 4);

			if ((int)BinaryMapForUPsample.at<uchar>(i, j) == 1){//放大區圖的橡皮擦回復
				RedPicCPY_forUPSAMPLE.at<Vec3b>(i, j) = RedPicCPY_forUPSAMPLEeraser.at<Vec3b>(i, j);
			}
		}
	}



	if (modeDC == 1){
		DUBBLECLICK = 0;
		imshow("RedPic", ROI);
	}
	else{
		DUBBLECLICK = 0;
		imshow("RedPic", RedPic);
	}
}

int main() {
	original = imread(imgName, 0);
	//RedPic = new cv::Mat(original.rows, original.cols, CV_8UC3, Scalar(255, 255, 255));
	RedPic = cv::Mat(original.rows, original.cols, CV_8UC3, Scalar(125, 125, 125));

	img = imread(imgName, 0);
	protectRegion = ~img.clone();
	imwrite("protectregion_original.png", protectRegion);

	// MessageBox::Show("aaa");

	img = ~img;//黑變白
	gRemoveSize = ((double)80 / (6155 * 6435)) * (img.rows * img.cols);
	gBigObj = ((double)500000 / (6155 * 6435)) * (img.rows * img.cols);


	Mat labelImage, stats, centroids;
	int nLabels = connectedComponentsWithStats(img, labelImage, stats, centroids, 8, CV_32S);


	//begin for copy
	int label;
	for (int h = 0; h < img.rows; h++)
	for (int w = 0; w < img.cols; w++) {
		label = labelImage.at<int>(h, w);

		//if (stats.at<int>(label, CC_STAT_AREA) < gRemoveSize || stats.at<int>(label, CC_STAT_AREA) > gBigObj)
		if (stats.at<int>(label, CC_STAT_AREA) < gRemoveSize)
			protectRegion.at<uchar>(h, w) = 0;

	} // for



	Mat kernel = getStructuringElement(MORPH_ELLIPSE, cv::Size(gProtectSize, gProtectSize));

	dilate(protectRegion, protectRegion, kernel, cv::Point(-1, -1), 1);


	bool * protectLabel = new bool[nLabels];
	for (int i = 0; i < nLabels; i++) protectLabel[i] = false;

	//------
	for (int h = 0; h < img.rows; h++)
	for (int w = 0; w < img.cols; w++)
	if (protectRegion.at<uchar>(h, w) != 0)
		protectLabel[labelImage.at<int>(h, w)] = true;


	for (int h = 0; h < img.rows; h++)
	for (int w = 0; w < img.cols; w++) {
		label = labelImage.at<int>(h, w);

		if (stats.at<int>(label, CC_STAT_AREA) < gRemoveSize && !protectLabel[label])
			img.at<uchar>(h, w) = 0;
	} // for
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



	namedWindow("RedPic", 0);//參數0代表以可包含全部圖片的視窗大小開啟
	imshow("RedPic", RedPic);

	imwrite("Protect_Out.png", protectRegion);
	imwrite("Final_Output.png", original);
	
	cvSetMouseCallback("RedPic", mymouse);


	int key = 0;
	while (1) {
		if (modeDC == 1){//若在放大模式下
			if (key == 'w' || key == 'W'){//上
				ROIyBEGIN = ROIyBEGIN - 500;
				if (ROIyBEGIN < 0)ROIyBEGIN = 0;
				ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
				imshow("RedPic", ROI);
			}
			if (key == 's' || key == 'S'){//下
				ROIyBEGIN = ROIyBEGIN + 500;
				if (ROIyBEGIN >original.rows - ROI.rows - 1)ROIyBEGIN = original.rows - ROI.rows - 1;
				ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
				imshow("RedPic", ROI);
			}
			if (key == 'A' || key == 'a'){//左
				ROIxBEGIN = ROIxBEGIN - 500;
				if (ROIxBEGIN < 0)ROIxBEGIN = 0;
				ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
				imshow("RedPic", ROI);
			}
			if (key == 'D' || key == 'd'){//右
				ROIxBEGIN = ROIxBEGIN + 500;
				if (ROIxBEGIN >original.cols - ROI.cols - 1)ROIxBEGIN = original.cols - ROI.cols - 1;
				ROI = RedPicCPY_forUPSAMPLE(Rect(ROIxBEGIN, ROIyBEGIN, RedPicCPY_forUPSAMPLE.cols / UPSAMPLERATE - 1, RedPicCPY_forUPSAMPLE.rows / UPSAMPLERATE - 1));
				imshow("RedPic", ROI);
			}
		}
		if (key != 27){
			key = waitKey(5000);
			if (key == '3') DUBBLECLICK = 1;
		}
		else{
			break;
		}
	}//

	cvWaitKey(0);

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
	 system("pause");
	return 0;
	
} // main()