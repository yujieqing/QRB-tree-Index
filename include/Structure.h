#ifndef STRUCTURE_H
#define STRUCTURE_H
#include<unordered_map>
#include<map>
#include"RTree.h"
using namespace std;
typedef uint64_t TCode;
typedef RTree<long long, double, 2, double> MyTree;
struct Rect
{
	double min[2], max[2];
};
class QRBtree
{
public:
	QRBtree(const char* dir, const char* indexname, int l0);
	void BuiltForDataset(int cflag);
	
	vector<long long>* RoughsearchFromDisk(const Rect& rect);
	vector<long long>* ExactsearchFromDisk_New(const Rect& rect);
	vector<long long>* ExactsearchFromDisk_Tra(const Rect& rect);

private:
	
	struct QRBvalue
	{
		MyTree R;
		vector<long long> B;
	};
	struct MBR
	{
		Rect envelope;
		long long ptr;
	};

	string SavePath, SaveName;
	Rect DsExtent;//extent for the dataset
	Rect CoorExtent;//extent for the coordinate
	long FeatureNum;
	int L0,LongOffset;
	int fileposition;
	int CFlag, SL;
	map<int, string>FILEID;
	unordered_map<TCode, QRBvalue> QRBIndex;

private:
	int CalSL();
	void CalMaxLevel();
	void BuiltForFile(const char* DsFileNameShp);
	void Insert(long long ptr, const Rect mbr, const char* dirForIndex);
	void dump(const char* dirForIndex);
	TCode CalCode4Feature(const Rect& mbr);
	long long FeatureIDPath2ptr(string filename, long long fid);
	void erase(const Rect& rect);
	void exact(const Rect& rect, const char* data_adress, std::vector<long long> FID, std::vector<long long> tmpbuf);
	void getDsIF();
	void getDsExtent(int fileNum,const char* DsFileNameShp);
	int getFilePostion(int size);
	void GetCodesFromRect(const Rect& rect, vector<uint64_t>& ArrItr, vector<uint64_t>& ArrBrd);
	void GetCodesFromRect(const Rect& rect, vector<uint64_t>& ArrItr, vector<uint64_t>& ArrBrd, vector<uint64_t>& ArrCrn);
};

class QRtree
{
public:
	QRtree(const char* dir, const char* indexname, int l0);
	void BuiltForDataset(int cflag);

	vector<long long>* QRRoughsearchFromDisk(const Rect& rect);
	vector<long long>* QRExactsearchFromDisk_New(const Rect& rect);
	vector<long long>* QRExactsearchFromDisk_Tra(const Rect& rect);

private:

	struct QRvalue
	{
		MyTree R;
	};
	struct MBR
	{
		Rect envelope;
		long long ptr;
	};

	string SavePath, SaveName;
	Rect DsExtent;//extent for the dataset
	Rect CoorExtent;//extent for the coordinate
	long FeatureNum;
	int L0, LongOffset;
	int fileposition;
	int CFlag, SL;
	map<int, string>FILEID;
	unordered_map<TCode, QRvalue> QRIndex;

private:
	int CalSL();
	void CalMaxLevel();
	void BuiltForFile(const char* DsFileNameShp);
	void Insert(long long ptr, const Rect mbr, const char* dirForIndex);
	void dump(const char* dirForIndex);
	TCode CalCode4Feature(const Rect& mbr);
	long long FeatureIDPath2ptr(string filename, long long fid);
	void erase(const Rect& rect);
	void exact(const Rect& rect, const char* data_adress, std::vector<long long> FID, std::vector<long long> tmpbuf);
	void getDsIF();
	void getDsExtent(int fileNum, const char* DsFileNameShp);
	int getFilePostion(int size);
	void GetCodesFromRect(const Rect& rect, vector<uint64_t>& ArrItr, vector<uint64_t>& ArrBrd);
	void GetCodesFromRect(const Rect& rect, vector<uint64_t>& ArrItr, vector<uint64_t>& ArrBrd, vector<uint64_t>& ArrCrn);
};

class Geohash
{
public:
	Geohash(const char* dir, const char* indexname, int l0);
	void BuiltForDataset();

	vector<long long>* RoughsearchFromDisk_Geohash(const Rect& rect);
	vector<long long>* ExactsearchFromDisk_Geohash(const Rect& rect);

private:

	struct MBR
	{
		Rect envelope;
		long long ptr;
	};

	string SavePath, SaveName;
	Rect DsExtent;//extent for the dataset
	Rect CoorExtent;//extent for the coordinate
	long FeatureNum;
	int L0, LongOffset;
	int fileposition;
	map<int, string>FILEID;
	unordered_map<string, vector<MBR>> geohashIndex;

private:
	void BuiltForFile(const char* DsFileNameShp);
	void GHInsert(long long ptr, const Rect mbr);
	void dump4Geohash(const char* dirForIndex);
	string CalGeohashcode4Feature(const Rect& mbr);
	long long FeatureIDPath2ptr(string filename, long long fid);
	void erase(const Rect& rect);
	void exact(const Rect& rect, const char* data_adress, std::vector<long long> FID, std::vector<long long> tmpbuf);
	void getDsIF();
	void getDsExtent(int fileNum, const char* DsFileNameShp);
	int getFilePostion(int size);
	void GetGeohashcodesFromRect(const Rect& rect, vector<string>& ArrItr, vector<string>& ArrBrd);
};
#endif // !STRUCTURE_H
#pragma once
