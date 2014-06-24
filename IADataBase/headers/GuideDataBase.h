#ifndef __GUIDE_DATABASE_H__
#define __GUIDE_DATABASE_H__
#include "IADataBase.h"
#include <math.h>


namespace db
{

//#define DBL_TRESHOLD 0.0000001
	
	// Forward declaration
	class DbGuideProvider;
	class DbGuideSeries;

	class DbScrew : public ScrewData
	{
		DbGuideSeries* m_pSeries;
			DbScrew()
		{
			m_pSeries = NULL; 
			memset(name, 0, sizeof(name)/sizeof(name[0]));
			memset(szModelName, 0, sizeof(szModelName)/sizeof(szModelName[0]));
			memset(artikul, 0, sizeof(artikul)/sizeof(artikul[0]));
		}
	public:
		DbGuideSeries* GetSeries(){ return m_pSeries; };
		DbScrew(DbGuideSeries* pSeries) : m_pSeries(pSeries)
		{
			memset(name, 0, sizeof(name)/sizeof(name[0]));
			memset(szModelName, 0, sizeof(szModelName)/sizeof(szModelName[0]));
			memset(artikul, 0, sizeof(artikul)/sizeof(artikul[0]));
			editable = true;
			state = ObjState::Undefined ;
			D = Dh = L = Lt = Lh = 0.0;
		}

		DbScrew& operator=(const ScrewData& src)
		{
			strncpy(name, src.name, NAME_SIZE);
			strncpy(artikul, src.artikul, ARTIKUL_SIZE);
			strncpy(szModelName, src.szModelName, _MAX_PATH);
			state = src.state;
			editable = src.editable;
			D  = src.D;
			Dh = src.Dh;
			L  = src.L;
			Lt = src.Lt;
			Lh = src.Lh;
			
			Segments.clear();
			Segments.reserve(src.Segments.size());
			std::copy(src.Segments.begin(), src.Segments.end(), std::back_inserter(Segments));
			return *this;
		}
		DbScrew& operator=(const DbScrew& src)
		{
			this->operator=(static_cast<const ScrewData&>(src));
			return *this;
		}

		bool operator==(const DbScrew& rhs) const
		{
			return !strncmp(name, rhs.name, NAME_SIZE) &&
				fabs(D  - rhs.D ) < DBL_TRESHOLD &&
				fabs(Dh - rhs.Dh) < DBL_TRESHOLD &&
				fabs(L  - rhs.L ) < DBL_TRESHOLD &&
				fabs(Lt - rhs.Lt) < DBL_TRESHOLD &&
				fabs(Lh - rhs.Lh) < DBL_TRESHOLD;
		}
	};

	class DbCutter : public CutterData
	{
		DbGuideSeries* m_pSeries;

	public:
		DbGuideSeries* GetSeries(){ return m_pSeries; };
		DbCutter() : m_pSeries(NULL) {}
		DbCutter(DbGuideSeries* pSeries) : m_pSeries(pSeries)
		{
			memset(name, 0, sizeof(name)/sizeof(name[0]));
			memset(artikul, 0, sizeof(artikul)/sizeof(artikul[0]));
			memset(szModelName, 0, sizeof(szModelName)/sizeof(szModelName[0]));
			memset(defcolor, 0, sizeof(defcolor)/sizeof(defcolor[0]));
			state = ObjState::Undefined ;
			D = Dw = Ds = L = Ls = Lw = 0.0;
			type=Pilot;
			editable=true;
		}
		
		DbCutter& operator=(const CutterData& src)
		{
			strncpy(name, src.name, NAME_SIZE);
			strncpy(artikul, src.artikul, ARTIKUL_SIZE);
			strncpy(szModelName, src.szModelName, _MAX_PATH);
			strncpy(defcolor, src.defcolor, ARTIKUL_SIZE);
			state = src.state;
			D  = src.D;
			Dw = src.Dw;
			Ds = src.Ds;
			L  = src.L;
			Ls = src.Ls;
			Lw = src.Lw;
			type = src.type;
			editable = src.editable;
			Segments.clear();
			Segments.reserve(src.Segments.size());
			std::copy(src.Segments.begin(), src.Segments.end(), std::back_inserter(Segments));
			return *this;
		}
		DbCutter& operator=(const DbCutter& src)
		{
			this->operator =(static_cast<const CutterData&>(src));
			return *this;
		}
		
