#include "StdAfx.h"
#include "GuideHelper.h"

GuideHelper::GuideHelper(void)
{
}

TiXmlElement* GuideHelper::ToXml(const db::GuideDataBase& indb, unsigned long flags)
{
	TiXmlElement * root = new TiXmlElement( "database" );
	db::GuideDataBase* pDB= const_cast<db::GuideDataBase*>(&indb);

    db::enumerator<db::t_GuideProvidersList::iterator> providersEnum = pDB->GetProvidersEnumerator(db::no_filter<db::DbGuideProvider*>());
	while (providersEnum.MoveNext())
	{
		db::DbGuideProvider* pProvider = providersEnum.GetCurrent();
		TiXmlElement* elProvider = GuideHelper::ToXml(pProvider, flags);
		if (elProvider != NULL)
		{
			root->LinkEndChild( elProvider );
		}
	}
	return root;
}

TiXmlElement* GuideHelper::ToXml(const db::DbGuideProvider* provider, unsigned long flags)
{
	db::DbGuideProvider* pProvider = const_cast<db::DbGuideProvider*>(provider);

	bool bNotEmpty = false;
	bool bWithoutCondition = (flags == 0);

	TiXmlElement* elProvider = new TiXmlElement( "provider" );
	TiXmlElement* elName = new TiXmlElement( "name" );
	elName->LinkEndChild( new TiXmlText( pProvider->name ));
	elProvider->LinkEndChild(elName);
	db::enumerator<db::t_GuideSeriesList::iterator> seriesEnum = pProvider->GetSeriesEnumerator(db::no_filter<db::DbGuideSeries*>());
	while (seriesEnum.MoveNext())
	{
		db::DbGuideSeries* pSeries = seriesEnum.GetCurrent();
		TiXmlElement* elSeries = GuideHelper::ToXml(pSeries, flags);
		if (elSeries != NULL)
		{
			elProvider->LinkEndChild( elSeries );
			bNotEmpty = true;
		}
	}
	if (bWithoutCondition || bNotEmpty)
	{
		return elProvider;
	}
	else
	{
		delete elProvider;
		return NULL;
	}
}

