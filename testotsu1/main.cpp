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
   Mat image=imread("image030.jpg");//����һ��ͼƬ
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
  
int otsuAlgThreshold(const Mat image)  
{  
    int T=0; //Otsu�㷨��ֵ  
    double varValue=0; //��䷽���м�ֵ����  
    double w0=0; //ǰ�����ص�����ռ����  
    double w1=0; //�������ص�����ռ����  
    double u0=0; //ǰ��ƽ���Ҷ�  
    double u1=0; //����ƽ���Ҷ�

    double Histogram[256]={0}; //�Ҷ�ֱ��ͼ���±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ�����ص�����  

    uchar *data=image.data;  
	double totalNum=image.rows*image.cols; //�������� 

    //����Ҷ�ֱ��ͼ�ֲ���Histogram�����±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ���ص���  
    for(int i=0;i<image.rows;i++)   //Ϊ������������û�а�rows��cols���������  
    {  
        for(int j=0;j<image.cols;j++)  
        {  
            Histogram[data[i*image.step+j]]++;  
        }  
    }  
    for(int i=0;i<255;i++)  
    {  
        //ÿ�α���֮ǰ��ʼ��������  
        w1=0;       
		u1=0;       
		w0=0;      
		u0=0;  

        //***********����������ֵ����**************************  
        for(int j=0;j<=i;j++) //�������ָ�ֵ����  
        {  
            w1+=Histogram[j];  //�����������ص�����  
            u1+=j*Histogram[j]; //�������������ܻҶȺ�  
        }  
        if(w1==0) //�����������ص���Ϊ0ʱ�˳�  
        {  
            break;  
        }  

        u1=u1/w1; //��������ƽ���Ҷ�  
        w1=w1/totalNum; // �����������ص�����ռ����  
        //***********����������ֵ����**************************  
      

        //***********ǰ��������ֵ����**************************  
        for(int k=i+1;k<255;k++)  
        {  
            w0+=Histogram[k];  //ǰ���������ص�����  
            u0+=k*Histogram[k]; //ǰ�����������ܻҶȺ�  
        }  
        if(w0==0) //ǰ���������ص���Ϊ0ʱ�˳�  
        {  
            break;  
        }  
        u0=u0/w0; //ǰ������ƽ���Ҷ�  
        w0=w0/totalNum; // ǰ���������ص�����ռ����  
        //***********ǰ��������ֵ����**************************  

      
        //***********��䷽�����******************************  
        double varValueI=w0*w1*(u1-u0)*(u1-u0); //��ǰ��䷽�����  
        if(varValue<varValueI)  
        {  
            varValue=varValueI;  
            T=i;  
        }  
    }  
    return T;  
}  