		bool operator==(const DbCutter& rhs) const
		{
			return !strncmp(name, rhs.name, NAME_SIZE) &&
				!strncmp(artikul, rhs.artikul, NAME_SIZE) &&
				fabs(D  - rhs.D ) < DBL_TRESHOLD &&
				fabs(Dw - rhs.Dw) < DBL_TRESHOLD &&
				fabs(Ds - rhs.Ds) < DBL_TRESHOLD &&
				fabs(L  - rhs.L ) < DBL_TRESHOLD &&
				fabs(Ls - rhs.Ls) < DBL_TRESHOLD &&
				fabs(Lw - rhs.Lw) < DBL_TRESHOLD ;
		}
	};


	class DbSleeve : public SleeveData
	{
		DbGuideSeries* m_pSeries;

	public:
		DbGuideSeries* GetSeries(){ return m_pSeries; };
		DbSleeve() : m_pSeries(NULL) {}
		DbSleeve(DbGuideSeries* pSeries) : m_pSeries(pSeries)
		{
			memset(name, 0, sizeof(name)/sizeof(name[0]));
			memset(artikul, 0, sizeof(artikul)/sizeof(artikul[0]));
			memset(szModelName, 0, sizeof(szModelName)/sizeof(szModelName[0]));
			memset(defcolor, 0, sizeof(defcolor)/sizeof(defcolor[0]));
			state = ObjState::Undefined ;
			D = Do = Dh = Lh = Lw =	Df = 0.0;
			editable=true;
		}
		
		DbSleeve& operator=(const SleeveData& src)
		{
			strncpy(name, src.name, NAME_SIZE);
			strncpy(artikul, src.artikul, ARTIKUL_SIZE);
			strncpy(szModelName, src.szModelName, _MAX_PATH);
			strncpy(defcolor, src.defcolor, ARTIKUL_SIZE);
			state = src.state;
			D  = src.D;
			Do = src.Do;
			Df = src.Df;
			Dh = src.Dh;
			Lh = src.Lh;
			Lw = src.Lw;
			editable = src.editable;
			Segments.clear();
			Segments.reserve(src.Segments.size());
			std::copy(src.Segments.begin(), src.Segments.end(), std::back_inserter(Segments));
			return *this;
		}
		DbSleeve& operator=(const DbSleeve& src)
		{
			this->operator =(static_cast<const SleeveData&>(src));
			return *this;
		}
		
		bool operator==(const DbSleeve& rhs) const
		{
			return !strncmp(name, rhs.name, NAME_SIZE) &&
				fabs(D  - rhs.D ) < DBL_TRESHOLD &&
				fabs(Do - rhs.Do) < DBL_TRESHOLD &&
				fabs(Df - rhs.Df) < DBL_TRESHOLD &&
				fabs(Dh - rhs.Dh) < DBL_TRESHOLD &&
				fabs(Lh - rhs.Lh) < DBL_TRESHOLD &&
				fabs(Lw - rhs.Lw) < DBL_TRESHOLD ;
		}
	};


	
	typedef std::vector<DbScrew*>  t_ScrewList;
	typedef std::vector<DbCutter*> t_CutterList;
	typedef std::vector<DbSleeve*> t_SleeveList;
    typedef std::map<QString, t_CutterList> t_ImplCtrMap;
		
	class DbGuideSeries : public Series
	{
		DbGuideProvider* m_pProvider;
		t_ScrewList	 m_Screws;
		t_CutterList m_Cutters;
		t_SleeveList m_Sleeves;
		t_ImplCtrMap m_ImCtrMap;
			
