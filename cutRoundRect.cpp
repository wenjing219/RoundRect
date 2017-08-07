#include <cv.h>
#include <highgui.h>
//#include <math.h>

using namespace cv;
using namespace std;


int getMask(Mat &mask, int h, int w, int r, int iB, int iG, int iR);

int main(int argc, const char** argv)
{
	/*
	load image and set parameters
	radius������Բ�ǰ뾶�����������Ĭ��Ϊ(min(height,width)/4)
	isPNG���Ƿ���͸��Բ�ǣ������������Ĭ����͸��Բ��
	h_bigMask & w_bigMask�����ñ�ԭͼ�������Ҹ���3���ص�ģ�档��Ϊ��Բ��������Ҫ�����ģ�棬�����޷���90�����ڵ�����
	*/
	string img_path = "img_274.jpg";
	//string img_path = argv[1];
	cv::Mat src = imread(img_path);

	int channels = src.channels();
	if (channels < 3)
	{
		cout << "img channels error!" << endl;
		return -1;
	}

	int h_src = src.rows;
	int w_src = src.cols;
	int h_bigMask = h_src + 6;
	int w_bigMask = w_src + 6;
	bool isPNG = false;
	//bool isPNG = true;
	int iR = 255;
	int iG = 255;
	int iB = 255;

	int radius = 0;
	if (radius == 0)
	{
		radius = h_src < w_src/6 ? h_src : w_src/6;
	}

	cout << "oriSize: " << h_src << "x" << w_src << "|radius:" << radius << endl;

	//target image
	/*
	����isPNG������ͬ������
	*/

	cv::Mat roundCornerImg;
	if (isPNG)
	{
		cv::Mat pngdst(h_src, w_src, CV_8UC4);
		//set alpha mask, 3 pixel bigger than src
		Mat alpha_Mask(h_bigMask, w_bigMask, CV_8UC1,Scalar::all(255));//one channel, bigger than src
		if(getMask(alpha_Mask, h_bigMask, w_bigMask, radius, iB, iG, iR)==-1)
			return -1;
		//cout << "set alpha mask done" << endl;
		//cut mask to src size
		alpha_Mask = alpha_Mask(Range(3, h_src + 3), Range(3, 3 + w_src));
		//cout << "cut alpha mask done" << endl;
		//cout << "mask size :" << alpha_Mask.rows << " x " << alpha_Mask.cols << endl;

		//imshow("final mask", alpha_Mask);
		//cv::waitKey();

		//combine channels
		cv::Mat srcImg[] = { src, alpha_Mask };

		int from_to[] = { 0,0, 1,1, 2,2, 3,3 };
		//cout << "start mixChannels ..." << endl;
		//cv::mixChannels(srcImg, 2, &roundCornerPNG, 1, from_to, 4);
		cv::mixChannels(srcImg, 2, &pngdst, 1, from_to, 4);
		//cout << "done with mix " << endl;
		
		roundCornerImg = pngdst;
		//show and save
		//imshow("result", roundCornerImg);
		//waitKey();
		//cv::imwrite("test001.png", roundCornerPNG);
		cv::imwrite("test001.png", roundCornerImg);
	}
	else
	{
		copyMakeBorder(src, roundCornerImg,3,3,3,3,(0,0,0));
		//imshow("copyMakeBorder", src);
		//cout << "before getting" << endl;
		getMask(roundCornerImg, h_bigMask, w_bigMask, radius,  iB, iG, iR);
		//imshow("getMask", roundCornerImg);
		//cout << "after getting" << endl;
		cv::imwrite("test001before.jpg", roundCornerImg);
		roundCornerImg = roundCornerImg(Range(3, h_src + 3), Range(3, 3 + w_src));
		imshow("getMask", roundCornerImg);
		cv::waitKey();
		cv::imwrite("test001.jpg", roundCornerImg);
	}
	

	return 0;
}


