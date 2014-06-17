#include "stdafx.h"
#pragma warning( disable : 4267 ) 

#include "IADataBase.h"
#include "ext_resources.h"
//#include "ticpp.h"
#include "ParserHelper.h"
#include <QFile>
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
}

bool IADataBase::LoadXml_2( IADataBase& indb, const char* fileName, unsigned long flags)
{
	bool loaded = false;
    QDomDocument pDoc;
	try
	{
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return false;
        }
        if(!pDoc.setContent(&file))
        {
            file.close();
            return false;
        }
        file.close();
        QDomElement root = pDoc.firstChildElement();
        QString v = root.tagName();

        //pDoc = new ticpp::Document(fileName);
        //pDoc->LoadFile(TIXML_ENCODING_UTF8);
        //ticpp::Element* root = pDoc->FirstChildElement();
        //std::string v = root->Value();
		if (v == "boost_serialization")
            ParserHelper::ParseOldDataBase(indb, root);
		else
            ParserHelper::ParseDataBase(indb, fileName, root, flags);

		loaded = true;


	}
    catch (...)
	{
        std::cout << "unknown error" /*<< e.m_details*/ << std::endl;
	}
//	if (pDoc != NULL)
//		delete pDoc;
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
}

void IADataBase::SaveXml_2(const char* fileName, unsigned long flags)
{
	return IADataBase::SaveXml_2(*this, fileName, flags);
}
void IADataBase::SaveXml_2(const IADataBase& indb, const char* fileName, unsigned long flags)
{
//	QDomDocument pDoc;
//	try
//	{
//		pDoc = new TiXmlDocument();
//		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
//		pDoc->LinkEndChild( decl );
//		pDoc->LinkEndChild( ParserHelper::ToXml(indb, fileName, flags) );
//		pDoc->SaveFile(fileName);
//	}
//	catch (ticpp::Exception& e)
//	{
//	}
//	if (pDoc != NULL)
//		delete pDoc;

    QDomDocument doc;
    QDomProcessingInstruction xmlDeclaration = doc.createProcessingInstruction("xml", "version=\"1.0\" ");
    doc.appendChild(xmlDeclaration);
    doc.appendChild(*(ParserHelper::ToXml(indb, fileName, doc, flags)));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    doc.save(out, 4);

    //out << doc.toString();

    file.close();
}

bool IADataBase::LoadXml_All(IADataBase& indb, const char* fileName)
{
    QString strFileN(fileName);
    QChar strExt = strFileN.at(strFileN.length() - 1);
    if(strExt == QChar('\\'))
        return LoadFromDir(indb, strFileN.toLocal8Bit().data());
	bool bLoadLib = LoadXml_2(indb, fileName, 0);
	strFileN+=".usr";
    bool bLoadUsr = LoadXml_2(indb, strFileN.toLocal8Bit().data(), USERS_INTEMS);
	return bLoadLib || bLoadUsr;
	
}
bool IADataBase::LoadXml_All(const char* fileName)
{
	return IADataBase::LoadXml_All(*this, fileName);
}
	
