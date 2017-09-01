#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/imgproc/imgproc.hpp>    
#include <opencv2/core/core.hpp>   
#include <iostream> 

using namespace cv;  
using namespace std;

int OSTU_Alg_Threshold(const Mat image,int value1,int value2);//对图片image中灰度值在value1-value2之间的像素点进行求阈值运算
int Imagine_Convert(const Mat image,int value1,int value2);//对图片image中灰度值在value1-value2之间的像素点灰度置为255(白)，其他像素点均置0(黑)
Mat deleteOutside(const Mat image,const Mat markImage,int threshold);//去除目标区域及以外部分
int deleteNS(const Mat markImage);//对于提取出的脑脊液，除去多余的脑室部分
int count(const Mat image); //用于计算一张图片的高亮像素点个数

//形态学
Mat changeToBinaryImage(Mat grayImage);//把灰度图像转化为二值图像
Mat binaryErosion(Mat binaryImage, Mat se);//二值图像腐蚀操作
Mat binaryDilation(Mat binaryImage, Mat se);//二值图像膨胀操作
Mat grayErosion(Mat grayImage,Mat se);//灰度图像腐蚀操作
Mat grayDilation(Mat grayImage,Mat se);//灰度图像膨胀操作
Mat binaryOpen(Mat binaryImage, Mat se);//二值图像开操作
Mat binaryClose(Mat binaryImage, Mat se);//二值图像闭操作
Mat grayOpen(Mat grayImage, Mat se);//灰度图像开操作
Mat grayClose(Mat grayImage, Mat se);//灰度图像闭操作
Mat binaryBorder(Mat binaryImage,Mat se);//二值图像边界提取
Mat grayBorder(Mat grayImage, Mat se);//灰度图像边界提取
Mat gradient(Mat grayImage, Mat se);//灰度图像梯度
Mat topHat(Mat grayImage,Mat se);//灰度图像的顶帽运算 T（f）=f-fob
Mat bottomHat(Mat grayImage, Mat se);	//灰度图像的底帽运算 B（f）=f⋅b-f

long int deal(Mat image);//对一张图片的所有操作

int main()
{
	char filename[52];
    cv::Mat image;
	int sum =0;//脑脊液和脑实质的像素点总和
    for(int i=1;i<53;i++)
    {
		sprintf(filename,"image/image(%d).jpg",i);
		image=imread(filename);//导入图片 
		sum = sum + deal(image);
    }
	cout<<"脑脊液和脑实质的像素点总和： "<<sum<<endl;
	double result = (double)sum/65530/52*729;//S局=N局/N总*S总，这里用到的是这个公式，S总是该图片的总面积，由于该图片是按比例缩小的，所以先计算出结果，再按比例还原。
	cout<<"总体积： "<<result<<endl;
	
	waitKey();
	system("pause");
	return 0;
}



