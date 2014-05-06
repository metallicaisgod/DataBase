#include "StdAfx.h"
#include "GuideHelper.h"

GuideHelper::GuideHelper(void)
{
}

QDomElement* GuideHelper::ToXml(const db::GuideDataBase& indb, QDomDocument& document, unsigned long flags)
{
    QDomElement * root = new QDomElement();
    *root = document.createElement("database" );
	db::GuideDataBase* pDB= const_cast<db::GuideDataBase*>(&indb);

    db::enumerator<db::t_GuideProvidersList::iterator> providersEnum = pDB->GetProvidersEnumerator(db::no_filter<db::DbGuideProvider*>());
	while (providersEnum.MoveNext())
	{
		db::DbGuideProvider* pProvider = providersEnum.GetCurrent();
        QDomElement * elProvider = GuideHelper::ToXml(pProvider, document, flags);
        if (elProvider && !elProvider->isNull())
		{
            root->appendChild(*elProvider );
		}
	}
	return root;
}

QDomElement* GuideHelper::ToXml(const db::DbGuideProvider* provider, QDomDocument& document, unsigned long flags)
{
	db::DbGuideProvider* pProvider = const_cast<db::DbGuideProvider*>(provider);

	bool bNotEmpty = false;
	bool bWithoutCondition = (flags == 0);

    QDomElement * elProvider = new QDomElement();
    *elProvider = document.createElement( "provider" );
    QDomElement elName = document.createElement( "name" );
    QDomText name = document.createTextNode(pProvider->name);
    elName.appendChild(name);// new TiXmlText( pProvider->name ));
    elProvider->appendChild(elName);
	db::enumerator<db::t_GuideSeriesList::iterator> seriesEnum = pProvider->GetSeriesEnumerator(db::no_filter<db::DbGuideSeries*>());
	while (seriesEnum.MoveNext())
	{
		db::DbGuideSeries* pSeries = seriesEnum.GetCurrent();
        QDomElement * elSeries = GuideHelper::ToXml(pSeries, document, flags);
        if (elSeries && !elSeries->isNull())
		{
            elProvider->appendChild( *elSeries );
			bNotEmpty = true;
		}
	}
	if (bWithoutCondition || bNotEmpty)
	{
		return elProvider;
	}
	else
	{
        //delete elProvider;
        return NULL;
	}
}