bool IADataBase::LoadFromDir(IADataBase& indb, const char* dirName)
{
    //WIN32_FIND_DATAA ffd;
    //HANDLE hFind = INVALID_HANDLE_VALUE;
	
    QString strDir;
    strDir = "*.iadbxml";
	bool bLoadLib = false;

    QDir directory(dirName);
    QFileInfoList fiList =  directory.entryInfoList(QStringList()<<strDir,QDir::Files);

	// List all the files in the directory with some info about them.
    for(int i =0; i < fiList.count(); i++)
    {
        if (!fiList[i].isDir())
		{
			unsigned long load_flag=0;
            QString currFile(fiList[i].fileName());
			//if(0==currFile.compare("user-defined.iadbxml"))
			if(currFile == "user-defined.iadbxml")
				load_flag = USERS_INTEMS;

			currFile = dirName;
            currFile += fiList[i].fileName();
            bLoadLib |= LoadXml_2(indb, currFile.toLocal8Bit().data(), load_flag);
		}
    }


	return true;
}
void IADataBase::SaveXml_All(const IADataBase& indb, const char* fileName)
{
	SaveXml_2(indb, fileName, LIBRARY_INTEMS);
    QString strFileN(fileName);
	strFileN+=".usr";
    SaveXml_2(indb, strFileN.toLocal8Bit().data(), USERS_INTEMS);
	
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
    QFile f_list(file_name);
    //FILE *f_list = fopen(file_name, "wt");
    //if(!f_list)
    bool bRet = f_list.open(QFile::WriteOnly | QFile::Text);
    if(!bRet)
        return false;

	for(t_ProvidersList::iterator ip = m_Providers.begin();ip != m_Providers.end();ip++)
	{
		// get provider
		DbProvider *prov = *ip;

		// write provider to the list file
        QString strProv(prov->name);
        //std::string strProv = prov->name;
        strProv += ".csv\n";
        f_list.write(strProv.toLocal8Bit().data());
        //fprintf(f_list, "%s\n", strProv.c_str());
		

		// open provider CSV file
        QString str(file_name);
        //std::string str=file_name;
        int slpos =	str.lastIndexOf('\\');
        str = str.left(slpos+1);
        str += prov->name;
        str += ".csv";
        QFile f(str);
        //FILE *f = fopen(str.c_str(), "wt");
        if(!f.open(QFile::WriteOnly | QFile::Text))
            return false;
	
		for(t_SeriesList::iterator is = prov->GetSeriesList().begin();is != prov->GetSeriesList().end();is++)
		{
			// Save implant to CSV
			DbSeries *ser = *is;
			if(ser->GetImplants().size())
			{
				// write header
                f.write(";;;;;;;;;;\n");
                f.write("ObjectType;Manufacturer;Series;Name;Articul;AbComp;D1;D2;L1;L2;Le;ModelFileName\n");
                //fprintf(f, ";;;;;;;;;;\n");
                //fprintf(f, "ObjectType;Manufacturer;Series;Name;Articul;AbComp;D1;D2;L1;L2;Le;ModelFileName\n");
			}
			for(t_ImplantList::const_iterator ii = ser->GetImplants().begin();ii != ser->GetImplants().end();ii++)
			{
				db::DbImplant* pImplant = *ii;
				//FillCompatibility(pImplant->szCompatibility, ser);
                QString impl = QString("Impl;%1;%2;%3;%4;%5;%6;%7;%8;%9;%10;%11\n").
                        arg(prov->name).arg(ser->name).arg(pImplant->name).arg(pImplant->artikul).arg(pImplant->szCompatibility).
                        arg(pImplant->D1,0, 'g', 3).arg(pImplant->D2,0, 'g', 3).arg(pImplant->L1,0, 'g', 3).arg(pImplant->L2,0, 'g', 3).
                        arg(pImplant->Le,0, 'g', 3).arg(pImplant->szModelName);
                f.write(impl.toLocal8Bit().data());
//				fprintf(f, "Impl;%s;%s;%s;%s;%s;%.3f;%.3f;%.3f;%.3f;%.3f;%s\n",
//					prov->name, ser->name, pImplant->name, pImplant->artikul, pImplant->szCompatibility,
//					pImplant->D1,pImplant->D2, pImplant->L1,pImplant->L2,
//					pImplant->Le, pImplant->szModelName);
			}
			

			// Save abutment to CSV
			if(ser->GetAbutment().size()>0)
			{
				// write header
                f.write(";;;;;;;;;;\n");
                f.write("ObjectType;Manufacturer;Series;Name;Articul;AbComp;D1;L1;Alpha; ; ;ModelFileName\n");
//				fprintf(f, ";;;;;;;;;;\n");
//				fprintf(f, "ObjectType;Manufacturer;Series;Name;Articul;AbComp;D1;L1;Alpha; ; ;ModelFileName\n");
			}
			for(t_AbutmentList::const_iterator ia = ser->GetAbutment().begin(); ia != ser->GetAbutment().end();ia++)
			{
				db::DbAbutment* pAbutment = *ia;
				//FillCompatibility(pImplant->szCompatibility, ser);
                QString abut = QString("Abut;%1;%2;%3;%4;%5;%6;%7;%8; ; ;%9\n").
                        arg(prov->name).arg(ser->name).arg(pAbutment->name).arg(pAbutment->artikul).arg(pAbutment->szCompatibility).
                        arg(pAbutment->D1,0, 'g', 3).arg(pAbutment->L1,0, 'g', 3).arg(pAbutment->Alpha,0, 'g', 3).arg(pAbutment->szModelName);
                f.write(abut.toLocal8Bit().data());
//				fprintf(f, "Abut;%s;%s;%s;%s;%s;%.3f;%.3f;%.3f; ; ;%s\n",
//					prov->name, ser->name, pAbutment->name, pAbutment->artikul, pAbutment->szCompatibility,
//					pAbutment->D1, pAbutment->L1, pAbutment->Alpha,pAbutment->szModelName);
			}
			
			// Save COmpatible series to CSV
			if(ser->m_CompSer.size()>0)
			{
				// write header
                f.write(";;;;;;;;;;\n");
                f.write("ObjectType;Manufacturer1;Series1;Manufacturer2;Series2\n");
//				fprintf(f, ";;;;;;;;;;\n");
//				fprintf(f, "ObjectType;Manufacturer1;Series1;Manufacturer2;Series2\n");
			}
			for(std::vector<CompatibleSeries>::iterator ics = ser->m_CompSer.begin(); ics != ser->m_CompSer.end();ics++)
			{
				CompatibleSeries ser_com = *ics;
				//FillCompatibility(pImplant->szCompatibility, ser);
                QString compSer = QString("CompSer;%1;%2;%3;%4\n").
                        arg(prov->name).arg(ser->name).arg(ser_com.prov).arg(ser_com.ser);
                f.write(compSer.toLocal8Bit().data());
//				fprintf(f, "CompSer;%s;%s;%s;%s\n",
//					prov->name, ser->name, ser_com.prov, ser_com.ser);
			}
		}
        f.close();
        //fclose(f);
	}
	// close manufacturers list file
    f_list.close();

    //fclose (f_list);

	return true;
}

