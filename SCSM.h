// Copyright [2013.4.15] <Copyright yuan zhang>
#ifndef _STUDY_CODE_TEST_TEST_SCSM_H_
#define _STUDY_CODE_TEST_TEST_SCSM_H_

#include <vector>
#include <map>
#include <string>
#include "bowIndex.h"

extern clock_t lstart, lfinish, allstart, allfinish;  // ��ʱ��
extern double ltotaltime;  // �������ʱ��
const int mapsize = 16;  // ͶƱͼ��������
const double sqrgaussian = 2.5;  // ��˹ƽ��
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
    int clustercenter;  // ĳ��query�£�������Щ������ĵ������ĵ����������
    vector<ObjkeywdVec> allRSvec;  // ������������µ����е��ڲ�ͬ�߶ȽǶ��µĵ��λ�ü��õ������ĵ���Ծ�����������
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
bool simCompareVM(const VoteMap& x, const VoteMap& y);  // ������ƶ��������

// �������ݿ�ͼƬ����query���Ƶķ���λ�õ����϶���
inline int RectangleLeftTopPoint(const int gridlocation, const int objlength, const int querylength) {
    return ((gridlocation + 0.5) * objlength/16 - querylength/2);
}

// �������ݿ�ͼƬ����query���Ƶķ���λ�õ����¶���
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
    // ����query��߶ȽǶ�������λ�ã������ľ�����Ϣ
    bool BuildSRLocation(logoInfor &logoFeat);  
    // ���㲻ͬ�߶ȽǶ�queryĳ������ƥ���������λ�ã������ľ����������˺�������ԭʼ�����������㣬����Ϊ��ʼ������λ��
    void TransformPoint(int angle, float scale, CvPoint orig_point,
        CvPoint &transform_point, CvPoint2D32f &transform_vector);  
    // ���㲻ͬ�߶ȽǶ�queryĳ������ƥ���������λ�ã������ľ����������˺������������߶ȽǶȾ����������㣬����Ϊ���Ծ�������
    void TransformVector(int angle, float scale, CvPoint2D32f orig_vector,
        CvPoint &transform_point, CvPoint2D32f &transform_vector);
    bool SearchDataset(const bowIndex &bow);
    bool ComputeVoteMap(const bowIndex &bow, const logoKeywordNum &centerlogoinfor, const ObjKeywordNum &centerobjinfor);
    // �������ݿ��������ĵ�ͶƱ����
    void ComputeDatasetCenter(const CvPoint datasetImgKW, const CvPoint2D32f pointcentervec, CvPoint &datasetImgcenter);
    // ���㵥�����ݿ�ͼƬ��Ӧ���������ƶ�����ͶƱͼ
    VoteMap ComputeMaxVM(VectVP onelogoVVP);
    // ���㵥��ͶƱͼ��������������
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
