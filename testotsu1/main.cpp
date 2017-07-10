#include <iostream> 
#include <math.h> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>  
using namespace cv;   
using namespace std;  


/***
* 大津算法是一种图像灰度自适应的阈值分割算法，是1979年由日本学者大津提出，并由他的名字命名的。
* 大津算法按照图像上灰度值的分布，将图像分成背景和前景两部分看待，前景就是我们要按照阈值分割出来的部分。
* 背景和前景的分界值就是我们要求出的阈值。遍历不同的阈值，计算不同阈值下对应的背景和前景之间的类内方差，
* 当类内方差取得极大值时，此时对应的阈值就是大津法（OTSU算法）所求的阈值。
*/


//***************Otsu算法通过求类间方差极大值求自适应阈值******************  
int otsuAlgThreshold(const Mat image);  


int main() 
{ 
    Mat image=imread("girl.jpg");//读入一张图片，Mat是一种基本图像容器
    imshow("原图",image);   //在窗口中显示图像
 
    //判断图像是否加载成功
    if(!image.data)
    {
        cout << "图像加载失败!" << endl;
        return false;
    }
    else
        cout << "图像加载成功!" << endl;

	 Mat grayImage;
	//将图像转换为灰度图，采用CV_前缀
    cvtColor(image,grayImage, CV_BGR2GRAY);
    imshow("灰度图",grayImage);   //窗口中显示图像
 
    Mat imageoutput;  
    Mat imageotsu;    

	//通过求类间方差极大值求自适应阈值
    int thresholdvalue=otsuAlgThreshold(grayImage);  

    cout << "类间方差为： " << thresholdvalue << endl;  

/*
 *********************************************************************************************************
cvThreshold()函数原型：
	
	void cvThreshold( const CvArr* src, CvArr* dst, double threshold, double maxval, int threshold_type );

参数信息：

第一个参数，InputArray类型的src，输入数组，填单通道 , 8或32位浮点类型的Mat即可。

第二个参数，OutputArray类型的dst，函数调用后的运算结果存在这里，即这个参数用于存放输出结果，且和第一个参数中的Mat变量有一样的尺寸和类型。

第三个参数，double类型的thresh，阈值的具体值。

第四个参数，double类型的maxval，当第五个参数阈值类型type取 THRESH_BINARY 或THRESH_BINARY_INV阈值类型时的最大值.

第五个参数，int类型的threshold_type，阈值类型,这个参数有以下几种类型：

0: CV_THRESH_BINARY  当前点值大于阈值时，取Maxval,也就是第四个参数，下面再不说明，否则设置为0;

1: CV_THRESH_BINARY_INV 当前点值大于阈值时，设置为0，否则设置为Maxval;

2: CV_THRESH_TRUNC 当前点值大于阈值时，设置为阈值，否则不改变;

3: CV_THRESH_TOZERO 当前点值大于阈值时，不改变，否则设置为0;

4: CV_THRESH_TOZERO_INV  当前点值大于阈值时，设置为0，否则不改变。

值得一说的是threshold_type可以使用CV_THRESH_OTSU类型，这样该函数就会使用大律法OTSU得到的全局自适应阈值来进行二值化图片，而参数中的threshold不再起作用。
比如：cvThreshold(src,dst,300,255,CV_THRESH_OTSU | CV_THRESH_BINARY_INV);这种方法对于灰度直方图呈现二峰特征的图片处理起来效果很好。

 *********************************************************************************************************
 */

	threshold(grayImage,imageoutput,thresholdvalue,255,CV_THRESH_BINARY); //自己写的函数
	threshold(grayImage,imageotsu,0,255,CV_THRESH_OTSU); //opencv otsu算法  

	imshow("自己实现的otsu",imageoutput);  
	imshow("opencv的otsu",imageotsu);   

    waitKey();  
    return 0;  

}
  
