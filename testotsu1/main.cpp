#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>    
#include <opencv2/core/core.hpp>   
#include <iostream> 

using namespace cv;  
using namespace std;

int OSTU_Alg_Threshold(const Mat image,int value1,int value2);//对图片image中灰度值在value1-value2之间的像素点进行求阈值运算
int Imagine_Convert(const Mat image,int value1,int value2);//对图片image中灰度值在value1-value2之间的像素点灰度置为255(白)，其他像素点均置0(黑)
Mat deleteBone(const Mat image,int threshold);//去出颅骨

int main()
{
	Mat image=imread("image030.jpg");
	if(!image.data)
	{
		cout<<"读入图片失败！"<<endl;
		return -1;
	}
	imshow("原图",image);

	Mat image_LUGU;  //定义颅骨
	Mat image_NSZ;   //定义脑实质
	Mat image_NJY;   //定义脊液

	cvtColor(image,image_LUGU,CV_RGB2GRAY);  //转换为单通道灰度图,初始化image_LUGU
	cvtColor(image,image_NSZ,CV_RGB2GRAY);  //转换为单通道灰度图,image_NSZ
	cvtColor(image,image_NJY,CV_RGB2GRAY);  //转换为单通道灰度图,image_NJY
	cvtColor(image,image,CV_RGB2GRAY);  //转换为单通道灰度图

	int thresholdValue1=OSTU_Alg_Threshold(image,0,255);  //分离出前景(脑实质+颅骨)与背景(脑脊液+图片背景)
	cout<<"阈值1(分离出前景(脑实质+颅骨)与背景(脑脊液+图片背景))为："<<thresholdValue1<<endl;

	int thresholdValue2=OSTU_Alg_Threshold(image,thresholdValue1,255);  //初次分离出颅骨
	cout<<"阈值2为："<<thresholdValue2<<endl;

	int thresholdValue3=OSTU_Alg_Threshold(image,thresholdValue2,255);  //对分离出颅骨进行一次迭代优化
	cout<<"阈值3为："<<thresholdValue3<<endl;

	int thresholdValue4=(thresholdValue2+thresholdValue3)/2;  //对两次分离颅骨的阈值取平均值
	cout<<"阈值4(分离颅骨)为："<<thresholdValue4<<endl;
	
	image=deleteBone(image,130);//去出颅骨
	imshow("去除颅骨",image);

	int thresholdValue5=OSTU_Alg_Threshold(image,0,thresholdValue1); //分离出脑脊液
	cout<<"阈值5为："<<thresholdValue5<<endl;

	int thresholdValue6=OSTU_Alg_Threshold(image,thresholdValue5,(thresholdValue5+thresholdValue1)/2); //对分离出脑脊液进行一次迭代优化
	cout<<"阈值6为："<<thresholdValue6<<endl;

	Imagine_Convert(image,thresholdValue5,thresholdValue6);//将脑脊液部分的像素点灰度置255(白)
	imshow("脑脊液",image);
	  
	Imagine_Convert(image_LUGU,thresholdValue4,255);//将颅骨部分的像素点灰度置255(白)
	imshow("颅骨",image_LUGU);	

	Imagine_Convert(image_NSZ,65,90);//将脑实质部分的像素点灰度置255(白)
	imshow("脑实质",image_NSZ);

	waitKey();
	system("pause");
	return 0;
}



int OSTU_Alg_Threshold(const Mat image,int value1,int value2)
{ 
	if(image.channels()!=1)
	{
		cout<<"please input Cray-image!"<<endl;  //不是单通道灰度图退出 
		return -1;
	}
	int HUIDU[256]={0};
	double N=0;  //像素点数
	int T=0;  //初始化阈值
	double g1=0;
	uchar*data=image.data;
	for(int i=0;i<image.rows;i++)
	{
		for(int j=0;j<image.cols;j++)
		{
			if((data[i*image.step+j]>value1)&&(data[i*image.step+j]<=value2)) 
			{
				HUIDU[data[i*image.step+j]]++;  
				N++;
			}
		}
	}
	for(int k=value1;k<=value2;k++)
	{
		int N0=0;     //前景数
		int N1=0;     //背景数
		double w0=0;  //前景比例
		double w1=0;  //背景比例
		double u0=0;  //前景平均灰度
		double u1=0;  //背景平均灰度
		double h0=0;  //前景总灰度
		double h1=0;  //背景总灰度
		for(int m=0;m<=k;m++)
		{
			N1=N1+HUIDU[m];
			h1=h1+m*HUIDU[m];
		}
		for(int n=k+1;n<=value2;n++)
		{
			N0=N0+HUIDU[n];
			h0=h0+n*HUIDU[n];
		}
		w1=N1/N;
		w0=N0/N;
		u1=h1/N1;
		u0=h0/N0;
		double g=w1*w0*(u0-u1)*(u0-u1);
		if(g>g1)
		{
		    g1=g;
			T=k;
		}
	}
	return(T);
}
int Imagine_Convert(const Mat image,int value1,int value2)
{
	uchar*data=image.data;
	for(int i=0;i<=image.rows*image.cols;i++)
	{
		if(data[i]<=value1||data[i]>value2) //对图片image中灰度值在value1-value2之间的像素点灰度置为255(白)，其他像素点均置0(黑)
		{
			data[i]=0;
		}
		else
		{ 
			data[i]=255;
		}
	}
	return 0;
}

Mat deleteBone(const Mat image,int threshold)
{
	uchar*data=image.data;
	int boneFlag[256][256]={0};

	//对图像中颅骨部分的灰度值置0，即去除颅骨，将其变成背景
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			if(data[i*image.step+j]>= threshold) //根据阈值，该灰度值属于颅骨区域
			{
				data[i*image.step+j] = 0;
				boneFlag[i][j] = 1;//颅骨标记

			}
		}		
	}

	//再次遍历图像，找到每一行中第一个属于颅骨的像素点，将该点前面的所有点置为0。
	//即将图像的左半部分颅骨及颅骨以外区域置为0
	for(int n=0;n<image.rows;n++)
	{
		for(int m=0;m<image.cols;m++)
		{
			if(data[n*image.step+m]==0 && boneFlag[n][m]==1) 
			{
				for(int k=0;k<m;k++)
					data[n*image.step+k]=0;
				break;//将第一个属于颅骨的像素点前面的所有点置为0后，跳转下一行。
			}			
		}	
	}

	
	//同理，再次遍历图像，找到每一行中最后一个属于颅骨的像素点，将该点后面的所有点置为0。
	//即将图像的右半部分颅骨及颅骨以外区域置为0
	for(int n=0;n<image.rows;n++)
	{
		for(int m=image.cols;m>=0;m--)
		{
			if(data[n*image.step+m]==0 && boneFlag[n][m]==1) 
			{
				for(int k=image.cols;k>m;k--)
					data[n*image.step+k]=0;
				break;//将最后一个属于颅骨的像素点后面的所有点置为0后，跳转下一行。
			}			
		}	
	}

	//去除颅骨上面的头皮。
	for(int n=0;n<image.rows;n++)
	{
		for(int m=image.cols;m>=0;m--)
		{
			if(boneFlag[n][m]!=1) 
				data[n*image.step+m]=0;
			else
				return image;
		}	
	}	
}