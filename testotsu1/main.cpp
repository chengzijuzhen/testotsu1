#include <iostream> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>  
using namespace cv;   
using namespace std;  

//***************Otsu�㷨ͨ������䷽���ֵ������Ӧ��ֵ******************  
int otsuAlgThreshold(const Mat image);  


int main() 
{ 
   Mat image=imread("girl.jpg");//����һ��ͼƬ
   imshow("ԭͼ",image);   //�ڴ�������ʾͼ��
 
    //�ж�ͼ���Ƿ���سɹ�
    if(!image.data)
    {
        cout << "ͼ�����ʧ��!" << endl;
        return false;
    }
    else
        cout << "ͼ����سɹ�!" << endl;

	 Mat grayImage;
	//��ͼ��ת��Ϊ�Ҷ�ͼ������CV_ǰ׺
    cvtColor(image,grayImage, CV_BGR2GRAY);
    imshow("�Ҷ�ͼ",grayImage);   //��������ʾͼ��
 
    Mat imageoutput;  
    Mat imageotsu;    

	//ͨ������䷽���ֵ������Ӧ��ֵ
    int thresholdvalue=otsuAlgThreshold(grayImage);  

    cout << "��䷽��Ϊ�� " << thresholdvalue << endl;  

/*
 *********************************************************************************************************
cvThreshold()����ԭ�ͣ�
	
	void cvThreshold( const CvArr* src, CvArr* dst, double threshold, double maxval, int threshold_type );

������Ϣ��

��һ��������InputArray���͵�src���������飬�ͨ�� , 8��32λ�������͵�Mat���ɡ�

�ڶ���������OutputArray���͵�dst���������ú���������������������������ڴ�����������Һ͵�һ�������е�Mat������һ���ĳߴ�����͡�

������������double���͵�thresh����ֵ�ľ���ֵ��

���ĸ�������double���͵�maxval���������������ֵ����typeȡ THRESH_BINARY ��THRESH_BINARY_INV��ֵ����ʱ�����ֵ.

�����������int���͵�threshold_type����ֵ����,������������¼������ͣ�

0: CV_THRESH_BINARY  ��ǰ��ֵ������ֵʱ��ȡMaxval,Ҳ���ǵ��ĸ������������ٲ�˵������������Ϊ0;

1: CV_THRESH_BINARY_INV ��ǰ��ֵ������ֵʱ������Ϊ0����������ΪMaxval;

2: CV_THRESH_TRUNC ��ǰ��ֵ������ֵʱ������Ϊ��ֵ�����򲻸ı�;

3: CV_THRESH_TOZERO ��ǰ��ֵ������ֵʱ�����ı䣬��������Ϊ0;

4: CV_THRESH_TOZERO_INV  ��ǰ��ֵ������ֵʱ������Ϊ0�����򲻸ı䡣

ֵ��һ˵����threshold_type����ʹ��CV_THRESH_OTSU���ͣ������ú����ͻ�ʹ�ô��ɷ�OTSU�õ���ȫ������Ӧ��ֵ�����ж�ֵ��ͼƬ���������е�threshold���������á�
���磺cvThreshold(src,dst,300,255,CV_THRESH_OTSU | CV_THRESH_BINARY_INV);���ַ������ڻҶ�ֱ��ͼ���ֶ���������ͼƬ��������Ч���ܺá�

*********************************************************************************************************
*/

	threshold(grayImage,imageoutput,thresholdvalue,255,CV_THRESH_BINARY); //�Լ�д�ĺ���
	threshold(grayImage,imageotsu,0,255,CV_THRESH_OTSU); //opencv otsu�㷨  

	imshow("�Լ�ʵ�ֵ�otsu",imageoutput);  
	imshow("opencv��otsu",imageotsu);   

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

	int T=0; //Otsu�㷨��ֵ  

	double varValue=0; //��䷽���м�ֵ����  

	double foregroundPixelsProportion = 0; //ǰ�����ص�����ռ����  
	double backgroundPixelsProportion = 0; //�������ص�����ռ����  
	double foregroundAvgGreylevel = 0; //ǰ��ƽ���Ҷ�  
	double backgroundAvgGreylevel = 0; //����ƽ���Ҷ�  

	double Histogram[256] = {0}; //�Ҷ�ֱ��ͼ���±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ�����ص�����  
	int Histogram1[256] = {0};   

	uchar *data = grayImage.data;  

	double totalNum = grayImage.rows * grayImage.cols; //��������  

	//����Ҷ�ֱ��ͼ�ֲ���Histogram�����±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ���ص���  
	for(int i=0; i<grayImage.rows; i++)   //Ϊ������������û�а�rows��cols���������  
	{  
		for(int j=0; j<grayImage.cols; j++)  
		{  
			Histogram[data[i*grayImage.step+j]]++;  
			Histogram1[data[i*grayImage.step+j]]++;  
		}  
	}  
  
	//***********����ͼ��ֱ��ͼ********************************  
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
	//***********����ͼ��ֱ��ͼ********************************  
  
	for(int i=0; i<255; i++)  
	{  
		//ÿ�α���֮ǰ��ʼ��������  
		backgroundPixelsProportion = 0;       
		backgroundAvgGreylevel = 0;       
		foregroundPixelsProportion = 0;       
		foregroundAvgGreylevel = 0; 

		//***********����������ֵ����**************************  
		for(int j=0;j<=i;j++) //�������ָ�ֵ����  
		{  
			backgroundPixelsProportion += Histogram[j];  //�����������ص�����  
			backgroundAvgGreylevel += j * Histogram[j]; //�������������ܻҶȺ�  
		}  
		if(backgroundPixelsProportion == 0) //�����������ص���Ϊ0ʱ�˳�  
		{  
			break;  
		}  
		backgroundAvgGreylevel = backgroundAvgGreylevel / backgroundPixelsProportion; //��������ƽ���Ҷ�  
		backgroundPixelsProportion = backgroundPixelsProportion / totalNum; // �����������ص�����ռ���� 
		//***********����������ֵ����**************************  

  
		//***********ǰ��������ֵ����**************************  
		for(int k=i+1;k<255;k++)  
		{  
			foregroundPixelsProportion += Histogram[k];  //ǰ���������ص�����  
			foregroundAvgGreylevel += k * Histogram[k]; //ǰ�����������ܻҶȺ�  
		}  
		if(foregroundPixelsProportion == 0) //ǰ���������ص���Ϊ0ʱ�˳�  
		{  
			break;  
		}  

		foregroundAvgGreylevel = foregroundAvgGreylevel / foregroundPixelsProportion; //ǰ������ƽ���Ҷ�  
		foregroundPixelsProportion = foregroundPixelsProportion / totalNum; // ǰ���������ص�����ռ����  
		//***********ǰ��������ֵ����**************************  
  

		//***********��䷽�����******************************  
		double varValueI = foregroundPixelsProportion * backgroundPixelsProportion * (backgroundAvgGreylevel - foregroundAvgGreylevel) * (backgroundAvgGreylevel - foregroundAvgGreylevel); //��ǰ��䷽�����  
		if(varValue < varValueI)  
		{  
			varValue = varValueI;  
			T=i;  
		}  
	}  
	//������TΪ��ֵ�ķָ���  
	line(image1, Point(T,235), Point(T,0), Scalar(0, 0, 255), 2, 8);  
	imshow("ֱ��ͼ", image1); 

	return T;  
}  