TiXmlElement* GuideHelper::ToXml(const db::DbGuideSeries* series, unsigned long flags)
{
	db::DbGuideSeries* pSeries = const_cast<db::DbGuideSeries*>(series);
	TiXmlElement* elSeries = new TiXmlElement( "series" );
	TiXmlElement* elName = new TiXmlElement( "name" );
	elName->LinkEndChild( new TiXmlText( pSeries->name ));
	elSeries->LinkEndChild(elName);

	bool bNotEmpty = false;
	bool bWithoutCondition = (flags == 0);
	bool condition = (flags == USERS_INTEMS);

	TiXmlElement* elScrews = new TiXmlElement( "screws" );
	const db::t_ScrewList& screwsList = pSeries->GetScrews();
	db::t_ScrewList::const_iterator it = screwsList.begin();
	for (; it != screwsList.end(); ++it)
	{
		db::ScrewData* pScrew = *it;
		if (bWithoutCondition || (condition == pScrew->editable))
		{
			TiXmlElement* elScrew = new TiXmlElement( "screw" );
			elScrew->LinkEndChild( MakeStringElement("name", pScrew->name));
			elScrew->LinkEndChild( MakeIntElement("state", (unsigned int)pScrew->state) );
			elScrew->LinkEndChild( MakeDoubleElement("D", pScrew->D) );
			elScrew->LinkEndChild( MakeDoubleElement("Dh", pScrew->Dh) );
			elScrew->LinkEndChild( MakeDoubleElement("L", pScrew->L) );
			elScrew->LinkEndChild( MakeDoubleElement("Lt", pScrew->Lt) );
			elScrew->LinkEndChild( MakeDoubleElement("Lh", pScrew->Lh) );
			elScrew->LinkEndChild( MakeStringElement("model", pScrew->szModelName));
			elScrew->LinkEndChild( MakeStringElement("article", pScrew->artikul));
			elScrew->LinkEndChild( MakeBoolElement("ed", pScrew->editable) );
			elScrew->LinkEndChild( MakeStringElement("color", pScrew->defcolor) );

			elScrews->LinkEndChild(elScrew);
			bNotEmpty = true;
		}
	}
	elSeries->LinkEndChild(elScrews);

	TiXmlElement* elCutters = new TiXmlElement( "cutters" );
	const db::t_CutterList& cutterList = pSeries->GetCutters();
	db::t_CutterList::const_iterator itCutter = cutterList.begin();
	for (; itCutter != cutterList.end(); ++itCutter)
	{
		db::CutterData* pCutter = *itCutter;
		if (bWithoutCondition || (condition == pCutter->editable))
		{
			TiXmlElement* elCutter = new TiXmlElement( "cutter" );
			elCutter->LinkEndChild(MakeStringElement("name", pCutter->name));
			elCutter->LinkEndChild( MakeIntElement("state", (unsigned int)pCutter->state) );
			elCutter->LinkEndChild( MakeDoubleElement("D", pCutter->D) );
			elCutter->LinkEndChild( MakeDoubleElement("Ds", pCutter->Ds) );
			elCutter->LinkEndChild( MakeDoubleElement("Dw", pCutter->Dw) );
			elCutter->LinkEndChild( MakeDoubleElement("L", pCutter->L) );
			elCutter->LinkEndChild( MakeDoubleElement("Ls", pCutter->Ls) );
			elCutter->LinkEndChild( MakeDoubleElement("Lw", pCutter->Lw) );
			elCutter->LinkEndChild( MakeIntElement("type", pCutter->type) );
			elCutter->LinkEndChild( MakeStringElement("model", pCutter->szModelName));
			elCutter->LinkEndChild( MakeStringElement("article", pCutter->artikul));
			elCutter->LinkEndChild( MakeBoolElement("ed", pCutter->editable) );
			elCutter->LinkEndChild( MakeStringElement("color", pCutter->defcolor) );
			elCutters->LinkEndChild(elCutter);
			bNotEmpty = true;
		}
	}
	elSeries->LinkEndChild(elCutters);

	TiXmlElement* elSleeves = new TiXmlElement( "sleeves" );
	const db::t_SleeveList& sleeveList = pSeries->GetSleeves();
	db::t_SleeveList::const_iterator itSleeve = sleeveList.begin();
	for (; itSleeve != sleeveList.end(); ++itSleeve)
	{
		db::SleeveData* pSleeve = *itSleeve;
		if (bWithoutCondition || (condition == pSleeve->editable))
		{
			TiXmlElement* elSleeve = new TiXmlElement( "sleeve" );
			elSleeve->LinkEndChild(MakeStringElement("name", pSleeve->name));
			elSleeve->LinkEndChild( MakeIntElement("state", (unsigned int)pSleeve->state) );
			elSleeve->LinkEndChild( MakeDoubleElement("D", pSleeve->D) );
			elSleeve->LinkEndChild( MakeDoubleElement("Do", pSleeve->Do) );
			elSleeve->LinkEndChild( MakeDoubleElement("Dh", pSleeve->Dh) );
			elSleeve->LinkEndChild( MakeDoubleElement("Df", pSleeve->Df) );
			elSleeve->LinkEndChild( MakeDoubleElement("Lw", pSleeve->Lw) );
			elSleeve->LinkEndChild( MakeDoubleElement("Lh", pSleeve->Lh) );
			elSleeve->LinkEndChild( MakeStringElement("model", pSleeve->szModelName));
			elSleeve->LinkEndChild( MakeStringElement("article", pSleeve->artikul));
			elSleeve->LinkEndChild( MakeBoolElement("ed", pSleeve->editable) );
			elSleeve->LinkEndChild( MakeStringElement("color", pSleeve->defcolor) );
			TiXmlElement* elSegments = GuideHelper::ToXml(pSleeve);
			if (elSegments != NULL)
			{
				elSleeve->LinkEndChild( elSegments );
			}

			elSleeves->LinkEndChild(elSleeve);
			bNotEmpty = true;
		}
	}
	elSeries->LinkEndChild(elSleeves);

	if (bWithoutCondition || bNotEmpty)
	{
		return elSeries;
	}
	else
	{
		delete elSeries;
		return NULL;
	}
}

TiXmlElement* GuideHelper::ToXml(const db::ProductionBase* prodbase)
{
	if (prodbase->Segments.size() > 0)
	{
		TiXmlElement* elSegments = new TiXmlElement( "segments" );
		std::vector<db::Segment>::const_iterator itSegment = prodbase->Segments.begin();
		for (; itSegment != prodbase->Segments.end(); ++itSegment)
		{
			db::Segment segment = *itSegment;
			TiXmlElement* elSegment = new TiXmlElement( "segment" );
			elSegment->LinkEndChild(MakeDoubleElement("D1", segment.D1));
			elSegment->LinkEndChild(MakeDoubleElement("D2", segment.D2));
			elSegment->LinkEndChild(MakeDoubleElement("L", segment.L));
			
			elSegments->LinkEndChild(elSegment);
		}

		return elSegments;
	}
	return NULL;
}

