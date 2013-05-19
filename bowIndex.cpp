#include "bowIndex.h"

int num[17] = {46, 75, 53, 33, 26, 47, 9, 35, 9, 8, 6, 19, 9, 129, 17, 52, 7};
string name[17] = {"奥运", "百度", "国徽","工商银行","家乐福", "建设银行", "交通银行", "蒙牛", "联合证券", "民生银行", "人民银行", "三星", "腾讯", "星巴克", "伊利", "中国银行", "青岛啤酒"};

clock_t lstart,lfinish, allstart, allfinish;
double ltotaltime;
bool tag = false;
void save_Index(const bowIndex &s, const char * filename){
	// make an archive
	setlocale(LC_ALL,"");
	std::ofstream ofs(filename);
	setlocale(LC_ALL,"C");
	boost::archive::text_oarchive oa(ofs);
	oa << s;
	ofs.close();
}

void restore_Index(bowIndex &s, const char * filename)
{
	// open the archive
	setlocale(LC_ALL,"");
	std::ifstream ifs(filename);
	setlocale(LC_ALL,"C");
	boost::archive::text_iarchive ia(ifs);

	// restore the schedule from the archive
	ia >> s;
	ifs.close();
}

void save_feat(const logoInfor &s, const char * filename){
	// make an archive
	setlocale(LC_ALL,"");
	std::ofstream ofs(filename);
	setlocale(LC_ALL,"C");
	boost::archive::binary_oarchive oa(ofs);
	oa << s;
	ofs.close();
}

void restore_feat(logoInfor &s, const char * filename)
{
	// open the archive
	setlocale(LC_ALL,"");
	std::ifstream ifs(filename);
	setlocale(LC_ALL,"C");
	boost::archive::binary_iarchive ia(ifs);

	// restore the schedule from the archive
	ia >> s;
	ifs.close();
}

bowIndex::bowIndex(int centers)
{
	cv::initModule_nonfree();//使用SIFT/SURF create之前，必须先initModule_<modulename>();   
	cout << "< Creating detector, descriptor extractor and descriptor matcher ...";  
	this->detector = FeatureDetector::create( "SIFT" );
	sdetector = new SiftFeatureDetector(200,3,0.01,20);
	sdescriptorExtractor = new SiftDescriptorExtractor(200,3,0.01,20);
	this->descriptorExtractor = DescriptorExtractor::create( "SIFT" );  
	this->descriptorMatcher = DescriptorMatcher::create( "BruteForce" );  
	this->bowDE = new BOWImgDescriptorExtractor(this->descriptorExtractor, this->descriptorMatcher);
	cout << ">" << endl;
	this->clusterNum = centers;
	this->bowTraining = new BOWKMeansTrainer(clusterNum,cvTermCriteria (CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 0.1),3,2);  
	this->keyWordIDf = new double [centers];
	memset(this->keyWordIDf,0, sizeof(double)*centers);
	for (int i = 0; i< centers ;i++)
	{
		LogoKeyWD logoKey;
		this->logoInvertedIX.insert(make_pair(i,logoKey));
	}
	for (int i=0;i<17;i++)
	{
		goodLogoNum.insert(make_pair(name[i],num[i]));
	}
}
bowIndex::~bowIndex(){
	delete this->bowTraining;
	this->bowTraining = NULL;
	delete []this->keyWordIDf;
	this->keyWordIDf = NULL;
	delete this->bowDE;
	this->bowDE = NULL;
	delete this->sdetector;
	this->sdetector = NULL;
	delete this->sdescriptorExtractor;
	this->sdescriptorExtractor = NULL;
	clusterNum =0;
}
int bowIndex::getclusterNUm(){
	return clusterNum;
}