bool IADataBase::ImportFromCSV(char* file_name)
{
	bool bList=true;
    //FILE *f_lst = NULL;
    QFile f_lst(file_name);
	// get CSV files path
    QString strPath(file_name), strFileName(file_name);
    int pos = strPath.lastIndexOf('\\');
    strPath = strPath.left(pos+1);
	
    // check if input file is csv
    strFileName = strFileName.toLower();
    //std::transform(strFileName.begin(), strFileName.end(), strFileName.begin(), ::tolower);
    if(strFileName.lastIndexOf(".csv")==(strFileName.length()-4))
		bList=false;

	// open manufacturers list file
	if(bList)
        if(!f_lst.open(QFile::ReadOnly | QFile::Text))
            return false;
	
	do
	{
		// read manufacturer files from list
        QString strPath1 = strPath;
		char szFirmFile[1000]="", szDum[255]="";
		if(bList)
		{
            f_lst.readLine(szFirmFile, 1000);
            //fscanf(f_lst, "%[^\n]s", szFirmFile);
            //fscanf(f_lst, "%[\n]s", szDum);
            strPath1 += szFirmFile;
            strPath1.truncate(strPath1.indexOf('\n'));
		}
		else
			strPath1 = file_name;

		
		// open manufacturer CSV file 
        QFile f(strPath1);

        //FILE *f = fopen(strPath1.c_str(), "rt");
        if(f.open(QFile::ReadOnly | QFile::Text))
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
                //fscanf(f, "%[^\n]s", szStr);
                //fscanf(f, "%[\n]s", szDummy);
                f.readLine(szStr, 1000);

                QString strF(szStr);
                strF.truncate(strF.indexOf('\n'));
				while (true)
				{

                    int tz2pos = strF.indexOf(";;");
					if(tz2pos==-1)
						break;
					strF.insert(tz2pos+1, " ");
					//if(-1==strF.Replace(";;", "; ;"))
					//	break;
				};
				const int items_num = 30;
                QString str_items[items_num];
				int curPos= 0;
				int posLeft=0, posRight=0;
				for(int i=0; i<items_num; i++)
				{
                    posLeft = strF.indexOf(";",posLeft+1);
					if(posLeft==-1)
						break;
                    if(i == 0)
                    {
                        str_items[i] = strF.left(posLeft);
                        posLeft = -1;
                        continue;
                    }
                    posRight = strF.indexOf(";",posLeft+1);
					if(posRight==-1)
						posRight=strF.length();
                    str_items[i]=strF.mid(posLeft+1, posRight-posLeft - 1);

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
                if(0==str_items[0].compare("ObjectType",Qt::CaseInsensitive)||0==str_items[0].compare("ObjType",Qt::CaseInsensitive)||
                    0==str_items[0].compare("Object Type",Qt::CaseInsensitive)||0==str_items[0].compare("Obj Type",Qt::CaseInsensitive))
				{
					for(int i=0; i<items_num; i++)
					{
						//str_items[i].Replace(" ", "");
						nnPos=0;
						while(nnPos!=-1)
						{
                            nnPos=str_items[i].indexOf(" ",nnPos);
							if(nnPos!=-1)
                                str_items[i].truncate(nnPos);
						}
						
						// Manufacturer
                        if(str_items[i].compare("Manufacturer",Qt::CaseInsensitive)==0 || str_items[i].compare("Manuf",Qt::CaseInsensitive)==0||
                            str_items[i].compare("Manufacturer1",Qt::CaseInsensitive)==0 || str_items[i].compare("Manuf1",Qt::CaseInsensitive)==0)
						{
							prov_idx=i;
						}
						// Series
                        if( str_items[i].compare("Series",Qt::CaseInsensitive)==0 || str_items[i].compare("Ser",Qt::CaseInsensitive)==0 ||
                            str_items[i].compare("Series1",Qt::CaseInsensitive)==0 || str_items[i].compare("Ser1",Qt::CaseInsensitive)==0)
						{
							ser_idx=i;
						}
						// Manufacturer2
                        if(str_items[i].compare("Manufacturer2",Qt::CaseInsensitive)==0 || str_items[i].compare("Manuf2",Qt::CaseInsensitive)==0)
							prov_cs_idx=i;
						// Series2
                        if(str_items[i].compare("Series2",Qt::CaseInsensitive)==0 || str_items[i].compare("Ser2",Qt::CaseInsensitive)==0)
							ser_cs_idx=i;

						// Name
                        if(str_items[i].compare("Name",Qt::CaseInsensitive)==0)
							name_idx=i;
						// Articul
                        if(str_items[i].compare("Articul",Qt::CaseInsensitive)==0)
							artikul_idx=i;
						// Compatibility
                        if(str_items[i].compare("AbComp",Qt::CaseInsensitive)==0)
							szCompatibility_idx=i;
						// szModelName
                        if(str_items[i].compare("ModelFileName",Qt::CaseInsensitive)==0 || str_items[i].compare("szModelName",Qt::CaseInsensitive)==0)
							szModelName_idx=i;
						// D1
                        if(str_items[i].compare("D1",Qt::CaseInsensitive)==0 || str_items[i].compare("Da",Qt::CaseInsensitive)==0)
							D1_idx=i;
						// D2
                        if(str_items[i].compare("D2",Qt::CaseInsensitive)==0)
							D2_idx=i;
						// L1
                        if(str_items[i].compare("L1",Qt::CaseInsensitive)==0|| str_items[i].compare("La",Qt::CaseInsensitive)==0)
							L1_idx=i;
						// L2
                        if(str_items[i].compare("L2",Qt::CaseInsensitive)==0)
							L2_idx=i;
						// Le
                        if(str_items[i].compare("Le",Qt::CaseInsensitive)==0)
							Le_idx=i;
						// Alpha
                        if(str_items[i].compare("Alpha",Qt::CaseInsensitive)==0||str_items[i].compare("Alfa",Qt::CaseInsensitive)==0)
							Alpha_idx=i;

						// Path 
                        if(str_items[i].compare("Path",Qt::CaseInsensitive)==0||str_items[i].compare("ModelPath",Qt::CaseInsensitive)==0||
                            str_items[i].compare("ModelsPath",Qt::CaseInsensitive)==0)
							szModelName_path_idx=i;
						// Extension 
                        if(str_items[i].compare("ext",Qt::CaseInsensitive)==0||str_items[i].compare("ModelFileNameExt",Qt::CaseInsensitive)==0)
							szModelName_ext_idx=i;
										
					} 
				}

				// read and add implant data
                if(0==str_items[0].compare("Impl",Qt::CaseInsensitive))
				{
					// add series abd provider 
                    db::DbProvider &prov = AddProvider((str_items[prov_idx].toLocal8Bit().data()));
                    db::DbSeries &series = prov.AddSeries((str_items[ser_idx].toLocal8Bit().data()));

					// get implant data
					db::ImplantData idata;

                    strcpy_s(idata.name, NAME_SIZE, (str_items[name_idx].toLocal8Bit().data()));
                    strcpy_s(idata.artikul, ARTIKUL_SIZE, (str_items[artikul_idx].toLocal8Bit().data()));
                    strcpy_s(idata.szCompatibility, _MAX_PATH, (str_items[szCompatibility_idx].toLocal8Bit().data()));

					if(szModelName_path_idx!=-1)
                        strcpy_s(idata.szModelName, _MAX_PATH, (str_items[szModelName_path_idx].toLocal8Bit().data()));
					else
						strcpy_s(idata.szModelName, _MAX_PATH, "");
                    strcat_s(idata.szModelName, _MAX_PATH, (str_items[szModelName_idx].toLocal8Bit().data()));
					if(szModelName_ext_idx!=-1)
                        strcat_s(idata.szModelName, _MAX_PATH, (str_items[szModelName_ext_idx].toLocal8Bit().data()));


					// replace "," by "." to make atof working
                    //str_items[D1_idx].replace(str_items[D1_idx].indexOf(",", 0), 1, ".");
                    //str_items[D2_idx].replace(str_items[D2_idx].indexOf(",", 0), 1, ".");
                    //str_items[L1_idx].replace(str_items[L1_idx].indexOf(",", 0), 1, ".");
                    //str_items[L2_idx].replace(str_items[L2_idx].indexOf(",", 0), 1, ".");
                    //str_items[Le_idx].replace(str_items[Le_idx].indexOf(",", 0), 1, ".");
                    QLocale::setDefault(QLocale::German);
					
                    idata.D1=str_items[D1_idx].toDouble();
                    idata.D2=str_items[D2_idx].toDouble();
                    idata.L1=str_items[L1_idx].toDouble();
                    idata.L2=str_items[L2_idx].toDouble();
                    idata.Le=str_items[Le_idx].toDouble();

					idata.state = db::ObjState::Active;
					idata.editable=false;
					
					// add implant 
					series.AddImplant(idata);
				}
			
				// read and add Abutment data
				if(0==str_items[0].compare("Abut"))
				{
					// add series abd provider 
                    db::DbProvider &prov = AddProvider((str_items[prov_idx].toLocal8Bit().data()));
                    db::DbSeries &series = prov.AddSeries((str_items[ser_idx].toLocal8Bit().data()));

					// get abutment data
					db::AbutmentData adata;
					if(0==str_items[4].compare("Conmet_Tolstyak"))
					{
						str_items[4]==str_items[4];
					}
                    strcpy_s(adata.name, NAME_SIZE, (str_items[name_idx].toLocal8Bit().data()));
                    strcpy_s(adata.artikul, ARTIKUL_SIZE, (str_items[artikul_idx].toLocal8Bit().data()));
                    strcpy_s(adata.szCompatibility, _MAX_PATH, (str_items[szCompatibility_idx].toLocal8Bit().data()));
					//strcpy_s(adata.szModelName, _MAX_PATH, LPCTSTR(str_items[szModelName_idx]));

					if(szModelName_path_idx!=-1)
                        strcpy_s(adata.szModelName, _MAX_PATH, (str_items[szModelName_path_idx].toLocal8Bit().data()));
					else
						strcpy_s(adata.szModelName, _MAX_PATH, "");
                    strcat_s(adata.szModelName, _MAX_PATH, (str_items[szModelName_idx].toLocal8Bit().data()));
					if(szModelName_ext_idx!=-1)
                        strcat_s(adata.szModelName, _MAX_PATH, (str_items[szModelName_ext_idx].toLocal8Bit().data()));



					// replace "," by "." to make atof working
//					str_items[D1_idx].replace(str_items[D1_idx].find(",", 0), 1, ".");
//					str_items[L1_idx].replace(str_items[L1_idx].find(",", 0), 1, ".");
//					str_items[Alpha_idx].replace(str_items[Alpha_idx].find(",", 0), 1, ".");
                    QLocale::setDefault(QLocale::German);
					

                    adata.D1=str_items[D1_idx].toDouble();
                    adata.L1=str_items[L1_idx].toDouble();
                    adata.Alpha=str_items[Alpha_idx].toDouble();
					
					adata.state = db::ObjState::Active;
					adata.editable=false;

					// add implant 
					series.AddAbutment(adata);
				}

				// read and add compatible series
				if(0==str_items[0].compare("CompSer"))
				{
					// add series abd provider 
                    db::DbProvider &prov = AddProvider((str_items[prov_idx].toLocal8Bit().data()));
                    db::DbSeries &series = prov.AddSeries((str_items[ser_idx].toLocal8Bit().data()));

					// get abutment data
					db::CompatibleSeries csr;
					//if(0==str_items[4].compare("Conmet_Tolstyak"))
					//{
					//	str_items[4]==str_items[4];
					//}
                    strcpy_s(csr.prov, NAME_SIZE, (str_items[prov_cs_idx].toLocal8Bit().data()));
                    strcpy_s(csr.ser, NAME_SIZE, (str_items[ser_cs_idx].toLocal8Bit().data()));

					// add Compatible Series
					series.m_CompSer.push_back(csr);
				}


            } while(!f.atEnd());
            f.close();
		}

		// load only one CSV if it is not list file
		if(!bList)
			break;
		

    } while(!f_lst.atEnd());
	
	if(bList)
        f_lst.close();

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

QColor __stdcall GetDefColor(char* inColor)
{
	typedef struct t_CrlMap
	{
		char szColor[ARTIKUL_SIZE];
        QColor Color;
	} CrlMap;

	CrlMap ColorMap[]=
	{
        {"red",    Qt::red},
        {"green",  Qt::green},
        {"blue",   Qt::blue},
        {"grey",   Qt::gray},
        {"white",  Qt::white},
        {"yellow", Qt::yellow},
        {"violet", QColor(255,0,255)},
        {"black",  Qt::black},
	};

	for(int i=0; i<sizeof(ColorMap)/sizeof(ColorMap[0]); i++)
	{
		if(0==strcmp(ColorMap[i].szColor, inColor))
			return ColorMap[i].Color;
	}

    return Qt::green;
};
} // end namespace db