TiXmlElement* GuideHelper::MakeStringElement( const char *name, const char * _value )
{
	TiXmlElement* element = new TiXmlElement( name );
	element->LinkEndChild( new TiXmlText( _value ));
	return element;
}

TiXmlElement* GuideHelper::MakeIntElement( const char *name, int _value )
{
	char buf [64];
	#if defined(TIXML_SNPRINTF)
		TIXML_SNPRINTF(buf, sizeof(buf), "%d", _value);
	#else
		sprintf (buf, "%d", _value);
	#endif
	TiXmlElement* element = new TiXmlElement( name );
	element->LinkEndChild( new TiXmlText( buf ));
	return element;
}

TiXmlElement* GuideHelper::MakeDoubleElement( const char *name, double _value )
{
	char buf [256];
	#if defined(TIXML_SNPRINTF)
		TIXML_SNPRINTF( buf, sizeof(buf), "%g", _value);
	#else
		sprintf (buf, "%g", _value);
	#endif
	TiXmlElement* element = new TiXmlElement( name );
	element->LinkEndChild( new TiXmlText( buf ));
	return element;
}


TiXmlElement* GuideHelper::MakeBoolElement( const char *name, bool _value )
{
	TiXmlElement* element = new TiXmlElement( name );
	element->LinkEndChild( new TiXmlText( _value ? "1" : "0" ));
	return element;
}


void GuideHelper::ParseDataBase(db::GuideDataBase& indb, const char* fileName, const ticpp::Element* element, unsigned long flags)
{
	ticpp::Iterator<ticpp::Element> child;
	ticpp::Element* pProvider = element->FirstChildElement("provider", false);
	std::string attrStr = pProvider->GetAttribute(std::string("ext_file"));
	if(!attrStr.empty())
	{
		indb.LoadXml(attrStr.c_str());
		return;
	}
	for( pProvider; pProvider; pProvider=pProvider->NextSiblingElement("provider", false))
	{
        std::string strName = ParseStringValue(pProvider, "name");
		db::DbGuideProvider& newProvider = indb.AddProvider(strName.c_str());
		ParseProvider(newProvider, *pProvider, flags);
	}
}

void GuideHelper::ParseProvider(db::DbGuideProvider& provider, const ticpp::Element& element, unsigned long flags)
{
	ticpp::Element* pSeries = element.FirstChildElement("series", false);
	for( pSeries; pSeries; pSeries=pSeries->NextSiblingElement("series", false))
	{
        std::string strName = ParseStringValue(pSeries, "name");
		db::DbGuideSeries& series = provider.AddSeries(strName.c_str());
		ParseSeries(series, *pSeries, flags);
	}
}

