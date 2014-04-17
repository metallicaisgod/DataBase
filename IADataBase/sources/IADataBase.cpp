#include "stdafx.h"
#pragma warning( disable : 4267 ) 

#include "IADataBase.h"
#include "ext_resources.h"
#include "ticpp.h"
#include "ParserHelper.h"

#include <iostream>

//char szProvArr[3][30]=
//{
//	"Astra-tech",
//	"Conmet",
//	"CDI"
//};

#include "common.h"

namespace db
{
template<class T>
class find_ptr : public std::unary_function<T*, bool>
{
	const T& m_value;
public:
	find_ptr(const T& val) : m_value(val){};
	find_ptr(const find_ptr& src) : m_value(src.m_value){}

	bool operator()(const T* ptr)
	{
		return *ptr == m_value;
	}
};


//////////////////////////////////////////////////////////////////////////
// IADataBase members
IADataBase::IADataBase()
{
}

IADataBase::~IADataBase()
{
	std::for_each(m_Providers.begin(), m_Providers.end(), delete_ptr());
	m_Providers.clear();
}

DbProvider& IADataBase::AddProvider(const char* providerName)
{
	DbProvider* pProv = new DbProvider(providerName);
	t_ProvidersList::iterator it = std::find_if(m_Providers.begin(), m_Providers.end(), find_ptr<DbProvider>(*pProv));
	if (it == m_Providers.end())
	{
		m_Providers.push_back(pProv);
		return *pProv;
	}
	delete pProv;
	return *(*it);
}

void IADataBase::RemoveProvider(const DbProvider& data)
{
	find_ptr<DbProvider> pred(data);
	t_ProvidersList::iterator it = std::find_if(m_Providers.begin(), m_Providers.end(), pred);
	if (it != m_Providers.end())
	{
		delete *it;
		m_Providers.erase(it);
	}
}

bool IADataBase::LoadXml(const char* fileName)
{
	return IADataBase::LoadXml(*this, fileName);
}

bool IADataBase::LoadXml(IADataBase& indb, const char* fileName)
{
	return LoadXml_All(indb, fileName);
	//bool loaded = false;
	//try
	//{
	//	ticpp::Document doc(fileName);
	//	doc.LoadFile(TIXML_ENCODING_UTF8);
	//	ticpp::Element* root = doc.FirstChildElement();
	//	std::string v = root->Value();
	//	if (v == "boost_serialization")
	//		ParserHelper::ParseOldDataBase(indb, root);
	//	else
	//		ParserHelper::ParseDataBase(indb, root);
	//	loaded = true;

	//}
	//catch (ticpp::Exception& e) 
	//{
	//	std::cout << "error:" << e.m_details << std::endl;
	//}
	//return loaded;
};

bool IADataBase::LoadXml_2( IADataBase& indb, const char* fileName, unsigned long flags)
{
	bool loaded = false;
	ticpp::Document* pDoc = NULL;
	try
	{
		//pDoc = new ticpp::Document(fileName);
		//pDoc->LoadFile(TIXML_ENCODING_UTF8);
		//ticpp::Element* root = pDoc->FirstChildElement();
		//ParserHelper::ParseDataBase(indb, root, flags);
		//loaded = true;

		pDoc = new ticpp::Document(fileName);
		pDoc->LoadFile(TIXML_ENCODING_UTF8);
		ticpp::Element* root = pDoc->FirstChildElement();
		std::string v = root->Value();
		if (v == "boost_serialization")
			ParserHelper::ParseOldDataBase(indb, root);
		else
			ParserHelper::ParseDataBase(indb, fileName, root, flags);

		loaded = true;


	}
	catch (ticpp::Exception& e) 
	{
		std::cout << "error:" << e.m_details << std::endl;
	}
	if (pDoc != NULL)
		delete pDoc;
	return loaded;
}

bool IADataBase::LoadXml_2( const char* fileName, unsigned long flags)
{
	return IADataBase::LoadXml_2(*this, fileName, flags);
}

void IADataBase::SaveXml(const char* fileName)
{
	IADataBase::SaveXml(*this, fileName);
}

void IADataBase::SaveXml(const IADataBase& indb, const char* fileName)
{
	SaveXml_All(indb, fileName);
};

void IADataBase::SaveXml_2(const char* fileName, unsigned long flags)
{
	return IADataBase::SaveXml_2(*this, fileName, flags);
};
void IADataBase::SaveXml_2(const IADataBase& indb, const char* fileName, unsigned long flags)
{
	TiXmlDocument* pDoc = NULL;
	try
	{
		pDoc = new TiXmlDocument();
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
		pDoc->LinkEndChild( decl );
		pDoc->LinkEndChild( ParserHelper::ToXml(indb, fileName, flags) );
		pDoc->SaveFile(fileName);
	}
	catch (ticpp::Exception& e) 
	{
	}
	if (pDoc != NULL)
		delete pDoc;
};

bool IADataBase::LoadXml_All(IADataBase& indb, const char* fileName)
{
	std::string strFileN(fileName);
	std::string strExt = strFileN.substr(strFileN.length()-1, 1);
	if(0==strExt.compare("\\"))
		return LoadFromDir(indb, strFileN.c_str());

	bool bLoadLib = LoadXml_2(indb, fileName, 0);
	strFileN+=".usr";
	bool bLoadUsr = LoadXml_2(indb, strFileN.c_str(), USERS_INTEMS);
	return bLoadLib || bLoadUsr;
	
};
bool IADataBase::LoadXml_All(const char* fileName)
{
	return IADataBase::LoadXml_All(*this, fileName);
};
	
bool IADataBase::LoadFromDir(IADataBase& indb, const char* dirName)
{
    WIN32_FIND_DATAA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	
	std::string strDir(dirName);
	strDir+="*.iadbxml";
	bool bLoadLib = false;

    hFind = FindFirstFileA(strDir.c_str(), &ffd);
	if (INVALID_HANDLE_VALUE == hFind) 
	{
		// error loading DB files
		return false;
	} 

	// List all the files in the directory with some info about them.
	do
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			unsigned long load_flag=0;
			std::string currFile(ffd.cFileName);
			//if(0==currFile.compare("user-defined.iadbxml"))
			if(currFile == "user-defined.iadbxml")
				load_flag = USERS_INTEMS;

			currFile = dirName;
			currFile+=ffd.cFileName;
			bLoadLib |= LoadXml_2(indb, currFile.c_str(), load_flag);
		}
	}
    while (FindNextFileA(hFind, &ffd) != 0);

	FindClose(hFind);

	return true;
}
void IADataBase::SaveXml_All(const IADataBase& indb, const char* fileName)
{
	SaveXml_2(indb, fileName, LIBRARY_INTEMS);
	std::string strFileN(fileName);
	strFileN+=".usr";
	SaveXml_2(indb, strFileN.c_str(), USERS_INTEMS);
	
};
void IADataBase::SaveXml_All(const char* fileName)
{
	return IADataBase::SaveXml_All(*this, fileName);
};


