// Copyright [2013.4.15] <Copyright yuan zhang>
#ifndef _STUDY_CODE_TEST_TEST_BOWINDEX_H_
#define _STUDY_CODE_TEST_TEST_BOWINDEX_H_

#include <fstream>
#include <iomanip>
#include <io.h>
#include <iostream>  
#include <math.h>
#include <time.h>
//#include <vld.h>

#include <boost/lexical_cast.hpp>
#include <boost/archive/tmpdir.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/calib3d/calib3d.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/nonfree/nonfree.hpp"  
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;  
using namespace std; 

//存储商标基本信息
struct LogoBaseInfor {
    friend class boost::serialization::access;
    string logoName;
    string filepath;
    int width;
    int height;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ar & logoName & filepath & width & height;
    }
};

//存储商标所有信息
struct  logoInfor
{
	friend class boost::serialization::access;
	LogoBaseInfor baseinfor;
	Mat pImg;
	vector<KeyPoint> keypoints;
	int keypointNum;
	Mat sifDescriptors;
	Mat bowDescriptors;
	double normBOWDptors;
	vector<vector<int>> pointIndexofClusters;
	string saveFile;
	double apoint;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){
		ar & baseinfor & pImg & keypoints & keypointNum & sifDescriptors;
	}
};

//用于建立倒排索引
struct logoKeywordNum{
	friend class boost::serialization::access;
	string logoName;
	double num;  // 某个商标中量化到某个中心的特征点数目
    double df;   // 该单词在该商标中的词频
	vector<CvPoint> pointvec;  // 属于该中心的特征点集合
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){
		ar & logoName &num & df &pointvec;
	}
};

//用于计算相似度
struct logoScore{
	friend class boost::serialization::access;
	string logoName;
	double normBOWDptors;
	double sim;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){
		ar & logoName &normBOWDptors &sim;
	}
};
typedef map<string, LogoBaseInfor> LogoBaseMap;
typedef map<string,logoInfor> LogosInfoMap; //所有商标map
typedef vector<logoKeywordNum> LogoKeyWD;  //倒排中每个单词的商标
typedef map<int,LogoKeyWD> InvertedIndex; //倒排索引map
typedef map<string, logoScore> SimilarityMap; //相似度map
typedef vector<logoScore> Similarity;  //相似度计算vector 用于排序
bool simCompare(const logoScore& x, const logoScore& y);

inline int QuantizeLocation(const int origL, const int origA) {
    int bin = origA/16;
    return bin == 0 ? 0 : origL/bin;
}

class bowIndex
{
public:
	bowIndex(int centers);
	~bowIndex();
	int getclusterNUm();

	bool computeALLBOWFt(const string &logo_path,const string &goodlogo_path, const string &ft_path);
	bool computeBOWFt(const string &logo_path,const string &ft_path);
	bool KeyImageFile1(const string &logo_path,const string &ft_path);
	bool KeyImageFile(const string &logo_path,const string &ft_path);
	bool AddKeyImage(const string file_path, string file_id,string ft_path);
	bool BOWFeature();

	bool InitKeyImage(string ft_path);
	bool InitOneKeyImage(string ft_path,string logo_id);

	bool initIndex(const string &ft_path);

	bool DetectAll(const string &test_path);
	bool DetectOne(const string  file_path, string file_name, string &result, bool &randw);
	bool DetectGood(double &m_ap);
	bool DetectGood2(const string &file_path, string file_name, string &result, double &m_ap);
	bool getSiftFeat(const Mat &img, logoInfor &logoFeat);
	bool getBowFeat(const string file_path, string file_name, logoInfor &logoBowFt);
	bool getBowFeat(const Mat &img, string file_name, logoInfor &logoBowFt);
    bool getBowFeat(const Mat &img, string file_name, logoInfor &logoBowFt) const;

	float compute_ap(logoInfor logoFt, Similarity ranked_list);

    friend class SCSM;  // SCSM为其友元，可以访问private成员

private:
	friend class boost::serialization::access;

	SiftFeatureDetector *sdetector;
	SiftDescriptorExtractor *sdescriptorExtractor;
	Ptr<FeatureDetector>  detector;
	Ptr<DescriptorExtractor> descriptorExtractor;
	Ptr<DescriptorMatcher> descriptorMatcher;

	map<string, int> goodLogoNum;
    int logonum;  // 数据库商标数目
    LogoBaseMap logobaseIormap; // 所有商标基本信息
	LogosInfoMap logosFtInfor; //所有logo信息
	LogosInfoMap goodlogosFtInfor;//所有好logo信息
	vector<Mat> logosDescriptors; //所有logosift特征
	int clusterNum; //聚类中心
	Mat dictionary;  //聚类单词词典
	BOWKMeansTrainer *bowTraining;  //bow训练
	BOWImgDescriptorExtractor *bowDE; // bow提特征
	double *keyWordIDf; // 各个单词idf*idf
	InvertedIndex logoInvertedIX; //倒排索引查找表
	SimilarityMap simMap; //相似度map
	Similarity simVec;  //相似度排序后的vector

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version){
		ar & logonum & logobaseIormap & clusterNum  & dictionary & logoInvertedIX & simMap;
		for (int i=0; i< this->clusterNum; i++)
		{
			ar & keyWordIDf[i];
		}
	}
};

#endif  // D:_STUDY_CODE_TEST_TEST_BOWINDEX_H_