long int deal(Mat image)
{
	if(!image.data)
	{
		cout<<"读入图片失败！"<<endl;
		return -1;
	}
	//imshow("原图",image);

	//创建模板  一般结构元素关于自身原点对称  
    //也可以自定义结构元素  下面的变量是3*3的矩阵 全部为0  
    Mat structureElement(2,2, CV_8UC1, Scalar(0));

	//Mat element = getStructuringElement( MORPH_ELLIPSE, Size(-1,-1), Point(-1,-1) );

	cvtColor(image,image,CV_RGB2GRAY);  //转换为单通道灰度图

	Mat copyLG1;
	Mat copyLG2;
	Mat copyNJY;
	image.copyTo(copyLG1);//备份原始图像，用来分割颅骨
	image.copyTo(copyLG2);//备份原始图像，用来去除颅骨
	image.copyTo(copyNJY);//备份原始图像，用来去除脑脊液

	int thresholdValue1=OSTU_Alg_Threshold(image,0,255);  //分离出前景(脑实质+颅骨)与背景(脑脊液+图片背景)
	//cout<<"阈值1(分离出前景(脑实质+颅骨)与背景(脑脊液+图片背景))为："<<thresholdValue1<<endl;

	int thresholdValue2=OSTU_Alg_Threshold(image,thresholdValue1,255);  //初次分离出颅骨
	int thresholdValue3=OSTU_Alg_Threshold(image,thresholdValue2,255);  //对分离出颅骨进行一次迭代优化

	int thresholdValue4=(thresholdValue2+thresholdValue3)/2;  //对两次分离颅骨的阈值取平均值
	//cout<<"阈值4(分割颅骨)为："<<thresholdValue4<<endl;
		
	int thresholdValue5=(OSTU_Alg_Threshold(image,0,thresholdValue1)+thresholdValue1)/2; 
	//cout<<"阈值5为(分割脑脊液)："<<thresholdValue5<<endl;

	int thresholdValue6=OSTU_Alg_Threshold(image,thresholdValue1,(thresholdValue1+thresholdValue2)/2); 

	int thresholdValue7=(thresholdValue1+thresholdValue6)/2; 
	//cout<<"阈值7(分割出脑实质)为："<<thresholdValue7<<endl;

	int thresholdValue8=(thresholdValue1+thresholdValue2)/2; 
	//cout<<"阈值8(分割出脑实质)为："<<thresholdValue8<<endl;
	//---------------------分割颅骨------------------------------
	Imagine_Convert(copyLG1,thresholdValue4,255);
	//imshow("颅骨",copyLG1);

	//---------------------去除颅骨及以外部分--------------------
	Imagine_Convert(copyLG2,thresholdValue2,255);//mark颅骨的位置	
	Mat img_deleteBone=deleteOutside(image,copyLG2,thresholdValue2);//去除颅骨及以外部分，此时选用的是初次分离颅骨的阈值
	//imshow("原图去除颅骨",img_deleteBone);

	//----------------------分割脑脊液---------------------------
	Imagine_Convert(img_deleteBone,0,thresholdValue5);//将脑脊液部分的像素点灰度置255(白)
	//deleteNS(img_deleteBone);//去除脑室
	//imshow("脑脊液",img_deleteBone);

    //调用二值图像开操作
    //imshow("开操作",binaryOpen(img_deleteBone,structureElement));
		    
	//img_deleteBone=binaryOpen(img_deleteBone,structureElement);

	int numberNJY = count(img_deleteBone);
	//cout<<"脑脊液像素点个数为："<<numberNJY<<endl;

	//---------------------去除脑脊液及以外部分------------------
	Mat img_deleteNJY=deleteOutside(copyNJY,img_deleteBone,255);//去除脑脊液及以外部分
	//imshow("原图去除脑脊液",img_deleteNJY);

	//-----------------------分割脑实质--------------------------
	Imagine_Convert(img_deleteNJY,thresholdValue7-5,thresholdValue8);//将脑实质部分的像素点灰度置255(白)
	//imshow("脑实质",img_deleteNJY);

	int numberNSZ = count(img_deleteNJY);
	//cout<<"脑实质像素点个数为："<<numberNSZ<<endl;

	int num = numberNJY+numberNSZ;
	//cout<<"脑实质+脑脊液的像素点个数和："<<num<<endl;

	return num;

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

int count(const Mat image){
	uchar*data=image.data;
	long int sum =0;
	for(int i=0;i<=image.rows*image.cols;i++)
		if(data[i]== 255)
			sum++;
	return sum;
}

//把灰度图像转化为二值图像
Mat changeToBinaryImage(Mat grayImage)
{
    Mat binaryImage(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    //转化为二值图像
    for (int i = 0; i < grayImage.rows; i++)
    {
        for (int j = 0; j < grayImage.cols; j++)
        {
            if (grayImage.data[i*grayImage.step + j]>100)
            {
                binaryImage.data[i*grayImage.step + j] = 255;
            }
            else
            {
                binaryImage.data[i*grayImage.step + j] = 0;
            }
        }
    }
    imshow("binaryImage", binaryImage);

    return binaryImage; 
}

//创建结构元素
//一般结构元素 关于原点对称
//Mat createSE()
//{
//  int a[3][3]={ 0,1,0,
//  1,1,1,
//  0,1,0};
//  Mat structureElement(3, 3, CV_8UC1, a);
//}

//二值图像腐蚀操作
Mat binaryErosion(Mat binaryImage, Mat se)
{
    //二值图像移动
    Mat window(se.rows, se.cols, CV_8UC1);

    //定义一个矩阵，存储腐蚀后的图像
    Mat binaryErosionImage(binaryImage.rows, binaryImage.cols, CV_8UC1, Scalar(0));

    for (int i = (se.rows-1)/2; i < binaryImage.rows-(se.rows-1)/2; i++)
    {
        for (int j = (se.cols - 1) / 2; j < binaryImage.cols - (se.cols - 1) / 2; j++)
        {
            //先设置第i行第j列像素值为255，即白色
            binaryErosionImage.data[i*binaryImage.step + j] = 255;
            for (int row = 0; row < se.rows; row++)
            {
                for (int col = 0; col < se.cols; col++)
                {
                    //把se对应的元素赋值到与se结构相同的矩阵中
                    window.data[row*window.step + col] = binaryImage.data[(i + row - (window.rows - 1) / 2)*binaryImage.step + (j + col - (window.cols - 1) / 2)];
                }
            }
            //比较se与window中的像素值
            int row, col;
            for (row = 0; row < se.rows; row++)
            {
                for (col = 0; col < se.cols; col++)
                {
                    if (se.data[row*se.step + col] != window.data[row*se.step + col])
                    {
                        break;
                    }
                }
                if (col == se.cols)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            if (row == se.rows&&col == se.cols)
            {
                binaryErosionImage.data[i*binaryImage.step + j] = 0;
        }
        }
    }

    //imshow("binaryErosionImage", binaryErosionImage);

    return binaryErosionImage;
}

//二值图像膨胀操作
Mat binaryDilation(Mat binaryImage, Mat se)
{
    //二值图像移动
    Mat window(se.rows, se.cols, CV_8UC1);

    //定义一个矩阵，存储膨胀后的图像
    Mat binaryDilationImage(binaryImage.rows, binaryImage.cols, CV_8UC1, Scalar(0));

    for (int i = (se.rows - 1) / 2; i < binaryImage.rows - (se.rows - 1) / 2; i++)
    {
        for (int j = (se.cols - 1) / 2; j < binaryImage.cols - (se.cols - 1) / 2; j++)
        {
            //先设置第i行第j列像素值为255，即白色
            binaryDilationImage.data[i*binaryImage.step + j] = 255;
            for (int row = 0; row < se.rows; row++)
            {
                for (int col = 0; col < se.cols; col++)
                {
                    //把se对应的元素赋值到与se结构相同的矩阵中
                    window.data[row*window.step + col] = binaryImage.data[(i + row - (window.rows - 1) / 2)*binaryImage.step + (j + col - (window.cols - 1) / 2)];
                }
            }
            //比较se与window中的像素值
            //只要有一个相匹配 就把像素值设为0，即置黑
            int flag = 0;  //标记是否有对应相等的像素值：0表示没有，1表示有
            int row, col;
            for (row = 0; row < se.rows; row++)
            {
                for (col = 0; col < se.cols; col++)
                {
                    if (se.data[row*se.step + col] == window.data[row*se.step + col])
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag)
                {
                    break;
                }
            }
            if (flag)
            {
                //如果有交集，就设置为黑，即0
                binaryDilationImage.data[i*binaryImage.step + j] = 0;
            }
        }
    }

    //imshow("binaryDilationImage", binaryDilationImage);
    return binaryDilationImage;
}

//灰度图像腐蚀操作
Mat grayErosion(Mat grayImage,Mat se)
{
    //结构元素移动时所对应的源图像区域
    Mat window(se.rows, se.cols, CV_8UC1);

    //定义一个矩阵，存储腐蚀后的图像
    Mat grayErosionImage(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    for (int i = (se.rows - 1) / 2; i < grayImage.rows - (se.rows - 1) / 2; i++)
    {
        for (int j = (se.cols - 1) / 2; j < grayImage.cols - (se.cols - 1) / 2; j++)
        {
            //先设置第i行第j列像素值为255，即白色
            grayErosionImage.data[i*grayImage.step + j] = 255;
            for (int row = 0; row < se.rows; row++)
            {
                for (int col = 0; col < se.cols; col++)
                {
                    //把se对应的元素赋值到与se结构相同的矩阵window中
                    window.data[row*window.step + col] = grayImage.data[(i + row - (window.rows - 1) / 2)*grayImage.step + (j + col - (window.cols - 1) / 2)];
                }
            }
            //比较se与window中的像素值
            //在灰度图像中，腐蚀是取window中最小的值赋值给原点所对用的像素
            int minPixel = 255;
            int row, col;
            for (row = 0; row < se.rows; row++)
            {
                for (col = 0; col < se.cols; col++)
                {
                    if (window.data[row*se.step + col] < minPixel)
                    {
                        minPixel = window.data[row*se.step + col];
                    }
                }   
            }   
            grayErosionImage.data[i*grayImage.step + j] = minPixel;
        }
    }

    /*imshow("grayErosionImage", grayErosionImage);*/

    return grayErosionImage;
}

//灰度图像膨胀操作
Mat grayDilation(Mat grayImage,Mat se)
{
    //结构元素移动时所对应的源图像区域
    Mat window(se.rows, se.cols, CV_8UC1);

    //定义一个矩阵，存储腐蚀后的图像
    Mat grayDilationImage(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    for (int i = (se.rows - 1) / 2; i < grayImage.rows - (se.rows - 1) / 2; i++)
    {
        for (int j = (se.cols - 1) / 2; j < grayImage.cols - (se.cols - 1) / 2; j++)
        {
            //先设置第i行第j列像素值为255，即白色
            grayDilationImage.data[i*grayImage.step + j] = 255;
            for (int row = 0; row < se.rows; row++)
            {
                for (int col = 0; col < se.cols; col++)
                {
                    //把se对应的元素赋值到与se结构相同的矩阵window中
                    window.data[row*window.step + col] = grayImage.data[(i + row - (window.rows - 1) / 2)*grayImage.step + (j + col - (window.cols - 1) / 2)];
                }
            }
            //比较se与window中的像素值
            //在灰度图像中，膨胀是取window中最大的值赋值给原点所对用的像素
            int maxPixel = 0;
            int row, col;
            for (row = 0; row < se.rows; row++)
            {
                for (col = 0; col < se.cols; col++)
                {
                    if (window.data[row*se.step + col] > maxPixel)
                    {
                        maxPixel = window.data[row*se.step + col];
                    }
                }
            }
            grayDilationImage.data[i*grayImage.step + j] = maxPixel;
        }
    }

    /*imshow("grayDilationImage", grayDilationImage);*/

    return grayDilationImage;
}

//二值图像开操作(先腐蚀，后膨胀)
Mat binaryOpen(Mat binaryImage, Mat se)
{
    Mat openImage(binaryImage.rows,binaryImage.cols,CV_8UC1,Scalar(0));	

    openImage = binaryDilation(binaryErosion(binaryImage, se), se);
	openImage=binaryErosion(binaryDilation(openImage, se), se);
	
    return openImage;
}

//二值图像闭操作(先膨胀，后腐蚀)
Mat binaryClose(Mat binaryImage, Mat se)
{
    Mat closeImage(binaryImage.rows, binaryImage.cols, CV_8UC1, Scalar(0));

    closeImage = binaryErosion(binaryDilation(binaryImage, se), se);

    return closeImage;
}

//灰度图像开操作
Mat grayOpen(Mat grayImage, Mat se)
{
    Mat openImage(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    openImage = grayDilation(grayErosion(grayImage, se), se);

    return openImage;
}

//灰度图像闭操作
Mat grayClose(Mat grayImage, Mat se)
{
    Mat closeImage(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    closeImage = grayErosion(grayDilation(grayImage, se), se);

    return closeImage;
}

//二值图像边界提取
Mat binaryBorder(Mat binaryImage,Mat se)
{
    Mat borderImage(binaryImage.rows, binaryImage.cols, CV_8UC1, Scalar(0));
    Mat erosionImage(binaryImage.rows, binaryImage.cols, CV_8UC1, Scalar(0));
    erosionImage = binaryErosion(binaryImage,se);

    for (int i = 0; i < erosionImage.rows; i++)
    {
        for (int j = 0; j < erosionImage.cols; j++)
        {
            if (binaryImage.data[i*erosionImage.step+j]!=erosionImage.data[i*erosionImage.step+j])
            {
                borderImage.data[i*erosionImage.step + j] = 255;
            }
        }
    }

    return borderImage;
}

//灰度图像边界提取
Mat grayBorder(Mat grayImage, Mat se)
{
    Mat borderImage(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    borderImage = grayImage - grayErosion(grayImage, se);

    return borderImage;
}

//灰度图像梯度
Mat gradient(Mat grayImage, Mat se)
{
    Mat gradient(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    gradient = grayDilation(grayImage, se) - grayErosion(grayImage, se);

    return gradient;
}

//灰度图像的顶帽运算 T（f）=f-fob
Mat topHat(Mat grayImage,Mat se)
{
    Mat topHatImage(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    topHatImage = grayImage - grayOpen(grayImage,se);

    return topHatImage;
}

//灰度图像的底帽运算 B（f）=f⋅b-f
Mat bottomHat(Mat grayImage, Mat se)
{
    Mat bottomHatImage(grayImage.rows, grayImage.cols, CV_8UC1, Scalar(0));

    bottomHatImage = grayClose(grayImage, se)-grayImage;

    return bottomHatImage;
}