void GuideHelper::ParseSeries(db::DbGuideSeries& series, const ticpp::Element& element, unsigned long flags)
{
	bool bWithoutCondition = (flags == 0);
	bool condition = (flags == USERS_INTEMS);

	ticpp::Element* pScrews = element.FirstChildElement("screws", false);
	if (pScrews != NULL)
	{
		ticpp::Element* pScrew = pScrews->FirstChildElement("screw", false);
		for( pScrew; pScrew; pScrew=pScrew->NextSiblingElement("screw", false))
		{
			bool editable = ParseBoolValue(pScrew, "ed");
			if (bWithoutCondition || (condition == editable))
			{
				db::ScrewData screwData;
				ParseStringValue(pScrew, "name", screwData.name, NAME_SIZE);
				screwData.state = (db::ObjState::State)ParseIntValue(pScrew, "state");
				screwData.D = ParseDoubleValue(pScrew, "D");
				screwData.Dh = ParseDoubleValue(pScrew, "Dh");
				screwData.L = ParseDoubleValue(pScrew, "L");
				screwData.Lh = ParseDoubleValue(pScrew, "Lh");
				screwData.Lt = ParseDoubleValue(pScrew, "Lt");
				ParseStringValue(pScrew, "model", screwData.szModelName, _MAX_PATH);
				ParseStringValue(pScrew, "article", screwData.artikul, ARTIKUL_SIZE);
				screwData.editable = editable;
				ParseStringValue(pScrew, "color", screwData.defcolor, ARTIKUL_SIZE);

				db::DbScrew& implant = series.AddScrew(screwData);
			}
		}
	}
	ticpp::Element* pCutters = element.FirstChildElement("cutters", false);
	if (pCutters != NULL)
	{
		ticpp::Element* pCutter = pCutters->FirstChildElement("cutter", false);
		for( pCutter; pCutter; pCutter=pCutter->NextSiblingElement("cutter", false))
		{
			bool editable = ParseBoolValue(pCutter, "ed");
			if (bWithoutCondition || (condition == editable))
			{
				db::CutterData data;
				ParseStringValue(pCutter, "name", data.name, NAME_SIZE);
				data.state = (db::ObjState::State)ParseIntValue(pCutter, "state");
				data.D = ParseDoubleValue(pCutter, "D");
				data.Ds = ParseDoubleValue(pCutter, "Ds");
				data.Dw = ParseDoubleValue(pCutter, "Dw");
				data.L = ParseDoubleValue(pCutter, "L");
				data.Ls = ParseDoubleValue(pCutter, "Ls");
				data.Lw = ParseDoubleValue(pCutter, "Lw");
				data.type = (db::CutterType)ParseIntValue(pCutter, "type");
				ParseStringValue(pCutter, "model", data.szModelName, _MAX_PATH);
				ParseStringValue(pCutter, "article", data.artikul, ARTIKUL_SIZE);
				data.editable = editable;
				ParseStringValue(pCutter, "color", data.defcolor, ARTIKUL_SIZE);
				db::DbCutter& cutter = series.AddCutter(data);
			}
		}
	}
	ticpp::Element* pSleeves = element.FirstChildElement("sleeves", false);
	if (pSleeves != NULL)
	{
		ticpp::Element* pSleeve = pSleeves->FirstChildElement("sleeve", false);
		for( pSleeve; pSleeve; pSleeve=pSleeve->NextSiblingElement("sleeve", false))
		{
			bool editable = ParseBoolValue(pSleeve, "ed");
			if (bWithoutCondition || (condition == editable))
			{
				db::SleeveData data;
				ParseStringValue(pSleeve, "name", data.name, NAME_SIZE);
				data.state = (db::ObjState::State)ParseIntValue(pSleeve, "state");
				data.D = ParseDoubleValue(pSleeve, "D");
				data.Do = ParseDoubleValue(pSleeve, "Do");
				data.Dh = ParseDoubleValue(pSleeve, "Dh");
				data.Df = ParseDoubleValue(pSleeve, "Df");
				data.Lw = ParseDoubleValue(pSleeve, "Lw");
				data.Lh = ParseDoubleValue(pSleeve, "Lh");
				ParseStringValue(pSleeve, "model", data.szModelName, _MAX_PATH);
				ParseStringValue(pSleeve, "article", data.artikul, ARTIKUL_SIZE);
				ParseSegments(data, pSleeve);
				data.editable = editable;
				ParseStringValue(pSleeve, "color", data.defcolor, ARTIKUL_SIZE);
				db::DbSleeve& sleeve = series.AddSleeve(data);
			}
		}
	}
	ticpp::Element* pICMap = element.FirstChildElement("impl_ctrs_map", false);
	if (pICMap != NULL)
	{
		ticpp::Element* pPair = pICMap->FirstChildElement("pair", false);
		for( pPair; pPair; pPair=pPair->NextSiblingElement("pair", false))
		{
			std::string impl_article= ParseStringValue(pPair, "impl");
			std::string cutr_article= ParseStringValue(pPair, "ctr");
			const db::t_CutterList& cutterList = series.GetCutters();
			db::t_CutterList::const_iterator itCutter = cutterList.begin();
			for (; itCutter != cutterList.end(); ++itCutter)
			{
				db::DbCutter* pCtr = *itCutter;
				db::t_ImplCtrMap& icmap = series.GetImCtrMap();
				if( cutr_article==std::string(pCtr->artikul))
				{
					icmap[impl_article].push_back(pCtr);
					break;
				}
			}
			
		}
	}
}