IADataBase& IADataBase::operator+=(const IADataBase& extraDB)
{
	//???
	IADataBase* pDb = const_cast<IADataBase*>(&extraDB);
	enumerator<t_ProvidersList::iterator> providersEnum = pDb->GetProvidersEnumerator(no_filter<DbProvider*>());
	while (providersEnum.MoveNext())
	{
		DbProvider* pProvider = providersEnum.GetCurrent();
		//bool prov_ok=true;
		//int num = 0;//sizeof(szProvArr)/sizeof(szProvArr[0]);
		//for(int i=0; i< num; i++)
		//{
		//	if(0==strcmp(szProvArr[i], pProvider->name))
		//	{
		//		prov_ok=false;
		//		break;
		//	}
		//}
		//if(prov_ok)
		//{
		DbProvider& newProvider = AddProvider(pProvider->name);
		newProvider += *pProvider;
		//}
	}
	return *this;
}

void IADataBase::BuildCompatibility()
{
	return;
	for(t_ProvidersList::iterator ip = m_Providers.begin();ip != m_Providers.end();ip++)
	{
		DbProvider *prov = *ip;
		for(t_SeriesList::iterator is = prov->GetSeriesList().begin();is != prov->GetSeriesList().end();is++)
		{
			DbSeries *ser = *is;
			for(t_ImplantList::const_iterator ii = ser->GetImplants().begin();ii != ser->GetImplants().end();ii++)
			{
				db::DbImplant* pImplant = *ii;
				FillCompatibility(pImplant->szCompatibility, ser);

			}
			for(t_AbutmentList::const_iterator ia = ser->GetAbutment().begin(); ia != ser->GetAbutment().end();ia++)
			{
				db::DbAbutment* pAbutment = *ia;
				FillCompatibility(pAbutment->szCompatibility, ser);

			}

	
		}

	}
	
}
bool IADataBase::ExportToCSV(char* file_name)
{
	// open manufacturers list file
	FILE *f_list = fopen(file_name, "wt");
	if(!f_list)
		return false;

	for(t_ProvidersList::iterator ip = m_Providers.begin();ip != m_Providers.end();ip++)
	{
		// get provider
		DbProvider *prov = *ip;

		// write provider to the list file
		std::string strProv = prov->name;
		strProv+=".csv";
		fprintf(f_list, "%s\n", strProv.c_str());
		

		// open provider CSV file
		std::string str=file_name;
		int slpos =	str.rfind('\\');
		str = str.substr(slpos+1);
		str+=prov->name;
		str+=".csv";
		FILE *f = fopen(str.c_str(), "wt");
	
		for(t_SeriesList::iterator is = prov->GetSeriesList().begin();is != prov->GetSeriesList().end();is++)
		{
			// Save implant to CSV
			DbSeries *ser = *is;
			if(ser->GetImplants().size())
			{
				// write header
				fprintf(f, ";;;;;;;;;;\n");
				fprintf(f, "ObjectType;Manufacturer;Series;Name;Articul;AbComp;D1;D2;L1;L2;Le;ModelFileName\n");
			}
			for(t_ImplantList::const_iterator ii = ser->GetImplants().begin();ii != ser->GetImplants().end();ii++)
			{
				db::DbImplant* pImplant = *ii;
				//FillCompatibility(pImplant->szCompatibility, ser);
				fprintf(f, "Impl;%s;%s;%s;%s;%s;%.3f;%.3f;%.3f;%.3f;%.3f;%s\n",
					prov->name, ser->name, pImplant->name, pImplant->artikul, pImplant->szCompatibility, 
					pImplant->D1,pImplant->D2, pImplant->L1,pImplant->L2,  
					pImplant->Le, pImplant->szModelName);	
			}
			

			// Save abutment to CSV
			if(ser->GetAbutment().size()>0)
			{
				// write header
				fprintf(f, ";;;;;;;;;;\n");
				fprintf(f, "ObjectType;Manufacturer;Series;Name;Articul;AbComp;D1;L1;Alpha; ; ;ModelFileName\n");
			}
			for(t_AbutmentList::const_iterator ia = ser->GetAbutment().begin(); ia != ser->GetAbutment().end();ia++)
			{
				db::DbAbutment* pAbutment = *ia;
				//FillCompatibility(pImplant->szCompatibility, ser);
				fprintf(f, "Abut;%s;%s;%s;%s;%s;%.3f;%.3f;%.3f; ; ;%s\n",
					prov->name, ser->name, pAbutment->name, pAbutment->artikul, pAbutment->szCompatibility, 
					pAbutment->D1, pAbutment->L1, pAbutment->Alpha,pAbutment->szModelName);	
			}
			
			// Save COmpatible series to CSV
			if(ser->m_CompSer.size()>0)
			{
				// write header
				fprintf(f, ";;;;;;;;;;\n");
				fprintf(f, "ObjectType;Manufacturer1;Series1;Manufacturer2;Series2\n");
			}
			for(std::vector<CompatibleSeries>::iterator ics = ser->m_CompSer.begin(); ics != ser->m_CompSer.end();ics++)
			{
				CompatibleSeries ser_com = *ics;
				//FillCompatibility(pImplant->szCompatibility, ser);
				fprintf(f, "CompSer;%s;%s;%s;%s\n",
					prov->name, ser->name, ser_com.prov, ser_com.ser);	
			}
		}
		fclose(f);
	}
	// close manufacturers list file
	fclose (f_list);

	return true;
};

