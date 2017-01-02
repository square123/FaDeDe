//尝试一下 完成显示就好，感觉许多用这个操作还是挺麻烦的，
//先写出正常的，然后再一点点改成类的写法
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "others.h"
//#include <opencv2/photo/photo.hpp>//inpaint函数的头文件
#include <iostream>
#include <strsafe.h>
#include <string>
#include <kinect.h>

#define FilePath "H://testColor//"
#define saveDepth 1
#define saveColor 2
using namespace cv;
using namespace std;

template<class Interface> 
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
	if ( pInterfaceToRelease != NULL )
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}



class Kinect
{
public:
	static const int DepthWidth = 512;
	static const int DepthHeight = 424;
	static const int ColorWidth = 1920;
	static const int ColorHeight = 1080;
	char saveTmpStrDep[1000];
	int saveTmpIntDep;
	char saveTmpStrClr[1000];
	int saveTmpIntClr;
	
	Mat depthMat8;
	Mat depthMat16;
	Mat colorMat;
	Mat colorHalfSizeMat;
	Mat CoordinateMapperMat;//匹配后的矩阵
	Mat CoordinateMapperMat_TM;//掩模后的函数，用于显示结果
	Mat TwoMat;//二值图像矩阵
	Mat TwoMatTemp;
	Mat depthDeTemp;//深度图像平均去噪的矩阵
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	void colorProcess();
	void depthProcess();//该方法在读取16位时，内存会出现问题,采用的是整体赋值
	void depthProcess2();//该方法没有问题，采用的是分别赋值
	void skinColorDet();//处理肤色以及相关的二值去噪
	void kincetSave(Mat a,string savePath,int opt);//有两种模式选择，保存彩色或深度
	void kinectSaveAll(string savePath);//同时保存彩色和深度
	Kinect();
	~Kinect();

private:
	IKinectSensor* pSensor ;
	IColorFrameSource* pColorSource;
	IColorFrameReader* pColorReader;
	IFrameDescription* pColorDescription;
	IColorFrame* pColorFrame;
	IDepthFrameSource* pDepthSource;
	IDepthFrameReader* pDepthReader;
	IFrameDescription* pDepthDescription;
	IDepthFrame* pDepthFrame;
	ICoordinateMapper* pCoordinateMapper;
};