	public:
		DbGuideSeries() : m_pProvider(NULL) {}
        DbGuideSeries(DbGuideProvider* pProvider, const char* szName) : m_pProvider(pProvider)
		{
            strncpy(name, szName, NAME_SIZE);
            name[NAME_SIZE-1] = '\0';
		}
		virtual ~DbGuideSeries();
		DbGuideSeries& operator+=(const DbGuideSeries& other);
		bool IsImCtrMap()
		{
			return (m_ImCtrMap.size()!=0);
		}
		t_ImplCtrMap& GetImCtrMap()                                                                            
		{
			return m_ImCtrMap;
		}
        t_CutterList* GetMapCutters(QString impl_id)
		{
			if(m_ImCtrMap.find(impl_id)==m_ImCtrMap.end())
				return NULL;
			return &(m_ImCtrMap[impl_id]);
		}

		DbGuideProvider& GetProvider() const
		{
			return *m_pProvider;
		}
		const t_ScrewList& GetScrews() const                                                                           
		{
			return m_Screws;
		}
		const t_CutterList& GetCutters() const                                                                           
		{
			return m_Cutters;
		}
		const t_SleeveList& GetSleeves() const                                                                           
		{
			return m_Sleeves;
		}

		DbScrew&  AddScrew (const ScrewData&  data);
		DbCutter& AddCutter(const CutterData& data);
		DbSleeve& AddSleeve(const SleeveData& data);

		void RemoveScrew (const DbScrew& data);
		void RemoveCutter(const DbCutter& data);
		void RemoveSleeve(const DbSleeve& data);

		void RemoveScrew (unsigned long index);
		void RemoveCutter(unsigned long index);
		void RemoveSleeve(unsigned long index);

		bool operator==(const DbGuideSeries& rhs) const
		{
			return !strncmp(name, rhs.name, NAME_SIZE);
		}
	};

	typedef std::list<DbGuideSeries*> t_GuideSeriesList;

	class DbGuideProvider : public Provider
	{
	private:
		t_GuideSeriesList m_Series;

	public:
		DbGuideProvider() {}
        DbGuideProvider(const char* provName)
		{
            strncpy(name, provName, NAME_SIZE);
            name[NAME_SIZE-1] = '\0';
		}
		
		virtual ~DbGuideProvider();
		DbGuideProvider& operator+=(const DbGuideProvider& other);

        size_t SeriesCount() const
		{
			return m_Series.size();
		}

		t_GuideSeriesList& GetSeriesList() 
		{
			return m_Series;
		}
		template <typename predicate>
        enumerator<t_GuideSeriesList::iterator, predicate> GetSeriesEnumerator(predicate filter=db::no_filter<db::DbGuideSeries*>())
		{
			return enumerator<t_GuideSeriesList::iterator, predicate>(m_Series.begin(), m_Series.end(), filter);
		}


        DbGuideSeries&	AddSeries(const char* szSeriesName);
		void		RemoveSeries(const DbGuideSeries& data);

		bool operator==(const DbGuideProvider& rhs) const
		{
			return !strncmp(name, rhs.name, NAME_SIZE);
		}
	};

	typedef std::list<DbGuideProvider*> t_GuideProvidersList;

	class GuideDataBase
	{
		t_GuideProvidersList m_Providers;
		
	public:
		GuideDataBase();
		virtual ~GuideDataBase();

		
        static bool LoadXml(GuideDataBase& indb, const char* fileName);
        bool LoadXml(const char* fileName);
	
        static void SaveXml(const GuideDataBase& indb, const char* fileName);
        void SaveXml(const char* fileName);

		GuideDataBase& operator+=(const GuideDataBase& extraDB);

        size_t ProvidersCount() const
		{
			return m_Providers.size();
		}

		template <typename predicate>
        enumerator<t_GuideProvidersList::iterator, predicate> GetProvidersEnumerator(predicate filter = predicate())
		{
			return enumerator<t_GuideProvidersList::iterator, predicate>(m_Providers.begin(), m_Providers.end(), filter);
		}

        DbGuideProvider& AddProvider(const char* providerName);
		void RemoveProvider(const DbGuideProvider& data);

