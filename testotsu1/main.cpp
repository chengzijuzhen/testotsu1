#include <iostream> 
#include <math.h> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>  
using namespace cv;   
using namespace std;  


/***
* ����㷨��һ��ͼ��Ҷ�����Ӧ����ֵ�ָ��㷨����1979�����ձ�ѧ�ߴ������������������������ġ�
* ����㷨����ͼ���ϻҶ�ֵ�ķֲ�����ͼ��ֳɱ�����ǰ�������ֿ�����ǰ����������Ҫ������ֵ�ָ�����Ĳ��֡�
* ������ǰ���ķֽ�ֵ��������Ҫ�������ֵ��������ͬ����ֵ�����㲻ͬ��ֵ�¶�Ӧ�ı�����ǰ��֮������ڷ��
* �����ڷ���ȡ�ü���ֵʱ����ʱ��Ӧ����ֵ���Ǵ�򷨣�OTSU�㷨���������ֵ��
*/


//***************Otsu�㷨ͨ������䷽���ֵ������Ӧ��ֵ******************  
int otsuAlgThreshold(const Mat image);  


int main() 
{ 
    Mat image=imread("girl.jpg");//����һ��ͼƬ��Mat��һ�ֻ���ͼ������
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
		
	double Histogram[256] = {0}; //�Ҷ�ֱ��ͼ���±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ�����ص�����  
	int Histogram1[256] = {0};

	int T=0; //Otsu�㷨��ֵ
	double varValue=0; //��䷽���м�ֵ���� 

	double foregroundPixelsSum = 0; //ǰ�����ص��� 
	double backgroundPixelsSum = 0; //�������ص��� 

	double foregroundProportion = 0; //ǰ�����ص�����ռ����  
	double backgroundProportion = 0; //�������ص�����ռ����

	double foregroundGreylevelSum = 0; //ǰ���Ҷ��ܺ�  
	double backgroundGreylevelSum = 0; //�����Ҷ��ܺ�  

	double foregroundGreylevelAvg = 0; //ǰ��ƽ���Ҷ�  
	double backgroundGreylevelAvg = 0; //����ƽ���Ҷ�  

	uchar *data = grayImage.data; //uchar�͵�ָ�롣Mat���Ϊ����������:����ͷ��ָ��������ݲ��ֵ�ָ�룬data����ָ��������ݵ�ָ�롣
	//Mat�Ĵ洢��ʽ��Matlab��������ʽ�е��񣬵�һ���Ƕ�ά����������ǻҶ�ͼ��һ����<uchar>���ͣ������RGB��ɫͼ�����<Vec3b>���͡�
	//cout<<"grayImage of data: " << grayImage.data << endl;

	double pixelSum = grayImage.rows * grayImage.cols; //�������� 

	cout<<"grayImage of rows: " << grayImage.rows << endl;  
	cout<<"grayImage of cols: " << grayImage.cols << endl;  

	//����Ҷ�ֱ��ͼ�ֲ���Histogram�����±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ���ص��� (Histogram means ֱ��ͼ) 
	for(int i=0; i<grayImage.rows; i++)   //Ϊ������������û�а�rows��cols���������  
	{  
		for(int j=0; j<grayImage.cols; j++)  
		{  
			Histogram[ data[i * grayImage.step + j] ]++;  
			Histogram1[ data[i * grayImage.step + j] ]++;  
		}  
	}  
	cout << "channel:" << grayImage.channels() << endl;//channels():ͼ���ͨ��������ͼ��ʱ��BGR����ʱͨ����Ϊ3���Ҷ�ͼ���ͨ����Ϊ1.
	cout << "dims:" << grayImage.dims << endl;//dims: Mat�����ά�ȡ�
	cout << "step[0]:" << grayImage.step[0] << endl;
	cout << "step[1]:" << grayImage.step[1] << endl;

	//***********����ͼ��ֱ��ͼ********************************  
	Mat image1(255, 255, CV_8UC3); //Mat�Ĺ��캯��������һ��256*256�Ĳ�ͼ��

	for(int i=0; i<255; i++)  
	{  
		Histogram1[i] = Histogram1[i] % 200;  
		line(image1, Point(i, 235), Point(i, 235-Histogram1[i]), Scalar(255, 0, 0), 1, 8, 0); 
		/*
		line()������
		��һ������img��Ҫ���������ڵ�ͼ��;
		�ڶ�������pt1��ֱ�����;
		�ڶ�������pt2��ֱ���յ�;
		����������color��ֱ�ߵ���ɫ e.g:Scalor(0,0,255);
		���ĸ�����thickness=1��������ϸ;
		���������line_type=8,8 (or 0) - 8-connected line��8�ڽ�)������,4 - 4-connected line(4�ڽ�)�����ߡ�CV_AA - antialiased ������
		������������������С����λ����
		*/

		//��ͼ��x��������ֱ�ע��0��50��100��150...
		if(i % 50 == 0)  
		{  
			char ch[255];  
			sprintf(ch, "%d", i);  
			string str = ch;  
			putText(image1, str, Point(i, 250), 1, 1, Scalar(0, 0, 255));  
		}  
	}  
	//***********����ͼ��ֱ��ͼ********************************  
  
	for(int i=0; i<=256; i++)  
	{  
		//ÿ�α���֮ǰ��ʼ��������  
		backgroundPixelsSum = 0;  		      
		foregroundPixelsSum = 0;
		backgroundGreylevelSum = 0; 
		foregroundGreylevelSum = 0; 

		//***********����������ֵ����**************************  
		for(int j=0;j<=i;j++) //�������ָ�ֵ����  
		{  
			backgroundPixelsSum += Histogram[j];  //�����������ص�����  
			backgroundGreylevelSum += j * Histogram[j]; //�������������ܻҶȺ�  
		}  
		if(backgroundPixelsSum == 0) //�����������ص���Ϊ0ʱ�˳�  
		{  
			break;  
		}  

		backgroundGreylevelAvg = backgroundGreylevelSum / backgroundPixelsSum; //��������ƽ���Ҷ�  
		backgroundProportion = backgroundPixelsSum / pixelSum; // �����������ص�����ռ���� 
		//***********����������ֵ����**************************  

  
		//***********ǰ��������ֵ����**************************  
		for(int k=i+1; k<256; k++)  
		{  
			foregroundPixelsSum += Histogram[k];  //ǰ���������ص�����  
			foregroundGreylevelSum += k * Histogram[k]; //ǰ�����������ܻҶȺ�  
		} 

		if(foregroundPixelsSum == 0) //ǰ���������ص���Ϊ0ʱ�˳�  
		{  
			break;  
		}  

		foregroundGreylevelAvg = foregroundGreylevelSum / foregroundPixelsSum; //ǰ������ƽ���Ҷ�  
		foregroundProportion = foregroundPixelsSum / pixelSum; // ǰ���������ص�����ռ����
		//***********ǰ��������ֵ����**************************  
  

		//***********��䷽�����******************************  
		double varValueI = foregroundProportion * backgroundProportion * pow((backgroundGreylevelAvg - foregroundGreylevelAvg),2); //��ǰ��䷽�����  
		
		//���ñ����ķ����õ�ʹ��䷽��g������ֵT,��Ϊ����
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
