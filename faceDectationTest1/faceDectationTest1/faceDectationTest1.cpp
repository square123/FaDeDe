#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>//inpaint函数的头文件
#include <iostream>
#include <kinect.h>
#define FilePath "H://testDepth//"
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

int main()
{
	IKinectSensor* pSensor ;
	HRESULT hResult = S_OK ;
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

	IColorFrameSource* pColorSource;
	hResult = pSensor->get_ColorFrameSource( &pColorSource );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_ColorFrameSource()"<<endl;
		return -1;
	}

	IColorFrameReader* pColorReader;
	hResult = pColorSource->OpenReader( &pColorReader );     
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
		return -1; 
	}

	IFrameDescription* pColorDescription;
	hResult = pColorSource->get_FrameDescription( &pColorDescription );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IColorFrameSource::get_FrameDescription()"<<endl;
		return -1;
	}

	int colorHeight = NULL;
	int colorWidth = NULL;
	pColorDescription->get_Height( &colorHeight );
	pColorDescription->get_Width( &colorWidth );
	unsigned int colorBufferSize = colorWidth * colorHeight * 4 * sizeof( unsigned char );

	Mat colorBufferMat( colorHeight, colorWidth, CV_8UC4 );
	Mat colorMat( colorHeight/2, colorWidth/2, CV_8UC4 );
	namedWindow( "color");

	IDepthFrameSource* pDepthSource;
	hResult = pSensor->get_DepthFrameSource( &pDepthSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_DepthFrameSource()"<<endl;
		return -1;
	}

	IDepthFrameReader* pDepthReader;
	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
		return -1;
	}

	IFrameDescription* pDepthDescription;
	hResult = pDepthSource->get_FrameDescription( &pDepthDescription );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::get_FrameDescription()"<<endl;
		return -1;
	}

	int depthWidth = NULL;
	int depthHeight = NULL;
	pDepthDescription->get_Width( &depthWidth );
	pDepthDescription->get_Height( &depthHeight );
	unsigned int depthBufferSize = depthWidth * depthHeight * sizeof( unsigned short );

	Mat depthBufferMat( depthHeight, depthWidth, CV_16UC1 );
	Mat depthMat( depthHeight, depthWidth,CV_8UC1 );
	namedWindow( "depth");

	ICoordinateMapper* pCoordinateMapper;
	hResult = pSensor->get_CoordinateMapper( &pCoordinateMapper );     
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_CoordinateMapper()"<<endl;
		return -1;
	}

	Mat CoordinateMapperMat( depthHeight, depthWidth, CV_8UC4 );
	namedWindow( "CoordinateMapper" );
	int a=0;
	char aa[100];

	//方案1：采取的是kinect的匹配函数，发现其有重影效果，可能并不能良好解决肤色区域的筛选问题，考虑先继续尝试，后续自己编写大致的匹配的程序，或者考虑采用点云去处理人脸
	while (1)
	{
		//Color Frame
		IColorFrame* pColorFrame = nullptr;
		hResult = pColorReader->AcquireLatestFrame( &pColorFrame );      
		if ( SUCCEEDED(hResult) )
		{
			hResult = pColorFrame->CopyConvertedFrameDataToArray( colorBufferSize,
				reinterpret_cast<BYTE*> (colorBufferMat.data), ColorImageFormat::ColorImageFormat_Bgra );
			if ( SUCCEEDED(hResult) )
			{
				resize( colorBufferMat, colorMat, Size(), 0.5, 0.5 );
			}
		}

		////方法1部分：将图像转变成YCBCR模式
		Mat colorBufferMatYCrCb( colorHeight, colorWidth, CV_8UC3);//转换后的图像矩阵
		cvtColor(colorBufferMat,colorBufferMatYCrCb,CV_RGB2YCrCb,3);//以后出现问题时一定要好好检查程序的算理，这次就是因为这句话耽误了很长时间
		Mat inpaintMask=Mat::zeros(CoordinateMapperMat.size(),CV_8U);//尝试把黑色的选出来，使用下inpaint函数，要恶补下opencv

		//Depth Frame
		IDepthFrame* pDepthFrame = nullptr;
		hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
		if ( SUCCEEDED(hResult) )
		{
			hResult = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, reinterpret_cast<UINT16**>(&depthBufferMat.data));
			if ( SUCCEEDED(hResult) )
			{
				depthBufferMat.convertTo( depthMat, CV_8U, -255.0f/8000.0f, 255.0f );
			}

		}
		// 将彩色图像转换成深度图像大小
		if ( SUCCEEDED(hResult) )
		{
			vector<ColorSpacePoint> colorSpacePoints( depthHeight*depthWidth );
			hResult = pCoordinateMapper->MapDepthFrameToColorSpace(depthWidth*depthHeight,reinterpret_cast<UINT16*>(depthBufferMat.data),depthWidth*depthHeight,&colorSpacePoints[0] );
			if ( SUCCEEDED(hResult) )
			{
				CoordinateMapperMat = Scalar( 0, 0, 0, 0);
				for ( int y = 0; y < depthHeight; y++)
				{
					for ( int x = 0; x < depthWidth; x++)
					{
						unsigned int index = y * depthWidth + x;
						ColorSpacePoint p = colorSpacePoints[index];
						int colorX = static_cast<int>( std::floor( p.X + 0.5 ) );  //std::floor向下取整，返回值是浮点数，但是前面static_cast<int>对其强制类型转换为整形来返回
						int colorY = static_cast<int>( std::floor( p.Y + 0.5 ) );
						if( ( colorX >= 0 ) && ( colorX < colorWidth ) && ( colorY >= 0 ) && ( colorY < colorHeight )){
							
							//方法1：通过下述if直接得到肤色区域，但这种方法不能直接进行形态学操作修补，用inpaint方法优于有大面积黑色区域应该不能修补
							if((colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[2]>139)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[2]<161)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[1]>97)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[1]<128)){//1是cb分量2是cr分量
							CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorBufferMat.at<cv::Vec4b>( colorY, colorX );  //将深度图坐标映射后的对应的彩色信息图赋给匹配图
							}
							//else{//inpaint直接这样修复效果不好
							//	inpaintMask.at<uchar>(y,x)=255;
							//}


							////方法2：经过考虑还是采用二值图像来表示，先通过形态学算法得到一些图像再与原图像相与 
							//CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorBufferMat.at<cv::Vec4b>( colorY, colorX ); 
						}
						
					}
				}
			}
		}
		
		SafeRelease( pDepthFrame );  
		SafeRelease( pColorFrame );
		//显示部分（可以去掉，最终）
		cv::imshow( "depth", depthMat );
		cv::imshow( "color", colorMat );
		cv::imshow( "CoordinateMapper", CoordinateMapperMat );
		string savePathTemp =FilePath;
		if (GetKeyState(VK_DELETE)<0){
			sprintf_s(aa,sizeof(aa),"%03d",a); 
			savePathTemp = savePathTemp + "1" +"_"+ aa+ ".png";
			imwrite(savePathTemp,depthMat);
			savePathTemp =FilePath;
			savePathTemp = savePathTemp + "2" +"_"+ aa+ ".png";
			imwrite(savePathTemp,colorMat);
			savePathTemp =FilePath;
			savePathTemp = savePathTemp + "3" +"_"+ aa+ ".png";
			imwrite(savePathTemp,CoordinateMapperMat);
			a++;   
			savePathTemp =FilePath;
		}
		//方法1部分：inpaint函数尝试
		//cv::imshow("inpaintMask",inpaintMask);
		//Mat colorBuffer( colorHeight, colorWidth, CV_8UC3);//转换用来修复的图像矩阵
		//cvtColor(CoordinateMapperMat,colorBuffer,CV_BGRA2BGR);
		//Mat inpaintedImg;
		//inpaint(colorBuffer,inpaintMask,inpaintedImg,3,INPAINT_TELEA);//修复函数图像的输入格式是BGR
		//imshow("inpaintImg",inpaintedImg);//尝试能不能用图像或者填充什么的算法，在openCV上找
		


		//通用：将深度图像去噪部分 要存16位数据 取平均

		//通用：将图像存下来
		if ( waitKey(27) == 27 )
		{
			break;
		}  
	} 

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
	return 0;
}	