	public:
		template <typename predicate, typename comparer>
        enumerator<t_ScrewList::iterator, predicate>* GetScrewEnumerator(comparer compare, predicate filter=db::no_filter<db::DbScrew*>())
		{
			t_ScrewList *all_ptr = new t_ScrewList;
			t_GuideProvidersList::iterator prov_iter = m_Providers.begin();
			for (; prov_iter != m_Providers.end(); prov_iter++)
			{
				enumerator<t_GuideSeriesList::iterator > enumer = (*prov_iter)->GetSeriesEnumerator(db::no_filter<DbGuideSeries*>());
				while (enumer.MoveNext())
				{
					DbGuideSeries* pSeries = enumer.GetCurrent();
					const t_ScrewList& screws = pSeries->GetScrews();
					all_ptr->reserve(all_ptr->size() + screws.size());
					std::copy(screws.begin(), screws.end(), std::back_inserter(*all_ptr));
				}
			}

			std::sort(all_ptr->begin(), all_ptr->end(), compare);
            return new enumerator_ref2ptr<t_ScrewList, predicate>(all_ptr, filter);
		}

		template <typename predicate, typename comparer>
        enumerator<t_CutterList::iterator, predicate>* GetCutterEnumerator(comparer compare, predicate filter=db::no_filter<db::DbCutter*>())
		{
			t_CutterList *all_ptr = new t_CutterList;
			t_GuideProvidersList::iterator prov_iter = m_Providers.begin();
			for (; prov_iter != m_Providers.end(); prov_iter++)
			{
				enumerator<t_GuideSeriesList::iterator > enumer = (*prov_iter)->GetSeriesEnumerator(db::no_filter<DbGuideSeries*>());
				while (enumer.MoveNext())
				{
					DbGuideSeries* pSeries = enumer.GetCurrent();
					const t_CutterList& cutters = pSeries->GetCutters();
					all_ptr->reserve(all_ptr->size() + cutters.size());
					std::copy(cutters.begin(), cutters.end(), std::back_inserter(*all_ptr));
				}
			}

			std::sort(all_ptr->begin(), all_ptr->end(), compare);
            return new enumerator_ref2ptr<t_CutterList, predicate>(all_ptr, filter);
		}

		template <typename predicate, typename comparer>
        enumerator<t_SleeveList::iterator, predicate>*  GetSleeveEnumerator(comparer compare, predicate filter=db::no_filter<db::DbSleeve*>())
		{
			t_SleeveList *all_ptr = new t_SleeveList;
			t_GuideProvidersList::iterator prov_iter = m_Providers.begin();
			for (; prov_iter != m_Providers.end(); prov_iter++)
			{
				enumerator<t_GuideSeriesList::iterator > enumer = (*prov_iter)->GetSeriesEnumerator(db::no_filter<DbGuideSeries*>());
				while (enumer.MoveNext())
				{
					DbGuideSeries* pSeries = enumer.GetCurrent();
					const t_SleeveList& sleeves = pSeries->GetSleeves();
					all_ptr->reserve(all_ptr->size() + sleeves.size());
					std::copy(sleeves.begin(), sleeves.end(), std::back_inserter(*all_ptr));
				}
			}

			std::sort(all_ptr->begin(), all_ptr->end(), compare);
            return new enumerator_ref2ptr<t_SleeveList, predicate>(all_ptr, filter);
		}


	};
}
	
typedef db::enumerator<db::t_CutterList::iterator, db::no_filter<db::DbCutter*> > t_cuttersEnumNF;
//typedef boost::shared_ptr<t_cuttersEnumNF> t_cuttersEnumNFsp;
typedef t_cuttersEnumNF *t_cuttersEnumNFsp;

typedef db::enumerator<db::t_SleeveList::iterator, db::no_filter<db::DbSleeve*> > t_sleevesEnumNF;
//typedef boost::shared_ptr<t_sleevesEnumNF> t_sleevesEnumNFsp;
typedef t_sleevesEnumNF *t_sleevesEnumNFsp;

typedef db::enumerator<db::t_ScrewList::iterator, db::no_filter<db::DbScrew*> > t_screwsEnumNF;
//typedef boost::shared_ptr<t_screwsEnumNF> t_screwsEnumNFsp;
typedef t_screwsEnumNF *t_screwsEnumNFsp;

//typedef db::enumerator<db::t_GuideSeriesList::iterator, db::no_filter<db::DbGuideSeries*> > t_guide_seriesEnumNF;
//typedef boost::shared_ptr<t_guide_seriesEnumNF> t_guide_seriesEnumNFsp;

#endif //__GUIDE_DATABASE_H__
