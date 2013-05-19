//// sift_test.cpp : 定义控制台应用程序的入口点。
//
// #include <stdio.h>
// #include <iostream>
// #include "opencv2/core/core.hpp"//因为在属性中已经配置了opencv等目录，所以把其当成了本地目录一样
// #include "opencv2/features2d/features2d.hpp"
// #include "opencv2/highgui/highgui.hpp"
// #include "opencv2/legacy/legacy.hpp"
// #include "opencv2/nonfree/features2d.hpp"
// using namespace cv;
// using namespace std;
// 
// void readme();
// 
// int main(int argc,char* argv[])
// {
//   Mat img_1=imread("海信2.jpg",CV_LOAD_IMAGE_GRAYSCALE);//宏定义时CV_LOAD_IMAGE_GRAYSCALE=0，也就是读取灰度图像
//   Mat img_2=imread("海信.jpg",CV_LOAD_IMAGE_GRAYSCALE);//一定要记得这里路径的斜线方向，这与Matlab里面是相反的
//   
//   if(!img_1.data || !img_2.data)//如果数据为空
//   {
//       cout<<"opencv error"<<endl;
//       return -1;
//   }
//   cout<<"open right"<<endl;
// 
//   //第一步，用SIFT算子检测关键点
//   
//   SiftFeatureDetector detector;//构造函数采用内部默认的
//   std::vector<KeyPoint> keypoints_1,keypoints_2;//构造2个专门由点组成的点向量用来存储特征点
// 
//   detector.detect(img_1,keypoints_1);//将img_1图像中检测到的特征点存储起来放在keypoints_1中
//   detector.detect(img_2,keypoints_2);//同理
// 
//   //在图像中画出特征点
//   Mat img_keypoints_1,img_keypoints_2;
// 
//   drawKeypoints(img_1,keypoints_1,img_keypoints_1,Scalar::all(-1),DrawMatchesFlags::DEFAULT);//在内存中画出特征点
//   drawKeypoints(img_2,keypoints_2,img_keypoints_2,Scalar::all(-1),DrawMatchesFlags::DEFAULT);
// 
//   imshow("sift_keypoints_1",img_keypoints_1);//显示特征点
//   imshow("sift_keypoints_2",img_keypoints_2);
// 
//   //计算特征向量
//   SiftDescriptorExtractor extractor;//定义描述子对象
// 
//   Mat descriptors_1,descriptors_2;//存放特征向量的矩阵
// 
//   extractor.compute(img_1,keypoints_1,descriptors_1);//计算特征向量
//   extractor.compute(img_2,keypoints_2,descriptors_2);
// 
//   //用burte force进行匹配特征向量
//   BruteForceMatcher<L2<float>>matcher;//定义一个burte force matcher对象
//   vector<DMatch>matches;
//   matcher.match(descriptors_1,descriptors_2,matches);
// 
//   //绘制匹配线段
//   Mat img_matches;
//   drawMatches(img_1,keypoints_1,img_2,keypoints_2,matches,img_matches);//将匹配出来的结果放入内存img_matches中
// 
//   //显示匹配线段
//   imshow("sift_Matches",img_matches);//显示的标题为Matches
//   waitKey(0);
//   return 0;
// }

