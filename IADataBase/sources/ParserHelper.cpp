#include "StdAfx.h"
#include "ParserHelper.h"
#include "Enumerator.h"

ParserHelper::ParserHelper(void)
{
}

void ParserHelper::ParseDataBase(db::IADataBase& indb, const char* fileName, const ticpp::Element* element, unsigned long flags)
{
	ticpp::Iterator<ticpp::Element> child;
	ticpp::Element* pProvider = element->FirstChildElement("provider", false);

	for( pProvider; pProvider; pProvider=pProvider->NextSiblingElement("provider", false))
	{
		std::string attrStr = pProvider->GetAttribute(std::string("ext_file"));
		if(!attrStr.empty())
		{
			std::string extFileName = fileName;
			int pathpos1 = extFileName.find_last_of("\\");
			int pathpos2 = extFileName.find_last_of("\/");
			pathpos1 = (pathpos1>pathpos2)?pathpos1:pathpos2;
			extFileName.erase(pathpos1+1,-1);
			extFileName.append( attrStr.c_str());
			indb.LoadXml_2(extFileName.c_str(), flags|PROVIDER_EXT_FILE);
			continue;
		}

        std::string strName = ParseStringValue(pProvider, "name");
		db::DbProvider& newProvider = indb.AddProvider(strName.c_str());
		if(flags & PROVIDER_EXT_FILE)
		{
			std::string extFileName = fileName;
			//int pathpos1 = extFileName.find_last_of("\\");
			//int pathpos2 = extFileName.find_last_of("\/");
			//pathpos1 = (pathpos1>pathpos2)?pathpos1:pathpos2;
			//extFileName.erase(0,pathpos1+1);
			
			strcpy_s(newProvider.m_szExtFile, NAME_SIZE, extFileName.c_str());
		}
		else if(!(flags&USERS_INTEMS))
			strcpy_s(newProvider.m_szExtFile, NAME_SIZE, "");

		ParseProvider(newProvider, *pProvider, flags);
	}
}

void ParserHelper::ParseProvider(db::DbProvider& provider, const ticpp::Element& element, unsigned long flags)
{
	
	ticpp::Element* pSeries = element.FirstChildElement("series", false);
	for( pSeries; pSeries; pSeries=pSeries->NextSiblingElement("series", false))
	{
        std::string strName = ParseStringValue(pSeries, "name");
		db::DbSeries& series = provider.AddSeries(strName.c_str());
		ParseSeries(series, *pSeries, flags);
	}
}

