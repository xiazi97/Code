#include "Similarity.h"
#include<iostream>
using namespace std;
int main()
{
	Similarity test = ("../dict");

	string file1 = "../test1.txt";
	string file2 = "../test2.txt";
	cout<<"Cos£º"<<test.getSimilarity(file1.c_str(),file2.c_str()) * 100 << "%";
	return 0;
}