// surf_test.cpp : 定义控制台应用程序的入口点。
//
//
//#include <stdio.h>
//#include <iostream>
//#include "opencv2/core/core.hpp"//因为在属性中已经配置了opencv等目录，所以把其当成了本地目录一样
//#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/legacy/legacy.hpp"
//#include "opencv2/nonfree/features2d.hpp"
//
//using namespace cv;
//using namespace std;
//
//void readme();
//
//int main(int argc,char* argv[])
//{
//	Mat img_1=imread("海信2.jpg",CV_LOAD_IMAGE_GRAYSCALE);//宏定义时CV_LOAD_IMAGE_GRAYSCALE=0，也就是读取灰度图像
//	Mat img_2=imread("海信.jpg",CV_LOAD_IMAGE_GRAYSCALE);//一定要记得这里路径的斜线方向，这与Matlab里面是相反的
//
//	if(!img_1.data || !img_2.data)//如果数据为空
//	{
//		cout<<"opencv error"<<endl;
//		return -1;
//	}
//	cout<<"open right"<<endl;
//
//	//第一步，用SURF算子检测关键点
//	int minHessian=400;
//
//	SurfFeatureDetector detector(minHessian);
//	std::vector<KeyPoint> keypoints_1,keypoints_2;//构造2个专门由点组成的点向量用来存储特征点
//
//	detector.detect(img_1,keypoints_1);//将img_1图像中检测到的特征点存储起来放在keypoints_1中
//	detector.detect(img_2,keypoints_2);//同理
//
//	//在图像中画出特征点
//	Mat img_keypoints_1,img_keypoints_2;
//
//	drawKeypoints(img_1,keypoints_1,img_keypoints_1,Scalar::all(-1),DrawMatchesFlags::DEFAULT);
//	drawKeypoints(img_2,keypoints_2,img_keypoints_2,Scalar::all(-1),DrawMatchesFlags::DEFAULT);
//
//	imshow("surf_keypoints_1",img_keypoints_1);
//	imshow("surf_keypoints_2",img_keypoints_2);
//
//	//计算特征向量
//	SurfDescriptorExtractor extractor;//定义描述子对象
//
//	Mat descriptors_1,descriptors_2;//存放特征向量的矩阵
//
//	extractor.compute(img_1,keypoints_1,descriptors_1);
//	extractor.compute(img_2,keypoints_2,descriptors_2);
//
//	//用burte force进行匹配特征向量
//	BruteForceMatcher<L2<float>>matcher;//定义一个burte force matcher对象
//	vector<DMatch>matches;
//	matcher.match(descriptors_1,descriptors_2,matches);
//
//	//绘制匹配线段
//	Mat img_matches;
//	drawMatches(img_1,keypoints_1,img_2,keypoints_2,matches,img_matches);//将匹配出来的结果放入内存img_matches中
//
//	//显示匹配线段
//	imshow("surf_Matches",img_matches);//显示的标题为Matches
//	waitKey(0);
//	return 0;
//}
 // 
 //  #include <stdio.h>
 //  #include <tchar.h>
 //  #include <opencv2/opencv.hpp>
 //  #include <stdio.h>
 //  #include <string.h>
 //  #include <ctype.h>
 //  #pragma comment(lib, "opencv_core243d.lib")
 //  #pragma comment(lib, "opencv_imgproc243d.lib")
 //  #pragma comment(lib, "opencv_highgui243d.lib")
 //  #pragma comment(lib, "opencv_objdetect243d.lib")
 //  
 //  using namespace cv;
 //  using namespace std;
 //  
 //  extern "C"{
 //  #include <vl/generic.h>
 //  #include <vl/stringop.h>
 //  #include <vl/pgm.h>
 //  #include <vl/sift.h>
 //  #include <vl/getopt_long.h>
 //  };
 //  
 //  int _tmain()
 //  {
 //  	VL_PRINT ("Hello world!\n") ;
 //  	char *ImagePath="雨润食品.jpg";
 //  	IplImage *Image=cvLoadImage(ImagePath,0);
 //  	//  int min=0;
 //  	//  min=Image->width>Image->height?Image->height:Image->width;
 //  	int noctaves=4,nlevels=2,o_min=0;
 //  	// noctaves=(int)(log(min)/log(2));
 //  	vl_sift_pix *ImageData=new vl_sift_pix[Image->height*Image->width];
 //  	unsigned char *Pixel;
 //  	for (int i=0;i<Image->height;i++)
 //  	{
 //  		for (int j=0;j<Image->width;j++)
 //  		{
 //  			Pixel=(unsigned char*)(Image->imageData+i*Image->width+j);
 //  			ImageData[i*Image->width+j]=*(Pixel);
 //  		}
 //  	}
 //  	VlSiftFilt *SiftFilt=NULL;
 //  	SiftFilt=vl_sift_new(Image->width,Image->height,noctaves,nlevels,o_min);
 //  	int KeyPoint=0;
 // 	int totalKey = 0;
 //  	int idx=0;
 //  	if (vl_sift_process_first_octave(SiftFilt,ImageData)!=VL_ERR_EOF)
 //  	{
 //  		while (TRUE)
 //  		{
 //  			//计算每组中的关键点
 //  			vl_sift_detect(SiftFilt);
 //  			//遍历并绘制每个点
 //  			KeyPoint+=SiftFilt->nkeys;
 // 			totalKey +=SiftFilt->nkeys;
 //  			VlSiftKeypoint *pKeyPoint=SiftFilt->keys;
 //  			for (int i=0;i<SiftFilt->nkeys;i++)
 //  			{
 //  				VlSiftKeypoint TemptKeyPoint=*pKeyPoint;
 //  				pKeyPoint++;
 //  				cvDrawCircle(Image,cvPoint(TemptKeyPoint.x,TemptKeyPoint.y),TemptKeyPoint.sigma/2,CV_RGB(255,0,0));
 //  				idx++;
 //  				//计算并遍历每个点的方向
 //  				double angles[4];
 //  				int angleCount=vl_sift_calc_keypoint_orientations(SiftFilt,angles,&TemptKeyPoint);
 //  				for (int j=0;j<angleCount;j++)
 //  				{
 //  					double TemptAngle=angles[j];
 //  					printf("%d: %f\n",j,TemptAngle);
 //  					//计算每个方向的描述
 //  					float *Descriptors=new float[128];
 //  					vl_sift_calc_keypoint_descriptor(SiftFilt,Descriptors,&TemptKeyPoint,TemptAngle);
 //  					int k=0;
 //  					while (k<128)
 //  					{  
 //  						printf("%d: %f",k,Descriptors[k]);
 //  						printf("; ");
 //  						k++;
 //  					}
 //  
 //  					printf("\n");
 //  					delete []Descriptors;
 //  					Descriptors=NULL;
 //  				}
 //  			}
 //  			
 //  			//下一阶
 //  			if (vl_sift_process_next_octave(SiftFilt)==VL_ERR_EOF)
 //  			{
 //  				break;
 //  			}
 //  			//free(pKeyPoint);
 //  			KeyPoint=NULL;
 //  		}
 //  	}
 //  	vl_sift_delete(SiftFilt);
 //  	delete []ImageData;
 //  	ImageData=NULL;
 //  	cvNamedWindow("Source Image",1);
 //  	cvShowImage("Source Image",Image);
 //  	cvWaitKey(0);
 //  	cvReleaseImage(&Image);
 //  	cvDestroyAllWindows();
 //  	return 0;
 //  }