bool bowIndex::computeALLBOWFt(const string &logo_path,const string &goodlogo_path, const string &ft_path)
{
	//所有图片计算sift特征
	bool ftRet = KeyImageFile(logo_path,ft_path);
	tag = true;
	bool ftRet2 = KeyImageFile(goodlogo_path,ft_path);

	bool BOWRet = BOWFeature();

	try
	{
		save_Index(*this,ft_path.c_str());
	}
	catch (boost::archive::archive_exception)
	{
		cout<<ft_path<<"保存错误"<<endl;
	}
	return true;
}
bool bowIndex::computeBOWFt(const string &logo_path,const string &ft_path){
	//所有图片计算sift特征
	bool ftRet = KeyImageFile(logo_path,ft_path);
	bool BOWRet = BOWFeature();

	try
	{
		save_Index(*this,ft_path.c_str());
	}
	catch (boost::archive::archive_exception)
	{
		cout<<ft_path<<"保存错误"<<endl;
	}
	return true;
}
bool bowIndex::KeyImageFile1(const string &logo_path,const string &ft_path)
{
	struct _finddata_t c_file;
	intptr_t hFile;
	string image_path = logo_path;
	image_path +="*.*";
	int i  =0;
	if ((hFile = _findfirst(image_path.c_str(), &c_file)) == -1L){
		printf("error");
		return false;
	}
	else{
		do {
			string logo_file = logo_path;

			if ((c_file.attrib & _A_SUBDIR))  
			{  
				if (strcmp(c_file.name,".") != 0 && strcmp  
					(c_file.name,"..") != 0)  
				{  
					string filename = c_file.name;
					string fulpath = logo_path + filename + "/";
					bool result = KeyImageFile(fulpath, ft_path);
					//----------------------
					if(result==false)
						cout<<"商标提取特征失败"<<endl;
				}  
			}
		} while (_findnext(hFile, &c_file) == 0);

	}
	_findclose(hFile);
	return true;
}

bool bowIndex::KeyImageFile(const string &logo_path,const string &ft_path)
{
	struct _finddata_t c_file;
	intptr_t hFile;
	string image_path = logo_path;
	image_path += "*.jpg";
	int i  =0;
	if ((hFile = _findfirst(image_path.c_str(), &c_file)) == -1L){
		printf("error");
		return false;
	}
	else{
		do {
			string logo_file = logo_path;
			logo_file += c_file.name;

			string logoId ="";
			int len = strlen(c_file.name);
			for (int i=0;i<len-4;i++)
				logoId += c_file.name[i];

			string featt_file = ft_path;
			featt_file += logoId;
			std::cout <<" No.:"<< i++ << logo_file.c_str() << " "<<logoId <<std::endl;
			bool result = AddKeyImage(logo_file, logoId,featt_file);
			//----------------------
			if(result==false)
				cout<<"商标提取特征失败"<<endl;

		} while (_findnext(hFile, &c_file) == 0);
	}
	_findclose(hFile);
    logonum = logobaseIormap.size();
	return true;
}

bool bowIndex::AddKeyImage(const string file_path, string file_id,string ft_path)  
{  
	if( detector.empty() || descriptorExtractor.empty() )  
	{  
		cout << "Can not create detector or descriptor exstractor or descriptor matcher of given types" << endl;  
		return false;  
	}  
	cout << endl << "< Reading images..." << endl;  
	Mat img = imread(file_path);
    if (img.empty())
    {
        return false;
    }
	cout<<endl<<">"<<endl;  

	logoInfor logoFt;
    LogoBaseInfor logoinfor;
	logoinfor.logoName = file_id;
	logoinfor.filepath = file_path;
	logoinfor.width = img.cols;
	logoinfor.height = img.rows;
    logoFt.baseinfor = logoinfor;
	logoFt.pImg = img;
	logoFt.saveFile = ft_path;
	bool getRet = getSiftFeat(img, logoFt);
	if (!getRet)
	{
		return false;
	}
	logoFt.keypointNum = logoFt.keypoints.size();
	if (tag ==false){
		logosFtInfor.insert(make_pair(file_id, logoFt));
		bowTraining->add(logoFt.sifDescriptors);

		logoScore simLogo;
		simLogo.logoName = file_id;
		simLogo.sim = 0;
		simMap.insert(make_pair(file_id,simLogo));
        logobaseIormap.insert(make_pair(file_id, logoinfor));
	}
	else
		goodlogosFtInfor.insert(make_pair(file_id, logoFt));
	//logosDescriptors.push_back(logoFt.sifDescriptors);

	return true;
}

