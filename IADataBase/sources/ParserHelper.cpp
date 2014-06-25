#include "StdAfx.h"
#include "ParserHelper.h"
#include "Enumerator.h"

ParserHelper::ParserHelper(void)
{
}

void ParserHelper::ParseDataBase(db::IADataBase& indb, const char* fileName, const QDomElement& element, unsigned long flags)
{
    //ticpp::Iterator<ticpp::Element> child;
    QDomElement pProvider = element.firstChildElement("provider");

    for( pProvider; !pProvider.isNull(); pProvider=pProvider.nextSiblingElement("provider"))
	{
        QString attrStr = pProvider.attribute("ext_file");
        if(!attrStr.isEmpty())
        {
            QFileInfo fi(fileName);
            QString extFileName = fi.absolutePath();
            extFileName += attrStr;
            indb.LoadXml_2(extFileName.toLocal8Bit().data(), flags|PROVIDER_EXT_FILE);
			continue;
		}

        QString strName = ParseStringValue(pProvider, "name");
        db::DbProvider& newProvider = indb.AddProvider(strName.toLocal8Bit().data());
		if(flags & PROVIDER_EXT_FILE)
		{
            QString extFileName = fileName;
			//int pathpos1 = extFileName.find_last_of("\\");
			//int pathpos2 = extFileName.find_last_of("\/");
			//pathpos1 = (pathpos1>pathpos2)?pathpos1:pathpos2;
			//extFileName.erase(0,pathpos1+1);
			
            strcpy_s(newProvider.m_szExtFile, NAME_SIZE, extFileName.toLocal8Bit().data());
		}
		else if(!(flags&USERS_INTEMS))
			strcpy_s(newProvider.m_szExtFile, NAME_SIZE, "");

        ParseProvider(newProvider, pProvider, flags);
	}
}

void ParserHelper::ParseProvider(db::DbProvider& provider, const QDomElement& element, unsigned long flags)
{
	
    QDomElement pSeries = element.firstChildElement("series");
    for( pSeries;!pSeries.isNull(); pSeries=pSeries.nextSiblingElement("series"))
	{
        QString strName = ParseStringValue(pSeries, "name");
        db::DbSeries& series = provider.AddSeries(strName.toLocal8Bit().data());
        ParseSeries(series, pSeries, flags);
	}
}

void ParserHelper::ParseSeries(db::DbSeries& series, const QDomElement& element, unsigned long flags)
{
	bool bWithoutCondition = (flags == 0);
	bool condition = (flags == USERS_INTEMS);

    QDomElement pImplants = element.firstChildElement("implants");
    if (!pImplants.isNull())
	{
        QDomElement pImplant = pImplants.firstChildElement("implant");
        for( pImplant; !pImplant.isNull(); pImplant=pImplant.nextSiblingElement("implant"))
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
                /*db::DbImplant& implant = */series.AddImplant(implData);
			}
		}
	}
    QDomElement pAbutments = element.firstChildElement("abutments");
    if (!pAbutments.isNull())
	{
        QDomElement pAbutment = pAbutments.firstChildElement("abutment");
        for( pAbutment; !pAbutment.isNull(); pAbutment=pAbutment.nextSiblingElement("abutment"))
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
                /*db::DbAbutment& abutment = */series.AddAbutment(data);
			}
		}
	}
	// востанавливаем из пользовательского только если m_CompSer пустой 
	if (series.m_CompSer.empty()) 
	{
        QDomElement pCompSeries = element.firstChildElement("comp_ser");
        if (!pCompSeries.isNull())
		{
            QDomElement pSeries = pCompSeries.firstChildElement("Series");
            for( pSeries; !pSeries.isNull(); pSeries=pSeries.nextSiblingElement("Series"))
			{
                QString strProducer = pSeries.attribute("Producer");
                QString strValue = pSeries.text();
				db::CompatibleSeries data;
                strncpy(data.prov, strProducer.toLocal8Bit().data(), NAME_SIZE);
                data.prov[NAME_SIZE-1] = '\0';
                strncpy(data.ser, strValue.toLocal8Bit().data(), NAME_SIZE);
                data.ser[NAME_SIZE-1] = '\0';
				series.m_CompSer.push_back(data);
			}
		}
	}

}

