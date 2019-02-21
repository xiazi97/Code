#define _CRT_SECURE_NO_WARNINGS
#include <cppjieba/Jieba.hpp>
#include <unordered_map>
#include<string>
#include <unordered_set>

class Similarity
{
public:
	typedef std::unordered_map<std::string,int> wordfreq;
	typedef std::unordered_set<std::string> wordset; 
	Similarity(std::string dict);
	double getSimilarity(const char* file1,const char* file2);
	



private:
	void getStopWord(const char* stopwordsFile);
	wordfreq getWordFreq(const char* file);    //词以及对应词频
	std::vector<std::pair<std::string,int>> sortByValue(Similarity::wordfreq& freq);
	void selectWords(std::vector<std::pair<std::string,int>>& freqvector,wordset& wset);
	std::vector<double> getOneHot(wordset& wset,wordfreq& freq);
	double Cos(std::vector<double> oneHot1,std::vector<double> oneHot2);
	std::string GBKToUTF8(std::string str);
	std::string UTF8ToGBK(std::string str);

	std::string DICT;
	std::string DICT_PATH;
    std::string HMM_PATH;
	std::string USER_DICT_PATH;
    std::string IDF_PATH;
    std::string STOP_WORD_PATH;
	cppjieba::Jieba _jieba;
	wordset _StopWordSet;   //停用词
	int MaxWordsNum;
};