#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>//inpaint������ͷ�ļ�
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

	//����1����ȡ����kinect��ƥ�亯��������������ӰЧ�������ܲ��������ý����ɫ�����ɸѡ���⣬�����ȼ������ԣ������Լ���д���µ�ƥ��ĳ��򣬻��߿��ǲ��õ���ȥ��������
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

		////����1���֣���ͼ��ת���YCBCRģʽ
		Mat colorBufferMatYCrCb( colorHeight, colorWidth, CV_8UC3);//ת�����ͼ�����
		cvtColor(colorBufferMat,colorBufferMatYCrCb,CV_RGB2YCrCb,3);//�Ժ��������ʱһ��Ҫ�úü������������ξ�����Ϊ��仰�����˺ܳ�ʱ��
		Mat inpaintMask=Mat::zeros(CoordinateMapperMat.size(),CV_8U);//���԰Ѻ�ɫ��ѡ������ʹ����inpaint������Ҫ����opencv

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
		// ����ɫͼ��ת�������ͼ���С
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
						int colorX = static_cast<int>( std::floor( p.X + 0.5 ) );  //std::floor����ȡ��������ֵ�Ǹ�����������ǰ��static_cast<int>����ǿ������ת��Ϊ����������
						int colorY = static_cast<int>( std::floor( p.Y + 0.5 ) );
						if( ( colorX >= 0 ) && ( colorX < colorWidth ) && ( colorY >= 0 ) && ( colorY < colorHeight )){
							
							//����1��ͨ������ifֱ�ӵõ���ɫ���򣬵����ַ�������ֱ�ӽ�����̬ѧ�����޲�����inpaint���������д������ɫ����Ӧ�ò����޲�
							if((colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[2]>139)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[2]<161)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[1]>97)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[1]<128)){//1��cb����2��cr����
							CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorBufferMat.at<cv::Vec4b>( colorY, colorX );  //�����ͼ����ӳ���Ķ�Ӧ�Ĳ�ɫ��Ϣͼ����ƥ��ͼ
							}
							//else{//inpaintֱ�������޸�Ч������
							//	inpaintMask.at<uchar>(y,x)=255;
							//}


							////����2���������ǻ��ǲ��ö�ֵͼ������ʾ����ͨ����̬ѧ�㷨�õ�һЩͼ������ԭͼ������ 
							//CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorBufferMat.at<cv::Vec4b>( colorY, colorX ); 
						}
						
					}
				}
			}
		}
		
		SafeRelease( pDepthFrame );  
		SafeRelease( pColorFrame );
		//��ʾ���֣�����ȥ�������գ�
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
		//����1���֣�inpaint��������
		//cv::imshow("inpaintMask",inpaintMask);
		//Mat colorBuffer( colorHeight, colorWidth, CV_8UC3);//ת�������޸���ͼ�����
		//cvtColor(CoordinateMapperMat,colorBuffer,CV_BGRA2BGR);
		//Mat inpaintedImg;
		//inpaint(colorBuffer,inpaintMask,inpaintedImg,3,INPAINT_TELEA);//�޸�����ͼ��������ʽ��BGR
		//imshow("inpaintImg",inpaintedImg);//�����ܲ�����ͼ��������ʲô���㷨����openCV����
		


		//ͨ�ã������ͼ��ȥ�벿�� Ҫ��16λ���� ȡƽ��

		//ͨ�ã���ͼ�������
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