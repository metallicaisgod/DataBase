#ifndef __CTA_DATABASE_H__
#define __CTA_DATABASE_H__

//#ifdef _DEBUG
//#define _HAS_ITERATOR_DEBUGGING 0
//#define _SECURE_SCL 0
//#endif //

#include <list>
#include <map>
#include <vector>
//#include <char.h>
//#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>

//#include <boost/shared_ptr.hpp>
#include <memory>               // for std::auto_ptr

#include <algorithm>            // for std::swap
#include <functional>           // for std::less
#include <typeinfo>             // for std::bad_cast
#include <iosfwd>               // for std::basic_ostream

#include "structs.h"
#include "Enumerator.h"



namespace db
{

#define DBL_TRESHOLD 0.0000001
	
	// Forward declaration
	class DbProvider;
	class DbSeries;

	void FillCompatibility(char *pCompatibility, db::DbSeries *pSeries);

	class DbImplant : public ImplantData
	{
		DbSeries* m_pSeries;
		
	
		DbImplant(){m_pSeries = NULL; memset(name, 0, sizeof(name)/sizeof(name[0]));}
	public:
		DbSeries* GetSeries(){ return m_pSeries; };
		DbImplant(DbSeries* pSeries) : m_pSeries(pSeries)
		{
			memset(name, 0, sizeof(name)/sizeof(name[0]));
			memset(artikul, 0, sizeof(artikul)/sizeof(artikul[0]));
			memset(szModelName, 0, sizeof(szModelName)/sizeof(szModelName[0]));
			memset(szCompatibility, 0, sizeof(szCompatibility)/sizeof(szCompatibility[0]));
			memset(defcolor, 0, sizeof(defcolor)/sizeof(defcolor[0]));
			szCompatibility[0]='Q';
			editable = true;
			state = ObjState::Undefined ;
			D1 = D2 = L1 = L2 = 0.0;

			//
			//FillCompatibility(szCompatibility, pSeries);
			//strcat(szCompatibility, pSeries->name);
			//strcat(szCompatibility, " @ ");
			//for(std::vector<CompatibleSeries>::iterator ic = pSeries->m_CompSer.begin(); ic!=pSeries->m_CompSer.end();ic++)
			//{
			//	strcat(szCompatibility, ic->prov);
			//	strcat(szCompatibility, " : ");
			//	strcat(szCompatibility, ic->ser);
			//	strcat(szCompatibility, " @ ");
			//}
			
		}

		DbImplant& operator=(const ImplantData& src)
		{
			strncpy(name, src.name, NAME_SIZE);
			strncpy(artikul, src.artikul, ARTIKUL_SIZE);
			state = src.state;
			D1 = src.D1;
			D2 = src.D2;
			L1 = src.L1;
			L2 = src.L2;
			Le = src.Le;
			editable = src.editable;
			strncpy(szModelName, src.szModelName, _MAX_PATH);
			strncpy(szCompatibility, src.szCompatibility, _MAX_PATH);
			strncpy(defcolor, src.defcolor, ARTIKUL_SIZE);
			
			Segments.clear();
			Segments.reserve(src.Segments.size());
			std::copy(src.Segments.begin(), src.Segments.end(), std::back_inserter(Segments));
			return *this;
		}
		DbImplant& operator=(const DbImplant& src)
		{
			this->operator=(static_cast<const ImplantData&>(src));
			return *this;
		}


		bool operator==(const DbImplant& rhs) const
		{
			return !strncmp(artikul, rhs.artikul, ARTIKUL_SIZE)&&
				!strncmp(szCompatibility, rhs.szCompatibility, _MAX_PATH) &&
				!strncmp(name, rhs.name, _MAX_PATH) &&
				fabs(D1 - rhs.D1) < DBL_TRESHOLD &&
				fabs(D2 - rhs.D2) < DBL_TRESHOLD &&
				fabs(L1 - rhs.L1) < DBL_TRESHOLD &&
				fabs(L2 - rhs.L2) < DBL_TRESHOLD;
		}
	};