void ParserHelper::ParseOldDataBase(db::IADataBase& indb, const QDomElement& element)
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
                db::DbProvider& newProvider = indb.AddProvider(strName.toLocal8Bit().data());
                ParseOldProvider(newProvider, pProvider);
			}
		}
	}
}

void ParserHelper::ParseOldProvider(db::DbProvider& provider, const QDomElement& element)
{
    QDomElement pSeriesRoot = element.firstChildElement("series");
    QDomElement pSeries = pSeriesRoot.firstChildElement("item");
    for( pSeries; !pSeries.isNull(); pSeries=pSeries.nextSiblingElement("item"))
	{
        QString strName = ParseStringValue(pSeries, "name");
        db::DbSeries& series = provider.AddSeries(strName.toLocal8Bit().data());
        ParseOldSeries(series, pSeries);
	}
}

void ParserHelper::ParseOldSeries(db::DbSeries& series, const QDomElement& element)
{
    QDomElement pImplants = element.firstChildElement("implants");
    if (!pImplants.isNull())
	{
        QDomElement pImplant = pImplants.firstChildElement("item");
        for( pImplant; !pImplant.isNull(); pImplant=pImplant.nextSiblingElement("item"))
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

            /*db::DbImplant& implant = */series.AddImplant(implData);
		}
	}
    QDomElement pAbutments = element.firstChildElement("abutments");
    if (!pAbutments.isNull())
	{
        QDomElement pAbutment = pAbutments.firstChildElement("item");
        for( pAbutment; !pAbutment.isNull(); pAbutment=pAbutment.nextSiblingElement("item"))
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
            /*db::DbAbutment& abutment = */series.AddAbutment(data);
		}
	}
    QDomElement pCompSeries = element.firstChildElement("comp_ser");
    if (!pCompSeries.isNull())
	{
        QDomElement pSeries = pCompSeries.firstChildElement("item");
        for( pSeries; !pSeries.isNull(); pSeries=pSeries.nextSiblingElement("item"))
		{
			db::CompatibleSeries data;
			ParseStringValue(pSeries, "Producer", data.prov, NAME_SIZE);
			ParseStringValue(pSeries, "Series", data.ser, NAME_SIZE);
			series.m_CompSer.push_back(data);
		}
	}
}

QDomElement* ParserHelper::ToXml(const db::IADataBase& indb, const char* fileName, QDomDocument document, unsigned long flags)
{
    QDomElement * root = new QDomElement();
    *root = document.createElement( "database" );
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

            QDomElement elProvider = document.createElement( "provider" );
			//if(flags&PROVIDER_EXT_FILE)
			{
                QString extFileName = QString::fromLocal8Bit(pProvider->m_szExtFile);
                QFileInfo fi(extFileName);
                extFileName = fi.fileName();
                elProvider.setAttribute("ext_file", extFileName);
                root->appendChild( elProvider );
			
			}
		}
		else if ((flags&PROVIDER_EXT_FILE)&&(0==strcmp(fileName, pProvider->m_szExtFile)))
		{
            QDomElement* elProvider = ParserHelper::ToXml(pProvider, document, flags);
            if (elProvider != NULL && !elProvider->isNull())
			{
                root->appendChild( *elProvider );
			}
			break;
		}
		else
		{
            QDomElement *elProvider = ParserHelper::ToXml(pProvider, document, flags);
            if (elProvider != NULL && !elProvider->isNull())
			{
                root->appendChild( *elProvider );
			}
		}
	}
    return root;
}

