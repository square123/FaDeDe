#include "cv.h"  
using namespace cv;  
using namespace std;  

void fillHole(const Mat srcBw, Mat &dstBw)//参考网上的填充函数，很巧妙
{
    Size m_Size = srcBw.size();
    Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//延展图像
    srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
 
    cv::floodFill(Temp, Point(0, 0), Scalar(255));//非常巧妙，用了填充算法的反运算
 
    Mat cutImg;//裁剪延展的图像
    Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
 
    dstBw = srcBw | (~cutImg);
}

void  BwLabel( const Mat &bwImg, Mat &labImg )  //参考网上的连通区域标记函数
{  
    assert( bwImg.type()==CV_8UC1 );  
    labImg.create( bwImg.size(), CV_32SC1 ); 
    labImg = Scalar(0);  
    labImg.setTo( Scalar(1), bwImg );  
    assert( labImg.isContinuous() );  
    const int Rows = bwImg.rows - 1, Cols = bwImg.cols - 1;  
    int label = 1;   
    vector<int> labelSet;  
    labelSet.push_back(0);    
    labelSet.push_back(1);    
    // the first pass  
    int *data_prev = (int*)labImg.data; // 0-th row : int* data_prev = labImg.ptr<int>(i-1);  
    int *data_cur = (int*)( labImg.data + labImg.step ); // 1-st row : int* data_cur = labImg.ptr<int>(i);  
    for( int i=1; i<Rows; i++ ){  
        data_cur++;  
        data_prev++;  
        for( int j=1; j<Cols; j++, data_cur++, data_prev++ ){  
            if( *data_cur!=1 )  
                continue;  
            int left = *(data_cur-1);  
            int up = *data_prev;  
            int neighborLabels[2];  
            int cnt = 0;  
            if( left>1 )  
                neighborLabels[cnt++] = left;  
            if( up>1 )  
                neighborLabels[cnt++] = up;  
            if( !cnt ){  
                labelSet.push_back( ++label );  
                labelSet[label] = label;  
                *data_cur = label;  
                continue;  
            }  
            int smallestLabel = neighborLabels[0];  
            if( cnt==2 && neighborLabels[1]<smallestLabel )  
                smallestLabel = neighborLabels[1];  
            *data_cur = smallestLabel;  
            // 保存最小等价表  
            for( int k=0; k<cnt; k++ ){  
                int tempLabel = neighborLabels[k];  
                int& oldSmallestLabel = labelSet[tempLabel];  
                if( oldSmallestLabel > smallestLabel ){                            
                    labelSet[oldSmallestLabel] = smallestLabel;  
                    oldSmallestLabel = smallestLabel;  
                }                         
                else if( oldSmallestLabel<smallestLabel )  
                    labelSet[smallestLabel] = oldSmallestLabel;  
            }  
        }  
        data_cur++;  
        data_prev++;  
    }  
    // 更新等价对列表,将最小标号给重复区域  
    for( size_t i = 2; i < labelSet.size(); i++ ){  
        int curLabel = labelSet[i];  
        int preLabel = labelSet[curLabel];  
        while( preLabel!=curLabel ){  
            curLabel = preLabel;  
            preLabel = labelSet[preLabel];  
        }  
        labelSet[i] = curLabel;  
    }  
    // second pass  
    data_cur = (int*)labImg.data;  
    for( int i=0; i<Rows; i++ ){  
        for( int j=0; j<bwImg.cols-1; j++, data_cur++ )  
            *data_cur = labelSet[ *data_cur ];    
        data_cur++;  
    }  
}

void bwLabelNext(Mat &src)//作为上述连通区域标记的后续处理
{
	int a[500]={0};//设置连通区域的个数最多为500个
	for(int i=0;i<src.rows;i++)
	{
		int* data= src.ptr<int>(i);
		for(int j=0;j<src.cols;j++)
		{
			if(data[j]!=0){//寻找元素的种类
				for(int k=0;k<500;k++)
				{
					if (data[j]==a[k])
					{
						data[j]=k+1;//这里简化运算直接重新赋值，一次性循环处理完成
						break;
					}
					else if(a[k]==0)
					{
						a[k]=data[j];
						data[j]=k+1;
						break; 
					}
				}
			}
		}
	}
}

int maxMatNum(Mat &src)//确定矩阵中的最大值,考虑速度，使用指针方式迭代
{
	int maxNum=0;
	for(int i=0;i<src.rows;i++)
	{
		int* data= src.ptr<int>(i);//输入数据格式是int型的，无需考虑其他函数类型，用其他类型的数据进行运算没有实际意义，所以无需用模板去写。
		for(int j=0;j<src.cols;j++){
			maxNum=(maxNum>data[j])?maxNum:data[j];
		}
	}
	return maxNum;
}

int areaMatForSpecNum(Mat &src,int num)//返回矩阵中特定值的面积
{
	int areaSum=0;
	Mat srcTemp=(src==(num));//此时的数据格式转换成UC1
	for(int i=0;i<srcTemp.rows;i++)
	{
		uchar* data= srcTemp.ptr<uchar>(i);
		for(int j=0;j<srcTemp.cols;j++){
			uchar a=data[j];
			areaSum=areaSum+int(a);
		}
	}
	return areaSum=areaSum/255;
}

int sumMat( Mat &src)//求和，必须是单通道的 注意这里数据格式是int型的
{
	Mat srcTemp=src.clone();
	int matSumNum=0;
	for(int i=0;i<srcTemp.rows;i++)
	{
		int* data= srcTemp.ptr<int>(i);
		for(int j=0;j<srcTemp.cols;j++){
			int a=data[j];
			matSumNum=matSumNum+a;
		}
	}
	return matSumNum;
}

int avgMat(Mat &src){//求平均，必须是单通道的 注意这里数据格式是int型的
	int h,hh,mm;
	h=src.rows*src.cols;
	hh=sumMat(src);
	if(h!=0)//对于除法一定要考虑除数为零的情况
	{
		mm=hh/h;
		return mm;
	}
	else
	{
		return 0;
	}
}

void deNoiseLabel( Mat &src)//连通区域去噪,去除面积较小和结构不太合理的值。, Mat &dst
{
	int maxNum;
	maxNum=maxMatNum(src);//得到连通标记区域的最大值为多少,进而得知一共标记了多少连通区域
	Mat saveArea(1,maxNum,CV_32SC1);//开辟一个数组，存储对应标号的每个连通区域的面积,避免构建动态数组
	int* data= saveArea.ptr<int>(0);
	for (int index=0;index<maxNum;index++)
	{
		data[index]=areaMatForSpecNum(src,(index+1));
	}
	int avgNum;
	avgNum=avgMat(saveArea);
	//cout<<maxNum<<" "<<saveArea<<" "<<avgNum<<endl;//用于测试

	//问题：连通区域标记的个数再检查，计算面积的最大值是否考虑的是黑色，能否考虑下先用深度剔除一部分区域，或者直接进行圆形检测。

}