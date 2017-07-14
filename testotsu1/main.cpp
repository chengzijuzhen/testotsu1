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
    Mat image=imread("image030.jpg");//����һ��ͼƬ��mat��һ�ֻ���ͼ������
    imshow("ԭͼ",image);   //�ڴ�������ʾͼ��
 
    //�ж�ͼ���Ƿ���سɹ�
    if(!image.data)
    {
        cout << "ͼ�����ʧ��!" << endl;
        return false;
    }
    else
        cout << "ͼ����سɹ�!" << endl;

	Mat grayimage;//�Ҷ�ͼ
	Mat imageoutput;   
    cvtColor(image,grayimage, CV_RGB2GRAY);//��ͼ��ת��Ϊ�Ҷ�ͼ������cv_ǰ׺
    imshow("�Ҷ�ͼ",grayimage);   //��������ʾͼ��

    int thresholdvalue1=otsualgthreshold1(grayimage); //ͨ������㷨�����ֵ

	int thresholdvalue2=otsualgthreshold2(grayimage,mark(grayimage,thresholdvalue1),thresholdvalue1); 
	threshold(grayimage,imageoutput,thresholdvalue2,255,CV_THRESH_BINARY); //��ǰ��ֵ������ֵʱ��ȡ255����������Ϊ0;
	imshow("otsu2",imageoutput);  

	int thresholdvalue3=otsualgthreshold2(grayimage,mark(grayimage,thresholdvalue2),thresholdvalue2); 
	threshold(grayimage,imageoutput,thresholdvalue3,255,CV_THRESH_BINARY); //��ǰ��ֵ������ֵʱ��ȡ255����������Ϊ0;
	imshow("otsu3",imageoutput);  

	threshold(grayimage,imageoutput,(thresholdvalue2+thresholdvalue3)/2,255,CV_THRESH_BINARY); //��ǰ��ֵ������ֵʱ��ȡ255����������Ϊ0;
	imshow("otsu4",imageoutput);  

    waitKey();  
    return 0;  
}
  
int otsualgthreshold1(const Mat grayimage)  
{  
	double grayvalue[256] = {0}; //�±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ�����ص�����  

	int t=0; //otsu�㷨��ֵ
	double gtemp=0; //��䷽���м�ֵ 

	double fn = 0; //ǰ�����ص��� 
	double bn = 0; //�������ص��� 

	double fp = 0; //ǰ�����ص�����ռ����  
	double bp = 0; //�������ص�����ռ����

	double ft = 0; //ǰ���Ҷ��ܺ�  
	double bt = 0; //�����Ҷ��ܺ�  

	double fa = 0; //ǰ��ƽ���Ҷ�  
	double ba = 0; //����ƽ���Ҷ�  

	uchar *data = grayimage.data; //ָ��ͼ��������ݵ�ָ��
	
	double pixelsum = grayimage.rows * grayimage.cols; //�������� 
 
	for(int i=0; i<grayimage.rows; i++) 
	{  
		for(int j=0; j<grayimage.cols; j++)  
			grayvalue[data[i * grayimage.step + j]]++; 			
	}  

	for(int i=0; i<256; i++)  
	{  
		//ÿ�α���֮ǰ��ʼ��������  
		bt = 0;  		      
		ft = 0;
		bn = 0; 
		fn = 0; 

		for(int j=0;j<=i;j++) //�������ָ�ֵ����  
		{  
			bn += grayvalue[j];  //�����������ص�����  
			bt += j * grayvalue[j]; //�������������ܻҶȺ�  
		}  
				
		ba = bt/bn; //��������ƽ���Ҷ�  
		bp = bn/pixelsum; // �����������ص�����ռ���� 

		for(int k=i+1; k<256; k++)  
		{  
			fn += grayvalue[k];  //ǰ���������ص�����  
			ft += k * grayvalue[k]; //ǰ�����������ܻҶȺ�  
		} 

		fa = ft / fn; //ǰ������ƽ���Ҷ�  
		fp = fn / pixelsum; // ǰ���������ص�����ռ����

		double g = fp * bp * (fa - ba)*(fa - ba); //��ǰ��䷽�����  
	
		//���ñ����ķ����õ�ʹ��䷽��g������ֵt,��Ϊ����
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
	double grayvalue[256] = {0}; //�±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ�����ص�����  

	int t = 0; //otsu�㷨��ֵ
	double gtemp = 0; //��䷽���м�ֵ

	double fn = 0; //ǰ�����ص��� 
	double bn = 0; //�������ص��� 

	double fp = 0; //ǰ�����ص�����ռ����  
	double bp = 0; //�������ص�����ռ����

	double ft = 0; //ǰ���Ҷ��ܺ�  
	double bt = 0; //�����Ҷ��ܺ�  

	double fa = 0; //ǰ��ƽ���Ҷ�  
	double ba = 0; //����ƽ���Ҷ�  

	uchar *data = grayimage.data; 
	int pixelsum=0; //�������� 

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
		//ÿ�α���֮ǰ��ʼ��������  
		bt = 0;  		      
		ft = 0;
		bn = 0; 
		fn = 0; 

		for(int j=thresholdvalue;j<=i;j++) //�������ָ�ֵ����  
		{  
			bn += grayvalue[j];  //�����������ص�����  
			bt += j * grayvalue[j]; //�������������ܻҶȺ�  
		}  
			
		ba = bt/bn; //��������ƽ���Ҷ�  
		bp = bn/pixelsum; //�����������ص�����ռ���� 
		
		for(int k=i+1; k<256; k++)  
		{  
			fn += grayvalue[k];  //ǰ���������ص�����  
			ft += k * grayvalue[k]; //ǰ�����������ܻҶȺ�  
		} 
			
		fa = ft/fn;//ǰ������ƽ���Ҷ�  
		fp = fn/pixelsum;//ǰ���������ص�����ռ����
		  
		double g = fp * bp * (fa - ba)*(fa - ba); //��ǰ��䷽�����  
		
		//���ñ����ķ����õ�ʹ��䷽��g������ֵt,��Ϊ����
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