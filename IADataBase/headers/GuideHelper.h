#pragma once
#include "GuideDataBase.h"
#include <QDomElement>
//#include "tinyxml.h"
//#include "ticpp.h"

class GuideHelper
{
public:
	GuideHelper(void);

    static QDomElement& ToXml(const db::GuideDataBase& indb, QDomDocument& document, unsigned long flags = 0x00);
    static QDomElement& ToXml(const db::DbGuideProvider* provider, QDomDocument& document, unsigned long flags = 0x00);
    static QDomElement& ToXml(const db::DbGuideSeries* series, QDomDocument& document, unsigned long flags = 0x00);
    static QDomElement& ToXml(const db::ProductionBase* prodbase, QDomDocument& document);

    static QDomElement& MakeStringElement( const char *name, const char * _value, QDomDocument& document);
    static QDomElement& MakeIntElement( const char *name, int _value, QDomDocument& document );
    static QDomElement& MakeDoubleElement( const char *name, double _value, QDomDocument& document );
    static QDomElement& MakeBoolElement( const char *name, bool _value, QDomDocument& document );

    static void ParseDataBase(db::GuideDataBase& indb, const char* fileName, const QDomElement& element, unsigned long flags = 0x00);
    static void ParseProvider(db::DbGuideProvider& provider, const QDomElement& element, unsigned long flags = 0x00);
    static void ParseSeries(db::DbGuideSeries& series, const QDomElement& element, unsigned long flags = 0x00);
    static void ParseSegments(db::ProductionBase& prodbase, const QDomElement& element);

    static void ParseOldDataBase(db::GuideDataBase& indb, const QDomElement& element);
    static void ParseOldProvider(db::DbGuideProvider& provider, const QDomElement& element);
    static void ParseOldSeries(db::DbGuideSeries& series, const QDomElement& element);
    static void ParseOldSegments(db::ProductionBase& prodbase, const QDomElement& element);

    static QString ParseStringValue( QDomElement element, const char *name );
    static void ParseStringValue( QDomElement element, const char *name, char* field, size_t length );
    static int ParseIntValue( QDomElement element, const char *name );
    static double ParseDoubleValue( QDomElement element, const char *name );
    static bool ParseBoolValue( QDomElement element, const char *name );
};
