// Copyright [2013.4.15] <Copyright yuan zhang>
#ifndef _STUDY_CODE_TEST_TEST_SCSM_H_
#define _STUDY_CODE_TEST_TEST_SCSM_H_

#include <vector>
#include <map>
#include <string>
#include "bowIndex.h"

extern clock_t lstart, lfinish, allstart, allfinish;  // 计时器
extern double ltotaltime;  // 总体测试时间
const int mapsize = 16;  // 投票图的量化数
const double sqrgaussian = 2.5;  // 高斯平方
typedef unsigned int uint;

// scale - rotation
struct ScaleRotate {
  float scale;  // 0.5-2 8 scales
  int rotation;  // 0-2pi 4or8 bins
};

// all the keypoint locations and distance vectors to the center under sr
struct ObjkeywdVec {
    ScaleRotate SR;  // scale && rotation
    vector<CvPoint> pLocation;  // point location of different SR
                                // according to the fixed center
    vector<CvPoint2D32f> PTCvec;  // center to point vector
};

// used to build multi-scale-rotation bow feature
struct ObjKeywordNum : public logoKeywordNum {
    int clustercenter;  // 某个query下，代表这些量化后的点所属的单词索引序号
    vector<ObjkeywdVec> allRSvec;  // 属于这个单词下的所有点在不同尺度角度下的点的位置及该点与中心的相对距离向量集合
};

// voting map
struct VoteMap {
    string logoname;  // dataset logo name
    string logopath;  // logo file path
    ScaleRotate SR;  // scale rotation
    float maxsim;  // max similarity
    CvPoint maxcenter;  // the location of the max similarity
    float simmap[mapsize][mapsize];  // similarity map
};
typedef vector<VoteMap> VectVP;
typedef map<string, VectVP> AllVP;
bool simCompareVM(const VoteMap& x, const VoteMap& y);  // 最后相似度排序规则

// 计算数据库图片中与query相似的方框位置的坐上顶点
inline int RectangleLeftTopPoint(const int gridlocation, const int objlength, const int querylength) {
    return ((gridlocation + 0.5) * objlength/16 - querylength/2);
}

// 计算数据库图片中与query相似的方框位置的右下顶点
inline int RectangleRightBottomPoint(const int gridlocation, const int objlength, const int querylength) {
    return ((gridlocation + 0.5) * objlength/16 + querylength/2);
}

class bowIndex;

class SCSM {
 public:
    SCSM(const uint step, const uint rotation, const uint scale,
        const float gasRatio, const bowIndex &bow);
    ~SCSM();
    bool DetectAll(const bowIndex &bow, const string &test_path);
    bool DetectOne(const bowIndex &bow, const string &logo_path,
        const string &pic_name);
    bool annotate();
    // 建立query多尺度角度特征点位置，与中心距离信息
    bool BuildSRLocation(logoInfor &logoFeat);  
    // 计算不同尺度角度query某特征点匹配特征点的位置，与中心距离向量，此函数用于原始距离向量计算，输入为初始特征点位置
    void TransformPoint(int angle, float scale, CvPoint orig_point,
        CvPoint &transform_point, CvPoint2D32f &transform_vector);  
    // 计算不同尺度角度query某特征点匹配特征点的位置，与中心距离向量，此函数用于其它尺度角度距离向量计算，输入为初试距离向量
    void TransformVector(int angle, float scale, CvPoint2D32f orig_vector,
        CvPoint &transform_point, CvPoint2D32f &transform_vector);
    bool SearchDataset(const bowIndex &bow);
    bool ComputeVoteMap(const bowIndex &bow, const logoKeywordNum &centerlogoinfor, const ObjKeywordNum &centerobjinfor);
    // 计算数据库相似中心的投票分数
    void ComputeDatasetCenter(const CvPoint datasetImgKW, const CvPoint2D32f pointcentervec, CvPoint &datasetImgcenter);
    // 计算单个数据库图片对应的中心相似度最大的投票图
    VoteMap ComputeMaxVM(VectVP onelogoVVP);
    // 计算单张投票图的最大分数的中心
    void ComputeMaxCenter(const float vmap[mapsize][mapsize], CvPoint &maxcenter, float &maxsimilarity);
    bool DrawRectangle();

 private:
    /*image map sizes 16*16*/
    uint    nQuantizedStep;
    /*quantized ratation bins,(0,2pai) is 4or8*/
    uint    nQuantizedNR;
    /*quantized scale bins 8*/
    uint    nQuantizedNS;
    // map number
    uint    mapnum;
    /*Gaussian voting*/
    float   fGaussianRadius;
    // query object location
    CvRect  objrectangle;
    // fixed center of the rectangle
    CvPoint fixedCenter;
    // information of the query, including rotation-scale,
    // keypoint location,vector between center and keypoints under different sr
    vector<ObjKeywordNum> objIfor;
    // voting maps of all images in dataset
    AllVP datasetVP;
    // final ranked list of images similar to query
    VectVP simlistVP;
    // gaussian weight in 5*5
    double gaussianmatrix[5][5];
    // init voting map
    void InitVotingMap(int mapnum, const bowIndex &bow);
    // draw image
    bool retrievaltotal;
};

#endif  // D:_STUDY_CODE_TEST_TEST_SCSM_H_
