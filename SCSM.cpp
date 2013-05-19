// Copyright [2013.4.15] <Copyright yuan zhang>
#include "SCSM.h"

Mat img;
CvPoint topleft;
CvPoint bottomright;
bool flag;
const double pai = 3.1415926;
const double cosrota[8] = {1.0000, 0.7071, 0.0000, -0.7071, -1.0000,
    -0.7071, 0.0000, 0.7071};
const double sinrota[8] = {0.0000, 0.7071, 1.0000, 0.7071, 0.0000,
    -0.7071, -1.0000, -0.7071};

void on_mouse(int event, int x, int y, int flags, void* param);

SCSM::SCSM(const uint step, const uint rotation, const uint scale,
    const float gasRatio, const bowIndex &bow)
    :nQuantizedStep(step), nQuantizedNR(rotation),
    nQuantizedNS(scale), fGaussianRadius(gasRatio),
    objrectangle(cvRect(0, 0, 0, 0)), fixedCenter(cvPoint(0, 0)),
    retrievaltotal(false) {
    mapnum = rotation*scale;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            float dist = sqrt(pow(static_cast<double>(abs(2-i)),2)+pow(static_cast<double>(abs(2-j)),2));
            gaussianmatrix[i][j] = exp(-dist/this->fGaussianRadius);
        }
    }
}

SCSM::~SCSM() 
{}

void SCSM::InitVotingMap(const int mapnum, const bowIndex &bow) {
    if(!datasetVP.empty())
        datasetVP.clear();
    if(!simlistVP.empty())
        simlistVP.clear();
    if(!objIfor.empty())
        objIfor.clear();
    LogoBaseMap::const_iterator lbmpos = bow.logobaseIormap.begin();
    string logofile;
    VoteMap onesample;
    memset(&onesample, 0, sizeof(VoteMap));
    for (int i = 0; i < bow.logonum; ++i, ++lbmpos) {
        VectVP logovotemap(mapnum, onesample);
        logofile = lbmpos->first;
        datasetVP.insert(make_pair(logofile, logovotemap));
    }
    simlistVP.reserve(bow.logonum);
}

bool SCSM::DetectAll(const bowIndex &bow, const string &test_path) {
    struct _finddata_t c_file;
    intptr_t hFile;
    string image_path = test_path;
    image_path += "*.jpg";
    ofstream rofs, wofs;
    rofs.open("./rightresult.txt", ios::out | ios::trunc);
    wofs.open("./wrongresult.txt", ios::out | ios::trunc);
    int totalright = 0, totalwrong = 0;
    allstart = clock();
    double m_ap = 0;
    ltotaltime = 0;
    if ((hFile = _findfirst(image_path.c_str(), &c_file)) == -1L) {
		printf("error");
		return false;
	} else {
		do {
            // 重置投票图内的信息
            InitVotingMap(mapnum, bow);

			string logo_file = test_path;
			logo_file += c_file.name;

			string picId = "";
			int len = strlen(c_file.name);
			for (int i = 0; i < len-4; i++)
				picId += c_file.name[i];

			string recResult = "";
			bool right = false;
			bool result = DetectOne(bow, logo_file, picId);
			std::cout << "        matching time:  "<<
                static_cast<double>(lfinish - lstart)/CLOCKS_PER_SEC<< std::endl;
			//----------------------
			if(result == false)
				cout << "识别失败" << endl;
		} while (_findnext(hFile, &c_file) == 0);
	}
	_findclose(hFile);

	m_ap = m_ap/85;
	rofs << "map: " << m_ap << endl;
	rofs << "total time: " << ltotaltime << endl;

	allfinish = clock();
	rofs.close();
	wofs.close();
	return true;
}