QDomElement* ParserHelper::ToXml(const db::DbProvider* provider, QDomDocument document, unsigned long flags)
{
	db::DbProvider* pProvider = const_cast<db::DbProvider*>(provider);

	bool bNotEmpty = false;
	bool bWithoutCondition = (flags == 0);

	db::enumerator<db::t_SeriesList::iterator> seriesEnum = pProvider->GetSeriesEnumerator(db::no_filter<db::DbSeries*>());

    QDomElement * elProvider = new QDomElement();
    *elProvider = document.createElement( "provider" );
	//if(flags&PROVIDER_EXT_FILE)
	//{
	//	std::string extFileName = provider->m_szExtFile;
	//	int pathpos1 = extFileName.find_last_of("\\");
	//	int pathpos2 = extFileName.find_last_of("\/");
	//	pathpos1 = (pathpos1>pathpos2)?pathpos1:pathpos2;
	//	extFileName.erase(0,pathpos1+1);

    //	elProvider->SetAttribute("ext_file", extFileName.toLocal8Bit().data());
	//	return elProvider;
	//}

    QDomElement elName = document.createElement( "name" );
    elName.appendChild( document.createTextNode(QString::fromLocal8Bit(pProvider->name)));
    elProvider->appendChild(elName);
	
	while (seriesEnum.MoveNext())
	{
		db::DbSeries* pSeries = seriesEnum.GetCurrent();
        QDomElement* elSeries = ParserHelper::ToXml(pSeries, document, flags);
        if (elSeries != NULL && !elSeries->isNull())
		{
            elProvider->appendChild(* elSeries );
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
        return NULL;//QDomElement();
	}
}

QDomElement* ParserHelper::ToXml(const db::DbSeries* series, QDomDocument document, unsigned long flags)
{
	db::DbSeries* pSeries = const_cast<db::DbSeries*>(series);
    QDomElement * elSeries = new QDomElement();

    *elSeries = document.createElement( "series" );
    QDomElement elName = document.createElement( "name" );
    elName.appendChild( document.createTextNode( QString::fromLocal8Bit(pSeries->name)));
    elSeries->appendChild(elName);

	bool bNotEmpty = false;
	bool bWithoutCondition = (flags == 0);
	bool condition = (flags == USERS_INTEMS);

    QDomElement elImplants = document.createElement( "implants" );
	const db::t_ImplantList& impList = pSeries->GetImplants();
	db::t_ImplantList::const_iterator it = impList.begin();
	for (; it != impList.end(); ++it)
	{
		db::ImplantData* pImplant = *it;
		if (bWithoutCondition || (condition == pImplant->editable))
		{
            QDomElement elImplant = document.createElement( "implant" );
            AddStringElement("name", pImplant->name, document, elImplant);
            AddNumberElement("state", (unsigned int)pImplant->state, document, elImplant);
            AddNumberElement("cyl-diam", pImplant->D1, document, elImplant);
            AddNumberElement("length", pImplant->L1, document, elImplant);
            AddNumberElement("cyl-diam2", pImplant->D2, document, elImplant);
            AddNumberElement("length2", pImplant->L2, document, elImplant);
            AddStringElement("model-file", pImplant->szModelName, document, elImplant);
            AddStringElement("article", pImplant->artikul, document, elImplant);
            AddNumberElement("len-e", pImplant->Le, document, elImplant);
            AddNumberElement("ed", pImplant->editable, document, elImplant);
            AddStringElement("comp_val", pImplant->szCompatibility, document, elImplant);
            AddStringElement("color", pImplant->defcolor, document, elImplant);
//            elImplant.appendChild( *(MakeStringElement("name", pImplant->name, document)));
//            elImplant.appendChild( *(MakeIntElement("state", (unsigned int)pImplant->state, document)) );
//            elImplant.appendChild( *(MakeDoubleElement("cyl-diam", pImplant->D1, document)) );
//            elImplant.appendChild( *(MakeDoubleElement("length", pImplant->L1, document)) );
//            elImplant.appendChild( *(MakeDoubleElement("cyl-diam2", pImplant->D2, document)) );
//            elImplant.appendChild( *(MakeDoubleElement("length2", pImplant->L2, document)) );
//            elImplant.appendChild( *(MakeStringElement("model-file", pImplant->szModelName, document)));
//            elImplant.appendChild( *(MakeStringElement("article", pImplant->artikul, document)));
//            elImplant.appendChild( *(MakeDoubleElement("len-e", pImplant->Le, document)));
//            elImplant.appendChild( *(MakeBoolElement("ed", pImplant->editable, document)) );
//            elImplant.appendChild( *(MakeStringElement("comp_val", pImplant->szCompatibility, document)) );
//            elImplant.appendChild( *(MakeStringElement("color", pImplant->defcolor, document)) );

            elImplants.appendChild(elImplant);
			bNotEmpty = true;
		}
	}
    elSeries->appendChild(elImplants);

    QDomElement elAbutments = document.createElement( "abutments" );
	const db::t_AbutmentList& abutmentList = pSeries->GetAbutment();
	db::t_AbutmentList::const_iterator itAbutment = abutmentList.begin();
	for (; itAbutment != abutmentList.end(); ++itAbutment)
	{
		db::AbutmentData* pAbutment = *itAbutment;
		if (bWithoutCondition || (condition == pAbutment->editable))
		{
            QDomElement elAbutment = document.createElement( "abutment" );
            AddStringElement("name", pAbutment->name, document, elAbutment);
            AddNumberElement("state", (unsigned int)pAbutment->state, document, elAbutment);
            AddNumberElement("cyl-diam", pAbutment->D1, document, elAbutment);
            AddNumberElement("length", pAbutment->L1, document, elAbutment);
            AddNumberElement("alpha", pAbutment->Alpha, document, elAbutment);
            AddStringElement("model-file", pAbutment->szModelName, document, elAbutment);
            AddStringElement("article", pAbutment->artikul, document, elAbutment);
            AddNumberElement("ed", pAbutment->editable, document, elAbutment);
            AddStringElement("comp_val", pAbutment->szCompatibility, document, elAbutment);
            AddStringElement("color", pAbutment->defcolor, document, elAbutment);
//            elAbutment.appendChild( *(MakeStringElement("name", pAbutment->name, document)));
//            elAbutment.appendChild( *(MakeIntElement("state", (unsigned int)pAbutment->state, document)) );
//            elAbutment.appendChild( *(MakeDoubleElement("cyl-diam", pAbutment->D1, document)) );
//            elAbutment.appendChild( *(MakeDoubleElement("length", pAbutment->L1, document)) );
//            elAbutment.appendChild( *(MakeDoubleElement("alpha", pAbutment->Alpha , document)) );
//            elAbutment.appendChild( *(MakeStringElement("model-file", pAbutment->szModelName, document)));
//            elAbutment.appendChild( *(MakeStringElement("article", pAbutment->artikul, document)));
//            elAbutment.appendChild( *(MakeBoolElement("ed", pAbutment->editable, document)) );
//            elAbutment.appendChild( *(MakeStringElement("comp_val", pAbutment->szCompatibility, document)) );
//            elAbutment.appendChild( *(MakeStringElement("color", pAbutment->defcolor, document) ));

            elAbutments.appendChild(elAbutment);
			bNotEmpty = true;
		}
	}
    elSeries->appendChild(elAbutments);

    QDomElement elCompSer = document.createElement( "comp_ser" );
	std::vector<db::CompatibleSeries>::const_iterator it3 = pSeries->m_CompSer.begin();
	for (; it3 != pSeries->m_CompSer.end(); ++it3)
	{
		db::CompatibleSeries pSer = *it3;
        QDomElement elSer = document.createElement( "Series" );
        elSer.setAttribute("Producer", QString::fromLocal8Bit(pSer.prov));
        elSer.appendChild( document.createTextNode(QString::fromLocal8Bit(pSer.ser)));
        elCompSer.appendChild(elSer);
	}
    elSeries->appendChild(elCompSer);

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

QString ParserHelper::ParseStringValue( QDomElement& element, const char *name )
{
    QDomElement el = element.firstChildElement(name);
    QString svalue;

    //el->GetText(&svalue, false);
    if(!el.isNull())
    {
        //QByteArray encodedString(el.text().toLocal8Bit().data());
        //QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        svalue = el.text();//codec->toUnicode(encodedString);
    }
	return svalue;
}

void ParserHelper::ParseStringValue( QDomElement& element, const char *name, char* field, size_t length )
{
	try
	{
        QDomElement el = element.firstChildElement(name);
        if(!el.isNull())
		{
            QString svalue;
            //el->GetText(&svalue, false);
            if(!el.isNull())
                svalue = el.text();
            if (!svalue.isEmpty())
			{
                strncpy(field, svalue.toLocal8Bit().data(), length);
                field[length-1] = '\0';
			}
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

int ParserHelper::ParseIntValue( QDomElement& element, const char *name )
{
	try
	{
        QDomElement el = element.firstChildElement(name);
        int ivalue;
        //el->GetText(&svalue, false);
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
        std::cerr << "error parse int value" ;
		return 0;
    }
}

double ParserHelper::ParseDoubleValue( QDomElement& element, const char *name )
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
        std::cerr << "error parse double value" ;
		return 0.;
    }
}

bool ParserHelper::ParseBoolValue( QDomElement& element, const char *name )
{
	try
	{
        QDomElement el = element.firstChildElement(name);
		bool bvalue;
        //el->GetText(&bvalue);
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
        std::cerr << "error parse bool value" ;
		return false;
    }
}

template <typename T>
void ParserHelper::AddNumberElement(const char *name, T _value, QDomDocument document, QDomElement parent)
{
    //QDomElement * element = new QDomElement();
    QDomElement element = document.createElement(QString(name));
    element.appendChild( document.createTextNode(QString("%1").arg( _value )));
    parent.appendChild(element);
    //return element;
}

void ParserHelper::AddStringElement(const char *name, char * _value, QDomDocument document, QDomElement parent)
{
    if(! _value)
        return;
    //QDomElement * element = new QDomElement();
    QDomElement element = document.createElement(QString(name));
    element.appendChild( document.createTextNode(QString::fromLocal8Bit(_value )));
    parent.appendChild(element);
    //return element;
}

//void ParserHelper::AddIntElement(const char *name, int _value, QDomDocument document, QDomElement parent)
//{

//}

//QDomElement* ParserHelper::MakeStringElement( const char *name, const char * _value, QDomDocument document )
//{
//    QDomElement * element = new QDomElement();
//    *element = document.createElement( name );
//    element->appendChild( document.createTextNode(QString("%1").arg( _value )));
//    return element;
//}

//QDomElement* ParserHelper::MakeIntElement( const char *name, int _value, QDomDocument document )
//{
////	char buf [64];
////	#if defined(TIXML_SNPRINTF)
////		TIXML_SNPRINTF(buf, sizeof(buf), "%d", _value);
////	#else
////		sprintf (buf, "%d", _value);
////	#endif
////	QDomElement& element = document.createElement( name );
////	element.appendChild( new TiXmlText( buf ));
////	return element;
//    QDomElement element = document.createElement( name );
//    element.appendChild( document.createTextNode(QString("%1").arg( _value )));
//    return &element;
//}

//QDomElement* ParserHelper::MakeDoubleElement( const char *name, double _value, QDomDocument document )
//{
////	char buf [256];
////	#if defined(TIXML_SNPRINTF)
////		TIXML_SNPRINTF( buf, sizeof(buf), "%g", _value);
////	#else
////		sprintf (buf, "%g", _value);
////	#endif
////	QDomElement& element = document.createElement( name );
////	element.appendChild( new TiXmlText( buf ));
////	return element;
//    QDomElement element = document.createElement( name );
//    element.appendChild( document.createTextNode(QString("%1").arg( _value )));
//    return &element;
//}


//QDomElement* ParserHelper::MakeBoolElement( const char *name, bool _value, QDomDocument document )
//{
////	QDomElement& element = document.createElement( name );
////	element.appendChild( new TiXmlText( _value ? "1" : "0" ));
////	return element;
//    QDomElement element = document.createElement( name );
//    element.appendChild( document.createTextNode(QString("%1").arg( _value )));
//    return &element;
//}