//#include "opencv2/highgui/highgui.hpp"  
//#include "opencv2/calib3d/calib3d.hpp"  
//#include "opencv2/imgproc/imgproc.hpp"  
//#include "opencv2/features2d/features2d.hpp"  
//#include "opencv2/nonfree/nonfree.hpp"  
//
//#include <iostream>  
//
//using namespace cv;  
//using namespace std;  
//
//#define ClusterNum 10  
//
//void DrawAndMatchKeypoints(const Mat& Img1,const Mat& Img2,const vector<KeyPoint>& Keypoints1,  
//	const vector<KeyPoint>& Keypoints2,const Mat& Descriptors1,const Mat& Descriptors2)  
//{  
//	Mat keyP1,keyP2;  
//	drawKeypoints(Img1,Keypoints1,keyP1,Scalar::all(-1),0);  
//	drawKeypoints(Img2,Keypoints2,keyP2,Scalar::all(-1),0);  
//	putText(keyP1, "drawKeyPoints", cvPoint(10,30), FONT_HERSHEY_SIMPLEX, 1 ,Scalar :: all(-1));  
//	putText(keyP2, "drawKeyPoints", cvPoint(10,30), FONT_HERSHEY_SIMPLEX, 1 ,Scalar :: all(-1));  
//	imshow("img1 keyPoints",keyP1);  
//	imshow("img2 keyPoints",keyP2);  
//
//	Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create( "BruteForce" );  
//	vector<DMatch> matches;  
//	descriptorMatcher->match( Descriptors1, Descriptors2, matches );  
//	Mat show;  
//	drawMatches(Img1,Keypoints1,Img2,Keypoints2,matches,show,Scalar::all(-1),CV_RGB(255,255,255),Mat(),4);  
//	putText(show, "drawMatchKeyPoints", cvPoint(10,30), FONT_HERSHEY_SIMPLEX, 1 ,Scalar :: all(-1));    
//	imshow("match",show);  
//}  
//
////测试OpenCV：class BOWTrainer  
//void BOWKeams(const Mat& img, const vector<KeyPoint>& Keypoints,   
//	const Mat& Descriptors, Mat& centers)  
//{  
//	//BOW的kmeans算法聚类;  
//	BOWKMeansTrainer bowK(ClusterNum,   
//		cvTermCriteria (CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 0.1),3,2);  
//	centers = bowK.cluster(Descriptors);  
//	cout<<endl<<"< cluster num: "<<centers.rows<<" >"<<endl;  
//
//	Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create( "BruteForce" );  
//	vector<DMatch> matches;  
//	descriptorMatcher->match(Descriptors,centers,matches);//const Mat& queryDescriptors, const Mat& trainDescriptors第一个参数是待分类节点，第二个参数是聚类中心;  
//	Mat demoCluster;  
//	img.copyTo(demoCluster);  
//
//	//为每一类keyPoint定义一种颜色  
//	Scalar color[]={CV_RGB(255,255,255),  
//		CV_RGB(255,0,0),CV_RGB(0,255,0),CV_RGB(0,0,255),  
//		CV_RGB(255,255,0),CV_RGB(255,0,255),CV_RGB(0,255,255),  
//		CV_RGB(123,123,0),CV_RGB(0,123,123),CV_RGB(123,0,123)};  
//
//
//	for (vector<DMatch>::iterator iter=matches.begin();iter!=matches.end();iter++)  
//	{  
//		cout<<"< descriptorsIdx:"<<iter->queryIdx<<"  centersIdx:"<<iter->trainIdx  
//			<<" distincs:"<<iter->distance<<" >"<<endl;  
//		Point center= Keypoints[iter->queryIdx].pt;  
//		circle(demoCluster,center,2,color[iter->trainIdx],-1);  
//	}  
//	putText(demoCluster, "KeyPoints Clustering: 一种颜色代表一种类型",  
//		cvPoint(10,30), FONT_HERSHEY_SIMPLEX, 1 ,Scalar :: all(-1));  
//	imshow("KeyPoints Clusrtering",demoCluster);  
//
//}  
//
//
//
//
//int main()  
//{  
//	cv::initModule_nonfree();//使用SIFT/SURF create之前，必须先initModule_<modulename>();   
//
//	cout << "< Creating detector, descriptor extractor and descriptor matcher ...";  
//	Ptr<FeatureDetector> detector = FeatureDetector::create( "SIFT" );  
//
//	Ptr<DescriptorExtractor> descriptorExtractor = DescriptorExtractor::create( "SIFT" );  
//
//	Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create( "BruteForce" );  
//
//
//
//	cout << ">" << endl;  
//
//	if( detector.empty() || descriptorExtractor.empty() )  
//	{  
//		cout << "Can not create detector or descriptor exstractor or descriptor matcher of given types" << endl;  
//		return -1;  
//	}  
//	cout << endl << "< Reading images..." << endl;  
//	Mat img1 = imread("海信2.jpg");  
//	Mat img2 = imread("海信.jpg");  
//	cout<<endl<<">"<<endl;  
//
//
//	//detect keypoints;  
//	cout << endl << "< Extracting keypoints from images..." << endl;  
//	vector<KeyPoint> keypoints1,keypoints2;  
//	detector->detect( img1, keypoints1 );  
//	detector->detect( img2, keypoints2 );  
//	cout <<"img1:"<< keypoints1.size() << " points  img2:" <<keypoints2.size()   
//		<< " points" << endl << ">" << endl;  
//
//	//compute descriptors for keypoints;  
//	cout << "< Computing descriptors for keypoints from images..." << endl;  
//	Mat descriptors1,descriptors2;  
//	descriptorExtractor->compute( img1, keypoints1, descriptors1 );  
//	descriptorExtractor->compute( img2, keypoints2, descriptors2 );  
//
//	cout<<endl<<"< Descriptoers Size: "<<descriptors2.size()<<" >"<<endl;  
//	cout<<endl<<"descriptor's col: "<<descriptors2.cols<<endl  
//		<<"descriptor's row: "<<descriptors2.rows<<endl;  
//	cout << ">" << endl;  
//
//	//Draw And Match img1,img2 keypoints  
//	//匹配的过程是对特征点的descriptors进行match;  
//	DrawAndMatchKeypoints(img1,img2,keypoints1,keypoints2,descriptors1,descriptors2);  
//
//	Mat center;  
//	//对img1提取特征点，并聚类  
//	//测试OpenCV：class BOWTrainer  
//	BOWKeams(img1,keypoints1,descriptors1,center);  
//
//
//	waitKey();  
//
//}

#include "bowIndex.h"
#include "SCSM.h"

void main(){
	bowIndex test(10000);
	//test.computeBOWFt("G:\张渊\各种图片\各大图片库/Oxford building dataset/oxbuild_images/", "./index.txt");
	test.initIndex("./index.txt");
	//double precision=0;
	//test.DetectAll("K:/张渊/各种图片/good/");
    SCSM scsmtest(mapsize, 1, 8, sqrgaussian, test);
    scsmtest.DetectAll(test, "C:/Users/zengraoli/Desktop/test/test/pic1/");
	//test.DetectGood(precision);
	waitKey();
}

//D:/study/code/objdetect/test/商标库_2278/
//D:/study/商标库/商标库/所有商标/
