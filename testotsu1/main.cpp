#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>    
#include <opencv2/core/core.hpp>   
#include <iostream> 

using namespace cv;  
using namespace std;

int OSTU_Alg_Threshold(const Mat image,int value1,int value2);//��ͼƬimage�лҶ�ֵ��value1-value2֮������ص��������ֵ����
int Imagine_Convert(const Mat image,int value1,int value2);//��ͼƬimage�лҶ�ֵ��value1-value2֮������ص�Ҷ���Ϊ255(��)���������ص����0(��)
Mat deleteBone(const Mat image,int threshold);//ȥ��­���������ⲿ��
Mat deleteNJY(const Mat image,const Mat markImage,int threshold);//ȥ���Լ�Һ�������ⲿ��

int main()
{
	Mat image=imread("image030.jpg");
	if(!image.data)
	{
		cout<<"����ͼƬʧ�ܣ�"<<endl;
		return -1;
	}
	imshow("ԭͼ",image);
	cvtColor(image,image,CV_RGB2GRAY);  //ת��Ϊ��ͨ���Ҷ�ͼ

	Mat copy;
	Mat copyNJY;

	int thresholdValue1=OSTU_Alg_Threshold(image,0,255);  //�����ǰ��(��ʵ��+­��)�뱳��(�Լ�Һ+ͼƬ����)
	cout<<"��ֵ1(�����ǰ��(��ʵ��+­��)�뱳��(�Լ�Һ+ͼƬ����))Ϊ��"<<thresholdValue1<<endl;

	int thresholdValue2=OSTU_Alg_Threshold(image,thresholdValue1,255);  //���η����­��
	int thresholdValue3=OSTU_Alg_Threshold(image,thresholdValue2,255);  //�Է����­�ǽ���һ�ε����Ż�

	int thresholdValue4=(thresholdValue2+thresholdValue3)/2;  //�����η���­�ǵ���ֵȡƽ��ֵ
	cout<<"��ֵ4(�ָ�­��)Ϊ��"<<thresholdValue4<<endl;
		
	int thresholdValue5=OSTU_Alg_Threshold(image,0,thresholdValue1); //������Լ�Һ
	cout<<"��ֵ5Ϊ(�ָ��Լ�Һ)��"<<thresholdValue5<<endl;

	Mat img_deleteBone=deleteBone(image,thresholdValue2);//ȥ��­�Ǽ����ⲿ�֣���ʱѡ�õ��ǳ��η���­�ǵ���ֵ
	imshow("ȥ��­��",img_deleteBone);

	image.copyTo(copy);
	img_deleteBone.copyTo(copyNJY);

	Imagine_Convert(img_deleteBone,0,thresholdValue5);//���Լ�Һ���ֵ����ص�Ҷ���255(��)	
	imshow("�Լ�Һ",img_deleteBone);

	imshow("ͼƬ����",copy);
	Imagine_Convert(copyNJY,0,27);//���Լ�Һ���ֵ����ص�Ҷ���255(��)
	Mat img_deleteNJY=deleteNJY(copy,copyNJY,255);//ȥ���Լ�Һ�����ⲿ�֣���ʱѡ�õ��ǳ��η���­�ǵ���ֵ
	imshow("ȥ���Լ�Һ",img_deleteNJY);
	
	Imagine_Convert(img_deleteNJY,65,90);//����ʵ�ʲ��ֵ����ص�Ҷ���255(��)
	imshow("��ʵ��",img_deleteNJY);

	waitKey();
	system("pause");
	return 0;
}