QDomElement* GuideHelper::ToXml(const db::DbGuideSeries* series, QDomDocument& document, unsigned long flags)
{
	db::DbGuideSeries* pSeries = const_cast<db::DbGuideSeries*>(series);
    QDomElement * elSeries = new QDomElement();
    *elSeries = document.createElement ( "series" );
    QDomElement elName = document.createElement( "name" );
    QDomText serName = document.createTextNode(pSeries->name);
    elName.appendChild(serName);//LinkEndChild( new TiXmlText( pSeries->name ));
    elSeries->appendChild(elName);

	bool bNotEmpty = false;
	bool bWithoutCondition = (flags == 0);
	bool condition = (flags == USERS_INTEMS);

    QDomElement elScrews = document.createElement( "screws" );
	const db::t_ScrewList& screwsList = pSeries->GetScrews();
	db::t_ScrewList::const_iterator it = screwsList.begin();
	for (; it != screwsList.end(); ++it)
	{
		db::ScrewData* pScrew = *it;
		if (bWithoutCondition || (condition == pScrew->editable))
		{
            QDomElement elScrew =  document.createElement( "screw" );
            AddElement("name", pScrew->name, document, elScrew);
            AddElement("state", (unsigned int)pScrew->state, document, elScrew);
            AddElement("D", pScrew->D, document, elScrew);
            AddElement("Dh", pScrew->Dh, document, elScrew);
            AddElement("L", pScrew->L, document, elScrew);
            AddElement("Lt", pScrew->Lt, document, elScrew);
            AddElement("Lh", pScrew->Lh, document, elScrew);
            AddElement("model", pScrew->szModelName, document, elScrew);
            AddElement("article", pScrew->artikul, document, elScrew);
            AddElement("ed", pScrew->editable, document, elScrew);
            AddElement("color", pScrew->defcolor, document, elScrew);
//            elScrew.appendChild( MakeIntElement("state", (unsigned int)pScrew->state, document) );
//            elScrew.appendChild( MakeDoubleElement("D", pScrew->D, document) );
//            elScrew.appendChild( MakeDoubleElement("Dh", pScrew->Dh, document) );
//            elScrew.appendChild( MakeDoubleElement("L", pScrew->L, document) );
//            elScrew.appendChild( MakeDoubleElement("Lt", pScrew->Lt, document) );
//            elScrew.appendChild( MakeDoubleElement("Lh", pScrew->Lh, document) );
//            elScrew.appendChild( MakeStringElement("model", pScrew->szModelName, document));
//            elScrew.appendChild( MakeStringElement("article", pScrew->artikul, document));
//            elScrew.appendChild( MakeBoolElement("ed", pScrew->editable, document) );
//            elScrew.appendChild( MakeStringElement("color", pScrew->defcolor, document) );

            elScrews.appendChild(elScrew);
			bNotEmpty = true;
		}
	}
    elSeries->appendChild(elScrews);

    QDomElement elCutters = document.createElement( "cutters" );
	const db::t_CutterList& cutterList = pSeries->GetCutters();
	db::t_CutterList::const_iterator itCutter = cutterList.begin();
	for (; itCutter != cutterList.end(); ++itCutter)
	{
		db::CutterData* pCutter = *itCutter;
		if (bWithoutCondition || (condition == pCutter->editable))
		{
            QDomElement elCutter = document.createElement( "cutter" );
            AddElement("name", pCutter->name, document, elCutter);
            AddElement("state", (unsigned int)pCutter->state, document, elCutter);
            AddElement("D", pCutter->D, document, elCutter);
            AddElement("Ds", pCutter->Ds, document, elCutter);
            AddElement("Dw", pCutter->Dw, document, elCutter);
            AddElement("L", pCutter->L, document, elCutter);
            AddElement("Ls", pCutter->Ls, document, elCutter);
            AddElement("Lw", pCutter->Lw, document, elCutter);
            AddElement("type", pCutter->type, document, elCutter);
            AddElement("model", pCutter->szModelName, document, elCutter);
            AddElement("article", pCutter->artikul, document, elCutter);
            AddElement("ed", pCutter->editable, document, elCutter);
            AddElement("color", pCutter->defcolor, document, elCutter);
//            elCutter.appendChild(MakeStringElement("name", pCutter->name, document));
//            elCutter.appendChild( MakeIntElement("state", (unsigned int)pCutter->state, document) );
//            elCutter.appendChild( MakeDoubleElement("D", pCutter->D, document) );
//            elCutter.appendChild( MakeDoubleElement("Ds", pCutter->Ds, document) );
//            elCutter.appendChild( MakeDoubleElement("Dw", pCutter->Dw, document) );
//            elCutter.appendChild( MakeDoubleElement("L", pCutter->L, document) );
//            elCutter.appendChild( MakeDoubleElement("Ls", pCutter->Ls, document) );
//            elCutter.appendChild( MakeDoubleElement("Lw", pCutter->Lw, document) );
//            elCutter.appendChild( MakeIntElement("type", pCutter->type, document) );
//            elCutter.appendChild( MakeStringElement("model", pCutter->szModelName, document));
//            elCutter.appendChild( MakeStringElement("article", pCutter->artikul, document));
//            elCutter.appendChild( MakeBoolElement("ed", pCutter->editable, document) );
//            elCutter.appendChild( MakeStringElement("color", pCutter->defcolor, document) );
            elCutters.appendChild(elCutter);
			bNotEmpty = true;
		}
	}
    elSeries->appendChild(elCutters);

    QDomElement elSleeves = document.createElement( "sleeves" );
	const db::t_SleeveList& sleeveList = pSeries->GetSleeves();
	db::t_SleeveList::const_iterator itSleeve = sleeveList.begin();
	for (; itSleeve != sleeveList.end(); ++itSleeve)
	{
		db::SleeveData* pSleeve = *itSleeve;
		if (bWithoutCondition || (condition == pSleeve->editable))
		{
            QDomElement elSleeve = document.createElement( "sleeve" );
            AddElement("name", pSleeve->name, document, elSleeve);
            AddElement("state", (unsigned int)pSleeve->state, document, elSleeve);
            AddElement("D", pSleeve->D, document, elSleeve);
            AddElement("Do", pSleeve->Do, document, elSleeve);
            AddElement("Dh", pSleeve->Dh, document, elSleeve);
            AddElement("Df", pSleeve->Df, document, elSleeve);
            AddElement("Lw", pSleeve->Lw, document, elSleeve);
            AddElement("Lh", pSleeve->Lh, document, elSleeve);
            AddElement("model", pSleeve->szModelName, document, elSleeve);
            AddElement("article", pSleeve->artikul, document, elSleeve);
            AddElement("ed", pSleeve->editable, document, elSleeve);
            AddElement("color", pSleeve->defcolor, document, elSleeve);
//            elSleeve.appendChild(MakeStringElement("name", pSleeve->name, document));
//            elSleeve.appendChild( MakeIntElement("state", (unsigned int)pSleeve->state, document) );
//            elSleeve.appendChild( MakeDoubleElement("D", pSleeve->D, document) );
//            elSleeve.appendChild( MakeDoubleElement("Do", pSleeve->Do, document) );
//            elSleeve.appendChild( MakeDoubleElement("Dh", pSleeve->Dh, document) );
//            elSleeve.appendChild( MakeDoubleElement("Df", pSleeve->Df, document) );
//            elSleeve.appendChild( MakeDoubleElement("Lw", pSleeve->Lw, document) );
//            elSleeve.appendChild( MakeDoubleElement("Lh", pSleeve->Lh, document) );
//            elSleeve.appendChild( MakeStringElement("model", pSleeve->szModelName, document));
//            elSleeve.appendChild( MakeStringElement("article", pSleeve->artikul, document));
//            elSleeve.appendChild( MakeBoolElement("ed", pSleeve->editable, document) );
//            elSleeve.appendChild( MakeStringElement("color", pSleeve->defcolor, document) );
            QDomElement* elSegments = GuideHelper::ToXml(pSleeve, document);
            if (elSegments && !elSegments->isNull())
			{
                elSleeve.appendChild( *elSegments );
			}

            elSleeves.appendChild(elSleeve);
			bNotEmpty = true;
		}
	}
    elSeries->appendChild(elSleeves);

	if (bWithoutCondition || bNotEmpty)
	{
		return elSeries;
	}
	else
	{
        //delete elSeries;
        return NULL;
	}
}

