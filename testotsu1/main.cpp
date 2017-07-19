#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>    
#include <opencv2/core/core.hpp>   
#include <iostream> 

#define NULL 0

using namespace cv;  
using namespace std;

int OSTU_Alg_Threshold(const Mat image,int value1,int value2);//对图片image中灰度值在value1-value2之间的像素点进行求阈值运算
int Imagine_Convert(const Mat image,int value1,int value2);//对图片image中灰度值在value1-value2之间的像素点灰度置为255(白)，其他像素点均置0(黑)
Mat deleteOutside(const Mat image,const Mat markImage,int threshold);//去除目标区域及以外部分
int deleteNS(const Mat markImage);//对于提取出的脑脊液，除去多余的脑室部分

int main()
{
	Mat image=imread("image030.jpg");
	if(!image.data)
	{
		cout<<"读入图片失败！"<<endl;
		return -1;
	}
	imshow("原图",image);
	cvtColor(image,image,CV_RGB2GRAY);  //转换为单通道灰度图

	Mat copyLG1;
	Mat copyLG2;
	Mat copyNJY;
	image.copyTo(copyLG1);//备份原始图像，用来分割颅骨
	image.copyTo(copyLG2);//备份原始图像，用来去除颅骨
	image.copyTo(copyNJY);//备份原始图像，用来去除脑脊液

	int thresholdValue1=OSTU_Alg_Threshold(image,0,255);  //分离出前景(脑实质+颅骨)与背景(脑脊液+图片背景)
	cout<<"阈值1(分离出前景(脑实质+颅骨)与背景(脑脊液+图片背景))为："<<thresholdValue1<<endl;

	int thresholdValue2=OSTU_Alg_Threshold(image,thresholdValue1,255);  //初次分离出颅骨
	int thresholdValue3=OSTU_Alg_Threshold(image,thresholdValue2,255);  //对分离出颅骨进行一次迭代优化

	int thresholdValue4=(thresholdValue2+thresholdValue3)/2;  //对两次分离颅骨的阈值取平均值
	cout<<"阈值4(分割颅骨)为："<<thresholdValue4<<endl;
		
	int thresholdValue5=(OSTU_Alg_Threshold(image,0,thresholdValue1)+thresholdValue1)/2; 
	cout<<"阈值5为(分割脑脊液)："<<thresholdValue5<<endl;

	int thresholdValue6=OSTU_Alg_Threshold(image,thresholdValue1,(thresholdValue1+thresholdValue2)/2); 

	int thresholdValue7=(thresholdValue1+thresholdValue6)/2; 
	cout<<"阈值7(分割出脑实质)为："<<thresholdValue7<<endl;

	int thresholdValue8=(thresholdValue1+thresholdValue2)/2; 
	cout<<"阈值8(分割出脑实质)为："<<thresholdValue8<<endl;
	//---------------------分割颅骨------------------------------
	Imagine_Convert(copyLG1,thresholdValue4,255);
	imshow("颅骨",copyLG1);

	//---------------------去除颅骨及以外部分--------------------
	Imagine_Convert(copyLG2,thresholdValue2,255);//mark颅骨的位置	
	Mat img_deleteBone=deleteOutside(image,copyLG2,thresholdValue2);//去除颅骨及以外部分，此时选用的是初次分离颅骨的阈值
	imshow("原图去除颅骨",img_deleteBone);

	//----------------------分割脑脊液---------------------------
	Imagine_Convert(img_deleteBone,0,thresholdValue5);//将脑脊液部分的像素点灰度置255(白)
	deleteNS(img_deleteBone);//去除脑室
	imshow("脑脊液",img_deleteBone);

	//---------------------去除脑脊液及以外部分------------------
	Mat img_deleteNJY=deleteOutside(copyNJY,img_deleteBone,255);//去除脑脊液及以外部分
	imshow("原图去除脑脊液",img_deleteNJY);

	//-----------------------分割脑实质--------------------------
	Imagine_Convert(img_deleteNJY,thresholdValue7-5,thresholdValue8);//将脑实质部分的像素点灰度置255(白)
	imshow("脑实质",img_deleteNJY);

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


//对于提取出的脑脊液，除去多余的脑室部分(即一个矩形区域)
int deleteNS(const Mat markImage)
{	
	uchar*markImageData = markImage.data;//提取目标区域后的图像，用来标记目标区域
	for(int i=0; i<120; i++)
	{
		for(int j=0; j<100; j++)
		{
			markImageData[(i+70)*markImage.step+(j+80)] = 0;
		}			
	}	
	return 0;
}

//去除目标区域及以外部分
Mat deleteOutside(const Mat image,const Mat markImage,int threshold)
{
	uchar*data = image.data;//待处理图片
	uchar*markImageData = markImage.data;//提取目标区域后的图像，用来标记目标区域
	int flag[256][256]={0};//标记矩阵
			
	//对图像中目标区域的灰度值置0，即去除目标区域，将其变成背景
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			if(markImageData[i*markImage.step+j]>= threshold) //灰度值大于等于阈值，属于目标区域
			{
				data[i*image.step+j] = 0;
				flag[i][j] = 1;//标记成目标区域
			}
		}			
	}	
		
	//再次遍历图像，找到每一行中第一个属于目标区域的像素点，将该点前面的所有点置为0。
	//即将图像的左半部分目标区域及目标区域以外区域置为0
	for(int n=0;n<image.rows;n++)
	{
		for(int m=0;m<image.cols;m++)
		{
			if( flag[n][m]==1 && data[n*image.step+m]==0) 
			{
				for(int k=0;k<m;k++)
					data[n*image.step+k]=0;
				break;//将第一个属于目标区域的像素点前面的所有点置为0后，跳转下一行。
			}			
		}	
	}
	
	//同理，再次遍历图像，找到每一行中最后一个属于目标区域的像素点，将该点后面的所有点置为0。
	//即将图像的右半部分目标区域及目标区域以外区域置为0
	for(int n=0;n<image.rows;n++)
	{
		for(int m=image.cols;m>=0;m--)
		{
			if(flag[n][m]==1 && data[n*image.step+m]==0) 
			{
				for(int k=image.cols;k>m;k--)
					data[n*image.step+k]=0;
				break;//将最后一个属于目标区域的像素点后面的所有点置为0后，跳转下一行。
			}			
		}	
	}
	
	//去除图像上下半部分的非0背景。
	for(int n=0;n<image.rows;n++)
	{
		for(int m=0;m<image.cols;m++)
		{
			if(flag[n][m]!=1) 
				data[n*image.step+m]=0;
			else
			{
				for(int n=image.rows-1;n>=0;n--)
				{
					for(int m=0;m<image.cols;m++)
					{
						if(flag[n][m]!=1) 
							data[n*image.step+m]=0;	
						else
							return image;
					}
				}			
			}				
		}	
	}
	return image;
}