void ParserHelper::ParseSeries(db::DbSeries& series, const ticpp::Element& element, unsigned long flags)
{
	bool bWithoutCondition = (flags == 0);
	bool condition = (flags == USERS_INTEMS);

	ticpp::Element* pImplants = element.FirstChildElement("implants", false);
	if (pImplants != NULL)
	{
		ticpp::Element* pImplant = pImplants->FirstChildElement("implant", false);
		for( pImplant; pImplant; pImplant=pImplant->NextSiblingElement("implant", false))
		{
			bool editable = ParseBoolValue(pImplant, "ed");
			if (bWithoutCondition || (condition == editable))
			{
				db::ImplantData implData;
				ParseStringValue(pImplant, "name", implData.name, NAME_SIZE);
				implData.state = (db::ObjState::State)ParseIntValue(pImplant, "state");
				implData.D1 = ParseDoubleValue(pImplant, "cyl-diam");
				implData.L1 = ParseDoubleValue(pImplant, "length");
				implData.D2 = ParseDoubleValue(pImplant, "cyl-diam2");
				implData.L2 = ParseDoubleValue(pImplant, "length2");
				ParseStringValue(pImplant, "model-file", implData.szModelName, _MAX_PATH);
				ParseStringValue(pImplant, "article", implData.artikul, ARTIKUL_SIZE);
				implData.Le = ParseDoubleValue(pImplant, "len-e");
				implData.editable = editable;
				ParseStringValue(pImplant, "comp_val", implData.szCompatibility, _MAX_PATH);
				ParseStringValue(pImplant, "color", implData.defcolor, ARTIKUL_SIZE);
				db::DbImplant& implant = series.AddImplant(implData);
			}
		}
	}
	ticpp::Element* pAbutments = element.FirstChildElement("abutments", false);
	if (pAbutments != NULL)
	{
		ticpp::Element* pAbutment = pAbutments->FirstChildElement("abutment", false);
		for( pAbutment; pAbutment; pAbutment=pAbutment->NextSiblingElement("abutment", false))
		{
			bool editable = ParseBoolValue(pAbutment, "ed");
			if (bWithoutCondition || (condition == editable))
			{
				db::AbutmentData data;
				ParseStringValue(pAbutment, "name", data.name, NAME_SIZE);
				data.state = (db::ObjState::State)ParseIntValue(pAbutment, "state");
				data.D1 = ParseDoubleValue(pAbutment, "cyl-diam");
				data.L1 = ParseDoubleValue(pAbutment, "length");
				data.Alpha = ParseDoubleValue(pAbutment, "alpha");
				ParseStringValue(pAbutment, "model-file", data.szModelName, _MAX_PATH);
				ParseStringValue(pAbutment, "article", data.artikul, ARTIKUL_SIZE);
				data.editable = editable;
				ParseStringValue(pAbutment, "comp_val", data.szCompatibility, _MAX_PATH);
				ParseStringValue(pAbutment, "color", data.defcolor, ARTIKUL_SIZE);
				db::DbAbutment& abutment = series.AddAbutment(data);
			}
		}
	}
	// востанавливаем из пользовательского только если m_CompSer пустой 
	if (series.m_CompSer.empty()) 
	{
		ticpp::Element* pCompSeries = element.FirstChildElement("comp_ser", false);
		if (pCompSeries != NULL)
		{
			ticpp::Element* pSeries = pCompSeries->FirstChildElement("Series", false);
			for( pSeries; pSeries; pSeries=pSeries->NextSiblingElement("Series", false))
			{
				std::string strProducer = pSeries->GetAttribute("Producer");
				std::string strValue = pSeries->GetText();
				db::CompatibleSeries data;
                strncpy(data.prov, strProducer.c_str(), NAME_SIZE);
                data.prov[NAME_SIZE-1] = '\0';
                strncpy(data.ser, strValue.c_str(), NAME_SIZE);
                data.ser[NAME_SIZE-1] = '\0';
				series.m_CompSer.push_back(data);
			}
		}
	}

}

void ParserHelper::ParseOldDataBase(db::IADataBase& indb, const ticpp::Element* element)
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
				db::DbProvider& newProvider = indb.AddProvider(strName.c_str());
				ParseOldProvider(newProvider, *pProvider);
			}
		}
	}
}

void ParserHelper::ParseOldProvider(db::DbProvider& provider, const ticpp::Element& element)
{
	ticpp::Element* pSeriesRoot = element.FirstChildElement("series", false);
	ticpp::Element* pSeries = pSeriesRoot->FirstChildElement("item", false);
	for( pSeries; pSeries; pSeries=pSeries->NextSiblingElement("item", false))
	{
        std::string strName = ParseStringValue(pSeries, "name");
		db::DbSeries& series = provider.AddSeries(strName.c_str());
		ParseOldSeries(series, *pSeries);
	}
}