//主程序
int main(int argc, char** argv )
{
	Kinect kinect;  

	//Mat depthDeNoise(kinect.DepthHeight, kinect.DepthWidth, CV_16UC1);
	kinect.InitKinect();
	kinect.InitDepth();
	kinect.InitColor();
	kinect.InitCoorMap();
	while(1){
		//************************************去噪部分************************************
		kinect.depthDeTemp = Mat::zeros(kinect.DepthHeight, kinect.DepthWidth, CV_16UC1);//opencv初始化的矩阵并不为零，需要重新归零，否则会出错
		for(int i=1;i<=5;i++){
			kinect.depthProcess2();
			add(kinect.depthDeTemp,kinect.depthMat16,kinect.depthDeTemp);
		}
		divide(kinect.depthDeTemp,5,kinect.depthDeTemp);
		//******************************************************************************** 感觉平均去噪去除了一些噪声，但还是需要中值滤波去去除一下噪声。虽然不知道是否会影响人脸的深度数据
		//imshow("深度图像",kinect.depthMat16); 觉得可以考虑用指数函数去影响数据
		//imshow("去噪深度图像",kinect.depthDeTemp);
		/*Mat temp;
		bilateralFilter(kinect.depthDeTemp,temp,25,25/2,25/2);*/
		//感觉是可行的，论文里面有提到维纳滤波，这个可以以后考虑一下，等报告结束后还是需要多读文献。文献中有宝，中文相关的论文较少。
		medianBlur(kinect.depthDeTemp,kinect.depthDeTemp,3);
		
		Mat gradX,gradY,absGradX,absGradY,dst;
		Scharr(kinect.depthDeTemp,gradX,CV_16UC1,1,0,1,0,BORDER_DEFAULT);
		convertScaleAbs(gradX,absGradX);
		imshow("x",absGradX);
		Scharr(kinect.depthDeTemp,gradY,CV_16UC1,0,1,1,0,BORDER_DEFAULT);
		convertScaleAbs(gradY,absGradY);
		imshow("y",absGradY);
		addWeighted(absGradX,0.5,absGradY,0.5,0,dst);
		imshow("he",dst);
		/*kinect.kincetSave(dst,FilePath,saveDepth);*/
		
	/*	medianBlur(temp1,temp,5);
		Canny(temp,temp,3,9,3);*/
		
		kinect.colorProcess();
		imshow("彩色图像",kinect.colorHalfSizeMat);
		imshow("dddddd",kinect.depthMat8);
		kinect.kincetSave(kinect.colorHalfSizeMat,FilePath,saveColor);
		kinect.kincetSave(kinect.depthMat8,FilePath,saveDepth);
		///delete
	/*	kinect.skinColorDet();
		Mat colorBufferMatYCrCb(kinect.DepthHeight,kinect.DepthWidth,CV_8UC3);
		cvtColor(kinect.CoordinateMapperMat,colorBufferMatYCrCb,CV_RGB2YCrCb,3);
		imshow("colorBufferMatYCrCb",colorBufferMatYCrCb);
		imshow("fse",kinect.CoordinateMapperMat);
		imshow("ddd",kinect.depthMat8);
		imshow("ddddd",kinect.TwoMat);*/
		//////


		//imshow("fuse",kinect.CoordinateMapperMat_TM);
		//imshow("erzhi",kinect.TwoMat);
		/*kinect.kincetSave(kinect.TwoMatTemp,FilePath,saveDepth);*/
	/*	imshow("edge",kinect.TwoMatTemp);*/
		//************************************存储部分************************************
		//kinect.kinectSaveAll(FilePath);
		//kinect.kincetSave(kinect.depthMat8,FilePath,saveDepth);
		//********************************************************************************
		if(waitKey(3)==27)
		{
			break;
		}
	}
	kinect.~Kinect();

}