bool IADataBase::ImportFromCSV(char* file_name)
{
	bool bList=true;
	FILE *f_lst = NULL;

	// get CSV files path
	std::string strPath(file_name), strFileName(file_name);
	int pos = strPath.rfind('\\');
	strPath = strPath.substr(pos+1);
	
	// check if input file is csv
	std::transform(strFileName.begin(), strFileName.end(), strFileName.begin(), ::tolower);
	if(strFileName.rfind(".csv")==(strFileName.length()-4))
		bList=false;

	// open manufacturers list file
	if(bList)
		f_lst =fopen(file_name, "rt");
	
	do
	{
		// read manufacturer files from list
		std::string strPath1 = strPath;
		char szFirmFile[1000]="", szDum[255]="";
		if(bList)
		{
			fscanf(f_lst, "%[^\n]s", szFirmFile);
			fscanf(f_lst, "%[\n]s", szDum);
			strPath1+=szFirmFile;
		}
		else
			strPath1 = file_name;

		
		// open manufacturer CSV file 
		FILE *f = fopen(strPath1.c_str(), "rt");
		if(f)
		{
			// index data
			int prov_idx=1,ser_idx=2,name_idx=3,artikul_idx=4, szCompatibility_idx=5,
			szModelName_idx=11, D1_idx=6,D2_idx=7,L1_idx=8,L2_idx=9,Le_idx=10, Alpha_idx=8,
			prov_cs_idx=3, ser_cs_idx=4;

			// additional data for model
			int szModelName_path_idx=-1, szModelName_ext_idx=-1;

			do
			{
				// Read data line by line
				char szStr[1000]="", szDummy[255]="";
				fscanf(f, "%[^\n]s", szStr);
				fscanf(f, "%[\n]s", szDummy);

				std::string strF(szStr);
				while (true)
				{

					int tz2pos = strF.find(";;");
					if(tz2pos==-1)
						break;
					strF.insert(tz2pos+1, " ");
					//if(-1==strF.Replace(";;", "; ;"))
					//	break;
				};
				const int items_num = 30;
				std::string str_items[items_num];
				int curPos= 0;
				int posLeft=0, posRight=0;
				for(int i=0; i<items_num; i++)
				{
					posLeft = strF.find_first_of(";",posLeft+1);
					if(posLeft==-1)
						break;
					posRight = strF.find_first_of(";",posLeft+1);
					if(posRight==-1)
						posRight=strF.length();
					str_items[i]=strF.substr(posLeft, posRight-posLeft);

					if(str_items[i]==" ")
						str_items[i]="";

					//str_items[i]= strF.Tokenize(";",curPos);
					//if(curPos==-1)
					//	break;
					//if(str_items[i]==" ")
					//	str_items[i]="";
				}
				int nnPos=0;
				// read and set index data
				if(0==str_items[0].compare("ObjectType")||0==str_items[0].compare("ObjType")||
					0==str_items[0].compare("Object Type")||0==str_items[0].compare("Obj Type"))
				{
					for(int i=0; i<items_num; i++)
					{
						//str_items[i].Replace(" ", "");
						nnPos=0;
						while(nnPos!=-1)
						{
							nnPos=str_items[i].find(" ",nnPos);
							if(nnPos!=-1)
								str_items[i].erase(nnPos);
						}
						
						// Manufacturer
						if(str_items[i].compare("Manufacturer")==0 || str_items[i].compare("Manuf")==0||
							str_items[i].compare("Manufacturer1")==0 || str_items[i].compare("Manuf1")==0)
						{
							prov_idx=i;
						}
						// Series
						if( str_items[i].compare("Series")==0 || str_items[i].compare("Ser")==0 ||
							str_items[i].compare("Series1")==0 || str_items[i].compare("Ser1")==0)
						{
							ser_idx=i;
						}
						// Manufacturer2
						if(str_items[i].compare("Manufacturer2")==0 || str_items[i].compare("Manuf2")==0)
							prov_cs_idx=i;
						// Series2
						if(str_items[i].compare("Series2")==0 || str_items[i].compare("Ser2")==0)
							ser_cs_idx=i;

						// Name
						if(str_items[i].compare("Name")==0)
							name_idx=i;
						// Articul
						if(str_items[i].compare("Articul")==0)
							artikul_idx=i;
						// Compatibility
						if(str_items[i].compare("AbComp")==0)
							szCompatibility_idx=i;
						// szModelName
						if(str_items[i].compare("ModelFileName")==0 || str_items[i].compare("szModelName")==0)
							szModelName_idx=i;
						// D1
						if(str_items[i].compare("D1")==0 || str_items[i].compare("Da")==0)
							D1_idx=i;
						// D2
						if(str_items[i].compare("D2")==0)
							D2_idx=i;
						// L1
						if(str_items[i].compare("L1")==0|| str_items[i].compare("La")==0)
							L1_idx=i;
						// L2
						if(str_items[i].compare("L2")==0)
							L2_idx=i;
						// Le
						if(str_items[i].compare("Le")==0)
							Le_idx=i;
						// Alpha
						if(str_items[i].compare("Alpha")==0||str_items[i].compare("Alfa")==0)
							Alpha_idx=i;

						// Path 
						if(str_items[i].compare("Path")==0||str_items[i].compare("ModelPath")==0||
							str_items[i].compare("ModelsPath")==0)
							szModelName_path_idx=i;
						// Extension 
						if(str_items[i].compare("ext")==0||str_items[i].compare("ModelFileNameExt")==0)
							szModelName_ext_idx=i;
										
					} 
				}

				// read and add implant data
				if(0==str_items[0].compare("Impl"))
				{
					// add series abd provider 
					db::DbProvider &prov = AddProvider((str_items[prov_idx].c_str()));
					db::DbSeries &series = prov.AddSeries((str_items[ser_idx].c_str()));

					// get implant data
					db::ImplantData idata;

					strcpy_s(idata.name, NAME_SIZE, (str_items[name_idx].c_str()));
					strcpy_s(idata.artikul, ARTIKUL_SIZE, (str_items[artikul_idx].c_str()));
					strcpy_s(idata.szCompatibility, _MAX_PATH, (str_items[szCompatibility_idx].c_str()));

					if(szModelName_path_idx!=-1)
						strcpy_s(idata.szModelName, _MAX_PATH, (str_items[szModelName_path_idx].c_str()));
					else
						strcpy_s(idata.szModelName, _MAX_PATH, "");
					strcat_s(idata.szModelName, _MAX_PATH, (str_items[szModelName_idx].c_str()));
					if(szModelName_ext_idx!=-1)
						strcat_s(idata.szModelName, _MAX_PATH, (str_items[szModelName_ext_idx].c_str()));


					// replace "," by "." to make atof working
					str_items[D1_idx].replace(str_items[D1_idx].find(",", 0), 1, ".");
					str_items[D2_idx].replace(str_items[D2_idx].find(",", 0), 1, ".");
					str_items[L1_idx].replace(str_items[L1_idx].find(",", 0), 1, ".");
					str_items[L2_idx].replace(str_items[L2_idx].find(",", 0), 1, ".");
					str_items[Le_idx].replace(str_items[Le_idx].find(",", 0), 1, ".");

					
					idata.D1=atof((str_items[D1_idx].c_str()));
					idata.D2=atof((str_items[D2_idx].c_str()));
					idata.L1=atof((str_items[L1_idx].c_str()));
					idata.L2=atof((str_items[L2_idx].c_str()));
					idata.Le=atof((str_items[Le_idx].c_str()));

					idata.state = db::ObjState::Active;
					idata.editable=false;
					
					// add implant 
					series.AddImplant(idata);
				}
			
				// read and add Abutment data
				if(0==str_items[0].compare("Abut"))
				{
					// add series abd provider 
					db::DbProvider &prov = AddProvider((str_items[prov_idx].c_str()));
					db::DbSeries &series = prov.AddSeries((str_items[ser_idx].c_str()));

					// get abutment data
					db::AbutmentData adata;
					if(0==str_items[4].compare("Conmet_Tolstyak"))
					{
						str_items[4]==str_items[4];
					}
					strcpy_s(adata.name, NAME_SIZE, (str_items[name_idx].c_str()));
					strcpy_s(adata.artikul, ARTIKUL_SIZE, (str_items[artikul_idx].c_str()));
					strcpy_s(adata.szCompatibility, _MAX_PATH, (str_items[szCompatibility_idx].c_str()));
					//strcpy_s(adata.szModelName, _MAX_PATH, LPCTSTR(str_items[szModelName_idx]));

					if(szModelName_path_idx!=-1)
						strcpy_s(adata.szModelName, _MAX_PATH, (str_items[szModelName_path_idx].c_str()));
					else
						strcpy_s(adata.szModelName, _MAX_PATH, "");
					strcat_s(adata.szModelName, _MAX_PATH, (str_items[szModelName_idx].c_str()));
					if(szModelName_ext_idx!=-1)
						strcat_s(adata.szModelName, _MAX_PATH, (str_items[szModelName_ext_idx].c_str()));



					// replace "," by "." to make atof working
					str_items[D1_idx].replace(str_items[D1_idx].find(",", 0), 1, ".");
					str_items[L1_idx].replace(str_items[L1_idx].find(",", 0), 1, ".");
					str_items[Alpha_idx].replace(str_items[Alpha_idx].find(",", 0), 1, ".");
					

					adata.D1=atof((str_items[D1_idx].c_str()));
					adata.L1=atof((str_items[L1_idx].c_str()));
					adata.Alpha=atof((str_items[Alpha_idx].c_str()));
					
					adata.state = db::ObjState::Active;
					adata.editable=false;

					// add implant 
					series.AddAbutment(adata);
				}

				// read and add compatible series
				if(0==str_items[0].compare("CompSer"))
				{
					// add series abd provider 
					db::DbProvider &prov = AddProvider((str_items[prov_idx].c_str()));
					db::DbSeries &series = prov.AddSeries((str_items[ser_idx].c_str()));

					// get abutment data
					db::CompatibleSeries csr;
					//if(0==str_items[4].compare("Conmet_Tolstyak"))
					//{
					//	str_items[4]==str_items[4];
					//}
					strcpy_s(csr.prov, NAME_SIZE, (str_items[prov_cs_idx].c_str()));
					strcpy_s(csr.ser, NAME_SIZE, (str_items[ser_cs_idx].c_str()));

					// add Compatible Series
					series.m_CompSer.push_back(csr);
				}


			} while(!feof(f));
			fclose(f);
		}

		// load only one CSV if it is not list file
		if(!bList)
			break;
		

	} while(!feof(f_lst));
	
	if(bList)
		fclose(f_lst);

	return true;
};