void ParserHelper::ParseOldSeries(db::DbSeries& series, const ticpp::Element& element)
{
	ticpp::Element* pImplants = element.FirstChildElement("implants", false);
	if (pImplants != NULL)
	{
		ticpp::Element* pImplant = pImplants->FirstChildElement("item", false);
		for( pImplant; pImplant; pImplant=pImplant->NextSiblingElement("item", false))
		{
			db::ImplantData implData;
			ParseStringValue(pImplant, "name", implData.name, NAME_SIZE);
			implData.state = (db::ObjState::State)ParseIntValue(pImplant, "state");
			implData.D1 = ParseDoubleValue(pImplant, "cyl-diam");
			implData.L1 = ParseDoubleValue(pImplant, "length");
			implData.D2 = ParseDoubleValue(pImplant, "cyl-diam2");
			implData.L2 = ParseDoubleValue(pImplant, "length2");
			ParseStringValue(pImplant, "model-file", implData.szModelName, _MAX_PATH);
			ParseStringValue(pImplant, "article", implData.artikul, ARTIKUL_SIZE);
			implData.Le = ParseDoubleValue(pImplant, "len-e");
			implData.editable = ParseBoolValue(pImplant, "ed");

			db::DbImplant& implant = series.AddImplant(implData);
		}
	}
	ticpp::Element* pAbutments = element.FirstChildElement("abutments", false);
	if (pAbutments != NULL)
	{
		ticpp::Element* pAbutment = pAbutments->FirstChildElement("item", false);
		for( pAbutment; pAbutment; pAbutment=pAbutment->NextSiblingElement("item", false))
		{
			db::AbutmentData data;
			ParseStringValue(pAbutment, "name", data.name, NAME_SIZE);
			data.state = (db::ObjState::State)ParseIntValue(pAbutment, "state");
			data.D1 = ParseDoubleValue(pAbutment, "cyl-diam");
			data.L1 = ParseDoubleValue(pAbutment, "length");
			data.Alpha = ParseDoubleValue(pAbutment, "alpha");
			ParseStringValue(pAbutment, "model-file", data.szModelName, _MAX_PATH);
			ParseStringValue(pAbutment, "article", data.artikul, ARTIKUL_SIZE);
			data.editable = ParseBoolValue(pAbutment, "ed");
			db::DbAbutment& abutment = series.AddAbutment(data);
		}
	}
	ticpp::Element* pCompSeries = element.FirstChildElement("comp_ser", false);
	if (pCompSeries != NULL)
	{
		ticpp::Element* pSeries = pCompSeries->FirstChildElement("item", false);
		for( pSeries; pSeries; pSeries=pSeries->NextSiblingElement("item", false))
		{
			db::CompatibleSeries data;
			ParseStringValue(pSeries, "Producer", data.prov, NAME_SIZE);
			ParseStringValue(pSeries, "Series", data.ser, NAME_SIZE);
			series.m_CompSer.push_back(data);
		}
	}
}

TiXmlElement* ParserHelper::ToXml(const db::IADataBase& indb, const char* fileName, unsigned long flags)
{
	TiXmlElement * root = new TiXmlElement( "database" );
	db::IADataBase* pDB= const_cast<db::IADataBase*>(&indb);

    db::enumerator<db::t_ProvidersList::iterator> providersEnum = pDB->GetProvidersEnumerator(db::no_filter<db::DbProvider*>());
	while (providersEnum.MoveNext())
	{
		db::DbProvider* pProvider = providersEnum.GetCurrent();

		// check if it was in external file
		if( (!(strlen(pProvider->m_szExtFile)==0))&&
			(!(flags&PROVIDER_EXT_FILE))&&
			(!(flags&USERS_INTEMS)) )
		{
			pDB->SaveXml_2( pProvider->m_szExtFile, flags|PROVIDER_EXT_FILE );

			TiXmlElement* elProvider = new TiXmlElement( "provider" );
			//if(flags&PROVIDER_EXT_FILE)
			{
				std::string extFileName = pProvider->m_szExtFile;
				int pathpos1 = extFileName.find_last_of("\\");
				int pathpos2 = extFileName.find_last_of("\/");
				pathpos1 = (pathpos1>pathpos2)?pathpos1:pathpos2;
				extFileName.erase(0,pathpos1+1);

				elProvider->SetAttribute("ext_file", extFileName.c_str());
				root->LinkEndChild( elProvider );
			
			}
		}
		else if ((flags&PROVIDER_EXT_FILE)&&(0==strcmp(fileName, pProvider->m_szExtFile)))
		{
			TiXmlElement* elProvider = ParserHelper::ToXml(pProvider, flags);
			if (elProvider != NULL)
			{
				root->LinkEndChild( elProvider );
			}
			break;
		}
		else
		{
			TiXmlElement* elProvider = ParserHelper::ToXml(pProvider, flags);
			if (elProvider != NULL)
			{
				root->LinkEndChild( elProvider );
			}
		}
	}
	return root;
}