int OSTU_Alg_Threshold(const Mat image,int value1,int value2)
{ 
	if(image.channels()!=1)
	{
		cout<<"please input Cray-image!"<<endl;  //���ǵ�ͨ���Ҷ�ͼ�˳� 
		return -1;
	}
	int HUIDU[256]={0};
	double N=0;  //���ص���
	int T=0;  //��ʼ����ֵ
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
		int N0=0;     //ǰ����
		int N1=0;     //������
		double w0=0;  //ǰ������
		double w1=0;  //��������
		double u0=0;  //ǰ��ƽ���Ҷ�
		double u1=0;  //����ƽ���Ҷ�
		double h0=0;  //ǰ���ܻҶ�
		double h1=0;  //�����ܻҶ�
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
		if(data[i]<=value1||data[i]>value2) //��ͼƬimage�лҶ�ֵ��value1-value2֮������ص�Ҷ���Ϊ255(��)���������ص����0(��)
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

//ȥ��­���������ⲿ��
Mat deleteBone(const Mat image,int threshold)
{
	uchar*data=image.data;
	int flag[256][256]={0};

	//��ͼ����Ŀ������ĻҶ�ֵ��0����ȥ��Ŀ�����򣬽����ɱ���
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			if(data[i*image.step+j]>= threshold) //�Ҷ�ֵ���ڵ�����ֵ������Ŀ������
			{
				data[i*image.step+j] = 0;
				flag[i][j] = 1;//��ǳ�Ŀ������
			}
		}			
	}
	
	//�ٴα���ͼ���ҵ�ÿһ���е�һ������­�ǵ����ص㣬���õ�ǰ������е���Ϊ0��
	//����ͼ�����벿��­�Ǽ�­������������Ϊ0
	for(int n=0;n<image.rows;n++)
	{
		for(int m=0;m<image.cols;m++)
		{
			if(data[n*image.step+m]==0 && flag[n][m]==1) 
			{
				for(int k=0;k<m;k++)
					data[n*image.step+k]=0;
				break;//����һ������­�ǵ����ص�ǰ������е���Ϊ0����ת��һ�С�
			}			
		}	
	}
	
	//ͬ���ٴα���ͼ���ҵ�ÿһ�������һ������­�ǵ����ص㣬���õ��������е���Ϊ0��
	//����ͼ����Ұ벿��­�Ǽ�­������������Ϊ0
	for(int n=0;n<image.rows;n++)
	{
		for(int m=image.cols;m>=0;m--)
		{
			if(data[n*image.step+m]==0 && flag[n][m]==1) 
			{
				for(int k=image.cols;k>m;k--)
					data[n*image.step+k]=0;
				break;//�����һ������­�ǵ����ص��������е���Ϊ0����ת��һ�С�
			}			
		}	
	}
	
	//ȥ��ͷƤ��ͼ�����°벿�ֵķ�0������
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

//ȥ���Լ�Һ�������ⲿ��
Mat deleteNJY(const Mat image,const Mat markImage,int threshold)
{
	uchar*markImageData=markImage.data;//��ȡ�Լ�Һ���ͼ����������Լ�Һ����
	uchar*data=image.data;//������ͼƬ
	int flag[256][256]={0};

	//��ͼ����Ŀ������ĻҶ�ֵ��0����ȥ��Ŀ�����򣬽����ɱ���
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			if(markImageData[i*markImage.step+j]>= threshold) //�Ҷ�ֵ���ڵ�����ֵ������Ŀ������
			{
				data[i*image.step+j] = 0;
				flag[i][j] = 1;//��ǳ�Ŀ������
			}
		}			
	}

	//�ٴα���ͼ���ҵ�ÿһ���е�һ������­�ǵ����ص㣬���õ�ǰ������е���Ϊ0��
	//����ͼ�����벿��­�Ǽ�­������������Ϊ0
	for(int n=0;n<image.rows;n++)
	{
		for(int m=0;m<image.cols;m++)
		{
			if( flag[n][m]==1 && data[n*image.step+m]==0) 
			{
				for(int k=0;k<m;k++)
					data[n*image.step+k]=0;
				break;//����һ������­�ǵ����ص�ǰ������е���Ϊ0����ת��һ�С�
			}			
		}	
	}
	
	//ͬ���ٴα���ͼ���ҵ�ÿһ�������һ������­�ǵ����ص㣬���õ��������е���Ϊ0��
	//����ͼ����Ұ벿��­�Ǽ�­������������Ϊ0
	for(int n=0;n<image.rows;n++)
	{
		for(int m=image.cols;m>=0;m--)
		{
			if(flag[n][m]==1 && data[n*image.step+m]==0) 
			{
				for(int k=image.cols;k>m;k--)
					data[n*image.step+k]=0;
				break;//�����һ������­�ǵ����ص��������е���Ϊ0����ת��һ�С�
			}			
		}	
	}
	
	//ȥ��ͷƤ��ͼ�����°벿�ֵķ�0������
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