bool SCSM::DetectOne(const bowIndex &bow, const string &logo_path,
    const string &pic_name) {
	// 读入图像
	IplImage *src_img = cvLoadImage(logo_path.c_str(), 1);
	Mat mimg = imread(logo_path);
    if (mimg.empty())
    {
        return false;
    }
	img = mimg;
	// 获取目标位置
	annotate();
	objrectangle.x = topleft.x;
	objrectangle.y = topleft.y;
	objrectangle.width = bottomright.x - topleft.x;
	objrectangle.height = bottomright.y - topleft.y;
 	fixedCenter.x = objrectangle.width/2;
 	fixedCenter.y = objrectangle.height/2;
//     fixedCenter.x = QuantizeLocation(objrectangle.width/2, objrectangle.width);  // 量化坐标
//     fixedCenter.y = QuantizeLocation(objrectangle.height/2, objrectangle.height);  // 量化坐标
	// 创建子图像
    if (objrectangle.width==0) {
        objrectangle.x = 0;
        objrectangle.y = 0;
        objrectangle.width = src_img->width;
        objrectangle.height = src_img->height;
        retrievaltotal = true;
    }
	cvSetImageROI(src_img, objrectangle);
	IplImage *pSubImage = cvCreateImage(cvSize(objrectangle.width, objrectangle.height),
        src_img->depth, src_img->nChannels);
	cvCopy(src_img, pSubImage, NULL);
	cvResetImageROI(src_img);
	// 提取子图像bow特征
	Mat sub_img(pSubImage, 0);
	logoInfor logoFeat;
	logoFeat.baseinfor.filepath = logo_path;
	bool ret1 = bow.getBowFeat(sub_img, pic_name, logoFeat);
	// 建立多角度尺度位置信息
    bool ret2 = BuildSRLocation(logoFeat);
    // 利用bow进行投票,simlistVP为最后相似度由高到底将数据库图片排序
    bool ret3 = SearchDataset(bow);
    // 在数据库图片中画出方框
    bool ret4 = DrawRectangle();
    retrievaltotal = false;
	cvReleaseImage(&pSubImage);
	cvReleaseImage(&src_img);
	return true;
}
bool SCSM::annotate() {
	cvNamedWindow("image", 1);
	cvShowImage("image", &IplImage(img));
	cvSetMouseCallback("image", on_mouse, 0);
	cvWaitKey(0);
    cvDestroyWindow("image");
	return 0;
}

bool SCSM::BuildSRLocation(logoInfor &logoFeat) {
    // build point vector
    for (int i = 0; i < logoFeat.pointIndexofClusters.size(); ++i) {
        if (!logoFeat.pointIndexofClusters[i].empty()) {
            ObjKeywordNum onecenterinfor;
            onecenterinfor.logoName = logoFeat.baseinfor.logoName;
            onecenterinfor.num = logoFeat.pointIndexofClusters[i].size();
            onecenterinfor.pointvec.reserve(onecenterinfor.num);
            onecenterinfor.allRSvec.reserve(mapnum);
            onecenterinfor.df = 1.0*onecenterinfor.num/logoFeat.keypointNum;
            for (int j = 0 ; j < onecenterinfor.num; j++) {
                KeyPoint keyloc = logoFeat.keypoints[logoFeat.pointIndexofClusters[i][j]];
//                 keyloc.pt.x = QuantizeLocation(keyloc.pt.x, logoFeat.baseinfor.width);  // 量化坐标
//                 keyloc.pt.y = QuantizeLocation(keyloc.pt.y, logoFeat.baseinfor.height);  // 量化坐标
                onecenterinfor.pointvec.push_back(keyloc.pt);
            }
            onecenterinfor.clustercenter = i;
            objIfor.push_back(onecenterinfor);
        }
    }
    // compute different center to point vectors in different scales and rotations
    CvPoint orig_point, trans_point;
    CvPoint2D32f orig_vector, trans_vector;
    for (int i = 0; i < objIfor.size(); ++i) {
        // original scale and rotation
        ObjkeywdVec objkeywdlocation_orig;
        objkeywdlocation_orig.SR.rotation = 0;
        objkeywdlocation_orig.SR.scale = 1;
        objkeywdlocation_orig.pLocation.reserve(objIfor[i].num);
        objkeywdlocation_orig.PTCvec.reserve(objIfor[i].num);
        for (int j = 0; j < objIfor[i].num; j++) {
            orig_point = objIfor[i].pointvec[j];
            TransformPoint(objkeywdlocation_orig.SR.rotation, objkeywdlocation_orig.SR.scale,
                orig_point, trans_point, trans_vector);
            objkeywdlocation_orig.pLocation.push_back(trans_point);
            objkeywdlocation_orig.PTCvec.push_back(trans_vector);
        }
        // other scales and rotations
        for (int j = 0; j < nQuantizedNS; ++j) {
            for (int  k= 0; k < nQuantizedNR; ++k) {
                if (j == 2 && k == 0) {
                    objIfor[i].allRSvec.push_back(objkeywdlocation_orig);
                    continue;
                }
                ObjkeywdVec objkeywdlocation_other;
                objkeywdlocation_other.SR.rotation = k;
                objkeywdlocation_other.SR.scale = 0.5 + j/4.0;
                objkeywdlocation_other.pLocation.reserve(objIfor[i].num);
                objkeywdlocation_other.PTCvec.reserve(objIfor[i].num);
                for (int l = 0; l < objIfor[i].pointvec.size(); l++) {
                    orig_vector = objkeywdlocation_orig.PTCvec[l];
                    TransformVector(objkeywdlocation_other.SR.rotation, objkeywdlocation_other.SR.scale,
                        orig_vector, trans_point, trans_vector);
                    objkeywdlocation_other.pLocation.push_back(trans_point);
                    objkeywdlocation_other.PTCvec.push_back(trans_vector);
                }
                objIfor[i].allRSvec.push_back(objkeywdlocation_other);
            }
        }
    }
    return true;
}