bool bowIndex::BOWFeature(){
	dictionary = bowTraining->cluster();
	bowDE->setVocabulary(dictionary); //dictionary是通过前面聚类得到的词典； 
	LogosInfoMap::iterator pos;
	SimilarityMap::iterator simPos;
	InvertedIndex::iterator Ipos;
	logoKeywordNum logoKWnum;
	for(pos=logosFtInfor.begin(); pos!= logosFtInfor.end(); ++pos)  
	{ 
		logoKWnum.logoName = pos->second.baseinfor.logoName;
		pos->second.normBOWDptors =0;
		bowDE->compute(pos->second.pImg, pos->second.keypoints, pos->second.bowDescriptors, &pos->second.pointIndexofClusters);
		for (int i=0;i<pos->second.pointIndexofClusters.size();i++)
		{
			if (!pos->second.pointIndexofClusters[i].empty())
			{
				logoKWnum.num = pos->second.pointIndexofClusters[i].size();
                logoKWnum.pointvec.reserve(logoKWnum.num);
                logoKWnum.df = 1.0*logoKWnum.num/pos->second.keypointNum;
				for (int j=0;j<logoKWnum.num;j++)
				{
					KeyPoint keyloc = pos->second.keypoints[pos->second.pointIndexofClusters[i][j]];
//                     keyloc.pt.x = QuantizeLocation(keyloc.pt.x, pos->second.baseinfor.width);  // 量化坐标
//                     keyloc.pt.y = QuantizeLocation(keyloc.pt.y, pos->second.baseinfor.height);  // 量化坐标
					logoKWnum.pointvec.push_back(keyloc.pt);
				}
				Ipos = logoInvertedIX.find(i);
				Ipos->second.push_back(logoKWnum);
                logoKWnum.pointvec.clear();
			}
		}
	}
	for(pos=goodlogosFtInfor.begin(); pos!= goodlogosFtInfor.end(); ++pos)  
	{ 
		logoKWnum.logoName = pos->second.baseinfor.logoName;
		pos->second.normBOWDptors =0;
		bowDE->compute(pos->second.pImg, pos->second.keypoints, pos->second.bowDescriptors, 
			&pos->second.pointIndexofClusters);
		for (int i=0;i<pos->second.pointIndexofClusters.size();i++)
		{
			if (!pos->second.pointIndexofClusters[i].empty())
			{
				logoKWnum.num = pos->second.pointIndexofClusters[i].size();
			}
		}
	}
	int i;
	for ( i=0, Ipos=logoInvertedIX.begin();i<this->clusterNum, Ipos!=logoInvertedIX.end();i++, Ipos++)
	{
		keyWordIDf[i] = log(1.0*this->logosFtInfor.size()/Ipos->second.size());
	}
	for (pos=logosFtInfor.begin(),simPos = simMap.begin(); pos!= logosFtInfor.end(),simPos!= simMap.end(); ++pos, ++simPos)
	{
		for (int i=0;i<pos->second.pointIndexofClusters.size();i++)
		{
			if (!pos->second.pointIndexofClusters[i].empty())
			{
				pos->second.normBOWDptors += pow((double)(pos->second.pointIndexofClusters[i].size()*keyWordIDf[i]),2);
			}
		}
		pos->second.normBOWDptors = sqrt(pos->second.normBOWDptors);
		simPos->second.normBOWDptors = pos->second.normBOWDptors;
	}
	for (pos=goodlogosFtInfor.begin(); pos!= goodlogosFtInfor.end(); ++pos)
	{
		for (int i=0;i<pos->second.pointIndexofClusters.size();i++)
		{
			if (!pos->second.pointIndexofClusters[i].empty())
			{
				pos->second.normBOWDptors += pow((double)(pos->second.pointIndexofClusters[i].size()*keyWordIDf[i]),2);
			}
		}
		pos->second.normBOWDptors = sqrt(pos->second.normBOWDptors);
	}
	return true;
}