//////////////////////////////////////////////////////////////////////////
// Provider Methods
DbProvider::~DbProvider()
{
	std::for_each(m_Series.begin(), m_Series.end(), delete_ptr());
}

DbSeries& DbProvider::AddSeries(const char* szSeriesName)
{
	DbSeries* pSeries = new DbSeries(this, szSeriesName);
	t_SeriesList::iterator it = std::find_if(m_Series.begin(), m_Series.end(), find_ptr<DbSeries>(*pSeries));
	if (it == m_Series.end())
	{
		m_Series.push_back(pSeries);
		return *pSeries;
	}
	delete pSeries;
	return *(*it);
}

void DbProvider::RemoveSeries(const DbSeries& data)
{
	t_SeriesList::iterator it = std::find_if(m_Series.begin(), m_Series.end(), find_ptr<DbSeries>(data));
	if (it != m_Series.end())
	{
		delete *it;
		m_Series.erase(it);
	}
}
DbProvider& DbProvider::operator+=(const DbProvider& other)
{
	//???
	DbProvider* pOther= const_cast<DbProvider*>(&other);
	enumerator<t_SeriesList::iterator> seriesEnum = pOther->GetSeriesEnumerator(no_filter<DbSeries*>());
	while (seriesEnum.MoveNext())
	{
		DbSeries* pSeries = seriesEnum.GetCurrent();
		DbSeries& newSeries = AddSeries(pSeries->name);
		newSeries += *pSeries;
	}
	return *this;
}