QDomElement* GuideHelper::ToXml(const db::ProductionBase* prodbase, QDomDocument& document)
{
	if (prodbase->Segments.size() > 0)
	{
        QDomElement * elSegments = new QDomElement();
        *elSegments = document.createElement( "segments" );
		std::vector<db::Segment>::const_iterator itSegment = prodbase->Segments.begin();
		for (; itSegment != prodbase->Segments.end(); ++itSegment)
		{
			db::Segment segment = *itSegment;
            QDomElement elSegment = document.createElement( "segment" );
            AddElement("D1", segment.D1, document, elSegment);
            AddElement("D2", segment.D2, document, elSegment);
            AddElement("L", segment.L, document, elSegment);
//            elSegment.appendChild(MakeDoubleElement("D1", segment.D1, document));
//            elSegment.appendChild(MakeDoubleElement("D2", segment.D2, document));
//            elSegment.appendChild(MakeDoubleElement("L", segment.L, document));
			
            elSegments->appendChild(elSegment);
		}

		return elSegments;
	}
    return NULL;
}


template <typename T>
void GuideHelper::AddElement(const char *name, T _value, QDomDocument document, QDomElement parent)
{
    //QDomElement * element = new QDomElement();
    QDomElement element = document.createElement(QString(name));
    element.appendChild( document.createTextNode(QString("%1").arg( _value )));
    parent.appendChild(element);
    //return element;
}