bool bowIndex::InitKeyImage(string ft_path)
{
	struct _finddata_t c_file;
	intptr_t hFile;
	string image_path = ft_path;
	image_path += "*.txt";

	if ((hFile = _findfirst(image_path.c_str(), &c_file)) == -1L){
		printf("error");
		return false;
	}
	else{
		do {
			string logoFt_file = ft_path;
			logoFt_file += c_file.name;

			string logoId ;
			int len = strlen(c_file.name);
			for (int i=0;i<len-4;i++)
				logoId += c_file.name[i];

			InitOneKeyImage(logoFt_file,logoId);
		} while (_findnext(hFile, &c_file) == 0);
	}
	_findclose(hFile);

	return true;
}

 bool bowIndex::InitOneKeyImage(string ft_path,string logo_id)
 {
// 	//---new---
// 	struct logoInfor logo_infor;
// 	int numhead = ft_path.find("#")+1;
// 	int numend = ft_path.rfind("#");
// 	string num;
// 	num.assign(ft_path,numhead,numend-numhead);
// 	int knum = boost::lexical_cast<int>(num);
// 	logo_infor->feats = (feature *)calloc( knum, sizeof(struct feature) );
// 	//-----
// 	try
// 	{
// 		cout<<logo_id<<"存入内存"<<endl;
// 		restore_indexfeat(*logo_infor,ft_path.c_str());
// 	}
// 	catch (boost::archive::archive_exception)
// 	{
// 		cout<<logo_id<<"读取错误"<<endl;
// 		return false;
// 	}
// 	catch(...){
// 		cout<<logo_id<<"出现异常"<<endl;
// 		throw;
// 	}
// 	if (logo_infor==NULL)
// 	{
// 		free(logo_infor->feats);
// 		delete logo_infor;
// 		logo_infor=NULL;
// 		return false;
// 	}
// 	logo_infor->hsvft->ID = logo_infor->file_id;
// 	KeyLogoIndexMap::iterator iter= pkInforIndexMap.find(logo_infor->logo_tag);
// 	SPK logoinfor(logo_infor);
// 	iter->second.insert(map<string, SPK> :: value_type(logo_infor->file_id,logoinfor));
// 	allKeyLogoInfor.insert(map<string, SPK> :: value_type(logo_infor->file_id,logoinfor));
// 
// 	//pKfInfoMap.insert(std::make_pair(logo_id, logo_infor));
// 
	 return true;
 }
