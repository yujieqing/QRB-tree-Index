#include"Structure.h"
#include<fstream>
#include<string>
#include<iostream>
#include<ctime>
#include<direct.h>
using namespace std;
int main()
{
	char prDir[_MAX_PATH];
	_getcwd(prDir, _MAX_PATH);
	string prCmd("PROJ_LIB=");
	string prPath = prDir;
	prPath.append("/bin");
	prCmd.append(prPath.c_str());
	const char* proj_lib = getenv("PROJ_LIB");
	char new_proj_lib[256];
	new_proj_lib[0] = 0;
	sprintf(new_proj_lib, prCmd.c_str(), proj_lib);
	if (new_proj_lib[0])
		_putenv(new_proj_lib);
	int L0 = 6;//set the level of QRB
	string dir = prDir;
	dir.append("/data/");
	string indexfolder = "IndexUK_G" + to_string(L0);
	QRBtree myQRBtree(dir.c_str(), indexfolder.c_str(), L0);//set the dir of dataset
	myQRBtree.BuiltForDataset(1);

	L0 = 6;//set the level of QR
	indexfolder = "IndexUK_G" + to_string(L0);
	QRtree myQRtree(dir.c_str(), indexfolder.c_str(), L0);
	myQRtree.BuiltForDataset(1);

	L0 = 4;//set the level of Geohash
	indexfolder = "IndexUK" + to_string(L0);
	Geohash myGeohash(dir.c_str(), indexfolder.c_str(), L0);
	myGeohash.BuiltForDataset();

	Rect srect[5];//test rects
	{
	//RECT ID 1
	srect[0].min[0] = -2.012; srect[0].max[0] = -1.734;
	srect[0].min[1] = 52.376; srect[0].max[1] = 52.608;
	//RECT ID 2
	srect[1].min[0] = -3.024; srect[1].max[0] = -2.085;
	srect[1].min[1] = 53.325; srect[1].max[1] = 53.481;
	//RECT ID 3
	srect[2].min[0] = -0.523; srect[2].max[0] = 0.342;
	srect[2].min[1] = 51.281; srect[2].max[1] = 51.691;
	//RECT ID 4
	srect[3].min[0] = -5.484; srect[3].max[0] = -2.648;
	srect[3].min[1] = 51.339; srect[3].max[1] = 53.426;
	//RECT ID 5
	srect[4].min[0] = -8.653; srect[4].max[0] = -0.706;
	srect[4].min[1] = 54.628; srect[4].max[1] = 60.867;
	}
	float qrbtim = 0, qrtim = 0, geohashtim = 0;
	clock_t starttime, endtime;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			starttime = clock();
			endtime = clock();
			if (j != 0)
			{
				starttime = clock();
			}
			vector<long long>* output = myQRBtree.ExactsearchFromDisk_New(srect[i]);//exact query of QRB
			output->clear();
			delete output;
			if (j != 0)
			{
				endtime = clock();
			}
			qrbtim += endtime - starttime;
		}
		for (int j = 0; j < 6; j++)
		{
			starttime = clock();
			endtime = clock();
			if (j != 0)
			{
				starttime = clock();
			}
			vector<long long>* output = myQRtree.QRExactsearchFromDisk_New(srect[i]);//exact query of QR
			output->clear();
			delete output;
			if (j != 0)
			{
				endtime = clock();
			}
			qrtim += endtime - starttime;
		}
		for (int j = 0; j < 6; j++)
		{
			starttime = clock();
			endtime = clock();
			if (j != 0)
			{
				starttime = clock();
			}
			vector<long long>* output = myGeohash.ExactsearchFromDisk_Geohash(srect[i]);//exact query of Geohash
			output->clear();
			delete output;
			if (j != 0)
			{
				endtime = clock();
			}
			geohashtim += endtime - starttime;
		}	
		cout << qrbtim / 5.0 << " " << qrtim / 5.0 << " " << geohashtim / 5.0 << endl;
		qrbtim = 0; qrtim = 0; geohashtim = 0;
	}
	system("pause");
}