	class DbAbutment : public AbutmentData
	{
		DbSeries* m_pSeries;
	
	public:
		DbSeries* GetSeries(){ return m_pSeries; };
		DbAbutment() : m_pSeries(NULL) {}
		DbAbutment(DbSeries* pSeries) : m_pSeries(pSeries)
		{
			memset(name, 0, sizeof(name)/sizeof(name[0]));
			memset(artikul, 0, sizeof(artikul)/sizeof(artikul[0]));
			memset(szModelName, 0, sizeof(szModelName)/sizeof(szModelName[0]));
			memset(szCompatibility, 0, sizeof(szCompatibility)/sizeof(szCompatibility[0]));
			memset(defcolor, 0, sizeof(defcolor)/sizeof(defcolor[0]));
			szCompatibility[0]='Q';
			state = ObjState::Undefined ;
			D1 = L1 = Alpha = 0.0;
			editable=true;
			//FillCompatibility(szCompatibility, pSeries);
		}
		
		DbAbutment& operator=(const AbutmentData& src)
		{
			strncpy(name, src.name, NAME_SIZE);
			strncpy(artikul, src.artikul, ARTIKUL_SIZE);
			state = src.state;
			D1 = src.D1;
			L1 = src.L1;
			Alpha = src.Alpha;
			editable = src.editable;
			strncpy(szModelName, src.szModelName, _MAX_PATH);
			strncpy(szCompatibility, src.szCompatibility, _MAX_PATH);
			strncpy(defcolor, src.defcolor, ARTIKUL_SIZE);
			Segments.clear();
			Segments.reserve(src.Segments.size());
			std::copy(src.Segments.begin(), src.Segments.end(), std::back_inserter(Segments));
			return *this;
		}
		DbAbutment& operator=(const DbAbutment& src)
		{
			this->operator =(static_cast<const AbutmentData&>(src));
			return *this;
		}
		
		bool operator==(const DbAbutment& rhs) const
		{
			return !strncmp(artikul, rhs.artikul, ARTIKUL_SIZE) &&
				//!strncmp(szModelName, rhs.szModelName, _MAX_PATH) &&
				!strncmp(name, rhs.name, NAME_SIZE) &&
				fabs(D1 - rhs.D1) < DBL_TRESHOLD &&
				fabs(L1 - rhs.L1) < DBL_TRESHOLD &&
				fabs(Alpha - rhs.Alpha) < DBL_TRESHOLD;
		}
	};

	typedef std::vector<DbImplant*> t_ImplantList;
	typedef std::vector<DbAbutment*> t_AbutmentList;
	
	class DbSeries : public Series
	{
		DbProvider* m_pProvider;
		t_ImplantList	m_Implants;
		t_AbutmentList	m_Abutments;
			
	public:
		std::vector<CompatibleSeries> m_CompSer;

		DbSeries() : m_pProvider(NULL) {}
		
        DbSeries(DbProvider* pProvider, const char* szName) : m_pProvider(pProvider)
		{
            strncpy(name, szName, NAME_SIZE);
            name[NAME_SIZE-1] = '\0';
		}

		virtual ~DbSeries();
		DbSeries& operator+=(const DbSeries& other);
		
		DbProvider& GetProvider() const
		{
			return *m_pProvider;
		}

		const t_ImplantList& GetImplants() const                                                                           
		{
			return m_Implants;
		}

		const t_AbutmentList& GetAbutment() const                                                                           
		{
			return m_Abutments;
		}

		DbImplant& AddImplant(const ImplantData& data);
		DbAbutment& AddAbutment(const AbutmentData& data);

		void RemoveImplant(const DbImplant& data);
		void RemoveAbutment(const DbAbutment& data);
		void RemoveImplant(unsigned long index);
		void RemoveAbutment(unsigned long index);

		bool operator==(const DbSeries& rhs) const
		{
			return !strncmp(name, rhs.name, NAME_SIZE);
		}
	};

	typedef std::list<DbSeries*> t_SeriesList;
	class DbProvider : public Provider
	{
	private:
		t_SeriesList m_Series;

