//����һ�� �����ʾ�ͺã��о�����������������ͦ�鷳�ģ�
//��д�������ģ�Ȼ����һ���ĳ����д��
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "others.h"
//#include <opencv2/photo/photo.hpp>//inpaint������ͷ�ļ�
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
	Mat CoordinateMapperMat;//ƥ���ľ���
	Mat CoordinateMapperMat_TM;//��ģ��ĺ�����������ʾ���
	Mat TwoMat;//��ֵͼ�����
	Mat TwoMatTemp;
	Mat depthDeTemp;//���ͼ��ƽ��ȥ��ľ���
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	void colorProcess();
	void depthProcess();//�÷����ڶ�ȡ16λʱ���ڴ���������,���õ������帳ֵ
	void depthProcess2();//�÷���û�����⣬���õ��Ƿֱ�ֵ
	void skinColorDet();//�����ɫ�Լ���صĶ�ֵȥ��
	void kincetSave(Mat a,string savePath,int opt);//������ģʽѡ�񣬱����ɫ�����
	void kinectSaveAll(string savePath);//ͬʱ�����ɫ�����
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

//������
int main(int argc, char** argv )
{
	Kinect kinect;  

	//Mat depthDeNoise(kinect.DepthHeight, kinect.DepthWidth, CV_16UC1);
	kinect.InitKinect();
	kinect.InitDepth();
	kinect.InitColor();
	kinect.InitCoorMap();
	while(1){
		//************************************ȥ�벿��************************************
		kinect.depthDeTemp = Mat::zeros(kinect.DepthHeight, kinect.DepthWidth, CV_16UC1);//opencv��ʼ���ľ��󲢲�Ϊ�㣬��Ҫ���¹��㣬��������
		for(int i=1;i<=5;i++){
			kinect.depthProcess2();
			add(kinect.depthDeTemp,kinect.depthMat16,kinect.depthDeTemp);
		}
		divide(kinect.depthDeTemp,5,kinect.depthDeTemp);
		//******************************************************************************** �о�ƽ��ȥ��ȥ����һЩ��������������Ҫ��ֵ�˲�ȥȥ��һ����������Ȼ��֪���Ƿ��Ӱ���������������
		//imshow("���ͼ��",kinect.depthMat16); ���ÿ��Կ�����ָ������ȥӰ������
		//imshow("ȥ�����ͼ��",kinect.depthDeTemp);
		/*Mat temp;
		bilateralFilter(kinect.depthDeTemp,temp,25,25/2,25/2);*/
		//�о��ǿ��еģ������������ᵽά���˲�����������Ժ���һ�£��ȱ������������Ҫ������ס��������б���������ص����Ľ��١�
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
		imshow("��ɫͼ��",kinect.colorHalfSizeMat);
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
		//************************************�洢����************************************
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
HRESULT Kinect::InitColor()//������InitKinect()
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

	//hResult = pColorSource->get_FrameDescription( &pColorDescription );//�Ѿ�֪��ֵ�ˣ�������øú���
	//if ( FAILED( hResult ) )
	//{
	//	cerr <<"Error:IColorFrameSource::get_FrameDescription()"<<endl;
	//	return -1;
	//}
}
HRESULT Kinect::InitDepth()//������InitKinect()
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

	//hResult = pDepthSource->get_FrameDescription( &pDepthDescription );//�Ѿ�֪��ֵ�ˣ�������øú���
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
void Kinect::depthProcess()//16λ��ʾ�ᱨ�� ���øú�������
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
	SafeRelease( pDepthFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
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
				USHORT depth = *pBuffer;  //16   �������ֽ�Ϊ��λ���ж�ȡ
				ptrDepth16[j] = depth;        //ֱ�Ӵ洢��ʾ
				ptrDepth8[j] = depth%256; //תΪ8λ�洢��ʾ
				pBuffer++;               //16   �������ֽ�Ϊ��λ���ж�ȡ
			}
		}
	}
	SafeRelease( pDepthFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
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
	SafeRelease( pColorFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
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
				imwrite(savePathTemp,a);//�洢ͼ��
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
				imwrite(savePathTemp,a);//�洢ͼ��
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
				imwrite(savePathTemp,depthDeTemp);//�洢ȥ������ͼ��
				//imwrite(savePathTemp,depthMat8);//�洢ȥ������ͼ��
				saveTmpIntDep++;   
				savePathTemp =savePath;
				sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
				savePathTemp = savePathTemp+"color" +"_"+ saveTmpStrClr + ".png";
				imwrite(savePathTemp,colorHalfSizeMat);//�洢��ɫͼ��
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
				CoordinateMapperMat = Scalar( 0, 0, 0, 0);//��Ҫ����ˢ����ͼƬ
				CoordinateMapperMat_TM = Scalar( 0, 0, 0, 0);//��Ҫ����ˢ����ͼƬ
				for ( int y = 0; y < DepthHeight; y++)
				{
					for ( int x = 0; x < DepthWidth; x++)
					{
						unsigned int index = y * DepthWidth + x;
						ColorSpacePoint p = colorSpacePoints[index];
						int colorX = static_cast<int>( std::floor( p.X + 0.5 ) );  //std::floor����ȡ��������ֵ�Ǹ�����������ǰ��static_cast<int>����ǿ������ת��Ϊ����������
						int colorY = static_cast<int>( std::floor( p.Y + 0.5 ) );
						if( ( colorX >= 0 ) && ( colorX < ColorWidth ) && ( colorY >= 0 ) && ( colorY < ColorHeight )){
							CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorMat.at<cv::Vec4b>( colorY, colorX );  //�����ͼ����ӳ���Ķ�Ӧ�Ĳ�ɫ��Ϣͼ����ƥ��ͼ
							//ͨ������ifֱ�ӵõ���ɫ����
							if((colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[2]>139)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[2]<161)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[1]>97)&&(colorBufferMatYCrCb.at<cv::Vec3b>(colorY, colorX)[1]<128)){//1��cb����2��cr����
							//CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorMat.at<cv::Vec4b>( colorY, colorX );  //�����ͼ����ӳ���Ķ�Ӧ�Ĳ�ɫ��Ϣͼ����ƥ��ͼ
							//������ֵͼ��
							TwoMat.at<uchar>(y,x)=0xff;
							}
						}
					}
				}
			}
		}
	fillHole(TwoMat,TwoMat);//���ն�
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(7,7));
	morphologyEx(TwoMat,TwoMat,CV_MOP_OPEN,element);//���п��������
	//element=getStructuringElement(MORPH_ELLIPSE,Size(1,7));//��ֹ���۾��Ĳⲻ������������������ ���迼�Ƿ������繹�����ӵĺ����������Ч��
	//morphologyEx(TwoMat,TwoMat,CV_MOP_DILATE,element);//�������Ͳ��� �������������������������������ǲ��ü��ÿ����ͨ����ľ�����һ���жϣ���С��һ�����򣬽���������������һ���ر�������Ϻ��µ�
	fillHole(TwoMat,TwoMat);//���ն�
	TwoMatTemp.create(TwoMat.size(),CV_32SC1);//������ʱ��Ҫ������ת����int��
	BwLabel(TwoMat,TwoMatTemp);//������ͨ�����ǣ����ص����Ѿ���źõ�����
	bwLabelNext(TwoMatTemp);//��ͨ�������
	//������ͨ�����ȥ��һ������ͨ���� ��Ҫ�ȱȽ�����ȡ�ķ�ɫ�������ȵĲ�ֵ�����ʲô���
	/*deNoiseLabel(TwoMatTemp);*/
	
}