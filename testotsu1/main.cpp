#include <iostream> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>  
using namespace cv;   
using namespace std;  

//***************Otsu算法通过求类间方差极大值求自适应阈值******************  
int otsuAlgThreshold(const Mat image);  


int main() 
{ 
   Mat image=imread("girl.jpg");//读入一张图片
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

	int T=0; //Otsu算法阈值  

	double varValue=0; //类间方差中间值保存  

	double foregroundPixelsProportion = 0; //前景像素点数所占比例  
	double backgroundPixelsProportion = 0; //背景像素点数所占比例  
	double foregroundAvgGreylevel = 0; //前景平均灰度  
	double backgroundAvgGreylevel = 0; //背景平均灰度  

	double Histogram[256] = {0}; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数  
	int Histogram1[256] = {0};   

	uchar *data = grayImage.data;  

	double totalNum = grayImage.rows * grayImage.cols; //像素总数  

	//计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数  
	for(int i=0; i<grayImage.rows; i++)   //为表述清晰，并没有把rows和cols单独提出来  
	{  
		for(int j=0; j<grayImage.cols; j++)  
		{  
			Histogram[data[i*grayImage.step+j]]++;  
			Histogram1[data[i*grayImage.step+j]]++;  
		}  
	}  
  
	//***********画出图像直方图********************************  
	Mat image1(255, 255, CV_8UC3); 

	for(int i=0; i<255; i++)  
	{  
		Histogram1[i] = Histogram1[i] % 200;  
		line(image1, Point(i, 235), Point(i, 235-Histogram1[i]), Scalar(255, 0, 0), 1, 8, 0);   

		if(i % 50 == 0)  
		{  
			char ch[255];  
			sprintf(ch, "%d", i);  
			string str = ch;  
			putText(image1, str, Point(i, 250), 1, 1, Scalar(0, 0, 255));  
		}  
	}  
	//***********画出图像直方图********************************  
  
	for(int i=0; i<255; i++)  
	{  
		//每次遍历之前初始化各变量  
		backgroundPixelsProportion = 0;       
		backgroundAvgGreylevel = 0;       
		foregroundPixelsProportion = 0;       
		foregroundAvgGreylevel = 0; 

		//***********背景各分量值计算**************************  
		for(int j=0;j<=i;j++) //背景部分各值计算  
		{  
			backgroundPixelsProportion += Histogram[j];  //背景部分像素点总数  
			backgroundAvgGreylevel += j * Histogram[j]; //背景部分像素总灰度和  
		}  
		if(backgroundPixelsProportion == 0) //背景部分像素点数为0时退出  
		{  
			break;  
		}  
		backgroundAvgGreylevel = backgroundAvgGreylevel / backgroundPixelsProportion; //背景像素平均灰度  
		backgroundPixelsProportion = backgroundPixelsProportion / totalNum; // 背景部分像素点数所占比例 
		//***********背景各分量值计算**************************  

  
		//***********前景各分量值计算**************************  
		for(int k=i+1;k<255;k++)  
		{  
			foregroundPixelsProportion += Histogram[k];  //前景部分像素点总数  
			foregroundAvgGreylevel += k * Histogram[k]; //前景部分像素总灰度和  
		}  
		if(foregroundPixelsProportion == 0) //前景部分像素点数为0时退出  
		{  
			break;  
		}  

		foregroundAvgGreylevel = foregroundAvgGreylevel / foregroundPixelsProportion; //前景像素平均灰度  
		foregroundPixelsProportion = foregroundPixelsProportion / totalNum; // 前景部分像素点数所占比例  
		//***********前景各分量值计算**************************  
  

		//***********类间方差计算******************************  
		double varValueI = foregroundPixelsProportion * backgroundPixelsProportion * (backgroundAvgGreylevel - foregroundAvgGreylevel) * (backgroundAvgGreylevel - foregroundAvgGreylevel); //当前类间方差计算  
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