	public:
		DbProvider() 
		{
			strcpy_s(m_szExtFile, NAME_SIZE, "");
		}
        DbProvider(const char* provName)
		{
            strncpy(name, provName, NAME_SIZE);
            name[NAME_SIZE-1] = '\0';
			strcpy_s(m_szExtFile, NAME_SIZE, "");
		}
		
		virtual ~DbProvider();
		DbProvider& operator+=(const DbProvider& other);

		const size_t SeriesCount() const
		{
			return m_Series.size();
		}

		t_SeriesList& GetSeriesList() 
		{
			return m_Series;
		}
		template <typename predicate>
        enumerator<t_SeriesList::iterator, predicate> GetSeriesEnumerator(predicate filter=db::no_filter<db::DbSeries*>())
		{
			return enumerator<t_SeriesList::iterator, predicate>(m_Series.begin(), m_Series.end(), filter);
		}


        DbSeries&	AddSeries(const char* szSeriesName);
		void		RemoveSeries(const DbSeries& data);

		bool operator==(const DbProvider& rhs) const
		{
			return !strncmp(name, rhs.name, NAME_SIZE);
		}

		char m_szExtFile[NAME_SIZE];
	};

	typedef std::list<DbProvider*> t_ProvidersList;

	class IADataBase
	{
		t_ProvidersList	m_Providers;
	public:
		IADataBase();
		virtual ~IADataBase();

        static bool LoadXml(IADataBase& indb, const char* fileName);
        bool LoadXml(const char* fileName);
		
        static void SaveXml(const IADataBase& indb, const char* fileName);
        void SaveXml(const char* fileName);


        static bool LoadXml_All(IADataBase& indb, const char* fileName);
        static bool LoadFromDir(IADataBase& indb, const char* dirName);
        bool LoadXml_All(const char* fileName);
		
        static void SaveXml_All(const IADataBase& indb, const char* fileName);
        void SaveXml_All(const char* fileName);

		////////////////////////////////////////////////////////////////////////////
		
#define LIBRARY_INTEMS     0x00000001 
#define USERS_INTEMS       0x00000002
#define PROVIDER_EXT_FILE  0x00000040

        static bool LoadXml_2(IADataBase& indb, const char* fileName, unsigned long flags);
        bool LoadXml_2(const char* fileName, unsigned long flags);

        static void SaveXml_2(const IADataBase& indb, const char* fileName, unsigned long flags);
        void SaveXml_2(const char* fileName, unsigned long flags);
		////////////////////////////////////////////////////////////////////////////

		IADataBase& operator+=(const IADataBase& extraDB);

		const size_t ProvidersCount() const
		{
			return m_Providers.size();
		}

		template <typename predicate>
        enumerator<t_ProvidersList::iterator, predicate> GetProvidersEnumerator(predicate filter = predicate())
		{
			return enumerator<t_ProvidersList::iterator, predicate>(m_Providers.begin(), m_Providers.end(), filter);
		}


        DbProvider& AddProvider(const char* providerName);
		void RemoveProvider(const DbProvider& data);

		void BuildCompatibility();

	public:

		template <typename predicate, typename comparer>
        enumerator<t_ImplantList::iterator, predicate>* GetImplantsEnumerator(comparer compare, predicate filter=db::no_filter<db::DbImplant*>())
		{
			t_ImplantList *all_ptr = new t_ImplantList;
			t_ProvidersList::iterator prov_iter = m_Providers.begin();
			for (; prov_iter != m_Providers.end(); prov_iter++)
			{
				enumerator<t_SeriesList::iterator > enumer = (*prov_iter)->GetSeriesEnumerator(db::no_filter<DbSeries*>());
				while (enumer.MoveNext())
				{
					DbSeries* pSeries = enumer.GetCurrent();
					const t_ImplantList& implants = pSeries->GetImplants();
					all_ptr->reserve(all_ptr->size() + implants.size());
					std::copy(implants.begin(), implants.end(), std::back_inserter(*all_ptr));
				}
			}

			std::sort(all_ptr->begin(), all_ptr->end(), compare);
            return new enumerator_ref2ptr<t_ImplantList, predicate>(all_ptr, filter);
		}