TiXmlElement* ParserHelper::ToXml(const db::DbProvider* provider, unsigned long flags)
{
	db::DbProvider* pProvider = const_cast<db::DbProvider*>(provider);

	bool bNotEmpty = false;
	bool bWithoutCondition = (flags == 0);

	db::enumerator<db::t_SeriesList::iterator> seriesEnum = pProvider->GetSeriesEnumerator(db::no_filter<db::DbSeries*>());

	TiXmlElement* elProvider = new TiXmlElement( "provider" );
	//if(flags&PROVIDER_EXT_FILE)
	//{
	//	std::string extFileName = provider->m_szExtFile;
	//	int pathpos1 = extFileName.find_last_of("\\");
	//	int pathpos2 = extFileName.find_last_of("\/");
	//	pathpos1 = (pathpos1>pathpos2)?pathpos1:pathpos2;
	//	extFileName.erase(0,pathpos1+1);

	//	elProvider->SetAttribute("ext_file", extFileName.c_str());
	//	return elProvider;
	//}

	TiXmlElement* elName = new TiXmlElement( "name" );
	elName->LinkEndChild( new TiXmlText( pProvider->name ));
	elProvider->LinkEndChild(elName);
	
	while (seriesEnum.MoveNext())
	{
		db::DbSeries* pSeries = seriesEnum.GetCurrent();
		TiXmlElement* elSeries = ParserHelper::ToXml(pSeries, flags);
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

TiXmlElement* ParserHelper::ToXml(const db::DbSeries* series, unsigned long flags)
{
	db::DbSeries* pSeries = const_cast<db::DbSeries*>(series);
	TiXmlElement* elSeries = new TiXmlElement( "series" );
	TiXmlElement* elName = new TiXmlElement( "name" );
	elName->LinkEndChild( new TiXmlText( pSeries->name ));
	elSeries->LinkEndChild(elName);

	bool bNotEmpty = false;
	bool bWithoutCondition = (flags == 0);
	bool condition = (flags == USERS_INTEMS);

	TiXmlElement* elImplants = new TiXmlElement( "implants" );
	const db::t_ImplantList& impList = pSeries->GetImplants();
	db::t_ImplantList::const_iterator it = impList.begin();
	for (; it != impList.end(); ++it)
	{
		db::ImplantData* pImplant = *it;
		if (bWithoutCondition || (condition == pImplant->editable))
		{
			TiXmlElement* elImplant = new TiXmlElement( "implant" );
			elImplant->LinkEndChild( MakeStringElement("name", pImplant->name));
			elImplant->LinkEndChild( MakeIntElement("state", (unsigned int)pImplant->state) );
			elImplant->LinkEndChild( MakeDoubleElement("cyl-diam", pImplant->D1) );
			elImplant->LinkEndChild( MakeDoubleElement("length", pImplant->L1) );
			elImplant->LinkEndChild( MakeDoubleElement("cyl-diam2", pImplant->D2) );
			elImplant->LinkEndChild( MakeDoubleElement("length2", pImplant->L2) );
			elImplant->LinkEndChild( MakeStringElement("model-file", pImplant->szModelName));
			elImplant->LinkEndChild( MakeStringElement("article", pImplant->artikul));
			elImplant->LinkEndChild( MakeDoubleElement("len-e", pImplant->Le));
			elImplant->LinkEndChild( MakeBoolElement("ed", pImplant->editable) );
			elImplant->LinkEndChild( MakeStringElement("comp_val", pImplant->szCompatibility) );
			elImplant->LinkEndChild( MakeStringElement("color", pImplant->defcolor) );

			elImplants->LinkEndChild(elImplant);
			bNotEmpty = true;
		}
	}
	elSeries->LinkEndChild(elImplants);

	TiXmlElement* elAbutments = new TiXmlElement( "abutments" );
	const db::t_AbutmentList& abutmentList = pSeries->GetAbutment();
	db::t_AbutmentList::const_iterator itAbutment = abutmentList.begin();
	for (; itAbutment != abutmentList.end(); ++itAbutment)
	{
		db::AbutmentData* pAbutment = *itAbutment;
		if (bWithoutCondition || (condition == pAbutment->editable))
		{
			TiXmlElement* elAbutment = new TiXmlElement( "abutment" );
			elAbutment->LinkEndChild(MakeStringElement("name", pAbutment->name));
			elAbutment->LinkEndChild( MakeIntElement("state", (unsigned int)pAbutment->state) );
			elAbutment->LinkEndChild( MakeDoubleElement("cyl-diam", pAbutment->D1) );
			elAbutment->LinkEndChild( MakeDoubleElement("length", pAbutment->L1) );
			elAbutment->LinkEndChild( MakeDoubleElement("alpha", pAbutment->Alpha ) );
			elAbutment->LinkEndChild( MakeStringElement("model-file", pAbutment->szModelName));
			elAbutment->LinkEndChild( MakeStringElement("article", pAbutment->artikul));
			elAbutment->LinkEndChild( MakeBoolElement("ed", pAbutment->editable) );
			elAbutment->LinkEndChild( MakeStringElement("comp_val", pAbutment->szCompatibility) );
			elAbutment->LinkEndChild( MakeStringElement("color", pAbutment->defcolor) );

			elAbutments->LinkEndChild(elAbutment);
			bNotEmpty = true;
		}
	}
	elSeries->LinkEndChild(elAbutments);

	TiXmlElement* elCompSer = new TiXmlElement( "comp_ser" );
	std::vector<db::CompatibleSeries>::const_iterator it3 = pSeries->m_CompSer.begin();
	for (; it3 != pSeries->m_CompSer.end(); ++it3)
	{
		db::CompatibleSeries pSer = *it3;
		TiXmlElement* elSer = new TiXmlElement( "Series" );
		elSer->SetAttribute("Producer", pSer.prov);
		elSer->LinkEndChild( new TiXmlText( pSer.ser ));
		elCompSer->LinkEndChild(elSer);
	}
	elSeries->LinkEndChild(elCompSer);

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

std::string ParserHelper::ParseStringValue( ticpp::Element* element, const char *name )
{
	ticpp::Element* el = element->FirstChildElement(name, false);
	std::string svalue;
	el->GetText(&svalue, false);
	return svalue;
}

void ParserHelper::ParseStringValue( ticpp::Element* element, const char *name, char* field, size_t length )
{
	try
	{
		ticpp::Element* el = element->FirstChildElement(name, false);
		if(el)
		{
			std::string svalue;
			el->GetText(&svalue, false);
			if (!svalue.empty())
			{
                strncpy(field, svalue.c_str(), length);
                field[length-1] = '\0';
			}
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

int ParserHelper::ParseIntValue( ticpp::Element* element, const char *name )
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

double ParserHelper::ParseDoubleValue( ticpp::Element* element, const char *name )
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

bool ParserHelper::ParseBoolValue( ticpp::Element* element, const char *name )
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

TiXmlElement* ParserHelper::MakeStringElement( const char *name, const char * _value )
{
	TiXmlElement* element = new TiXmlElement( name );
	element->LinkEndChild( new TiXmlText( _value ));
	return element;
}

TiXmlElement* ParserHelper::MakeIntElement( const char *name, int _value )
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

TiXmlElement* ParserHelper::MakeDoubleElement( const char *name, double _value )
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


TiXmlElement* ParserHelper::MakeBoolElement( const char *name, bool _value )
{
	TiXmlElement* element = new TiXmlElement( name );
	element->LinkEndChild( new TiXmlText( _value ? "1" : "0" ));
	return element;
}