//QDomElement* GuideHelper::MakeStringElement( const char *name, const char * _value, QDomDocument& document )
//{
//    QDomElement* element = new QDomElement();
//    *element = document.createElement( name );
//    element->appendChild( document.createTextNode(QString("%1").arg( _value )));
//	return element;
//}

//QDomElement* GuideHelper::MakeIntElement( const char *name, int _value, QDomDocument& document )
//{
////	char buf [64];
////	#if defined(TIXML_SNPRINTF)
////		TIXML_SNPRINTF(buf, sizeof(buf), "%d", _value);
////	#else
////		sprintf (buf, "%d", _value);
////	#endif
////	QDomElement* element = new QDomElement( name );
////	element.appendChild( new TiXmlText( buf ));
////	return element;
//    QDomElement* element = new QDomElement();
//    *element = document.createElement( name );
//    element->appendChild( document.createTextNode(QString("%1").arg( _value )));
//	return element;
//}

//QDomElement* GuideHelper::MakeDoubleElement( const char *name, double _value, QDomDocument& document )
//{
////	char buf [256];
////	#if defined(TIXML_SNPRINTF)
////		TIXML_SNPRINTF( buf, sizeof(buf), "%g", _value);
////	#else
////		sprintf (buf, "%g", _value);
////	#endif
////	QDomElement* element = new QDomElement( name );
////	element.appendChild( new TiXmlText( buf ));
////	return element;
//    QDomElement* element = new QDomElement();
//    *element = document.createElement( name );
//    element->appendChild( document.createTextNode(QString("%1").arg( _value )));
//	return element;
//}


//QDomElement* GuideHelper::MakeBoolElement( const char *name, bool _value, QDomDocument& document )
//{
////	QDomElement* element = new QDomElement( name );
////	element.appendChild( new TiXmlText( _value ? "1" : "0" ));
////	return element;
//    QDomElement* element = new QDomElement();
//    *element = document.createElement( name );
//    element->appendChild( document.createTextNode(QString("%1").arg( _value )));
//	return element;
//}


void GuideHelper::ParseDataBase(db::GuideDataBase& indb, const char* fileName, const QDomElement& element, unsigned long flags)
{
    //ticpp::Iterator<ticpp::Element> child;
    QDomElement Provider = element.firstChildElement("provider");
    //QDomElement pProvider = element.firstChildElement("provider", false);
    QString attrStr = Provider.attribute("ext_file");
    if(!attrStr.isEmpty())
	{
        indb.LoadXml(attrStr.toLocal8Bit().data());
		return;
	}
    for( Provider; !Provider.isNull(); Provider=Provider.nextSiblingElement("provider"))
	{
        QString strName = ParseStringValue(Provider, "name");
        db::DbGuideProvider& newProvider = indb.AddProvider(strName.toLocal8Bit().data());
        ParseProvider(newProvider, Provider, flags);
	}
}

void GuideHelper::ParseProvider(db::DbGuideProvider& provider, const QDomElement& element, unsigned long flags)
{
    QDomElement pSeries = element.firstChildElement("series");
    for( pSeries; !pSeries.isNull(); pSeries=pSeries.nextSiblingElement("series"))
	{
        QString strName = ParseStringValue(pSeries, "name");
        db::DbGuideSeries& series = provider.AddSeries(strName.toLocal8Bit().data());
        ParseSeries(series, pSeries, flags);
	}
}