		template <typename predicate, typename comparer>
        enumerator<t_AbutmentList::iterator, predicate>*  GetAbutmentsEnumerator(comparer compare, predicate filter=db::no_filter<db::DbAbutment*>())
		{
			t_AbutmentList *all_ptr = new t_AbutmentList;
			t_ProvidersList::iterator prov_iter = m_Providers.begin();
			for (; prov_iter != m_Providers.end(); prov_iter++)
			{
				enumerator<t_SeriesList::iterator> enumer = (*prov_iter)->GetSeriesEnumerator(db::no_filter<DbSeries*>());
				while (enumer.MoveNext())
				{
					DbSeries* pSeries = enumer.GetCurrent();
					const t_AbutmentList& abutments = pSeries->GetAbutment();
					all_ptr->reserve(all_ptr->size() + abutments.size());
					std::copy(abutments.begin(), abutments.end(), std::back_inserter(*all_ptr));
				}
			}

			std::sort(all_ptr->begin(), all_ptr->end(), compare);
			return new enumerator_ref2ptr<t_AbutmentList, predicate>(all_ptr, filter);
		}
		bool ExportToCSV(char* file_name);
		bool ImportFromCSV(char* file_name);
	};

	inline void FillCompatibility(char *pCompatibility, db::DbSeries *pSeries)
	{
		return;
		//std::vector<std::string> comp_strings;
		////
		//if(pSeries)
		//{
		//	if(1)//(!pSeries->GetAbutment().empty()) && (!pSeries->GetImplants().empty()) )
		//	{
		//		std::string strComp = std::string(pSeries->GetProvider().name);
		//		strComp += std::string(" # " );
		//		strComp += std::string(pSeries->name);
		//		strComp += std::string(" @@@ " );
		//		comp_strings.push_back(strComp);
		//		//strcat(pCompatibility, pSeries->name);
		//	}
		//	for(std::vector<CompatibleSeries>::iterator ic = pSeries->m_CompSer.begin(); ic!=pSeries->m_CompSer.end();ic++)
		//	{
		//		std::string strComp = std::string(ic->prov);
		//		strComp += std::string(" # " );
		//		strComp += std::string(ic->ser);
		//		strComp += std::string(" @@@ " );
		//		comp_strings.push_back(strComp);

		//		//strcat(pCompatibility, " @ ");
		//		//strcat(pCompatibility, ic->prov);
		//		//strcat(pCompatibility, " : ");
		//		//strcat(pCompatibility, ic->ser);
		//	}
		//	std::sort(comp_strings.begin(), comp_strings.end());

		//	for(std::vector<std::string>::iterator is = comp_strings.begin(); is!=comp_strings.end();is++)
		//		strcat(pCompatibility, (*is).c_str());
		//}
	}
	//COLORREF __stdcall GetDefColor(char* inColor);
    //unsigned long __stdcall GetDefColor(char* inColor);
    QColor __stdcall GetDefColor(char* inColor);
}
	

typedef db::enumerator<db::t_ImplantList::iterator, db::no_filter<db::DbImplant*> > t_implantsEnumNF;
//typedef boost::shared_ptr<t_implantsEnumNF> t_implantsEnumNFsp;
typedef t_implantsEnumNF *t_implantsEnumNFsp;

typedef db::enumerator<db::t_AbutmentList::iterator, db::no_filter<db::DbAbutment*> > t_abutmentEnumNF;
//typedef boost::shared_ptr<t_abutmentEnumNF> t_abutmentEnumNFsp;
typedef t_abutmentEnumNF *t_abutmentEnumNFsp;

//typedef db::enumerator<db::t_SeriesList::iterator, db::no_filter<db::DbSeries*> > t_seriesEnumNF;
//typedef boost::shared_ptr<t_seriesEnumNF> t_seriesEnumNFsp;


#endif