DbSeries::~DbSeries()
{
	std::for_each(m_Implants.begin(), m_Implants.end(), delete_ptr());
	std::for_each(m_Abutments.begin(), m_Abutments.end(), delete_ptr());
}

DbImplant& DbSeries::AddImplant(const ImplantData& data)
{
	DbImplant* pImplant = new DbImplant(this);
	m_Implants.push_back(pImplant);
	(*pImplant) = data;
	return *pImplant;
}
DbAbutment& DbSeries::AddAbutment(const AbutmentData& data)
{
	DbAbutment* pAbutment = new DbAbutment(this);
	m_Abutments.push_back(pAbutment);
	(*pAbutment) = data;
	return *pAbutment;
}

void DbSeries::RemoveImplant(const DbImplant& data)
{
	t_ImplantList::iterator it = std::find_if(m_Implants.begin(), m_Implants.end(), find_ptr<DbImplant>(data));
	if (it != m_Implants.end())
	{
		delete *it;
		m_Implants.erase(it);
	}
}

DbSeries& DbSeries::operator+=(const DbSeries& other)
{
	const t_ImplantList& newList = other.GetImplants();
	t_ImplantList::const_iterator it = newList.begin();
	for (; it != newList.end(); ++it)
	{
		t_ImplantList::iterator fnd = std::find_if(
			m_Implants.begin(), 
			m_Implants.end(),
			equal_ptr_obj<DbImplant>(*it));
		if (fnd == m_Implants.end())
			AddImplant(*(*it));
		else
			(*fnd)->operator=(*(*it));
	}

	const t_AbutmentList& newAb = other.GetAbutment();
	t_AbutmentList::const_iterator itA = newAb.begin();
	for (; itA != newAb.end(); ++itA)
	{
		t_AbutmentList::iterator fnd = std::find_if(
			m_Abutments.begin(), 
			m_Abutments.end(),
			equal_ptr_obj<DbAbutment>(*itA));
		if (fnd == m_Abutments.end())
			AddAbutment(*(*itA));
		else
			(*fnd)->operator=(*(*itA));
	}
	return *this;

}

