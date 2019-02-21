#include "Similarity.h"
#include<fstream>
#include<iostream>
#include<windows.h>
#include<algorithm>
#include<assert.h>

using namespace std;

Similarity::Similarity(string dict)
	:DICT(dict)
	,DICT_PATH(dict + "/jieba.dict.utf8")
	,HMM_PATH(dict + "/hmm_model.utf8")
	,USER_DICT_PATH(dict + "/user.dict.utf8")
	,IDF_PATH(dict + "/idf.utf8")
	,STOP_WORD_PATH(dict + "/stop_words.utf8")
	,_jieba(DICT_PATH,
		    HMM_PATH,
			USER_DICT_PATH,
			IDF_PATH,
		    STOP_WORD_PATH)
	,MaxWordsNum(10)
{
	getStopWord(STOP_WORD_PATH.c_str());      //初始化停用词表
}

//获取词频
Similarity::wordfreq Similarity::getWordFreq(const char* filename)
{
	ifstream fin(filename);
	if(!fin.is_open())
	{
		cout << "Open File" << filename << "Failed"<<endl;
		return wordfreq();
	}

	string line;
	wordfreq freq;
	while(!fin.eof())
	{
		getline(fin,line);
		//GBK-->UTF8
		line = GBKToUTF8(line);
		//分词
		vector<string> words;
		_jieba.Cut(line,words,true);
		//统计词频，统计时先对应停用词表去掉停用词
		for(const auto& e : words)
		{
			if(_StopWordSet.count(e) > 0)
				continue;
			else
			{
				if(freq.count(e) > 0)
					freq[e]++;
				else
					freq[e] = 1;
			}
		}
		return freq;
	}
}
	//ifstream fin(filename);
	//if(!fin.is_open())
	//{
	//	cout << "Open File" << filename << "failed" << endl;
	//	return wordfreq();  //打开文件失败则返回一个空的map(没有任何词汇)
	//}
	//string line;
	//wordfreq freq;
	//while (!fin.eof())
	//{
	//	//读取并进行分词,分词前需要转码
	//	getline(fin,line);
	//	//GBK---==>UTF8
	//	line = GBKToUTF8(line);
	//	vector<string> words;
	//	_jieba.Cut(line,words,true);
	//	//统计词频
	//	for(const auto& e : words)
	//	{
	//		//统计词频时要去掉停用词
	//		if(_StopWordSet.count(e) > 0)   //如果存在停用词,返回值为1时表示存在
	//			continue;
	//		else
	//		{
	//			if(freq.count(e) > 0)
	//				freq[e]++;
	//			else
	//				freq[e] = 1;
	//		}
	//	}
	//}
	//return freq;

string Similarity::GBKToUTF8(string str)
{
	int len = MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,NULL,0);
	wchar_t* wstr = new wchar_t[len];
	MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,wstr,len);
	len = WideCharToMultiByte(CP_UTF8,0,wstr,-1,NULL,0,NULL,NULL);
	char* UTF8Char = new char[len];
	WideCharToMultiByte(CP_UTF8,0,wstr,-1,UTF8Char,len,NULL,NULL);
	return (UTF8Char);
	if(wstr)
	{
		delete[] wstr;
		wstr = nullptr;
	}
	if(UTF8Char)
	{
		delete[] UTF8Char;
		UTF8Char = nullptr;
	}
}

string Similarity::UTF8ToGBK(string str)
{
	int len = MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,NULL,0);
	wchar_t* wstr = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,wstr,len);
	len = WideCharToMultiByte(CP_ACP,0,wstr,-1,NULL,0,NULL,NULL);
	char* GBKChar = new char[len];
	WideCharToMultiByte(CP_ACP,0,wstr,-1,GBKChar,len,NULL,NULL);
	return (GBKChar);
	if(wstr)
	{
		delete[] wstr;
		wstr = nullptr;
	}
	if(GBKChar)
	{
		delete[] GBKChar;
		GBKChar = nullptr;
	}
}