void SCSM::TransformPoint(int angle, float scale, CvPoint orig_point,
    CvPoint &transform_point, CvPoint2D32f &transform_vector) {
    double origvec_x = static_cast<double>(orig_point.x - fixedCenter.x);
	double origvec_y = static_cast<double>(orig_point.y - fixedCenter.y);
    TransformVector(angle, scale, cvPoint2D32f(origvec_x, origvec_y),
        transform_point, transform_vector);
}

void SCSM::TransformVector(int angle, float scale, CvPoint2D32f orig_vector,
    CvPoint &transform_point, CvPoint2D32f &transform_vector) {
    // 变换后的位置可能不是整数
    // first rotation
    transform_vector.x = orig_vector.x*cosrota[angle] - orig_vector.y*sinrota[angle];
    transform_vector.y = orig_vector.x*sinrota[angle] + orig_vector.y*cosrota[angle];
    // second s
    transform_vector.x *= scale;
    transform_vector.y *= scale;
    // transformed location
    double trans_x = static_cast<double>(transform_vector.x + fixedCenter.x);
    double trans_y = static_cast<double>(transform_vector.y + fixedCenter.y);
    transform_point = cvPointFrom32f(cvPoint2D32f(trans_x, trans_y));
}

bool SCSM::SearchDataset(const bowIndex &bow) {
    InvertedIndex::const_iterator pos;
    LogoKeyWD logoclustervec;
    logoKeywordNum logoKWD;
    ObjKeywordNum objKWD;
    bool ret;
    // build all voting map
    for (int i = 0; i < objIfor.size(); ++i) {
        objKWD = objIfor[i];
        pos = bow.logoInvertedIX.find(objKWD.clustercenter);
        logoclustervec = pos->second;
        for (int j =0; j < logoclustervec.size(); ++j) {
            logoKWD = logoclustervec[j];
            ret = ComputeVoteMap(bow, logoKWD, objKWD);
        }
    }
    // get max voting map for each image in dataset
    for (AllVP::iterator vmpos = datasetVP.begin(); vmpos != datasetVP.end(); ++vmpos) {
        VectVP oneVP = vmpos->second;
        VoteMap maxVP= ComputeMaxVM(oneVP);
        maxVP.logoname = vmpos->first;
        maxVP.logopath = bow.logobaseIormap.find(maxVP.logoname)->second.filepath;
        simlistVP.push_back(maxVP);
    }
    // get the ranked list of the query
    std::sort(this->simlistVP.begin(), this->simlistVP.end(), simCompareVM);
    return ret;
}

bool SCSM::ComputeVoteMap(const bowIndex &bow, const logoKeywordNum &centerlogoinfor, const ObjKeywordNum &centerobjinfor) {
    AllVP::iterator vmpos = datasetVP.find(centerlogoinfor.logoName);
    LogoBaseMap::const_iterator logopos = bow.logobaseIormap.find(centerlogoinfor.logoName);
    int logowidth = logopos->second.width;
    int logoheight = logopos->second.height;
    int quantizex, quantizey;
    ObjkeywdVec objSRVec;
    CvPoint logoKplocation, logocenter;
    CvPoint2D32f pointcentervec;
    if (centerlogoinfor.num * centerobjinfor.num > 10)
        return true;
    double score = pow(bow.keyWordIDf[centerobjinfor.clustercenter], 2);///(centerlogoinfor.df*centerobjinfor.df)
    double score_dist;
    for (int i = 0; i < centerobjinfor.allRSvec.size(); ++i) {
        objSRVec = centerobjinfor.allRSvec[i];
        vmpos->second[i].SR = objSRVec.SR;
        for (int j = 0; j < objSRVec.PTCvec.size(); ++j) {
            pointcentervec = objSRVec.PTCvec[j];
            for (int k = 0; k < centerlogoinfor.pointvec.size(); ++k) {
                logoKplocation = centerlogoinfor.pointvec[k];
                ComputeDatasetCenter(logoKplocation, pointcentervec, logocenter);
                quantizey = QuantizeLocation(logocenter.y, logoheight);  //  量化中心
                quantizex = QuantizeLocation(logocenter.x, logowidth);
                for(int l = -2+quantizey, ay = 0; l <= 2+quantizey; ++l, ++ay) {
                    for(int m = -2+quantizex, ax = 0; m <= 2+quantizex; ++m, ++ax) {
                        if(l>=0 && l<this->nQuantizedStep && m >= 0 && m < this->nQuantizedStep) {
                            // 高斯加权得分
                            vmpos->second[i].simmap[l][m] += score*gaussianmatrix[ay][ax];
                        }
                    }
                }
//                 for(int l = -2+logocenter.y, ay = 0; l <= 2+logocenter.y; ++l, ++ay) {
//                     for(int m = -2+logocenter.x, ax = 0; m <= 2+logocenter.x; ++m, ++ax) {
//                         if(l>=0 && l<this->nQuantizedStep && m >= 0 && m < this->nQuantizedStep) {
//                             // 高斯加权得分
//                             vmpos->second[i].simmap[l][m] += score*gaussianmatrix[ay][ax];
//                         }
//                     }
//                 }
            }
        }
    }
    return true;
}

