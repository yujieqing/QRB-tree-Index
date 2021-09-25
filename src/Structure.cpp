#include<unordered_set>
#include<fstream>
#include<io.h>
#include<ctime>
#include<stack>
#include"Structure.h"
#include"ogrsf_frmts.h"
#include"cgeohash.h"
#pragma warning(disable: 4996)
#pragma comment(lib,"gdal_i.lib")
#pragma comment(lib,"cgeohash.lib")
using namespace cgeohash;
using namespace std;
std::vector<long long> ptrbuf1;
std::vector<long long> ptrbuf2;
std::vector<long long> ptrbuf3;
std::unordered_set<long long> ptrset;

int CreateDir(string path)
{
	int len = path.length();
	char tmpDirPath[256] = { 0 };
	for (int i = 0; i < len; i++)
	{
		tmpDirPath[i] = path[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (_access(tmpDirPath, 0) == -1)
			{
				int ret = _mkdir(tmpDirPath);
				if (ret == -1) return ret;
				else return ret;
			}
		}
	}
	return 1;
}
bool BorderSearchCallback(long long id1)
{
	ptrbuf1.push_back(id1);
	return true; // keep going
}
bool CornerSearchCallbackByRect(long long id1)
{
	ptrset.insert(id1);
	return true; // keep going
}
bool TraSearchCallbackByRect(long long id1)
{
	ptrbuf2.push_back(id1);
	return true; // keep going
}
bool CornerSearchCallbackByPoint(long long id1)
{
	ptrbuf2.push_back(id1);
	return true; // keep going
}
bool IsOverlap(Rect a_rectA, Rect a_rectB)
{
	if (a_rectA.min[0] > a_rectB.max[0] || a_rectA.max[0] < a_rectB.min[0] ||
		a_rectA.min[1] > a_rectB.max[1] || a_rectA.max[1] < a_rectB.min[1])
		return false;
	return true;
}
bool IsOverlap1(Rect a_rectA, Rect a_rectB)
{
	if (a_rectA.min[0] > a_rectB.max[0] || a_rectA.max[0] < a_rectB.min[0] ||
		a_rectA.min[1] > a_rectB.max[1] || a_rectA.max[1] < a_rectB.min[1])
		return false;
	else if (a_rectA.min[0] <= a_rectB.min[0] && a_rectA.max[0] >= a_rectB.max[0] &&
		a_rectA.min[1] <= a_rectB.min[1] && a_rectA.max[1] >= a_rectB.max[1])
		return false;
	return true;
}

QRBtree::QRBtree(const char* dir, const char* indexname, int l0) :L0(l0), SavePath(dir), SaveName(indexname) {};
QRtree::QRtree(const char* dir, const char* indexname, int l0) :L0(l0), SavePath(dir), SaveName(indexname) {};
Geohash::Geohash(const char* dir, const char* indexname, int l0) :L0(l0), SavePath(dir), SaveName(indexname) {};

void QRBtree::BuiltForDataset(int cflag)
{
	CFlag = cflag;
	string folder = SavePath + SaveName + "QRB/";
	int flag = CreateDir(folder);
	getDsIF();
	if (CFlag == 1)
	{
		CoorExtent.min[0] = 0; CoorExtent.min[1] = 0;
		CoorExtent.max[0] = 360; CoorExtent.max[1] = 180;
		if (DsExtent.min[0] < 0) LongOffset = (DsExtent.min[0] / -10) + 1;
		else LongOffset = 0;
		SL = CalSL();
	}
	else
	{
		CoorExtent = DsExtent;
		SL = 0;
	}
	if (L0 == 0) CalMaxLevel();
	if (flag == 0)
	{
		intptr_t handle;
		struct _finddata_t fileinfo;
		handle = _findfirst((SavePath + "/*.shp").c_str(), &fileinfo);
		if (handle == -1)
			return;
		do
		{
			string fullname, filename, extensionname;
			fullname = fileinfo.name;
			extensionname = fullname.substr(fullname.rfind("."), fullname.size());
			filename = fullname.substr(0, fullname.rfind("."));
			fullname = SavePath + fullname;;
			if (extensionname == ".shp") BuiltForFile(fullname.c_str());
		} while (!_findnext(handle, &fileinfo));
		_findclose(handle);
		if (QRBIndex.size() < 1000)
		{
			dump((SavePath + SaveName + "QRB/").c_str());
			QRBIndex.clear();
		}
	}
}
void QRtree::BuiltForDataset(int cflag)
{
	CFlag = cflag;
	string folder = SavePath + SaveName + "QR/";
	int flag = CreateDir(folder);
	getDsIF();
	if (CFlag == 1)
	{
		CoorExtent.min[0] = 0; CoorExtent.min[1] = 0;
		CoorExtent.max[0] = 360; CoorExtent.max[1] = 180;
		if (DsExtent.min[0] < 0) LongOffset = (DsExtent.min[0] / -10) + 1;
		else LongOffset = 0;
		SL = CalSL();
	}
	else
	{
		CoorExtent = DsExtent;
		SL = 0;
	}
	if (L0 == 0) CalMaxLevel();
	if (flag == 0)
	{
		intptr_t handle;
		struct _finddata_t fileinfo;
		handle = _findfirst((SavePath + "/*.shp").c_str(), &fileinfo);
		if (handle == -1)
			return;
		do
		{
			string fullname, filename, extensionname;
			fullname = fileinfo.name;
			extensionname = fullname.substr(fullname.rfind("."), fullname.size());
			filename = fullname.substr(0, fullname.rfind("."));
			fullname = SavePath + fullname;;
			if (extensionname == ".shp") BuiltForFile(fullname.c_str());
		} while (!_findnext(handle, &fileinfo));
		_findclose(handle);
		if (QRIndex.size() < 1000)
		{
			dump((SavePath + SaveName + "QR/").c_str());
			QRIndex.clear();
		}
	}
}
void Geohash::BuiltForDataset()
{
	string folder = SavePath + SaveName + "Geohash/";
	int flag = CreateDir(folder);
	getDsIF();
	CoorExtent.min[0] = 0; CoorExtent.min[1] = 0;
	CoorExtent.max[0] = 360; CoorExtent.max[1] = 180;
	if (DsExtent.min[0] < 0) LongOffset = (DsExtent.min[0] / -10) + 1;
	else LongOffset = 0;
	if (flag == 0)
	{
		intptr_t handle;
		struct _finddata_t fileinfo;
		handle = _findfirst((SavePath + "/*.shp").c_str(), &fileinfo);
		if (handle == -1)
			return;
		do
		{
			string fullname, filename, extensionname;
			fullname = fileinfo.name;
			extensionname = fullname.substr(fullname.rfind("."), fullname.size());
			filename = fullname.substr(0, fullname.rfind("."));
			fullname = SavePath + fullname;;
			if (extensionname == ".shp") BuiltForFile(fullname.c_str());
		} while (!_findnext(handle, &fileinfo));
		_findclose(handle);
		if (geohashIndex.size() < 1000)
		{
			dump4Geohash((SavePath + SaveName + "Geohash/").c_str());
			geohashIndex.clear();
		}
	}
}

