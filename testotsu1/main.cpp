#include <iostream> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>  
using namespace cv;   
using namespace std;  

int otsualgthreshold1(const Mat image); 
int * mark(const Mat image,int thresholdvalue);
int otsualgthreshold2(const Mat image,int value[],int thresholdvalue); 

int main() 
{ 
    Mat image=imread("image030.jpg");//读入一张图片，mat是一种基本图像容器
    imshow("原图",image);   //在窗口中显示图像
 
    //判断图像是否加载成功
    if(!image.data)
    {
        cout << "图像加载失败!" << endl;
        return false;
    }
    else
        cout << "图像加载成功!" << endl;

	Mat grayimage;//灰度图
	Mat imageoutput;   
    cvtColor(image,grayimage, CV_RGB2GRAY);//将图像转换为灰度图，采用cv_前缀
    imshow("灰度图",grayimage);   //窗口中显示图像

    int thresholdvalue1=otsualgthreshold1(grayimage); //通过大津算法求得阈值

	int thresholdvalue2=otsualgthreshold2(grayimage,mark(grayimage,thresholdvalue1),thresholdvalue1); 
	threshold(grayimage,imageoutput,thresholdvalue2,255,CV_THRESH_BINARY); //当前点值大于阈值时，取255，否则设置为0;
	imshow("otsu2",imageoutput);  

	int thresholdvalue3=otsualgthreshold2(grayimage,mark(grayimage,thresholdvalue2),thresholdvalue2); 
	threshold(grayimage,imageoutput,thresholdvalue3,255,CV_THRESH_BINARY); //当前点值大于阈值时，取255，否则设置为0;
	imshow("otsu3",imageoutput);  

	threshold(grayimage,imageoutput,(thresholdvalue2+thresholdvalue3)/2,255,CV_THRESH_BINARY); //当前点值大于阈值时，取255，否则设置为0;
	imshow("otsu4",imageoutput);  

    waitKey();  
    return 0;  
}
  
int otsualgthreshold1(const Mat grayimage)  
{  
	double grayvalue[256] = {0}; //下标是灰度值，保存内容是灰度值对应的像素点总数  

	int t=0; //otsu算法阈值
	double gtemp=0; //类间方差中间值 

	double fn = 0; //前景像素点数 
	double bn = 0; //背景像素点数 

	double fp = 0; //前景像素点数所占比例  
	double bp = 0; //背景像素点数所占比例

	double ft = 0; //前景灰度总和  
	double bt = 0; //背景灰度总和  

	double fa = 0; //前景平均灰度  
	double ba = 0; //背景平均灰度  

	uchar *data = grayimage.data; //指向图像矩阵数据的指针
	
	double pixelsum = grayimage.rows * grayimage.cols; //像素总数 
 
	for(int i=0; i<grayimage.rows; i++) 
	{  
		for(int j=0; j<grayimage.cols; j++)  
			grayvalue[data[i * grayimage.step + j]]++; 			
	}  

	for(int i=0; i<256; i++)  
	{  
		//每次遍历之前初始化各变量  
		bt = 0;  		      
		ft = 0;
		bn = 0; 
		fn = 0; 

		for(int j=0;j<=i;j++) //背景部分各值计算  
		{  
			bn += grayvalue[j];  //背景部分像素点总数  
			bt += j * grayvalue[j]; //背景部分像素总灰度和  
		}  
				
		ba = bt/bn; //背景像素平均灰度  
		bp = bn/pixelsum; // 背景部分像素点数所占比例 

		for(int k=i+1; k<256; k++)  
		{  
			fn += grayvalue[k];  //前景部分像素点总数  
			ft += k * grayvalue[k]; //前景部分像素总灰度和  
		} 

		fa = ft / fn; //前景像素平均灰度  
		fp = fn / pixelsum; // 前景部分像素点数所占比例

		double g = fp * bp * (fa - ba)*(fa - ba); //当前类间方差计算  
	
		//采用遍历的方法得到使类间方差g最大的阈值t,即为所求。
		if(gtemp < g)  
		{  
			gtemp = g;  
			t=i;  
		}  
	}
	cout<<"t1: "<<t<<endl;  
	return t;  
} 


int otsualgthreshold2(const Mat grayimage, int value[], int thresholdvalue)  
{  
	double grayvalue[256] = {0}; //下标是灰度值，保存内容是灰度值对应的像素点总数  

	int t = 0; //otsu算法阈值
	double gtemp = 0; //类间方差中间值

	double fn = 0; //前景像素点数 
	double bn = 0; //背景像素点数 

	double fp = 0; //前景像素点数所占比例  
	double bp = 0; //背景像素点数所占比例

	double ft = 0; //前景灰度总和  
	double bt = 0; //背景灰度总和  

	double fa = 0; //前景平均灰度  
	double ba = 0; //背景平均灰度  

	uchar *data = grayimage.data; 
	int pixelsum=0; //像素总数 

	for(int i=0; i<grayimage.rows; i++)   
	{  
		for(int j=0; j<grayimage.cols; j++)  
		{  
			if(value[i*grayimage.step+j]==1)
			{
				grayvalue[data[i * grayimage.step + j]]++;
				pixelsum++;
			}	
		}  
	}  

	for(int i=thresholdvalue; i<256; i++)  
	{  
		//每次遍历之前初始化各变量  
		bt = 0;  		      
		ft = 0;
		bn = 0; 
		fn = 0; 

		for(int j=thresholdvalue;j<=i;j++) //背景部分各值计算  
		{  
			bn += grayvalue[j];  //背景部分像素点总数  
			bt += j * grayvalue[j]; //背景部分像素总灰度和  
		}  
			
		ba = bt/bn; //背景像素平均灰度  
		bp = bn/pixelsum; //背景部分像素点数所占比例 
		
		for(int k=i+1; k<256; k++)  
		{  
			fn += grayvalue[k];  //前景部分像素点总数  
			ft += k * grayvalue[k]; //前景部分像素总灰度和  
		} 
			
		fa = ft/fn;//前景像素平均灰度  
		fp = fn/pixelsum;//前景部分像素点数所占比例
		  
		double g = fp * bp * (fa - ba)*(fa - ba); //当前类间方差计算  
		
		//采用遍历的方法得到使类间方差g最大的阈值t,即为所求。
		if(gtemp < g)  
		{  
			gtemp = g;  
			t=i;  
		}  
	}
	cout<<"t2: "<<t<<endl;  
	return t;  
} 

int* mark(const Mat image,int thresholdvalue)
{  
	uchar *data = image.data;
	int value[65536] = {0};
	for(int i=0; i<=image.rows*image.cols; i++)
    {
		if(data[i]>=thresholdvalue)
		{
			value[i]=1;			
		}
    }
	return value;
}