void GuideHelper::ParseSegments(db::ProductionBase& prodbase, const ticpp::Element* element)
{
	ticpp::Element* pSegments = element->FirstChildElement("segments", false);
	if (pSegments != NULL)
	{
		ticpp::Element* pSegment = pSegments->FirstChildElement("segment", false);
		if (pSegment != NULL)
		{
			prodbase.Segments.clear();
			for( pSegment; pSegment; pSegment=pSegment->NextSiblingElement("segment", false))
			{
				db::Segment data;
				data.D1 = ParseDoubleValue(pSegment, "D1");
				data.D2 = ParseDoubleValue(pSegment, "D2");
				data.L = ParseDoubleValue(pSegment, "L");
				prodbase.Segments.push_back(data);
			}
		}
	}
}


void GuideHelper::ParseOldDataBase(db::GuideDataBase& indb, const ticpp::Element* element)
{
	ticpp::Element* pDataBase = element->FirstChildElement("database", false);
	if (pDataBase == NULL)
		pDataBase = element->FirstChildElement("indb", false);
	if (pDataBase != NULL)
	{
		ticpp::Element* pProviders = pDataBase->FirstChildElement("providers", false);
		if (pProviders != NULL)
		{
			ticpp::Element* pProvider = pProviders->FirstChildElement("item", false);
			for( pProvider; pProvider; pProvider=pProvider->NextSiblingElement("item", false))
			{
				std::string strName = ParseStringValue(pProvider, "name");
				db::DbGuideProvider& newProvider = indb.AddProvider(strName.c_str());
				ParseOldProvider(newProvider, *pProvider);
			}
		}
	}
}

void GuideHelper::ParseOldProvider(db::DbGuideProvider& provider, const ticpp::Element& element)
{
	ticpp::Element* pSeriesRoot = element.FirstChildElement("series", false);
	ticpp::Element* pSeries = pSeriesRoot->FirstChildElement("item", false);
	for( pSeries; pSeries; pSeries=pSeries->NextSiblingElement("item", false))
	{
        std::string strName = ParseStringValue(pSeries, "name");
		db::DbGuideSeries& series = provider.AddSeries(strName.c_str());
		ParseOldSeries(series, *pSeries);
	}
}

void GuideHelper::ParseOldSeries(db::DbGuideSeries& series, const ticpp::Element& element)
{
	ticpp::Element* pScrews = element.FirstChildElement("screws", false);
	if (pScrews != NULL)
	{
		ticpp::Element* pScrew = pScrews->FirstChildElement("item", false);
		for( pScrew; pScrew; pScrew=pScrew->NextSiblingElement("item", false))
		{
			db::ScrewData screwData;
			ParseStringValue(pScrew, "name", screwData.name, NAME_SIZE);
			screwData.state = (db::ObjState::State)ParseIntValue(pScrew, "state");
			screwData.D = ParseDoubleValue(pScrew, "D");
			screwData.Dh = ParseDoubleValue(pScrew, "Dh");
			screwData.L = ParseDoubleValue(pScrew, "L");
			screwData.Lh = ParseDoubleValue(pScrew, "Lh");
			screwData.Lt = ParseDoubleValue(pScrew, "Lt");
			ParseStringValue(pScrew, "model", screwData.szModelName, _MAX_PATH);
			ParseStringValue(pScrew, "article", screwData.artikul, ARTIKUL_SIZE);
			screwData.editable = ParseBoolValue(pScrew, "ed");

			db::DbScrew& implant = series.AddScrew(screwData);
		}
	}
	ticpp::Element* pCutters = element.FirstChildElement("cutters", false);
	if (pCutters != NULL)
	{
		ticpp::Element* pCutter = pCutters->FirstChildElement("item", false);
		for( pCutter; pCutter; pCutter=pCutter->NextSiblingElement("item", false))
		{
			db::CutterData data;
			ParseStringValue(pCutter, "name", data.name, NAME_SIZE);
			data.state = (db::ObjState::State)ParseIntValue(pCutter, "state");
			data.D = ParseDoubleValue(pCutter, "D");
			data.Ds = ParseDoubleValue(pCutter, "Ds");
			data.Dw = ParseDoubleValue(pCutter, "Dw");
			data.L = ParseDoubleValue(pCutter, "L");
			data.Ls = ParseDoubleValue(pCutter, "Ls");
			data.Lw = ParseDoubleValue(pCutter, "Lw");
			ParseStringValue(pCutter, "model", data.szModelName, _MAX_PATH);
			ParseStringValue(pCutter, "article", data.artikul, ARTIKUL_SIZE);
			data.editable = ParseBoolValue(pCutter, "ed");
			db::DbCutter& cutter = series.AddCutter(data);
		}
	}
	ticpp::Element* pSleeves = element.FirstChildElement("sleeves", false);
	if (pSleeves != NULL)
	{
		ticpp::Element* pSleeve = pSleeves->FirstChildElement("item", false);
		for( pSleeve; pSleeve; pSleeve=pSleeve->NextSiblingElement("item", false))
		{
			db::SleeveData data;
			ParseStringValue(pSleeve, "name", data.name, NAME_SIZE);
			data.state = (db::ObjState::State)ParseIntValue(pSleeve, "state");
			data.D = ParseDoubleValue(pSleeve, "D");
			data.Do = ParseDoubleValue(pSleeve, "Do");
			data.Dh = ParseDoubleValue(pSleeve, "Dh");
			data.Df = ParseDoubleValue(pSleeve, "Df");
			data.Lw = ParseDoubleValue(pSleeve, "Lw");
			data.Lh = ParseDoubleValue(pSleeve, "Lh");
			ParseStringValue(pSleeve, "model", data.szModelName, _MAX_PATH);
			ParseStringValue(pSleeve, "article", data.artikul, ARTIKUL_SIZE);
			ParseOldSegments(data, pSleeve);
			data.editable = ParseBoolValue(pSleeve, "ed");
			db::DbSleeve& sleeve = series.AddSleeve(data);
		}
	}
}