int getMask(Mat &mask, int h, int w,int r,int iB,int iG,int iR)
{
	int xleft = r + 2;
	int xright = w - r - 3;
	int ytop = r + 2;
	int ybottom = h - r - 3;
	CvPoint rightTop = { xright ,ytop };
	CvPoint rightBottom = { xright,ybottom};
	CvPoint leftTop = { xleft,ytop };
	CvPoint leftBottom = {xleft,ybottom };
	CvPoint cur;

	int distance = r*r;
	cout << " distance = "<<distance << endl;
	cout << "rightTop:" << rightTop.x << " , " << rightTop.y << endl;
	cout << "rightBottom:" << rightBottom.x << " , " << rightBottom.y << endl;
	cout << "leftTop:" << leftTop.x << " , " << leftTop.y << endl;
	cout << "leftBottom:" << leftBottom.x << " , " << leftBottom.y << endl;
	
	
	int chs = mask.channels();
	vector<Mat> channels;

	cout << "start cutting ... " << endl;
	//cut roundCorner for rightTop point
	for (cur.x = w-r; cur.x < w; cur.x++)
	{
		for (cur.y = 0; cur.y < r; cur.y++)
		{
			int cur_dist = ((cur.x - rightTop.x)*(cur.x - rightTop.x) + (cur.y - rightTop.y)*(cur.y - rightTop.y));
			if (cur_dist > distance)
			{
				if(chs==1)
					mask.at<uchar>(cur.y, cur.x) = 0;
				else
				{
					mask.at<Vec3b>(cur.y, cur.x)[0] = iB;
					mask.at<Vec3b>(cur.y, cur.x)[1] = iG;
					mask.at<Vec3b>(cur.y, cur.x)[2] = iR;
				}
			}
		}
	} 

	//cut roundCorner for rightBottom point
	
	for (cur.x = w-r; cur.x < w; cur.x++)
	{
		for (cur.y = h-r; cur.y < h; cur.y++)
		{
			int cur_dist = ((cur.x - rightBottom.x)*(cur.x - rightBottom.x) + (cur.y - rightBottom.y)*(cur.y - rightBottom.y));
			if (cur_dist > distance)
			{
				if (chs == 1)
					mask.at<uchar>(cur.y, cur.x) = 0;
				else
				{
					mask.at<Vec3b>(cur.y, cur.x)[0] = iB;
					mask.at<Vec3b>(cur.y, cur.x)[1] = iG;
					mask.at<Vec3b>(cur.y, cur.x)[2] = iR;
				}
			}
		}
	}
	//cut roundCorner for leftTop point
	for (cur.x = 0; cur.x < r; cur.x++)
	{
		for (cur.y = 0; cur.y < r; cur.y++)
		{
			int cur_dist = ((cur.x - leftTop.x)*(cur.x - leftTop.x) + (cur.y - leftTop.y)*(cur.y - leftTop.y));
			if (cur_dist > distance)
			{
				if (chs == 1)
					mask.at<uchar>(cur.y, cur.x) = 0;
				else
				{
					mask.at<Vec3b>(cur.y, cur.x)[0] = iB;
					mask.at<Vec3b>(cur.y, cur.x)[1] = iG;
					mask.at<Vec3b>(cur.y, cur.x)[2] = iR;
				}
			}
		}
	}
	//cut roundCorner for leftBottom point
	for (cur.x = 0; cur.x < r; cur.x++)
	{
		for (cur.y = h-r; cur.y < h; cur.y++)
		{
			int cur_dist = ((cur.x - leftBottom.x)*(cur.x - leftBottom.x) + (cur.y - leftBottom.y)*(cur.y - leftBottom.y));
			if (cur_dist > distance)
			{
				if (chs == 1)
					mask.at<uchar>(cur.y, cur.x) = 0;
				else
				{
					mask.at<Vec3b>(cur.y, cur.x)[0] = iB;
					mask.at<Vec3b>(cur.y, cur.x)[1] = iG;
					mask.at<Vec3b>(cur.y, cur.x)[2] = iR;
				}
			}
		}
	}
	cout << "start drawing ... " << endl;
	//draw CV_AA line
	ellipse(mask, rightTop, Size(r, r), 0, 270, 360, Scalar(iB,iG,iR), 1, CV_AA);
	ellipse(mask, rightBottom, Size(r, r), 0, 0, 90, Scalar(iB, iG, iR), 1, CV_AA);
	ellipse(mask, leftTop, Size(r, r), 0, 180, 270, Scalar(iB, iG, iR), 1, CV_AA);
	ellipse(mask, leftBottom, Size(r, r), 0, 90, 180, Scalar(iB, iG, iR), 1, CV_AA);

	cout << "done with drawing" << endl;
	return 0;
}