int otsuAlgThreshold(const Mat grayImage)  
{  
	if(grayImage.channels()!=1)  
	{  
		cout<<"Please input Gray-image!"<<endl;  
		return 0;  
	}  
		
	double Histogram[256] = {0}; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数  
	int Histogram1[256] = {0};

	int T=0; //Otsu算法阈值
	double varValue=0; //类间方差中间值保存 

	double foregroundPixelsSum = 0; //前景像素点数 
	double backgroundPixelsSum = 0; //背景像素点数 

	double foregroundProportion = 0; //前景像素点数所占比例  
	double backgroundProportion = 0; //背景像素点数所占比例

	double foregroundGreylevelSum = 0; //前景灰度总和  
	double backgroundGreylevelSum = 0; //背景灰度总和  

	double foregroundGreylevelAvg = 0; //前景平均灰度  
	double backgroundGreylevelAvg = 0; //背景平均灰度  

	uchar *data = grayImage.data; //uchar型的指针。Mat类分为了两个部分:矩阵头和指向矩阵数据部分的指针，data就是指向矩阵数据的指针。
	//Mat的存储形式和Matlab里的数组格式有点像，但一般是二维向量，如果是灰度图，一般存放<uchar>类型；如果是RGB彩色图，存放<Vec3b>类型。
	//cout<<"grayImage of data: " << grayImage.data << endl;

	double pixelSum = grayImage.rows * grayImage.cols; //像素总数 

	cout<<"grayImage of rows: " << grayImage.rows << endl;  
	cout<<"grayImage of cols: " << grayImage.cols << endl;  

	//计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数 (Histogram means 直方图) 
	for(int i=0; i<grayImage.rows; i++)   //为表述清晰，并没有把rows和cols单独提出来  
	{  
		for(int j=0; j<grayImage.cols; j++)  
		{  
			Histogram[ data[i * grayImage.step + j] ]++;  
			Histogram1[ data[i * grayImage.step + j] ]++;  
		}  
	}  
	cout << "channel:" << grayImage.channels() << endl;//channels():图像的通道数，当图像时由BGR构成时通道数为3，灰度图像的通道数为1.
	cout << "dims:" << grayImage.dims << endl;//dims: Mat矩阵的维度。
	cout << "step[0]:" << grayImage.step[0] << endl;
	cout << "step[1]:" << grayImage.step[1] << endl;

	//***********画出图像直方图********************************  
	Mat image1(255, 255, CV_8UC3); //Mat的构造函数。这是一张256*256的彩图。

	for(int i=0; i<255; i++)  
	{  
		Histogram1[i] = Histogram1[i] % 200;  
		line(image1, Point(i, 235), Point(i, 235-Histogram1[i]), Scalar(255, 0, 0), 1, 8, 0); 
		/*
		line()函数：
		第一个参数img：要划的线所在的图像;
		第二个参数pt1：直线起点;
		第二个参数pt2：直线终点;
		第三个参数color：直线的颜色 e.g:Scalor(0,0,255);
		第四个参数thickness=1：线条粗细;
		第五个参数line_type=8,8 (or 0) - 8-connected line（8邻接)连接线,4 - 4-connected line(4邻接)连接线。CV_AA - antialiased 线条。
		第六个参数：坐标点的小数点位数。
		*/

		//给图像x轴添加数字标注：0，50，100，150...
		if(i % 50 == 0)  
		{  
			char ch[255];  
			sprintf(ch, "%d", i);  
			string str = ch;  
			putText(image1, str, Point(i, 250), 1, 1, Scalar(0, 0, 255));  
		}  
	}  
	//***********画出图像直方图********************************  
  
	for(int i=0; i<=256; i++)  
	{  
		//每次遍历之前初始化各变量  
		backgroundPixelsSum = 0;  		      
		foregroundPixelsSum = 0;
		backgroundGreylevelSum = 0; 
		foregroundGreylevelSum = 0; 

		//***********背景各分量值计算**************************  
		for(int j=0;j<=i;j++) //背景部分各值计算  
		{  
			backgroundPixelsSum += Histogram[j];  //背景部分像素点总数  
			backgroundGreylevelSum += j * Histogram[j]; //背景部分像素总灰度和  
		}  
		if(backgroundPixelsSum == 0) //背景部分像素点数为0时退出  
		{  
			break;  
		}  

		backgroundGreylevelAvg = backgroundGreylevelSum / backgroundPixelsSum; //背景像素平均灰度  
		backgroundProportion = backgroundPixelsSum / pixelSum; // 背景部分像素点数所占比例 
		//***********背景各分量值计算**************************  

  
		//***********前景各分量值计算**************************  
		for(int k=i+1; k<256; k++)  
		{  
			foregroundPixelsSum += Histogram[k];  //前景部分像素点总数  
			foregroundGreylevelSum += k * Histogram[k]; //前景部分像素总灰度和  
		} 

		if(foregroundPixelsSum == 0) //前景部分像素点数为0时退出  
		{  
			break;  
		}  

		foregroundGreylevelAvg = foregroundGreylevelSum / foregroundPixelsSum; //前景像素平均灰度  
		foregroundProportion = foregroundPixelsSum / pixelSum; // 前景部分像素点数所占比例
		//***********前景各分量值计算**************************  
  

		//***********类间方差计算******************************  
		double varValueI = foregroundProportion * backgroundProportion * pow((backgroundGreylevelAvg - foregroundGreylevelAvg),2); //当前类间方差计算  
		
		//采用遍历的方法得到使类间方差g最大的阈值T,即为所求。
		if(varValue < varValueI)  
		{  
			varValue = varValueI;  
			T=i;  
		}  
	}  
	//画出以T为阈值的分割线  
	line(image1, Point(T,235), Point(T,0), Scalar(0, 0, 255), 2, 8);  
	imshow("直方图", image1); 

	return T;  
}  