void Similarity::getStopWord(const char* stopwordsFile)
{
	ifstream fin(stopwordsFile);
	if(!fin.is_open())
	{
		cout<<"Open File" << stopwordsFile <<"Failed" <<endl;
		return;
	}

	string line;
	while(!fin.eof())
	{
		getline(fin,line);
		_StopWordSet.insert(line);
	}
	fin.close();
}

bool compare(pair<string,int> left,pair<string,int> right)
{
	return left.second > right.second;    //逆序排序
}

vector<pair<string,int>> Similarity::sortByValue(wordfreq& freq)
{
	vector<pair<string,int>> freqVector(freq.begin(),freq.end());
	sort(freqVector.begin(),freqVector.end(),compare);
	return freqVector;
}

void Similarity::selectWords(vector<pair<string,int>>& freqvector,wordset& wset)
{
	int len = freqvector.size();
	int sz = len > MaxWordsNum ? MaxWordsNum : len;

	for(int i=0;i<sz;i++)
	{
		wset.insert(freqvector[i].first);     //first对应的是词(key)，second对应的是词频(value)
	}
}

vector<double> Similarity::getOneHot(wordset& wset,wordfreq& freq)
{
	//遍历wset中的词，根据词频建立向量
	vector<double> oneHot;
	for(const auto& e : wset)
	{
		if(freq.count(e))
			oneHot.push_back(freq[e]);     //存放词频
		else
			oneHot.push_back(0);
	}
	return oneHot;
}

double Similarity::Cos(vector<double> oneHot1,vector<double> oneHot2)
{
	double result = 0;;
	double mod1= 0,mod2 = 0;
	assert(oneHot1.size() == oneHot2.size());
	for(int i=0;i<oneHot1.size();i++)
	{
		result += oneHot1[i] * oneHot2[i];
	}

	for(int i=0;i<oneHot1.size();i++)
	{
		mod1 += pow(oneHot1[i],2);
	}
	mod1 = pow(mod1 ,0.5);
	for(int i=0;i<oneHot1.size();i++)
	{
		mod2 += pow(oneHot1[i],2);
	}
	mod2 = pow(mod2 ,0.5);

	return result / (mod1*mod2);
}

double Similarity::getSimilarity(const char* file1,const char* file2)
{
	wordfreq wf1 = getWordFreq(file1);
	wordfreq wf2 = getWordFreq(file2);
	vector<pair<string,int>> freqvector1 = sortByValue(wf1);
	vector<pair<string,int>> freqvector2 = sortByValue(wf2);

	cout << "freqvector1："<<endl;
	for(int i=0;i<MaxWordsNum;i++)
	{
		cout << UTF8ToGBK(freqvector1[i].first)<<":"<<freqvector1[i].second <<"\t";
	}
	cout<<endl;
	cout << "freqvector2："<<endl;
	for(int i=0;i<MaxWordsNum;i++)
	{
		cout << UTF8ToGBK(freqvector1[i].first)<<":"<<freqvector1[i].second <<"\t";
	}
	cout<<endl;

	wordset wset;
	selectWords(freqvector1,wset);
	selectWords(freqvector2,wset);
	cout<<"wset:"<<endl;
	for(const auto& e : wset)
	{
		cout << UTF8ToGBK(e) << " ";
	}
	cout<<endl;

	vector<double> oneHot1 = getOneHot(wset,wf1);
	vector<double> oneHot2 = getOneHot(wset,wf2);
	cout<<"OneHot1:"<<endl;
	for(const auto& i : oneHot1)
	{
		cout << i << " ";
	}
	cout<< endl;
	cout<<"OneHot2:"<<endl;
	for(const auto& j : oneHot2)
	{
		cout << j << " ";
	}
	cout<< endl;

	return Cos(oneHot1,oneHot2);

}