bool bowIndex::initIndex(const string &ft_path)
{
	this->dictionary = Mat(this->clusterNum,128, CV_64F, Scalar::all(0));
	try
	{
		cout<<ft_path<<"存入内存"<<endl;
		restore_Index(*this,ft_path.c_str());
	}
	catch (boost::archive::archive_exception)
	{
		cout<<ft_path<<"读取错误"<<endl;
		return false;
	}
	catch(...){
		cout<<ft_path<<"出现异常"<<endl;
		throw;
	}
	this->bowDE->setVocabulary(this->dictionary); //dictionary是通过前面聚类得到的词典； 
	return true;
}
bool bowIndex::DetectAll(const string &test_path)
{
	struct _finddata_t c_file;
	intptr_t hFile;
	string image_path = test_path;
	image_path += "*.jpg";
	ofstream rofs,wofs;
	rofs.open("./rightresult.txt",ios::out | ios::trunc);
	wofs.open("./wrongresult.txt",ios::out | ios::trunc);
	int totalright = 0, totalwrong =0;
	allstart = clock();
	double m_ap=0;
	ltotaltime =0;
	if ((hFile = _findfirst(image_path.c_str(), &c_file)) == -1L){
		printf("error");
		return false;
	}
	else{
		do {
			string logo_file = test_path;
			logo_file += c_file.name;

			string picId ="";
			int len = strlen(c_file.name);
			for (int i=0;i<len-4;i++)
				picId += c_file.name[i];

			string recResult = "";
			//lstart = clock();
			bool right = false;
			
			//bool result = DetectOne(logo_file, picId, recResult,right);
			bool result = DetectGood2(logo_file, picId, recResult,m_ap);
			//lfinish = clock();
			std::cout << "        matching time:  "<<(double)(lfinish - lstart)/CLOCKS_PER_SEC<< std::endl;
			//----------------------
			if(result==false)
				cout<<"识别失败"<<endl;
			/*std::cout << logo_file.c_str() << " "<<picId <<" "<<recResult<<std::endl;*/
			/*if (right)
			{
			totalright++;
			rofs << setiosflags(ios::left)<<setw(36) <<logo_file.c_str() << setw(36) << picId << setw(36) << recResult << setw(36)<< " matching time："<< (double)(lfinish - lstart)/CLOCKS_PER_SEC<<endl;
			}else{
			totalwrong++;
			wofs << setiosflags(ios::left)<<setw(36) <<logo_file.c_str() << setw(36) << picId << setw(36) << recResult << setw(36)<< " matching time："<< (double)(lfinish - lstart)/CLOCKS_PER_SEC<<endl;
			}*/
			
		} while (_findnext(hFile, &c_file) == 0);
	}
	_findclose(hFile);

	m_ap = m_ap/85;
	rofs<<"map: "<<m_ap<<endl;
	rofs << "total time: "<<ltotaltime<<endl;

	allfinish = clock();
	//ltotaltime = (double)(allfinish - allstart)/CLOCKS_PER_SEC;
	//rofs << "total time: "<<ltotaltime<<endl;
	//rofs<< "total right: "<<totalright<<endl;
	//wofs << "total time: "<<ltotaltime<<endl;
	//wofs<< "total wrong: "<<totalwrong<<endl;
	rofs.close();
	wofs.close();
	return true;
}