void DbSeries::RemoveImplant(unsigned long index)
{
	if (index < 0 || index >= m_Implants.size())
		return;
	t_ImplantList::iterator it = m_Implants.begin() + index;
	delete *it;
	m_Implants.erase(it);
}

void DbSeries::RemoveAbutment(const DbAbutment& data)
{
	t_AbutmentList::iterator it = std::find_if(m_Abutments.begin(), m_Abutments.end(), find_ptr<DbAbutment>(data));
	if (it != m_Abutments.end())
	{
		delete *it;
		m_Abutments.erase(it);
	}
}

void DbSeries::RemoveAbutment(unsigned long index)
{
	if (index < 0 || index >= m_Abutments.size())
		return;
	t_AbutmentList::iterator it = m_Abutments.begin() + index;
	delete *it;
	m_Abutments.erase(it);
}

COLORREF __stdcall GetDefColor(char* inColor)
{
	typedef struct t_CrlMap
	{
		char szColor[ARTIKUL_SIZE];
		COLORREF Color;
	} CrlMap;

	CrlMap ColorMap[]=
	{
		{"red",    RGB(255,0,0)},
		{"green",  RGB(0,255,0)},
		{"blue",   RGB(0,0,255)},
		{"grey",   RGB(128,128,128)},
		{"white",  RGB(255,255,255)},
		{"yellow", RGB(255,255,0)},
		{"violet", RGB(255,0,255)},
		{"black",  RGB(0,0,0)},
	};

	for(int i=0; i<sizeof(ColorMap)/sizeof(ColorMap[0]); i++)
	{
		if(0==strcmp(ColorMap[i].szColor, inColor))
			return ColorMap[i].Color;
	}

	return RGB(0,255,0);
};
} // end namespace db