int QRBtree::CalSL()
{
	int i, imin[2], imax[2];
	double xsize, ysize;
	if (CFlag == 1)
	{
		for (i = 0; ; i++)
		{
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((DsExtent.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((DsExtent.min[1] + 90 - CoorExtent.min[1]) / ysize);
			imax[0] = int((DsExtent.max[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imax[1] = int((DsExtent.max[1] + 90 - CoorExtent.min[1]) / ysize);
			if (imin[0] == imax[0] && imin[1] == imax[1])
			{
				continue;
			}
			else
			{
				return i;
			}
		}
	}
	else
	{
		for (i = 0; ; i++)
		{
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((DsExtent.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((DsExtent.min[1] - CoorExtent.min[1]) / ysize);
			imax[0] = int((DsExtent.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((DsExtent.max[1] - CoorExtent.min[1]) / ysize);
			if (imin[0] == imax[0] && imin[1] == imax[1])
			{
				continue;
			}
			else
			{
				return i - 1;
			}
		}
	}
}
int QRtree::CalSL()
{
	int i, imin[2], imax[2];
	double xsize, ysize;
	if (CFlag == 1)
	{
		for (i = 0; ; i++)
		{
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((DsExtent.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((DsExtent.min[1] + 90 - CoorExtent.min[1]) / ysize);
			imax[0] = int((DsExtent.max[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imax[1] = int((DsExtent.max[1] + 90 - CoorExtent.min[1]) / ysize);
			if (imin[0] == imax[0] && imin[1] == imax[1])
			{
				continue;
			}
			else
			{
				return i;
			}
		}
	}
	else
	{
		for (i = 0; ; i++)
		{
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((DsExtent.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((DsExtent.min[1] - CoorExtent.min[1]) / ysize);
			imax[0] = int((DsExtent.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((DsExtent.max[1] - CoorExtent.min[1]) / ysize);
			if (imin[0] == imax[0] && imin[1] == imax[1])
			{
				continue;
			}
			else
			{
				return i - 1;
			}
		}
	}
}

void QRBtree::CalMaxLevel()
{
	int s, a = 1;
	for (int i = 1; ; i++)
	{
		a = a + pow(4, i);
		s = (FeatureNum * 8) / a;
		if (s < 4096)
		{
			L0 = i - 1;
			return;
		}
	}
}
void QRtree::CalMaxLevel()
{
	int s, a = 1;
	for (int i = 1; ; i++)
	{
		a = a + pow(4, i);
		s = (FeatureNum * 8) / a;
		if (s < 4096)
		{
			L0 = i - 1;
			return;
		}
	}
}

void QRBtree::BuiltForFile(const char* DsFileNameShp)
{
	string filename = DsFileNameShp;

	const char* pszDriverName = "ESRI Shapefile";
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");

	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	GDALDataset* poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer;
	poLayer = poDS->GetLayer(0);

	OGRFeature* poFeature;
	poLayer->ResetReading();

	while ((poFeature = poLayer->GetNextFeature()) != NULL)
	{
		OGRGeometry* poGeometry = poFeature->GetGeometryRef();
		long long FID = poFeature->GetFID();
		double minx, miny, maxx, maxy;
		OGREnvelope* poEnvelope = new OGREnvelope;
		poGeometry->getEnvelope(poEnvelope);
		
		Rect mbr;

		minx = poEnvelope->MinX;
		miny = poEnvelope->MinY;
		maxx = poEnvelope->MaxX;
		maxy = poEnvelope->MaxY;

		mbr.min[0] = minx;
		mbr.min[1] = miny;
		mbr.max[0] = maxx;
		mbr.max[1] = maxy;

		long long ptr = FeatureIDPath2ptr(filename, FID);
		Insert(ptr, mbr, (SavePath + SaveName + "QRB/").c_str());
		if (QRBIndex.size() >= 1000)
		{
			dump((SavePath + SaveName + "QRB/").c_str());
			QRBIndex.clear();
		}
		OGRFeature::DestroyFeature(poFeature);
		delete poEnvelope;
	}
	GDALClose(poDS);
}
void QRtree::BuiltForFile(const char* DsFileNameShp)
{
	string filename = DsFileNameShp;

	const char* pszDriverName = "ESRI Shapefile";
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");

	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	GDALDataset* poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer;
	poLayer = poDS->GetLayer(0);

	OGRFeature* poFeature;
	poLayer->ResetReading();

	while ((poFeature = poLayer->GetNextFeature()) != NULL)
	{
		OGRGeometry* poGeometry = poFeature->GetGeometryRef();
		long long FID = poFeature->GetFID();
		double minx, miny, maxx, maxy;
		OGREnvelope* poEnvelope = new OGREnvelope;
		poGeometry->getEnvelope(poEnvelope);

		Rect mbr;

		minx = poEnvelope->MinX;
		miny = poEnvelope->MinY;
		maxx = poEnvelope->MaxX;
		maxy = poEnvelope->MaxY;

		mbr.min[0] = minx;
		mbr.min[1] = miny;
		mbr.max[0] = maxx;
		mbr.max[1] = maxy;

		long long ptr = FeatureIDPath2ptr(filename, FID);
		Insert(ptr, mbr, (SavePath + SaveName + "QR/").c_str());
		if (QRIndex.size() >= 1000)
		{
			dump((SavePath + SaveName + "QR/").c_str());
			QRIndex.clear();
		}
		OGRFeature::DestroyFeature(poFeature);
		delete poEnvelope;
	}
	GDALClose(poDS);
}
void Geohash::BuiltForFile(const char* DsFileNameShp)
{
	string filename = DsFileNameShp;

	const char* pszDriverName = "ESRI Shapefile";
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");

	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	GDALDataset* poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer;
	poLayer = poDS->GetLayer(0);

	OGRFeature* poFeature;
	poLayer->ResetReading();

	while ((poFeature = poLayer->GetNextFeature()) != NULL)
	{
		OGRGeometry* poGeometry = poFeature->GetGeometryRef();
		long long FID = poFeature->GetFID();
		double minx, miny, maxx, maxy;
		OGREnvelope* poEnvelope = new OGREnvelope;
		poGeometry->getEnvelope(poEnvelope);

		Rect mbr;

		minx = poEnvelope->MinX;
		miny = poEnvelope->MinY;
		maxx = poEnvelope->MaxX;
		maxy = poEnvelope->MaxY;

		mbr.min[0] = minx;
		mbr.min[1] = miny;
		mbr.max[0] = maxx;
		mbr.max[1] = maxy;

		long long ptr = FeatureIDPath2ptr(filename, FID);
		GHInsert(ptr, mbr);
		if (geohashIndex.size() >= 1000)
		{
			dump4Geohash((SavePath + SaveName + "Geohash/").c_str());
			geohashIndex.clear();
		}
		OGRFeature::DestroyFeature(poFeature);
		delete poEnvelope;
	}
	GDALClose(poDS);
}

void QRBtree::Insert(long long ptr, const Rect mbr, const char* dirForIndex)
{
	TCode code = CalCode4Feature(mbr);
	unordered_map<TCode, QRBvalue>::iterator it = QRBIndex.find(code);
	string stream = dirForIndex + to_string(code);
	if (it == QRBIndex.end())
	{
		QRBvalue newvalue;
		newvalue.R.Load((stream + "R").c_str());
		newvalue.R.Insert(mbr.min, mbr.max, ptr);
		newvalue.B.push_back(ptr);
		QRBIndex.insert(make_pair(code, newvalue));
		newvalue.B.clear();
	}
	else
	{
		it->second.B.push_back(ptr);
		it->second.R.Insert(mbr.min, mbr.max, ptr);
	}
}
void QRtree::Insert(long long ptr, const Rect mbr, const char* dirForIndex)
{
	TCode code = CalCode4Feature(mbr);
	unordered_map<TCode, QRvalue>::iterator it = QRIndex.find(code);
	string stream = dirForIndex + to_string(code);
	if (it == QRIndex.end())
	{
		QRvalue newvalue;
		newvalue.R.Load((stream + "R").c_str());
		newvalue.R.Insert(mbr.min, mbr.max, ptr);
		QRIndex.insert(make_pair(code, newvalue));
	}
	else
	{
		it->second.R.Insert(mbr.min, mbr.max, ptr);
	}
}
void Geohash::GHInsert(long long ptr, const Rect mbr)
{
	vector<MBR> g_value;
	string code = CalGeohashcode4Feature(mbr);
	unordered_map<string, vector<MBR>>::iterator it = geohashIndex.find(code);
	MBR r; r.ptr = ptr;
	r.envelope.min[0] = mbr.min[0]; r.envelope.min[1] = mbr.min[1];
	r.envelope.max[0] = mbr.max[0]; r.envelope.max[1] = mbr.max[1];
	if (it == geohashIndex.end())
	{
		g_value.push_back(r);
		geohashIndex.insert(make_pair(code, g_value));
		g_value.clear();
	}
	else
	{
		it->second.push_back(r);
	}
}

long long QRBtree::FeatureIDPath2ptr(string filename, long long fid)
{
	long long ptr;
	int fileID = 0;
	for (map<int, string>::iterator idter = FILEID.begin(); idter != FILEID.end(); idter++)
	{
		if (idter->second == filename)
		{
			fileID = idter->first;
			break;
		}
	}
	ptr = (fid << fileposition) | (uint64_t)fileID;
	return ptr;
}
long long QRtree::FeatureIDPath2ptr(string filename, long long fid)
{
	long long ptr;
	int fileID = 0;
	for (map<int, string>::iterator idter = FILEID.begin(); idter != FILEID.end(); idter++)
	{
		if (idter->second == filename)
		{
			fileID = idter->first;
			break;
		}
	}
	ptr = (fid << fileposition) | (uint64_t)fileID;
	return ptr;
}
long long Geohash::FeatureIDPath2ptr(string filename, long long fid)
{
	long long ptr;
	int fileID = 0;
	for (map<int, string>::iterator idter = FILEID.begin(); idter != FILEID.end(); idter++)
	{
		if (idter->second == filename)
		{
			fileID = idter->first;
			break;
		}
	}
	ptr = (fid << fileposition) | (uint64_t)fileID;
	return ptr;
}

TCode QRBtree::CalCode4Feature(const Rect& mbr)
{
	TCode morton = 0;
	int i, imin[2], imax[2];
	double xsize, ysize;
	if (CFlag == 1)
	{
		for (i = SL; i <= SL + L0; i++)
		{
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((mbr.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] + 90 - CoorExtent.min[1]) / ysize);
			imax[0] = int((mbr.max[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imax[1] = int((mbr.max[1] + 90 - CoorExtent.min[1]) / ysize);
			if (imin[0] == imax[0] && imin[1] == imax[1])
			{
				continue;
			}
			else
			{
				break;
			}
		}
		int l = i - 1;
		xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << l); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << l);
		imin[0] = int((mbr.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] + 90 - CoorExtent.min[1]) / ysize);

		for (int k = l; k >= 0; k--)
		{
			morton |= ((imin[0] >> l - k) & 1) << (2 * (l - k) + 1) | ((imin[1] >> l - k) & 1) << (2 * (l - k));
		}
		morton = (morton << 4) | (uint64_t)(l);
		return morton;
	}
	else
	{
		for (i = SL; i <= SL + L0; i++)
		{
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			if (mbr.max[0] == CoorExtent.max[0] && mbr.max[1] != CoorExtent.max[1])
			{
				imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);
				imax[0] = int((mbr.max[0] - CoorExtent.min[0] - 0.0000000001) / xsize); imax[1] = int((mbr.max[1] - CoorExtent.min[1]) / ysize);
			}
			else if (mbr.max[1] == CoorExtent.max[1] && mbr.max[0] != CoorExtent.max[0])
			{
				imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);
				imax[0] = int((mbr.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((mbr.max[1] - CoorExtent.min[1] - 0.0000000001) / ysize);
			}
			else if (mbr.max[1] == CoorExtent.max[1] && mbr.max[0] == CoorExtent.max[0])
			{
				imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);
				imax[0] = int((mbr.max[0] - CoorExtent.min[0] - 0.0000000001) / xsize); imax[1] = int((mbr.max[1] - CoorExtent.min[1] - 0.0000000001) / ysize);
			}
			else
			{
				imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);
				imax[0] = int((mbr.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((mbr.max[1] - CoorExtent.min[1]) / ysize);
			}
			if (imin[0] == imax[0] && imin[1] == imax[1])
			{
				continue;
			}
			else
			{
				break;
			}
		}
		int l = i - 1;
		xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << l); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << l);
		imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);

		for (int k = l; k >= 0; k--)
		{
			morton |= ((imin[0] >> l - k) & 1) << (2 * (l - k) + 1) | ((imin[1] >> l - k) & 1) << (2 * (l - k));
		}
		morton = (morton << 4) | (uint64_t)(l);
		return morton;
	}
}
TCode QRtree::CalCode4Feature(const Rect& mbr)
{
	TCode morton = 0;
	int i, imin[2], imax[2];
	double xsize, ysize;
	if (CFlag == 1)
	{
		for (i = SL; i <= SL + L0; i++)
		{
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((mbr.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] + 90 - CoorExtent.min[1]) / ysize);
			imax[0] = int((mbr.max[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imax[1] = int((mbr.max[1] + 90 - CoorExtent.min[1]) / ysize);
			if (imin[0] == imax[0] && imin[1] == imax[1])
			{
				continue;
			}
			else
			{
				break;
			}
		}
		int l = i - 1;
		xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << l); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << l);
		imin[0] = int((mbr.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] + 90 - CoorExtent.min[1]) / ysize);

		for (int k = l; k >= 0; k--)
		{
			morton |= ((imin[0] >> l - k) & 1) << (2 * (l - k) + 1) | ((imin[1] >> l - k) & 1) << (2 * (l - k));
		}
		morton = (morton << 4) | (uint64_t)(l);
		return morton;
	}
	else
	{
		for (i = SL; i <= SL + L0; i++)
		{
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			if (mbr.max[0] == CoorExtent.max[0] && mbr.max[1] != CoorExtent.max[1])
			{
				imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);
				imax[0] = int((mbr.max[0] - CoorExtent.min[0] - 0.0000000001) / xsize); imax[1] = int((mbr.max[1] - CoorExtent.min[1]) / ysize);
			}
			else if (mbr.max[1] == CoorExtent.max[1] && mbr.max[0] != CoorExtent.max[0])
			{
				imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);
				imax[0] = int((mbr.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((mbr.max[1] - CoorExtent.min[1] - 0.0000000001) / ysize);
			}
			else if (mbr.max[1] == CoorExtent.max[1] && mbr.max[0] == CoorExtent.max[0])
			{
				imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);
				imax[0] = int((mbr.max[0] - CoorExtent.min[0] - 0.0000000001) / xsize); imax[1] = int((mbr.max[1] - CoorExtent.min[1] - 0.0000000001) / ysize);
			}
			else
			{
				imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);
				imax[0] = int((mbr.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((mbr.max[1] - CoorExtent.min[1]) / ysize);
			}
			if (imin[0] == imax[0] && imin[1] == imax[1])
			{
				continue;
			}
			else
			{
				break;
			}
		}
		int l = i - 1;
		xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << l); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << l);
		imin[0] = int((mbr.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((mbr.min[1] - CoorExtent.min[1]) / ysize);

		for (int k = l; k >= 0; k--)
		{
			morton |= ((imin[0] >> l - k) & 1) << (2 * (l - k) + 1) | ((imin[1] >> l - k) & 1) << (2 * (l - k));
		}
		morton = (morton << 4) | (uint64_t)(l);
		return morton;
	}
}
string Geohash::CalGeohashcode4Feature(const Rect& mbr)
{
	for (int i = 1; i <= L0; i++)
	{
		string code1 = encode(mbr.min[1], mbr.min[0] + (LongOffset * 10), i);
		string code2 = encode(mbr.max[1], mbr.max[0] + (LongOffset * 10), i);
		if (code1 == code2)
		{
			continue;
		}
		else
		{
			return encode(mbr.min[1], mbr.min[0] + (LongOffset * 10), i - 1);
		}
	}
	return encode(mbr.min[1], mbr.min[0] + (LongOffset * 10), L0);
}

void QRBtree::getDsIF()
{
	intptr_t handle;
	struct _finddata_t fileinfo;
	int fileNum = 0;
	handle = _findfirst((SavePath+"/*.shp").c_str(), &fileinfo);
	if (handle == -1)
		return;
	do
	{
		if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
			string fullname, filename, extensionname;
			fullname = fileinfo.name;
			fileNum++;
			fullname = SavePath + fullname;
			FILEID.insert(make_pair(fileNum, fullname));
			getDsExtent(fileNum, fullname.c_str());
		}
	} while (!_findnext(handle, &fileinfo));
	_findclose(handle);
	fileposition = getFilePostion(FILEID.size());
	ofstream outFile1;
	outFile1.open((SavePath + SaveName + "QRB/fileID.txt").c_str(), ios::out | ios::binary);
	for (map<int, string>::iterator idter = FILEID.begin(); idter != FILEID.end(); idter++) {
		outFile1 << idter->first << idter->second << endl;
	}
	outFile1.close();
}
void QRtree::getDsIF()
{
	intptr_t handle;
	struct _finddata_t fileinfo;
	int fileNum = 0;
	handle = _findfirst((SavePath + "/*.shp").c_str(), &fileinfo);
	if (handle == -1)
		return;
	do
	{
		if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
			string fullname, filename, extensionname;
			fullname = fileinfo.name;
			fileNum++;
			fullname = SavePath + fullname;
			FILEID.insert(make_pair(fileNum, fullname));
			getDsExtent(fileNum, fullname.c_str());
		}
	} while (!_findnext(handle, &fileinfo));
	_findclose(handle);
	fileposition = getFilePostion(FILEID.size());
	ofstream outFile1;
	outFile1.open((SavePath + SaveName + "QR/fileID.txt").c_str(), ios::out | ios::binary);
	for (map<int, string>::iterator idter = FILEID.begin(); idter != FILEID.end(); idter++) {
		outFile1 << idter->first << idter->second << endl;
	}
	outFile1.close();
}
void Geohash::getDsIF()
{
	intptr_t handle;
	struct _finddata_t fileinfo;
	int fileNum = 0;
	handle = _findfirst((SavePath + "/*.shp").c_str(), &fileinfo);
	if (handle == -1)
		return;
	do
	{
		if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
			string fullname, filename, extensionname;
			fullname = fileinfo.name;
			fileNum++;
			fullname = SavePath + fullname;
			FILEID.insert(make_pair(fileNum, fullname));
			getDsExtent(fileNum, fullname.c_str());
		}
	} while (!_findnext(handle, &fileinfo));
	_findclose(handle);
	fileposition = getFilePostion(FILEID.size());
	ofstream outFile1;
	outFile1.open((SavePath + SaveName + "Geohash/fileID.txt").c_str(), ios::out | ios::binary);
	for (map<int, string>::iterator idter = FILEID.begin(); idter != FILEID.end(); idter++) {
		outFile1 << idter->first << idter->second << endl;
	}
	outFile1.close();
}

void QRBtree::getDsExtent(int fileNum, const char* DsFileNameShp)
{
	string filename = DsFileNameShp;

	const char* pszDriverName = "ESRI Shapefile";
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");

	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	GDALDataset* poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer;
	poLayer = poDS->GetLayer(0);

	OGREnvelope* poEnvelope = new OGREnvelope;
	poLayer->GetExtent(poEnvelope);
	double minx, miny, maxx, maxy;

	minx = poEnvelope->MinX;
	miny = poEnvelope->MinY;
	maxx = poEnvelope->MaxX;
	maxy = poEnvelope->MaxY;

	if (fileNum == 1)
	{
		FeatureNum = poLayer->GetFeatureCount();
		DsExtent.min[0] = minx; DsExtent.min[1] = miny;
		DsExtent.max[0] = maxx; DsExtent.max[1] = maxy;
	}
	else
	{
		FeatureNum += poLayer->GetFeatureCount();
		if (DsExtent.min[0] > minx)
		{
			DsExtent.min[0] = minx;
		}
		if (DsExtent.min[1] > miny)
		{
			DsExtent.min[1] = miny;
		}
		if (DsExtent.max[0] < maxx)
		{
			DsExtent.max[0] = maxx;
		}
		if (DsExtent.max[1] < maxy)
		{
			DsExtent.max[1] = maxy;
		}
	}
	delete poEnvelope;
	GDALClose(poDS);
}
void QRtree::getDsExtent(int fileNum, const char* DsFileNameShp)
{
	string filename = DsFileNameShp;

	const char* pszDriverName = "ESRI Shapefile";
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");

	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	GDALDataset* poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer;
	poLayer = poDS->GetLayer(0);

	OGREnvelope* poEnvelope = new OGREnvelope;
	poLayer->GetExtent(poEnvelope);
	double minx, miny, maxx, maxy;

	minx = poEnvelope->MinX;
	miny = poEnvelope->MinY;
	maxx = poEnvelope->MaxX;
	maxy = poEnvelope->MaxY;

	if (fileNum == 1)
	{
		FeatureNum = poLayer->GetFeatureCount();
		DsExtent.min[0] = minx; DsExtent.min[1] = miny;
		DsExtent.max[0] = maxx; DsExtent.max[1] = maxy;
	}
	else
	{
		FeatureNum += poLayer->GetFeatureCount();
		if (DsExtent.min[0] > minx)
		{
			DsExtent.min[0] = minx;
		}
		if (DsExtent.min[1] > miny)
		{
			DsExtent.min[1] = miny;
		}
		if (DsExtent.max[0] < maxx)
		{
			DsExtent.max[0] = maxx;
		}
		if (DsExtent.max[1] < maxy)
		{
			DsExtent.max[1] = maxy;
		}
	}
	delete poEnvelope;
	GDALClose(poDS);
}
void Geohash::getDsExtent(int fileNum, const char* DsFileNameShp)
{
	string filename = DsFileNameShp;

	const char* pszDriverName = "ESRI Shapefile";
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");

	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	GDALDataset* poDS = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer;
	poLayer = poDS->GetLayer(0);

	OGREnvelope* poEnvelope = new OGREnvelope;
	poLayer->GetExtent(poEnvelope);
	double minx, miny, maxx, maxy;

	minx = poEnvelope->MinX;
	miny = poEnvelope->MinY;
	maxx = poEnvelope->MaxX;
	maxy = poEnvelope->MaxY;

	if (fileNum == 1)
	{
		FeatureNum = poLayer->GetFeatureCount();
		DsExtent.min[0] = minx; DsExtent.min[1] = miny;
		DsExtent.max[0] = maxx; DsExtent.max[1] = maxy;
	}
	else
	{
		FeatureNum += poLayer->GetFeatureCount();
		if (DsExtent.min[0] > minx)
		{
			DsExtent.min[0] = minx;
		}
		if (DsExtent.min[1] > miny)
		{
			DsExtent.min[1] = miny;
		}
		if (DsExtent.max[0] < maxx)
		{
			DsExtent.max[0] = maxx;
		}
		if (DsExtent.max[1] < maxy)
		{
			DsExtent.max[1] = maxy;
		}
	}
	delete poEnvelope;
	GDALClose(poDS);
}

int QRBtree::getFilePostion(int size)
{
	int i;
	for (i = 0;; i++)
	{
		if (size / ((long)1 << i) < 1)break;
	}
	return i;
}
int QRtree::getFilePostion(int size)
{
	int i;
	for (i = 0;; i++)
	{
		if (size / ((long)1 << i) < 1)break;
	}
	return i;
}
int Geohash::getFilePostion(int size)
{
	int i;
	for (i = 0;; i++)
	{
		if (size / ((long)1 << i) < 1)break;
	}
	return i;
}

void QRBtree::dump(const char* dirForIndex)
{
	unordered_map<TCode, QRBvalue>::iterator b_map;
	for (b_map = QRBIndex.begin(); b_map != QRBIndex.end(); b_map++) {
		string fileadress = dirForIndex + to_string(b_map->first) + "B";
		FILE* m_file;
		m_file = fopen(fileadress.c_str(), "ab+");
		fwrite((void*)&b_map->second.B[0], sizeof(long long), b_map->second.B.size(), m_file);
		fileadress = dirForIndex + to_string(b_map->first) + "R";
		b_map->second.R.CalnByte();
		b_map->second.R.Save(fileadress.c_str());
		b_map->second.R.RemoveAll();
		fclose(m_file);
		m_file = NULL;
	}
}
void QRtree::dump(const char* dirForIndex)
{
	unordered_map<TCode, QRvalue>::iterator b_map;
	for (b_map = QRIndex.begin(); b_map != QRIndex.end(); b_map++) {
		string fileadress = dirForIndex + to_string(b_map->first) + "R";
		b_map->second.R.CalnByte();
		b_map->second.R.Save(fileadress.c_str());
		b_map->second.R.RemoveAll();
	}
}
void Geohash::dump4Geohash(const char* dirForIndex)
{
	unordered_map<string, vector<MBR>>::iterator g_map;
	for (g_map = geohashIndex.begin(); g_map != geohashIndex.end(); g_map++) {
		string fileadress = dirForIndex + g_map->first + "B";
		FILE* m_file;
		m_file = fopen(fileadress.c_str(), "ab+");
		fwrite((void*)&g_map->second[0], sizeof(MBR), g_map->second.size(), m_file);
		fclose(m_file);
		m_file = NULL;
	}
}

void QRBtree::GetCodesFromRect(const Rect& rect, vector<TCode>& ArrItr,vector<TCode>& ArrBrd)
/*************************************************

Function:       GetCodesFromRect

Description:    Calculate linear codes of the overlapping and interior grids for the rectangle

Input:          rect,rectangle for rough query

                ArrItr, array codes of the interior grids(empty)

				ArrBrd, array codes of the overlapping grids(empty)

Output:         ArrItr, array codes of the interior grids

				ArrBrd, array codes of the overlapping grids

*************************************************/
{
	uint64_t startcode = 0, tempC = startcode;
	if (CFlag == 1)
	{
		for (int i = 0; i <= SL + L0; i++)
		{
			double xsize, ysize;
			int imin[2], imax[2];
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((rect.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((rect.min[1] + 90 - CoorExtent.min[1]) / ysize);
			imax[0] = int((rect.max[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imax[1] = int((rect.max[1] + 90 - CoorExtent.min[1]) / ysize);
			for (int ix = imin[0]; ix <= imax[0]; ix++)
			{
				for (int iy = imin[1]; iy <= imax[1]; iy++)
				{
					if (ix == imin[0] || iy == imin[1] || ix == imax[0] || iy == imax[1])//code for border_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrBrd.push_back(tempC);
					}
					else//code for interior_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrItr.push_back(tempC);
					}
					tempC = startcode;
				}
			}
		}
	}
	else
	{
		for (int i = SL; i <= SL + L0; i++)
		{
			double xsize, ysize;
			int imin[2], imax[2];
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((rect.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((rect.min[1] - CoorExtent.min[1]) / ysize);
			imax[0] = int((rect.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((rect.max[1] - CoorExtent.min[1]) / ysize);
			for (int ix = imin[0]; ix <= imax[0]; ix++)
			{
				for (int iy = imin[1]; iy <= imax[1]; iy++)
				{
					if (ix == imin[0] || iy == imin[1] || ix == imax[0] || iy == imax[1])//code for border_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrBrd.push_back(tempC);
					}
					else//code for interior_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrItr.push_back(tempC);
					}
					tempC = startcode;
				}
			}
		}
	}
}
void QRBtree::GetCodesFromRect(const Rect& rect, vector<TCode>& ArrItr,vector<TCode>& ArrBrd,vector<TCode>& ArrCrn)
/*************************************************

Function:       GetCodesFromRect

Description:    Calculate linear codes of the corner, the border and the interior grids for the rectangle

Input:          rect,rectangle for exact query

				ArrItr, array codes of the interior grids(empty)

				ArrBrd, array codes of the border grids(empty)

				ArrCrn, array codes of the corner grids(empty)

Output:         ArrItr, array codes of the interior grids

				ArrBrd, array codes of the border grids

				ArrCrn, array codes of the corner grids

*************************************************/
{
	uint64_t startcode = 0, tempC = startcode;
	if (CFlag == 1)
	{
		for (int i = 0; i <= SL + L0; i++)
		{
			double xsize, ysize;
			int imin[2], imax[2];
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((rect.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((rect.min[1] + 90 - CoorExtent.min[1]) / ysize);
			imax[0] = int((rect.max[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imax[1] = int((rect.max[1] + 90 - CoorExtent.min[1]) / ysize);
			for (int ix = imin[0]; ix <= imax[0]; ix++)
			{
				for (int iy = imin[1]; iy <= imax[1]; iy++)
				{
					if (ix == imin[0] || iy == imin[1] || ix == imax[0] || iy == imax[1])
					{
						if (ix == imin[0] && iy == imin[1] ||
							ix == imax[0] && iy == imin[1] ||
							ix == imin[0] && iy == imax[1] ||
							ix == imax[0] && iy == imax[1])//code for corner_codes
						{
							for (int k = i; k > 0; k--)
							{
								tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
							}
							tempC = (tempC << 4) | (uint64_t)i;
							ArrCrn.push_back(tempC);
						}
						else//code for border_codes
						{
							for (int k = i; k > 0; k--)
							{
								tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
							}
							tempC = (tempC << 4) | (uint64_t)i;
							ArrBrd.push_back(tempC);
						}
					}
					else//code for interior_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrItr.push_back(tempC);
					}
					tempC = startcode;
				}
			}
		}
	}
	else
	{
		for (int i = SL; i <= SL + L0; i++)
		{
			double xsize, ysize;
			int imin[2], imax[2];
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((rect.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((rect.min[1] - CoorExtent.min[1]) / ysize);
			imax[0] = int((rect.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((rect.max[1] - CoorExtent.min[1]) / ysize);
			for (int ix = imin[0]; ix <= imax[0]; ix++)
			{
				for (int iy = imin[1]; iy <= imax[1]; iy++)
				{
					if (ix == imin[0] || iy == imin[1] || ix == imax[0] || iy == imax[1])
					{
						if (ix == imin[0] && iy == imin[1] ||
							ix == imax[0] && iy == imin[1] ||
							ix == imin[0] && iy == imax[1] ||
							ix == imax[0] && iy == imax[1])//code for corner_codes
						{
							for (int k = i; k > 0; k--)
							{
								tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
							}
							tempC = (tempC << 4) | (uint64_t)i;
							ArrCrn.push_back(tempC);
						}
						else//code for border_codes
						{
							for (int k = i; k > 0; k--)
							{
								tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
							}
							tempC = (tempC << 4) | (uint64_t)i;
							ArrBrd.push_back(tempC);
						}
					}
					else//code for interior_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrItr.push_back(tempC);
					}
					tempC = startcode;
				}
			}
		}
	}
}
void QRtree::GetCodesFromRect(const Rect& rect, vector<TCode>& ArrItr, vector<TCode>& ArrBrd)
{
	uint64_t startcode = 0, tempC = startcode;
	if (CFlag == 1)
	{
		for (int i = 0; i <= SL + L0; i++)
		{
			double xsize, ysize;
			int imin[2], imax[2];
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((rect.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((rect.min[1] + 90 - CoorExtent.min[1]) / ysize);
			imax[0] = int((rect.max[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imax[1] = int((rect.max[1] + 90 - CoorExtent.min[1]) / ysize);
			for (int ix = imin[0]; ix <= imax[0]; ix++)
			{
				for (int iy = imin[1]; iy <= imax[1]; iy++)
				{
					if (ix == imin[0] || iy == imin[1] || ix == imax[0] || iy == imax[1])//code for border_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrBrd.push_back(tempC);
					}
					else//code for interior_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrItr.push_back(tempC);
					}
					tempC = startcode;
				}
			}
		}
	}
	else
	{
		for (int i = SL; i <= SL + L0; i++)
		{
			double xsize, ysize;
			int imin[2], imax[2];
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((rect.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((rect.min[1] - CoorExtent.min[1]) / ysize);
			imax[0] = int((rect.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((rect.max[1] - CoorExtent.min[1]) / ysize);
			for (int ix = imin[0]; ix <= imax[0]; ix++)
			{
				for (int iy = imin[1]; iy <= imax[1]; iy++)
				{
					if (ix == imin[0] || iy == imin[1] || ix == imax[0] || iy == imax[1])//code for border_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrBrd.push_back(tempC);
					}
					else//code for interior_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrItr.push_back(tempC);
					}
					tempC = startcode;
				}
			}
		}
	}
}
void QRtree::GetCodesFromRect(const Rect& rect, vector<TCode>& ArrItr, vector<TCode>& ArrBrd, vector<TCode>& ArrCrn)
{
	uint64_t startcode = 0, tempC = startcode;
	if (CFlag == 1)
	{
		for (int i = 0; i <= SL + L0; i++)
		{
			double xsize, ysize;
			int imin[2], imax[2];
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((rect.min[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imin[1] = int((rect.min[1] + 90 - CoorExtent.min[1]) / ysize);
			imax[0] = int((rect.max[0] + (LongOffset * 10) - CoorExtent.min[0]) / xsize); imax[1] = int((rect.max[1] + 90 - CoorExtent.min[1]) / ysize);
			for (int ix = imin[0]; ix <= imax[0]; ix++)
			{
				for (int iy = imin[1]; iy <= imax[1]; iy++)
				{
					if (ix == imin[0] || iy == imin[1] || ix == imax[0] || iy == imax[1])
					{
						if (ix == imin[0] && iy == imin[1] ||
							ix == imax[0] && iy == imin[1] ||
							ix == imin[0] && iy == imax[1] ||
							ix == imax[0] && iy == imax[1])//code for corner_codes
						{
							for (int k = i; k > 0; k--)
							{
								tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
							}
							tempC = (tempC << 4) | (uint64_t)i;
							ArrCrn.push_back(tempC);
						}
						else//code for border_codes
						{
							for (int k = i; k > 0; k--)
							{
								tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
							}
							tempC = (tempC << 4) | (uint64_t)i;
							ArrBrd.push_back(tempC);
						}
					}
					else//code for interior_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrItr.push_back(tempC);
					}
					tempC = startcode;
				}
			}
		}
	}
	else
	{
		for (int i = SL; i <= SL + L0; i++)
		{
			double xsize, ysize;
			int imin[2], imax[2];
			xsize = (CoorExtent.max[0] - CoorExtent.min[0]) / ((long)1 << i); ysize = (CoorExtent.max[1] - CoorExtent.min[1]) / ((long)1 << i);
			imin[0] = int((rect.min[0] - CoorExtent.min[0]) / xsize); imin[1] = int((rect.min[1] - CoorExtent.min[1]) / ysize);
			imax[0] = int((rect.max[0] - CoorExtent.min[0]) / xsize); imax[1] = int((rect.max[1] - CoorExtent.min[1]) / ysize);
			for (int ix = imin[0]; ix <= imax[0]; ix++)
			{
				for (int iy = imin[1]; iy <= imax[1]; iy++)
				{
					if (ix == imin[0] || iy == imin[1] || ix == imax[0] || iy == imax[1])
					{
						if (ix == imin[0] && iy == imin[1] ||
							ix == imax[0] && iy == imin[1] ||
							ix == imin[0] && iy == imax[1] ||
							ix == imax[0] && iy == imax[1])//code for corner_codes
						{
							for (int k = i; k > 0; k--)
							{
								tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
							}
							tempC = (tempC << 4) | (uint64_t)i;
							ArrCrn.push_back(tempC);
						}
						else//code for border_codes
						{
							for (int k = i; k > 0; k--)
							{
								tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
							}
							tempC = (tempC << 4) | (uint64_t)i;
							ArrBrd.push_back(tempC);
						}
					}
					else//code for interior_codes
					{
						for (int k = i; k > 0; k--)
						{
							tempC |= ((ix >> i - k) & 1) << (2 * (i - k) + 1) | ((iy >> i - k) & 1) << (2 * (i - k));
						}
						tempC = (tempC << 4) | (uint64_t)i;
						ArrItr.push_back(tempC);
					}
					tempC = startcode;
				}
			}
		}
	}
}
void Geohash::GetGeohashcodesFromRect(const Rect& rect, vector<string>& ArrItr, vector<string>& ArrBrd)
{
	DecodedBBox DB1;
	double dlon, dlat;
	int numlong, numlat;
	for (int i = 1; i <= 4; i++)
	{
		string code1= encode(rect.min[1], rect.min[0] + (LongOffset * 10), i);
		DB1 = decode_bbox(code1);
		dlon = DB1.maxlon - DB1.minlon; dlat = DB1.maxlat - DB1.minlat;
		numlong = ((rect.max[0] + (LongOffset * 10) - DB1.minlon) / dlon);
		numlat = ((rect.max[1] - DB1.minlat) / dlat);
		for (int j = 0; j <= numlong; j++)
		{
			for (int k = 0; k <= numlat; k++)
			{
				if (j != 0 && j != numlong && k != 0 && k != numlat)
				{
					code1 = encode(rect.min[1] + dlat * k, rect.min[0] + (LongOffset * 10 ) + dlon * j, i);
					ArrItr.push_back(code1);
				}
				else
				{
					code1 = encode(rect.min[1] + dlat * k, rect.min[0] + (LongOffset * 10) + dlon * j, i);
					ArrBrd.push_back(code1);
				}
			}
		}
	}
}

vector<long long>* QRBtree::RoughsearchFromDisk(const Rect& rect)
/*************************************************

Function:       RoughsearchFromDisk

Description:    Rough Search

Input:          rect,rectangle for query

Output:         ftrs, features retrieved by the QRB-tree

*************************************************/
{
	vector<long long>* ftrs = new vector<long long>;
	vector<TCode> ArrItr;
	vector<TCode> ArrBrd;
	GetCodesFromRect(rect, ArrItr, ArrBrd);
	for (size_t i = 0; i < ArrBrd.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QRB/" + to_string(ArrBrd[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else 
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrItr.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QRB/" + to_string(ArrItr[i]);
		FILE* m_file = fopen((FinPath + "B").c_str(), "rb");
		if (!m_file)continue;
		fseek(m_file, 0, SEEK_END);
		long lSize = ftell(m_file);
		int num = lSize / sizeof(long long);
		rewind(m_file);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, m_file);
		fclose(m_file);
		m_file = NULL;
		for (size_t j = 0; j < num; j++)
		{
			ftrs->push_back(*((long long*)buffer + j));
		}
		free(buffer);
	}
	return ftrs;
}
vector<long long>* QRBtree::ExactsearchFromDisk_New(const Rect& rect)
/*************************************************

Function:       ExactsearchFromDisk_New

Description:    Exact Search by new refinement method;
                
				In the exact algorithm, the corners of 
				the rectangle are used to search the 
				associated R-trees of the corner 
				grids to differentiate corner and 
				non-corner features.An exact test 
				is performed for each corner feature. 
				Disjointed features are eliminated 
				from the candidate features.

Input:          rect,rectangle for exact query

Output:         ftrs, features retrieved by the QRB-tree

*************************************************/
{
	vector<long long>* ftrs = new vector<long long>;
	vector<TCode> ArrItr;
	vector<TCode> ArrBrd;
	vector<TCode> ArrCrn;
	GetCodesFromRect(rect, ArrItr, ArrBrd, ArrCrn);
	Rect cornerPt1, cornerPt2, cornerPt3, cornerPt4;
	cornerPt1.min[0] = rect.min[0];
	cornerPt1.min[1] = rect.min[1];
	cornerPt1.max[0] = rect.min[0];
	cornerPt1.max[1] = rect.min[1];
	cornerPt2.min[0] = rect.max[0];
	cornerPt2.min[1] = rect.min[1];
	cornerPt2.max[0] = rect.max[0];
	cornerPt2.max[1] = rect.min[1];
	cornerPt3.min[0] = rect.max[0];
	cornerPt3.min[1] = rect.max[1];
	cornerPt3.max[0] = rect.max[0];
	cornerPt3.max[1] = rect.max[1];
	cornerPt4.min[0] = rect.min[0];
	cornerPt4.min[1] = rect.max[1];
	cornerPt4.max[0] = rect.min[0];
	cornerPt4.max[1] = rect.max[1];
	for (size_t i = 0; i < ArrBrd.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QRB/" + to_string(ArrBrd[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrCrn.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QRB/" + to_string(ArrCrn[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					long long id;
					if (IsOverlap(rect, nrect))
					{
						id = *((long long*)curNodePtr);
						ptrset.insert(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
					if (IsOverlap(cornerPt1, nrect) || IsOverlap(cornerPt2, nrect) ||
						IsOverlap(cornerPt3, nrect) || IsOverlap(cornerPt4, nrect))
					{
						ptrbuf2.push_back(id);
					}

				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrItr.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QRB/" + to_string(ArrItr[i]);
		FILE* m_file = fopen((FinPath + "B").c_str(), "rb");
		if (!m_file)continue;
		fseek(m_file, 0, SEEK_END);
		long lSize = ftell(m_file);
		int num = lSize / sizeof(long long);
		rewind(m_file);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, m_file);
		fclose(m_file);
		m_file = NULL;
		for (size_t j = 0; j < num; j++)
		{
			ftrs->push_back(*((long long*)buffer + j));
		}
		free(buffer);
	}
	erase(rect);
	ftrs->insert(ftrs->end(), ptrset.begin(), ptrset.end());
	ptrbuf1.clear();
	ptrbuf2.clear();
	ptrset.clear();
	return ftrs;
}
vector<long long>* QRBtree::ExactsearchFromDisk_Tra(const Rect& rect)
/*************************************************

Function:       ExactsearchFromDisk_Tra

Description:    Exact Search by tranditional refinement method

Input:          rect,rectangle for exact query

Output:         ftrs, features retrieved by the QRB-tree

*************************************************/
{
	vector<long long>* ftrs = new vector<long long>;
	vector<TCode> ArrItr;
	vector<TCode> ArrBrd;
	vector<TCode> ArrCrn;
	GetCodesFromRect(rect, ArrItr, ArrBrd, ArrCrn);
	for (size_t i = 0; i < ArrBrd.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QRB/" + to_string(ArrBrd[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;

		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);

		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrCrn.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QRB/" + to_string(ArrCrn[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);

		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					long long id;
					if (IsOverlap(rect, nrect))
					{
						id = *((long long*)curNodePtr);
						ptrset.insert(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		curPtr = buffer + 7 * sizeint;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap1(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					long long id;
					if (IsOverlap1(rect, nrect))
					{
						id = *((long long*)curNodePtr);
						ptrbuf2.push_back(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrItr.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QRB/" + to_string(ArrItr[i]);
		FILE* m_file = fopen((FinPath + "B").c_str(), "rb");
		if (!m_file) continue;
		fseek(m_file, 0, SEEK_END);
		long lSize = ftell(m_file);
		int num = lSize / sizeof(long long);
		rewind(m_file);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, m_file);
		fclose(m_file);
		m_file = NULL;
		for (size_t j = 0; j < num; j++)
		{
			ftrs->push_back(*((long long*)buffer + j));
		}
		free(buffer);
	}
	erase(rect);
	ftrs->insert(ftrs->end(), ptrset.begin(), ptrset.end());
	ptrbuf1.clear();
	ptrbuf2.clear();
	ptrset.clear();
	return ftrs;
}

vector<long long>* QRtree::QRRoughsearchFromDisk(const Rect& rect)
{
	vector<long long>* ftrs = new vector<long long>;
	vector<TCode> ArrItr;
	vector<TCode> ArrBrd;
	GetCodesFromRect(rect, ArrItr, ArrBrd);
	for (size_t i = 0; i < ArrBrd.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QR/" + to_string(ArrBrd[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
						//callback(id);
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrItr.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QR/" + to_string(ArrItr[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
						//callback(id);
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	return ftrs;
}
vector<long long>* QRtree::QRExactsearchFromDisk_New(const Rect& rect)
{
	vector<long long>* ftrs = new vector<long long>;
	vector<TCode> ArrItr;
	vector<TCode> ArrBrd;
	vector<TCode> ArrCrn;
	GetCodesFromRect(rect, ArrItr, ArrBrd, ArrCrn);
	Rect cornerPt1, cornerPt2, cornerPt3, cornerPt4;
	cornerPt1.min[0] = rect.min[0];
	cornerPt1.min[1] = rect.min[1];
	cornerPt1.max[0] = rect.min[0];
	cornerPt1.max[1] = rect.min[1];
	cornerPt2.min[0] = rect.max[0];
	cornerPt2.min[1] = rect.min[1];
	cornerPt2.max[0] = rect.max[0];
	cornerPt2.max[1] = rect.min[1];
	cornerPt3.min[0] = rect.max[0];
	cornerPt3.min[1] = rect.max[1];
	cornerPt3.max[0] = rect.max[0];
	cornerPt3.max[1] = rect.max[1];
	cornerPt4.min[0] = rect.min[0];
	cornerPt4.min[1] = rect.max[1];
	cornerPt4.max[0] = rect.min[0];
	cornerPt4.max[1] = rect.max[1];
	for (size_t i = 0; i < ArrBrd.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QR/" + to_string(ArrBrd[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrCrn.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QR/" + to_string(ArrCrn[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					long long id;
					if (IsOverlap(rect, nrect))
					{
						id = *((long long*)curNodePtr);
						ptrset.insert(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
					if (IsOverlap(cornerPt1, nrect) || IsOverlap(cornerPt2, nrect) ||
						IsOverlap(cornerPt3, nrect) || IsOverlap(cornerPt4, nrect))
					{
						ptrbuf2.push_back(id);
					}

				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrItr.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QR/" + to_string(ArrItr[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
						//callback(id);
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	erase(rect);
	ftrs->insert(ftrs->end(), ptrset.begin(), ptrset.end());
	ptrbuf1.clear();
	ptrbuf2.clear();
	ptrset.clear();
	return ftrs;
}
vector<long long>* QRtree::QRExactsearchFromDisk_Tra(const Rect& rect)
{
	vector<long long>* ftrs = new vector<long long>;
	vector<TCode> ArrItr;
	vector<TCode> ArrBrd;
	vector<TCode> ArrCrn;
	GetCodesFromRect(rect, ArrItr, ArrBrd, ArrCrn);
	for (size_t i = 0; i < ArrBrd.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QR/" + to_string(ArrBrd[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrCrn.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QR/" + to_string(ArrCrn[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					long long id;
					if (IsOverlap(rect, nrect))
					{
						id = *((long long*)curNodePtr);
						ptrset.insert(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		curPtr = buffer + 7 * sizeint;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap1(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					long long id;
					if (IsOverlap1(rect, nrect))
					{
						id = *((long long*)curNodePtr);
						ptrbuf2.push_back(id);
						curNodePtr += 8;
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrItr.size(); i++)
	{
		string FinPath = SavePath + SaveName + "QR/" + to_string(ArrItr[i]);
		FILE* rfile = fopen((FinPath + "R").c_str(), "rb");
		if (!rfile) continue;
		fseek(rfile, 0, SEEK_END);
		long lSize = ftell(rfile);
		rewind(rfile);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, rfile);
		fclose(rfile);
		rfile = NULL;
		int sizeint = sizeof(int);//r int byte
		char* curPtr = buffer + 7 * sizeint;
		stack<char*> feed;
		feed.push(curPtr);
		while (!feed.empty())
		{
			char* curNodePtr = feed.top();
			char* beginNodePtr = feed.top();
			feed.pop();
			if (*(int*)curNodePtr > 0)// non-leaf node
			{
				int count = *((int*)curNodePtr + 1);
				long nByte = *((long*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}

					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						nByte = *((long*)curNodePtr + 2);
						feed.push(curNodePtr);
						curNodePtr = curNodePtr + nByte;
					}
					else
					{
						nByte = *((long*)curNodePtr + 2);
						curNodePtr = curNodePtr + nByte;
					}
				}
			}
			else
			{
				int count = *((int*)curNodePtr + 1);
				int nByte = *((int*)curNodePtr + 2);
				for (int index = 0; index < count; ++index)
				{
					Rect nrect;
					if (index == 0)
					{
						curNodePtr += sizeint * 3;
					}
					nrect.min[0] = *(double*)(curNodePtr);
					curNodePtr += 8;

					nrect.min[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[0] = *((double*)curNodePtr);
					curNodePtr += 8;

					nrect.max[1] = *((double*)curNodePtr);
					curNodePtr += 8;

					if (IsOverlap(rect, nrect))
					{
						long long id = *((long long*)curNodePtr);
						ftrs->push_back(id);
						curNodePtr += 8;
						//callback(id);
					}
					else
					{
						curNodePtr += 8;
					}
				}
			}
		}
		free(buffer);
	}
	erase(rect);
	ftrs->insert(ftrs->end(), ptrset.begin(), ptrset.end());
	ptrbuf1.clear();
	ptrbuf2.clear();
	ptrset.clear();
	return ftrs;
}

vector<long long>* Geohash::RoughsearchFromDisk_Geohash(const Rect& rect)
{
	vector<long long>* ftrs = new vector<long long>;
	vector<string> ArrItr;
	vector<string> ArrBrd;
	GetGeohashcodesFromRect(rect, ArrItr, ArrBrd);
	for (size_t i = 0; i < ArrBrd.size(); i++)
	{
		string FinPath = SavePath + SaveName + "Geohash/" + ArrBrd[i];
		FILE* m_file = fopen((FinPath + "B").c_str(), "rb");
		if (!m_file)continue;
		fseek(m_file, 0, SEEK_END);
		long lSize = ftell(m_file);
		int num = lSize / sizeof(MBR);
		rewind(m_file);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, m_file);
		fclose(m_file);
		m_file = NULL;
		for (size_t j = 0; j < num; j++)
		{
			MBR mbr = *((MBR*)buffer + j);
			if (mbr.envelope.min[0] > rect.max[0] || mbr.envelope.max[0] < rect.min[0] ||
				mbr.envelope.min[1] > rect.max[1] || mbr.envelope.max[1] < rect.min[1]) {
			}
			else
			{
				ftrs->push_back(mbr.ptr);
			}
		}
		free(buffer);
	}
	for (size_t i = 0; i < ArrItr.size(); i++)
	{
		string FinPath = SavePath + SaveName + "Geohash/" + ArrItr[i];
		FILE* m_file = fopen((FinPath + "B").c_str(), "rb");
		if (!m_file)continue;
		fseek(m_file, 0, SEEK_END);
		long lSize = ftell(m_file);
		int num = lSize / sizeof(MBR);
		rewind(m_file);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, m_file);
		fclose(m_file);
		m_file = NULL;
		for (size_t j = 0; j < num; j++)
		{
			MBR mbr = *((MBR*)buffer + j);
			ftrs->push_back(mbr.ptr);
		}
		free(buffer);
	}
	return ftrs;
}
vector<long long>* Geohash::ExactsearchFromDisk_Geohash(const Rect& rect)
{
	vector<long long>* ftrs = new vector<long long>;
	vector<string> ArrItr;
	vector<string> ArrBrd;
	GetGeohashcodesFromRect(rect, ArrItr, ArrBrd);
	for (size_t i = 0; i < ArrBrd.size(); i++)
	{
		string FinPath = SavePath + SaveName + "Geohash/" + ArrBrd[i];
		FILE* m_file = fopen((FinPath + "B").c_str(), "rb");
		if (!m_file)continue;
		fseek(m_file, 0, SEEK_END);
		long lSize = ftell(m_file);
		int num = lSize / sizeof(MBR);
		rewind(m_file);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, m_file);
		fclose(m_file);
		m_file = NULL;
		for (size_t j = 0; j < num; j++)
		{
			MBR mbr = *((MBR*)buffer + j);
			if (mbr.envelope.min[0] >= rect.min[0] && mbr.envelope.max[0] <= rect.max[0] &&
				mbr.envelope.min[1] >= rect.min[1] && mbr.envelope.max[1] <= rect.max[1])
			{
				ftrs->push_back(mbr.ptr);
			}
			else if (mbr.envelope.min[0] > rect.max[0] || mbr.envelope.max[0] < rect.min[0] ||
				mbr.envelope.min[1] > rect.max[1] || mbr.envelope.max[1] < rect.min[1]) {
			}
			else
			{
				ptrbuf2.push_back(mbr.ptr);
			}
		}
		free(buffer);
	}
	ptrset.insert(ptrbuf2.begin(), ptrbuf2.end());
	erase(rect);
	ftrs->insert(ftrs->end(), ptrset.begin(), ptrset.end());
	for (size_t i = 0; i < ArrItr.size(); i++)
	{
		string FinPath = SavePath + SaveName + "Geohash/" + ArrItr[i];
		FILE* m_file = fopen((FinPath + "B").c_str(), "rb");
		if (!m_file)continue;
		fseek(m_file, 0, SEEK_END);
		long lSize = ftell(m_file);
		int num = lSize / sizeof(MBR);
		rewind(m_file);
		char* buffer = (char*)malloc(sizeof(char) * lSize);
		size_t result = fread(buffer, 1, lSize, m_file);
		fclose(m_file);
		m_file = NULL;
		for (size_t j = 0; j < num; j++)
		{
			MBR mbr = *((MBR*)buffer + j);
			ftrs->push_back(mbr.ptr);
		}
		free(buffer);
	}
	ptrbuf2.clear();
	ptrset.clear();
	return ftrs;
}

void QRBtree::erase(const Rect& rect)
{
	if (ptrbuf2.size() != 0)
	{
		for (int i = 1; i <= FILEID.size(); i++) {
			string readadress;
			vector<long long> tmpbuf;
			vector<long long> FID;
			for (vector<long long>::iterator rit = ptrbuf2.begin(); rit != ptrbuf2.end(); rit++)
			{
				long long n = *rit >> fileposition;
				int a = pow(2, fileposition) - 1;
				int m = *rit & a;
				if (m == i)
				{
					tmpbuf.push_back(*rit);
					FID.push_back(n);
				}
				readadress.empty();
				map<int, string>::iterator idter = FILEID.find(i);
				readadress = idter->second;
			}
			if (FID.size() != 0)
			{
				exact(rect, readadress.c_str(), FID, tmpbuf);
			}
			FID.clear();
			tmpbuf.clear();
		}
	}
}
void QRBtree::exact(const Rect& rect, const char* data_adress, std::vector<long long> FID, std::vector<long long> tmpbuf)
{
	string openfilename = data_adress;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");
	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
	GDALDataset* poDS1 = (GDALDataset*)GDALOpenEx(openfilename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer1 = poDS1->GetLayer(0);
	OGRLinearRing* ring = new OGRLinearRing;
	OGRPolygon* poPolygon = new OGRPolygon;
	ring->addPoint(rect.min[0], rect.min[1]);
	ring->addPoint(rect.min[0], rect.max[1]);
	ring->addPoint(rect.max[0], rect.max[1]);
	ring->addPoint(rect.max[0], rect.min[1]);
	ring->closeRings();
	poPolygon->addRing(ring);
	clock_t starttime, endtime;
	for (int i = 0; i < FID.size(); i++) {
		OGRFeature* poFeature = poLayer1->GetFeature(FID[i]);
		OGRGeometry* poGeometry = poFeature->GetGeometryRef();
		if (poPolygon->Intersects(poGeometry))
		{
			//ptrbuf3.push_back(tmpbuf[i]);
		}
		else
		{
			ptrset.erase(tmpbuf[i]);
		}
		endtime = clock();
		OGRFeature::DestroyFeature(poFeature);
	}
	GDALClose(poDS1);
}
void QRtree::erase(const Rect& rect)
{
	if (ptrbuf2.size() != 0)
	{
		for (int i = 1; i <= FILEID.size(); i++) {
			string readadress;
			vector<long long> tmpbuf;
			vector<long long> FID;
			for (vector<long long>::iterator rit = ptrbuf2.begin(); rit != ptrbuf2.end(); rit++)
			{
				long long n = *rit >> fileposition;
				int a = pow(2, fileposition) - 1;
				int m = *rit & a;
				if (m == i)
				{
					tmpbuf.push_back(*rit);
					FID.push_back(n);
				}
				readadress.empty();
				map<int, string>::iterator idter = FILEID.find(i);
				readadress = idter->second;
			}
			if (FID.size() != 0)
			{
				exact(rect, readadress.c_str(), FID, tmpbuf);
			}
			FID.clear();
			tmpbuf.clear();
		}
	}
}
void QRtree::exact(const Rect& rect, const char* data_adress, std::vector<long long> FID, std::vector<long long> tmpbuf)
{
	string openfilename = data_adress;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");
	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
	GDALDataset* poDS1 = (GDALDataset*)GDALOpenEx(openfilename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer1 = poDS1->GetLayer(0);
	OGRLinearRing* ring = new OGRLinearRing;
	OGRPolygon* poPolygon = new OGRPolygon;
	ring->addPoint(rect.min[0], rect.min[1]);
	ring->addPoint(rect.min[0], rect.max[1]);
	ring->addPoint(rect.max[0], rect.max[1]);
	ring->addPoint(rect.max[0], rect.min[1]);
	ring->closeRings();
	poPolygon->addRing(ring);
	clock_t starttime, endtime;
	for (int i = 0; i < FID.size(); i++) {
		OGRFeature* poFeature = poLayer1->GetFeature(FID[i]);
		OGRGeometry* poGeometry = poFeature->GetGeometryRef();
		if (poPolygon->Intersects(poGeometry))
		{
			//ptrbuf3.push_back(tmpbuf[i]);
		}
		else
		{
			ptrset.erase(tmpbuf[i]);
		}
		endtime = clock();
		OGRFeature::DestroyFeature(poFeature);
	}
	GDALClose(poDS1);
}
void Geohash::erase(const Rect& rect)
{
	if (ptrbuf2.size() != 0)
	{
		for (int i = 1; i <= FILEID.size(); i++) {
			string readadress;
			vector<long long> tmpbuf;
			vector<long long> FID;
			for (vector<long long>::iterator rit = ptrbuf2.begin(); rit != ptrbuf2.end(); rit++)
			{
				long long n = *rit >> fileposition;
				int a = pow(2, fileposition) - 1;
				int m = *rit & a;
				if (m == i)
				{
					tmpbuf.push_back(*rit);
					FID.push_back(n);
				}
				readadress.empty();
				map<int, string>::iterator idter = FILEID.find(i);
				readadress = idter->second;
			}
			if (FID.size() != 0)
			{
				exact(rect, readadress.c_str(), FID, tmpbuf);
			}
			FID.clear();
			tmpbuf.clear();
		}
	}
}
void Geohash::exact(const Rect& rect, const char* data_adress, std::vector<long long> FID, std::vector<long long> tmpbuf)
{
	string openfilename = data_adress;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");
	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
	GDALDataset* poDS1 = (GDALDataset*)GDALOpenEx(openfilename.c_str(), GDAL_OF_READONLY, NULL, NULL, NULL);
	OGRLayer* poLayer1 = poDS1->GetLayer(0);
	OGRLinearRing* ring = new OGRLinearRing;
	OGRPolygon* poPolygon = new OGRPolygon;
	ring->addPoint(rect.min[0], rect.min[1]);
	ring->addPoint(rect.min[0], rect.max[1]);
	ring->addPoint(rect.max[0], rect.max[1]);
	ring->addPoint(rect.max[0], rect.min[1]);
	ring->closeRings();
	poPolygon->addRing(ring);
	clock_t starttime, endtime;
	for (int i = 0; i < FID.size(); i++) {
		OGRFeature* poFeature = poLayer1->GetFeature(FID[i]);
		OGRGeometry* poGeometry = poFeature->GetGeometryRef();
		if (poPolygon->Intersects(poGeometry))
		{
			//ptrbuf3.push_back(tmpbuf[i]);
		}
		else
		{
			ptrset.erase(tmpbuf[i]);
		}
		endtime = clock();
		OGRFeature::DestroyFeature(poFeature);
	}
	GDALClose(poDS1);
}