#include "stdafx.h"
//#pragma warning( disable : 4267 )

#include "GuideDataBase.h"
#include "ext_resources.h"
//#include "ticpp.h"
#include "GuideHelper.h"
#include <QFile>

#include <iostream>

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
	// GuideDataBase members
	GuideDataBase::GuideDataBase()
	{}

	GuideDataBase::~GuideDataBase()
	{
		std::for_each(m_Providers.begin(), m_Providers.end(), delete_ptr());
		m_Providers.clear();
	}


    DbGuideProvider& GuideDataBase::AddProvider(const char* providerName)
	{
		DbGuideProvider* pProv = new DbGuideProvider(providerName);
		t_GuideProvidersList::iterator it = std::find_if(m_Providers.begin(), m_Providers.end(), find_ptr<DbGuideProvider>(*pProv));
		if (it == m_Providers.end())
		{
			m_Providers.push_back(pProv);
			return *pProv;
		}
		
		return *(*it);
	}

	void GuideDataBase::RemoveProvider(const DbGuideProvider& data)
	{
		find_ptr<DbGuideProvider> pred(data);
		t_GuideProvidersList::iterator it = std::find_if(m_Providers.begin(), m_Providers.end(), pred);
		if (it != m_Providers.end())
		{
			delete *it;
			m_Providers.erase(it);
		}
	}

    bool GuideDataBase::LoadXml(const char* fileName)
	{
		return LoadXml(*this, fileName);
	}
    bool GuideDataBase::LoadXml(GuideDataBase& indb, const char* fileName)
	{
		bool loaded = false;
        QDomDocument Doc;
        try
        {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                return false;
            }
            if(!Doc.setContent(&file))
            {
                file.close();
                return false;
            }
            file.close();
            QDomElement root = Doc.firstChildElement();
            QString v = root.tagName();
			if (v == "boost_serialization")
				GuideHelper::ParseOldDataBase(indb, root);
			else
				GuideHelper::ParseDataBase(indb, fileName, root);
            loaded = true;
        }
        catch (...)
        {
            std::cout << "unknown error " << std::endl;
        }
		return loaded;
	}

    void GuideDataBase::SaveXml(const char* fileName)
	{
		SaveXml(*this, fileName);
	}

    void GuideDataBase::SaveXml(const GuideDataBase& indb, const char* fileName)
	{
        QDomDocument doc;
        QDomProcessingInstruction xmlDeclaration = doc.createProcessingInstruction("xml", "version=\"1.0\" ");
        doc.appendChild(xmlDeclaration);
        doc.appendChild( *(GuideHelper::ToXml(indb, doc)) );

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        doc.save(out, 4);

        file.close();
	}

	GuideDataBase& GuideDataBase::operator+=(const GuideDataBase& extraDB)
	{
		//???
		GuideDataBase* pDb = const_cast<GuideDataBase*>(&extraDB);
		enumerator<t_GuideProvidersList::iterator> providersEnum = pDb->GetProvidersEnumerator(no_filter<DbGuideProvider*>());
		while (providersEnum.MoveNext())
		{
			DbGuideProvider* pProvider = providersEnum.GetCurrent();
			DbGuideProvider& newProvider = AddProvider(pProvider->name);
			newProvider += *pProvider;
		}
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	// Provider Methods
	DbGuideProvider::~DbGuideProvider()
	{
		std::for_each(m_Series.begin(), m_Series.end(), delete_ptr());
	}

    DbGuideSeries& DbGuideProvider::AddSeries(const char* szSeriesName)
	{
		DbGuideSeries* pSeries = new DbGuideSeries(this, szSeriesName);
		t_GuideSeriesList::iterator it = std::find_if(m_Series.begin(), m_Series.end(), find_ptr<DbGuideSeries>(*pSeries));
		if (it == m_Series.end())
		{
			m_Series.push_back(pSeries);
			return *pSeries;
		}

		return *(*it);
	}

	void DbGuideProvider::RemoveSeries(const DbGuideSeries& data)
	{
		t_GuideSeriesList::iterator it = std::find_if(m_Series.begin(), m_Series.end(), find_ptr<DbGuideSeries>(data));
		if (it != m_Series.end())
		{
			delete *it;
			m_Series.erase(it);
		}
	}
	DbGuideProvider& DbGuideProvider::operator+=(const DbGuideProvider& other)
	{
		//???
		DbGuideProvider* pOther= const_cast<DbGuideProvider*>(&other);
		enumerator<t_GuideSeriesList::iterator> seriesEnum = pOther->GetSeriesEnumerator(no_filter<DbGuideSeries*>());
		while (seriesEnum.MoveNext())
		{
			DbGuideSeries* pSeries = seriesEnum.GetCurrent();
			DbGuideSeries& newSeries = AddSeries(pSeries->name);
			newSeries += *pSeries;
		}
		return *this;
	}

	DbGuideSeries::~DbGuideSeries()
	{
		std::for_each(m_Screws.begin(),  m_Screws.end(),  delete_ptr());
		std::for_each(m_Cutters.begin(), m_Cutters.end(), delete_ptr());
		std::for_each(m_Sleeves.begin(), m_Sleeves.end(), delete_ptr());
	}

	DbScrew& DbGuideSeries::AddScrew(const ScrewData& data)
	{
		DbScrew* pScrew = new DbScrew(this);
		m_Screws.push_back(pScrew);
		(*pScrew) = data;
		return *pScrew;
	}
	DbCutter& DbGuideSeries::AddCutter(const CutterData& data)
	{
		DbCutter* pCutter = new DbCutter(this);
		m_Cutters.push_back(pCutter);
		(*pCutter) = data;
		return *pCutter;
	}
	DbSleeve& DbGuideSeries::AddSleeve(const SleeveData& data)
	{
		DbSleeve* pSleeve = new DbSleeve(this);
		m_Sleeves.push_back(pSleeve);
		(*pSleeve) = data;
		return *pSleeve;
	}

	void DbGuideSeries::RemoveScrew(const DbScrew& data)
	{
		t_ScrewList::iterator it = std::find_if(m_Screws.begin(), m_Screws.end(), find_ptr<DbScrew>(data));
		if (it != m_Screws.end())
		{
			delete *it;
			m_Screws.erase(it);
		}
	}
	void DbGuideSeries::RemoveScrew(unsigned long index)
	{
        if (index >= m_Screws.size())
			return;
		t_ScrewList::iterator it = m_Screws.begin() + index;
		delete *it;
		m_Screws.erase(it);
	}

	void DbGuideSeries::RemoveCutter(const DbCutter& data)
	{
		t_CutterList::iterator it = std::find_if(m_Cutters.begin(), m_Cutters.end(), find_ptr<DbCutter>(data));
		if (it != m_Cutters.end())
		{
			delete *it;
			m_Cutters.erase(it);
		}
	}

	void DbGuideSeries::RemoveCutter(unsigned long index)
	{
        if (index >= m_Cutters.size())
			return;
		t_CutterList::iterator it = m_Cutters.begin() + index;
		delete *it;
		m_Cutters.erase(it);
	}

	void DbGuideSeries::RemoveSleeve(const DbSleeve& data)
	{
		t_SleeveList::iterator it = std::find_if(m_Sleeves.begin(), m_Sleeves.end(), find_ptr<DbSleeve>(data));
		if (it != m_Sleeves.end())
		{
			delete *it;
			m_Sleeves.erase(it);
		}
	}

	void DbGuideSeries::RemoveSleeve(unsigned long index)
	{
        if (index >= m_Sleeves.size())
			return;
		t_SleeveList::iterator it = m_Sleeves.begin() + index;
		delete *it;
		m_Sleeves.erase(it);
	}

	DbGuideSeries& DbGuideSeries::operator+=(const DbGuideSeries& other)
	{
		const t_ScrewList& newSc = other.GetScrews();
		t_ScrewList::const_iterator itS = newSc.begin();
		for (; itS != newSc.end(); ++itS)
		{
			t_ScrewList::iterator fnd = std::find_if(
				m_Screws.begin(), 
				m_Screws.end(),
				equal_ptr_obj<DbScrew>(*itS));
			if (fnd == m_Screws.end())
				AddScrew(*(*itS));
 			else
				(*fnd)->operator=(*(*itS));
		}

		const t_CutterList& newCt = other.GetCutters();
		t_CutterList::const_iterator itC = newCt.begin();
		for (; itC != newCt.end(); ++itC)
		{
			t_CutterList::iterator fnd = std::find_if(
				m_Cutters.begin(), 
				m_Cutters.end(),
				equal_ptr_obj<DbCutter>(*itC));
			if (fnd == m_Cutters.end())
				AddCutter(*(*itC));
			else
				(*fnd)->operator=(*(*itC));
		}

		const t_SleeveList& newSl = other.GetSleeves();
		t_SleeveList::const_iterator itSl = newSl.begin();
		for (; itSl != newSl.end(); ++itSl)
		{
			t_SleeveList::iterator fnd = std::find_if(
				m_Sleeves.begin(), 
				m_Sleeves.end(),
				equal_ptr_obj<DbSleeve>(*itSl));
			if (fnd == m_Sleeves.end())
				AddSleeve(*(*itSl));
			else
				(*fnd)->operator=(*(*itSl));
		}
		return *this;

	}


} // end namespace db