void SCSM::ComputeDatasetCenter(const CvPoint datasetImgKW, const CvPoint2D32f pointcentervec, CvPoint &datasetImgcenter) {
    // logocenter
    double center_x = static_cast<double>(datasetImgKW.x - pointcentervec.x);
    double center_y = static_cast<double>(datasetImgKW.y - pointcentervec.y);
    datasetImgcenter = cvPointFrom32f(cvPoint2D32f(center_x, center_y));
}

VoteMap SCSM::ComputeMaxVM(VectVP onelogoVVP) {
    // get the max similar center for each voting map
    for (int i = 0; i < onelogoVVP.size(); ++i) {
       ComputeMaxCenter(onelogoVVP[i].simmap, onelogoVVP[i].maxcenter, onelogoVVP[i].maxsim);
    }
    std::sort(onelogoVVP.begin(), onelogoVVP.end(), simCompareVM);
    return onelogoVVP[0];
}

void SCSM::ComputeMaxCenter(const float vmap[mapsize][mapsize], CvPoint &maxcenter, float &maxsimilarity) {
    maxsimilarity = 0.0;
    for (int i = 0; i < mapsize; ++i) {
        for (int j = 0; j < mapsize; ++j) {
            if (vmap[i][j] > maxsimilarity) {
                maxsimilarity = vmap[i][j];
                maxcenter = cvPoint(i, j);
            }
        }
    }
}

bool SCSM::DrawRectangle() {
    VoteMap onevote;
    Mat img;
    Point ptlt, ptrb;
    int width, height;
    int objwidth, objheight;

    for (int i = 0; i < 15; ++i) {
        onevote = simlistVP[i];
        img = imread(onevote.logopath);
        if (img.empty())
            return false;

        if (retrievaltotal==false) {
            objwidth = onevote.SR.scale * objrectangle.width;
            objheight = onevote.SR.scale * objrectangle.height;

            width = img.cols;
            height = img.rows;

            ptlt.x = RectangleLeftTopPoint(onevote.maxcenter.x, width, objwidth);
            ptlt.y = RectangleLeftTopPoint(onevote.maxcenter.y, height, objheight);

            ptrb.x = RectangleRightBottomPoint(onevote.maxcenter.x, width, objwidth);
            ptrb.y = RectangleRightBottomPoint(onevote.maxcenter.y, height, objheight);

            rectangle(img, ptlt, ptrb, CV_RGB(255,0,0));
        }
        
        namedWindow(onevote.logoname, 1);

        imshow(onevote.logoname, img);

        waitKey();

        destroyWindow(onevote.logoname);
    }
    return true;
}
void on_mouse(int event, int x, int y, int flags, void* param) {
    IplImage pimg = img;
    if (!&pimg)
        return;

    if (event == CV_EVENT_LBUTTONDOWN) {
        flag = 1;
        topleft = cvPoint(x, y);
        cvRectangle(&pimg, topleft, topleft, CV_RGB(0, 0, 255), 2, 8, 0);
        cvShowImage("image", &pimg);
    } else if (event == CV_EVENT_LBUTTONUP) {
        flag = 0;
        bottomright = cvPoint(x, y);
        cvRectangle(&pimg, topleft, bottomright, CV_RGB(0, 0, 255), 2, 8, 0);
        cvShowImage("image", &pimg);
    }
}

bool simCompareVM(const VoteMap& x, const VoteMap& y) {  
    return x.maxsim > y.maxsim;
} 