bool bowIndex::DetectOne(const string  file_path, string file_name, string &result , bool &randw)
{
	logoInfor logoFt;
	bool ret = getBowFeat(file_path,file_name,logoFt);
	if (ret ==false)
	{
		return false;
	}

	InvertedIndex::iterator pos;
	SimilarityMap::iterator Simpos;
	int i,j;
	for ( i=0,pos=logoInvertedIX.begin();i<clusterNum,pos!=logoInvertedIX.end();i++,pos++)
	{
		if (!logoFt.pointIndexofClusters[i].empty())
		{
			LogoKeyWD keyvec = pos->second;
			logoFt.normBOWDptors +=  pow((double)(logoFt.pointIndexofClusters[i].size()*keyWordIDf[i]),2);
			for ( j=0; j<keyvec.size();j++)
			{
				Simpos = simMap.find(keyvec[j].logoName);
				Simpos->second.sim += logoFt.pointIndexofClusters[i].size()*keyvec[j].num* pow(keyWordIDf[i],2);
			}
		}
	}
	logoFt.normBOWDptors = sqrt(logoFt.normBOWDptors);
	for (Simpos= simMap.begin();Simpos!=simMap.end();++Simpos)
	{
		logoScore logoSR = Simpos->second;
		logoSR.sim = logoSR.sim/(logoSR.normBOWDptors*logoFt.normBOWDptors); 
		this->simVec.push_back(logoSR);
		Simpos->second.sim =0;
	}
	std::partial_sort(this->simVec.begin(),this->simVec.begin()+10, this->simVec.end(),simCompare);
	if (this->simVec[0].logoName ==file_name)
	{
		randw=true;
	}
	result = "siftnum: "+ boost::lexical_cast<string>(logoFt.keypointNum)+ " result: "+ this->simVec[0].logoName+ " sim: "+boost::lexical_cast<string>(this->simVec[0].sim);
	this->simVec.clear();
	return true;
}
bool bowIndex::DetectGood(double &m_ap){
	ofstream ofs;
	ofs.open("./result.txt",ios::out | ios::trunc);
	InvertedIndex::iterator pos;
	SimilarityMap::iterator Simpos;
	LogosInfoMap::iterator goodpos;
	int i,j,k;
	ltotaltime = 0;
	for (goodpos = goodlogosFtInfor.begin();goodpos!=goodlogosFtInfor.end();goodpos++)
	{
		logoInfor logoFt = goodpos->second;
		lstart = clock();
		for ( i=0,pos=logoInvertedIX.begin();i<clusterNum,pos!=logoInvertedIX.end();i++,pos++)
		{
			if (!logoFt.pointIndexofClusters[i].empty())
			{
				LogoKeyWD keyvec = pos->second;
				for ( j=0; j<keyvec.size();j++)
				{
					Simpos = simMap.find(keyvec[j].logoName);
					Simpos->second.sim += logoFt.pointIndexofClusters[i].size()*keyvec[j].num* pow(keyWordIDf[i],2);
				}
			}
		}
		for (Simpos= simMap.begin();Simpos!=simMap.end();++Simpos)
		{
			logoScore logoSR = Simpos->second;
			logoSR.sim = logoSR.sim/(logoSR.normBOWDptors*logoFt.normBOWDptors); 
			this->simVec.push_back(logoSR);
			Simpos->second.sim =0;
		}
		std::sort(this->simVec.begin(), this->simVec.end(),simCompare);
		lfinish = clock();
		ltotaltime += (double)(lfinish - lstart)/CLOCKS_PER_SEC;
		logoFt.apoint = compute_ap(logoFt,simVec);
		m_ap += logoFt.apoint;
		ofs << setiosflags(ios::left)<<setw(36) <<logoFt.baseinfor.filepath.c_str() << setw(36) << logoFt.baseinfor.logoName << setw(36) << logoFt.apoint << setw(36)<< " matching time："<< (double)(lfinish - lstart)/CLOCKS_PER_SEC<<endl;
		this->simVec.clear();
	}
	m_ap = m_ap/goodlogosFtInfor.size();
	ofs<<"map: "<<m_ap<<endl;
	ofs << "total time: "<<ltotaltime<<endl;
	ofs.close();
	return true;
}
bool bowIndex::DetectGood2(const string &file_path, string file_name, string &result, double &m_ap)
{
	logoInfor logoFt;
	bool ret = getBowFeat(file_path,file_name,logoFt);
	if (ret ==false)
	{
		return false;
	}

	ofstream ofs;
	ofs.open("./result.txt",ios::out | ios::app);
	InvertedIndex::iterator pos;
	SimilarityMap::iterator Simpos;
	int i,j,k;
	lstart = clock();
	for ( i=0,pos=logoInvertedIX.begin();i<clusterNum,pos!=logoInvertedIX.end();i++,pos++)
	{
		if (!logoFt.pointIndexofClusters[i].empty())
		{
			LogoKeyWD keyvec = pos->second;
			logoFt.normBOWDptors +=  pow((double)(logoFt.pointIndexofClusters[i].size()*keyWordIDf[i]),2);
			for ( j=0; j<keyvec.size();j++)
			{
				Simpos = simMap.find(keyvec[j].logoName);
				Simpos->second.sim += logoFt.pointIndexofClusters[i].size()*keyvec[j].num* pow(keyWordIDf[i],2);
			}
		}
	}
	logoFt.normBOWDptors = sqrt(logoFt.normBOWDptors);
	for (Simpos= simMap.begin();Simpos!=simMap.end();++Simpos)
	{
		logoScore logoSR = Simpos->second;
		logoSR.sim = logoSR.sim/(logoSR.normBOWDptors*logoFt.normBOWDptors); 
		this->simVec.push_back(logoSR);
		Simpos->second.sim =0;
	}
	std::sort(this->simVec.begin(), this->simVec.end(),simCompare);
	lfinish = clock();
	ltotaltime += (double)(lfinish - lstart)/CLOCKS_PER_SEC;
	logoFt.apoint = compute_ap(logoFt,simVec);
	m_ap += logoFt.apoint;
	ofs << setiosflags(ios::left)<<setw(36) <<logoFt.baseinfor.filepath.c_str() << setw(36) << logoFt.baseinfor.logoName << setw(36) << logoFt.apoint << setw(36)<< " matching time："<< (double)(lfinish - lstart)/CLOCKS_PER_SEC<<endl;
	this->simVec.clear();
	ofs.close();
	return true;
}
bool bowIndex::getBowFeat(const string  file_path, string file_name,logoInfor &logoBowFt)
{
	Mat img = imread(file_path);
    if (img.empty())
    {
        return false;
    }
	bool ret = getBowFeat(img,file_name,logoBowFt);
	logoBowFt.baseinfor.filepath = file_path;
	return ret;
}
bool bowIndex::getBowFeat(const Mat &img, string file_name,logoInfor &logoBowFt)
{
    const bool flag = (*this).getBowFeat(img, file_name, logoBowFt);
	return const_cast<bool &>(flag);
}