Kinect::Kinect()  
{
	pColorFrame = nullptr;
	pDepthFrame = nullptr;
	colorHalfSizeMat.create(ColorHeight/2, ColorWidth/2, CV_8UC4);
	colorMat.create( ColorHeight, ColorWidth, CV_8UC4);
	depthMat16.create(DepthHeight, DepthWidth, CV_16UC1);
	depthMat8.create(DepthHeight, DepthWidth, CV_8UC1 );
	
	saveTmpIntClr=0;
	saveTmpIntDep=0;
}
Kinect::~Kinect()
{
	SafeRelease( pDepthSource );
	SafeRelease( pDepthReader );
	SafeRelease( pDepthDescription );
	SafeRelease( pColorSource );
	SafeRelease( pColorReader );
	SafeRelease( pColorDescription );
	SafeRelease( pCoordinateMapper );
	if ( pSensor )
	{
		pSensor->Close();
	}
	SafeRelease( pSensor );
	cv::destroyAllWindows();
}
HRESULT Kinect::InitKinect()
{
	hResult = GetDefaultKinectSensor( &pSensor );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:GetDefaultKinectSensor" <<endl;
		return -1;
	}
	hResult = pSensor->Open();
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::Open()" <<endl;
		return -1;
	}
}
HRESULT Kinect::InitColor()//必须先InitKinect()
{
	hResult = pSensor->get_ColorFrameSource( &pColorSource );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_ColorFrameSource()"<<endl;
		return -1;
	}
	hResult = pColorSource->OpenReader( &pColorReader );     
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
		return -1; 
	}

	//hResult = pColorSource->get_FrameDescription( &pColorDescription );//已经知道值了，无需调用该函数
	//if ( FAILED( hResult ) )
	//{
	//	cerr <<"Error:IColorFrameSource::get_FrameDescription()"<<endl;
	//	return -1;
	//}
}
HRESULT Kinect::InitDepth()//必须先InitKinect()
{
	hResult = pSensor->get_DepthFrameSource( &pDepthSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_DepthFrameSource()"<<endl;
		return -1;
	}
	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
		return -1;
	}

	//hResult = pDepthSource->get_FrameDescription( &pDepthDescription );//已经知道值了，无需调用该函数
	//if ( FAILED( hResult ) )
	//{
	//	cerr <<"Error:IDepthFrameSource::get_FrameDescription()"<<endl;
	//	return -1;
	//}
}
HRESULT Kinect::InitCoorMap()
{
	hResult = pSensor->get_CoordinateMapper( &pCoordinateMapper );     
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_CoordinateMapper()"<<endl;
		return -1;
	}
}
void Kinect::depthProcess()//16位显示会报错 不用该函数处理
{
	unsigned int depthBufferSize = DepthWidth * DepthHeight * sizeof( unsigned short );
	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, reinterpret_cast<UINT16**>(&depthMat16.data));
		if ( SUCCEEDED(hResult) )
		{
			depthMat16.convertTo(depthMat8, CV_8U, -255.0f/8000.0f, 255.0f );
		}

	}
	SafeRelease( pDepthFrame );  //这个frame一定要释放，不然无法更新
}
void Kinect::depthProcess2()
{
	UINT16 *pBuffer = NULL;
	UINT nBufferSize = 0; 
	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		for(int i=0;i<DepthHeight;i++)
		{
			ushort* ptrDepth16 = depthMat16.ptr<ushort>(i);  // 16
			uchar* ptrDepth8 = depthMat8.ptr<uchar>(i);  //8
			for(int j=0;j<DepthWidth;j++)
			{
				USHORT depth = *pBuffer;  //16   以两个字节为单位进行读取
				ptrDepth16[j] = depth;        //直接存储显示
				ptrDepth8[j] = depth%256; //转为8位存储显示
				pBuffer++;               //16   以两个字节为单位进行读取
			}
		}
	}
	SafeRelease( pDepthFrame );  //这个frame一定要释放，不然无法更新
}
void Kinect::colorProcess()
{
	unsigned int colorBufferSize = ColorWidth * ColorHeight * 4 * sizeof( unsigned char );
	hResult = pColorReader->AcquireLatestFrame( &pColorFrame );      
	if ( SUCCEEDED(hResult) )
	{
		hResult = pColorFrame->CopyConvertedFrameDataToArray( colorBufferSize,
			reinterpret_cast<BYTE*> (colorMat.data), ColorImageFormat::ColorImageFormat_Bgra );
		if ( SUCCEEDED(hResult) )
		{
			resize( colorMat,colorHalfSizeMat , Size(), 0.5, 0.5 );
		}
	}
	SafeRelease( pColorFrame );  //这个frame一定要释放，不然无法更新
}
void Kinect::kincetSave(Mat a,string savePath,int opt)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	char optTemp[2];
	sprintf_s(optTemp,sizeof(optTemp),"%d",opt);
	string savePathTemp=savePath;
	switch (opt)
	{
	case saveDepth:
		{
			if( GetKeyState( VK_NUMPAD1 ) < 0 ){
				sprintf_s(saveTmpStrDep,sizeof(saveTmpStrDep),"%03d",saveTmpIntDep); 
				savePathTemp = savePathTemp +  optTemp +"_"+ saveTmpStrDep + ".png";
				imwrite(savePathTemp,a);//存储图像
				saveTmpIntDep++;   
				savePathTemp =savePath;
			}
			break;
		}
	case saveColor:
		{
			if( GetKeyState( VK_NUMPAD2 ) < 0 ){
				sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
				savePathTemp = savePathTemp + optTemp +"_"+ saveTmpStrClr + ".png";
				imwrite(savePathTemp,a);//存储图像
				saveTmpIntClr++;   
				savePathTemp =savePath;
			}
			break;
		}
	}
}
void Kinect::kinectSaveAll(string savePath)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	string savePathTemp=savePath;
	if( GetKeyState( VK_RETURN) < 0 ){
				sprintf_s(saveTmpStrDep,sizeof(saveTmpStrDep),"%03d",saveTmpIntDep); 
				savePathTemp = savePathTemp +"depth" +"_"+ saveTmpStrDep + ".png";
				imwrite(savePathTemp,depthDeTemp);//存储去噪的深度图像
				//imwrite(savePathTemp,depthMat8);//存储去噪的深度图像
				saveTmpIntDep++;   
				savePathTemp =savePath;
				sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
				savePathTemp = savePathTemp+"color" +"_"+ saveTmpStrClr + ".png";
				imwrite(savePathTemp,colorHalfSizeMat);//存储彩色图像
				saveTmpIntClr++;   
				savePathTemp =savePath;
			}
}
void Kinect::skinColorDet()
{
	Mat colorBufferMatYCrCb( ColorHeight, ColorWidth, CV_8UC3);
	CoordinateMapperMat.create(DepthHeight,DepthWidth,CV_8UC4);
	CoordinateMapperMat_TM.create(DepthHeight,DepthWidth,CV_8UC4);
	TwoMat=Mat::zeros(DepthHeight,DepthWidth,CV_8UC1);
	cvtColor(colorMat,colorBufferMatYCrCb,CV_RGB2YCrCb,3);

	if ( SUCCEEDED(hResult) )
		{
			vector<ColorSpacePoint> colorSpacePoints( DepthHeight*DepthWidth );
			hResult = pCoordinateMapper->MapDepthFrameToColorSpace(DepthWidth*DepthHeight,reinterpret_cast<UINT16*>(depthDeTemp.data),DepthWidth*DepthHeight,&colorSpacePoints[0] );
			if ( SUCCEEDED(hResult) )
			{
				CoordinateMapperMat = Scalar( 0, 0, 0, 0);//需要重新刷新下图片
				CoordinateMapperMat_TM = Scalar( 0, 0, 0, 0);//需要重新刷新下图片
				for ( int y = 0; y < DepthHeight; y++)
				{
					for ( int x = 0; x < DepthWidth; x++)
					{
						unsigned int index = y * DepthWidth + x;
						ColorSpacePoint p = colorSpacePoints[index];
						int colorX = static_cast<int>( std::floor( p.X + 0.5 ) );  //std::floor向下取整，返回值是浮点数，但是前面static_cast<int>对其强制类型转换为整形来返回
						int colorY = static_cast<int>( std::floor( p.Y + 0.5 ) );
						if( ( colorX >= 0 ) && ( colorX < ColorWidth ) && ( colorY >= 0 ) && ( colorY < ColorHeight )){
							CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorMat.at<cv::Vec4b>( colorY, colorX );  //将深度图坐标映射后的对应的彩色信息图赋给匹配图
							//通过下述if直接得到肤色区域
							if((colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[2]>139)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[2]<161)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[1]>97)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[1]<128)){//1是cb分量2是cr分量
							//CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorMat.at<cv::Vec4b>( colorY, colorX );  //将深度图坐标映射后的对应的彩色信息图赋给匹配图
							//构建二值图像
							TwoMat.at<uchar>(y,x)=0xff;
							}
						}
					}
				}
			}
		}
	fillHole(TwoMat,TwoMat);//填充空洞
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(7,7));
	morphologyEx(TwoMat,TwoMat,CV_MOP_OPEN,element);//进行开运算操作
	//element=getStructuringElement(MORPH_ELLIPSE,Size(1,7));//防止带眼睛的测不出来。。。。。。。 仍需考虑方法比如构建连接的函数，聚类的效果
	//morphologyEx(TwoMat,TwoMat,CV_MOP_DILATE,element);//进行膨胀操作 。。。。。。。。。。。。。。考虑采用检测每个连通区域的距离做一个判断，若小于一个区域，将两个区域连接在一起，特别是针对上和下的
	fillHole(TwoMat,TwoMat);//填充空洞
	TwoMatTemp.create(TwoMat.size(),CV_32SC1);//在运算时需要将数据转化成int型
	BwLabel(TwoMat,TwoMatTemp);//进行连通区域标记，返回的是已经标号好的数组
	bwLabelNext(TwoMatTemp);//连通区域后处理
	//考虑先通过深度去除一部分连通区域 需要先比较下提取的肤色区域的深度的差值大概在什么情况
	/*deNoiseLabel(TwoMatTemp);*/
	
}