void GuideHelper::ParseOldSegments(db::ProductionBase& prodbase, const ticpp::Element* element)
{
	ticpp::Element* pSegments = element->FirstChildElement("segments", false);
	if (pSegments != NULL)
	{
		ticpp::Element* pSegment = pSegments->FirstChildElement("item", false);
		if (pSegment != NULL)
		{
			prodbase.Segments.clear();
			for( pSegment; pSegment; pSegment=pSegment->NextSiblingElement("item", false))
			{
				db::Segment data;
				data.D1 = ParseDoubleValue(pSegment, "D1");
				data.D2 = ParseDoubleValue(pSegment, "D2");
				data.L = ParseDoubleValue(pSegment, "L");
				prodbase.Segments.push_back(data);
			}
		}
	}
}



std::string GuideHelper::ParseStringValue( ticpp::Element* element, const char *name )
{
	ticpp::Element* el = element->FirstChildElement(name, false);
	std::string svalue;
	el->GetText(&svalue, false);
	return svalue;
}

void GuideHelper::ParseStringValue( ticpp::Element* element, const char *name, char* field, size_t length )
{
	try
	{
		ticpp::Element* el = element->FirstChildElement(name, false);
		std::string svalue;
		el->GetText(&svalue, false);
		if (!svalue.empty())
		{
            strncpy(field, svalue.c_str(), length);
            field[length-1] = '\0';
		}
	}
    catch(ticpp::Exception& ex)
    {
		std::cout << "error parse string value:" << ex.m_details << std::endl;
    }
	catch(...)
    {
		std::cerr << "unknown error" ;
		return ;
    }
}

int GuideHelper::ParseIntValue( ticpp::Element* element, const char *name )
{
	try 
	{
		ticpp::Element* el = element->FirstChildElement(name, false);
		int ivalue;
		el->GetText(&ivalue);
		return ivalue;
	}
	catch(ticpp::Exception& ex)
    {
		std::cerr << "error parse int value:" << ex.m_details << std::endl;
		return 0;
    }
	catch(...)
    {
		std::cerr << "unknown error" ;
		return 0;
    }
}

double GuideHelper::ParseDoubleValue( ticpp::Element* element, const char *name )
{
	try
	{
		ticpp::Element* el = element->FirstChildElement(name, false);
		double dvalue;
		el->GetText(&dvalue);
		return dvalue;
	}
	catch(ticpp::Exception& ex)
    {
		std::cout << "error parse double value:" << ex.m_details << std::endl;
		return 0.0;
    }
	catch(...)
    {
		std::cerr << "unknown error" ;
		return 0.;
    }
}

bool GuideHelper::ParseBoolValue( ticpp::Element* element, const char *name )
{
	try
	{
		ticpp::Element* el = element->FirstChildElement(name, false);
		bool bvalue;
		el->GetText(&bvalue);
		return bvalue;
	}
	catch(ticpp::Exception& ex)
    {
		std::cout << "error parse bool value:" << ex.m_details << std::endl;
		return false;
    }
	catch(...)
    {
		std::cerr << "unknown error" ;
		return false;
    }
}