bool bowIndex::getBowFeat(const Mat &img, string file_name, logoInfor &logoBowFt) const {
    logoBowFt.baseinfor.logoName = file_name;
    logoBowFt.baseinfor.width = img.cols;
    logoBowFt.baseinfor.height = img.rows;
    logoBowFt.pImg = img;
    logoBowFt.normBOWDptors =0;
    bool getRet = const_cast<bowIndex &>(*this).getSiftFeat(img, logoBowFt);
    if (logoBowFt.sifDescriptors.empty())
    {
        cout<<" 无sift特征"<<endl;
        return false;
    }
    logoBowFt.keypointNum = logoBowFt.keypoints.size();
    bowDE->compute(img,logoBowFt.keypoints,logoBowFt.bowDescriptors,&logoBowFt.pointIndexofClusters);
    return true;
}
bool bowIndex::getSiftFeat(const Mat &img, logoInfor &logoFeat){

	//detect keypoints;  
	cout << endl << "< Extracting keypoints from images..." << endl;  
	//detector->detect( img, logoFeat.keypoints );    
	detector->detect(img, logoFeat.keypoints);
	cout <<"img1:"<< logoFeat.keypoints.size() << endl;  
	if (logoFeat.keypoints.empty())
	{
		cout<<" 无sift特征"<<endl;
		return false;
	}
	//compute descriptors for keypoints;
	cout << "< Computing descriptors for keypoints from images..." << endl;   
	//descriptorExtractor->compute( img, logoFeat.keypoints, logoFeat.sifDescriptors );  
	descriptorExtractor->compute( img, logoFeat.keypoints, logoFeat.sifDescriptors );  

	cout<<endl<<"< Descriptoers Size: "<<logoFeat.sifDescriptors.size()<<" >"<<endl;  
	cout<<endl<<"descriptor's col: "<<logoFeat.sifDescriptors.cols<<endl  
		<<"descriptor's row: "<<logoFeat.sifDescriptors.rows<<endl;  
	cout << ">" << endl;  

	return true;
}

float bowIndex::compute_ap(logoInfor logoFt, Similarity ranked_list)
{
	float old_recall = 0.0;
	float old_precision = 1.0;
	float ap = 0.0;

	size_t intersect_size = 0;
	size_t i = 0;
	int tag;
	if (logoFt.baseinfor.logoName.find("_")==std::string::npos) // npos 代表没找到
	{
		tag = logoFt.baseinfor.logoName.length();
	}else{
		tag = logoFt.baseinfor.logoName.find("_");
	}
	
	string name = logoFt.baseinfor.logoName.substr(0,tag);
	map<string, int>::iterator pos= goodLogoNum.find(name);
	int num = pos->second;

	for ( ; i<ranked_list.size(); ++i) {
		if (ranked_list[i].logoName.find(name)!= std::string::npos) 
			intersect_size++;

		float recall = intersect_size / num;
		float precision = intersect_size / (i + 1.0);

		//if( old_precision ==1 && precision < 1 )
		//{
		//	printf( "precision  i=%d\n", i );
		//}

		ap += (recall - old_recall)*((old_precision + precision)/2.0);  // 面积

		old_recall = recall;
		old_precision = precision;
	}
	return ap;
}

bool simCompare(const logoScore& x, const logoScore& y)
{  
	return x.sim > y.sim;  
}  