void GuideHelper::ParseSeries(db::DbGuideSeries& series, const QDomElement& element, unsigned long flags)
{
	bool bWithoutCondition = (flags == 0);
	bool condition = (flags == USERS_INTEMS);

    QDomElement pScrews = element.firstChildElement("screws");
    if (!pScrews.isNull())
	{
        QDomElement pScrew = pScrews.firstChildElement("screw");
        for( pScrew; !pScrew.isNull(); pScrew=pScrew.nextSiblingElement("screw"))
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
    QDomElement pCutters = element.firstChildElement("cutters");
    if (!pCutters.isNull())
	{
        QDomElement pCutter = pCutters.firstChildElement("cutter");
        for( pCutter; !pCutter.isNull(); pCutter=pCutter.nextSiblingElement("cutter"))
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
    QDomElement pSleeves = element.firstChildElement("sleeves");
    if (!pSleeves.isNull())
	{
        QDomElement pSleeve = pSleeves.firstChildElement("sleeve");
        for( pSleeve; !pSleeve.isNull(); pSleeve=pSleeve.nextSiblingElement("sleeve"))
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
    QDomElement pICMap = element.firstChildElement("impl_ctrs_map");
    if (!pICMap.isNull())
	{
        QDomElement pPair = pICMap.firstChildElement("pair");
        for( pPair; !pPair.isNull(); pPair=pPair.nextSiblingElement("pair"))
		{
            QString impl_article= ParseStringValue(pPair, "impl");
            QString cutr_article= ParseStringValue(pPair, "ctr");
			const db::t_CutterList& cutterList = series.GetCutters();
			db::t_CutterList::const_iterator itCutter = cutterList.begin();
			for (; itCutter != cutterList.end(); ++itCutter)
			{
				db::DbCutter* pCtr = *itCutter;
				db::t_ImplCtrMap& icmap = series.GetImCtrMap();
                if( cutr_article==QString(pCtr->artikul))
				{
					icmap[impl_article].push_back(pCtr);
					break;
				}
			}
			
		}
	}
}

void GuideHelper::ParseSegments(db::ProductionBase& prodbase, const QDomElement& element)
{
    QDomElement pSegments = element.firstChildElement("segments");
    if (!pSegments.isNull())
	{
        QDomElement pSegment = pSegments.firstChildElement("segment");
        if (!pSegment.isNull())
		{
			prodbase.Segments.clear();
            for( pSegment; !pSegment.isNull(); pSegment=pSegment.nextSiblingElement("segment"))
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


void GuideHelper::ParseOldDataBase(db::GuideDataBase& indb, const QDomElement& element)
{
    QDomElement pDataBase = element.firstChildElement("database");
    if (pDataBase.isNull())
        pDataBase = element.firstChildElement("indb");
    if (!pDataBase.isNull())
	{
        QDomElement pProviders = pDataBase.firstChildElement("providers");
        if (!pProviders.isNull())
		{
            QDomElement pProvider = pProviders.firstChildElement("item");
            for( pProvider; !pProvider.isNull(); pProvider=pProvider.nextSiblingElement("item"))
			{
                QString strName = ParseStringValue(pProvider, "name");
                db::DbGuideProvider& newProvider = indb.AddProvider(strName.toLocal8Bit().data());
                ParseOldProvider(newProvider, pProvider);
			}
		}
	}
}

void GuideHelper::ParseOldProvider(db::DbGuideProvider& provider, const QDomElement& element)
{
    QDomElement pSeriesRoot = element.firstChildElement("series");
    QDomElement pSeries = pSeriesRoot.firstChildElement("item");
    for( pSeries; !pSeries.isNull(); pSeries=pSeries.nextSiblingElement("item"))
	{
        QString strName = ParseStringValue(pSeries, "name");
        db::DbGuideSeries& series = provider.AddSeries(strName.toLocal8Bit().data());
        ParseOldSeries(series, pSeries);
	}
}

void GuideHelper::ParseOldSeries(db::DbGuideSeries& series, const QDomElement& element)
{
    QDomElement pScrews = element.firstChildElement("screws");
    if (!pScrews.isNull())
	{
        QDomElement pScrew = pScrews.firstChildElement("item");
        for( pScrew; !pScrew.isNull(); pScrew=pScrew.nextSiblingElement("item"))
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
    QDomElement pCutters = element.firstChildElement("cutters");
    if (!pCutters.isNull())
	{
        QDomElement pCutter = pCutters.firstChildElement("item");
        for( pCutter; !pCutter.isNull(); pCutter=pCutter.nextSiblingElement("item"))
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
    QDomElement pSleeves = element.firstChildElement("sleeves");
    if (!pSleeves.isNull())
	{
        QDomElement pSleeve = pSleeves.firstChildElement("item");
        for( pSleeve; !pSleeve.isNull(); pSleeve=pSleeve.nextSiblingElement("item"))
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

void GuideHelper::ParseOldSegments(db::ProductionBase& prodbase, const QDomElement& element)
{
    QDomElement pSegments = element.firstChildElement("segments");
    if (!pSegments.isNull())
	{
        QDomElement pSegment = pSegments.firstChildElement("item");
        if (!pSegment.isNull())
		{
			prodbase.Segments.clear();
            for( pSegment; !pSegment.isNull(); pSegment=pSegment.nextSiblingElement("item"))
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



QString GuideHelper::ParseStringValue( QDomElement element, const char *name )
{
    QDomElement el = element.firstChildElement(name);
    QString string;
    if(!el.isNull())
        string = el.text();
    return string;
}

void GuideHelper::ParseStringValue( QDomElement element, const char *name, char* field, size_t length )
{
	try
	{
        QDomElement el = element.firstChildElement(name);
        QString string;
        if(!el.isNull())
            string = el.text();
        if (!string.isEmpty())
		{
            strncpy(field, string.toLocal8Bit().data(), length);
            field[length-1] = '\0';
		}
	}
//    catch(ticpp::Exception& ex)
//    {
//		std::cout << "error parse string value:" << ex.m_details << std::endl;
//    }
	catch(...)
    {
		std::cerr << "unknown error" ;
		return ;
    }
}

int GuideHelper::ParseIntValue( QDomElement element, const char *name )
{
	try 
	{
        QDomElement el = element.firstChildElement(name);
		int ivalue;
        if(!el.isNull())
            ivalue = el.text().toInt();
		return ivalue;
	}
//	catch(ticpp::Exception& ex)
//    {
//		std::cerr << "error parse int value:" << ex.m_details << std::endl;
//		return 0;
//    }
	catch(...)
    {
		std::cerr << "unknown error" ;
		return 0;
    }
}

double GuideHelper::ParseDoubleValue( QDomElement element, const char *name )
{
	try
	{
        QDomElement el = element.firstChildElement(name);
		double dvalue;
        if(!el.isNull())
            dvalue = el.text().toDouble();
		return dvalue;
	}
//	catch(ticpp::Exception& ex)
//    {
//		std::cout << "error parse double value:" << ex.m_details << std::endl;
//		return 0.0;
//    }
	catch(...)
    {
		std::cerr << "unknown error" ;
		return 0.;
    }
}

bool GuideHelper::ParseBoolValue( QDomElement element, const char *name )
{
	try
	{
        QDomElement el = element.firstChildElement(name);
		bool bvalue;
        if(!el.isNull())
            bvalue = (bool)el.text().toInt();
		return bvalue;
	}
//	catch(ticpp::Exception& ex)
//    {
//		std::cout << "error parse bool value:" << ex.m_details << std::endl;
//		return false;
//    }
	catch(...)
    {
		std::cerr << "unknown error" ;
		return false;
    }
}
