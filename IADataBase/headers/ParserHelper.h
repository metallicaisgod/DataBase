#pragma once
#include "IADataBase.h"
#include <QDomElement>
//#include "ticpp.h"

class ParserHelper
{
public:
	ParserHelper(void);

    static void ParseDataBase(db::IADataBase& indb, const char* fileName, const QDomElement& element, unsigned long flags = 0x00);
    static void ParseProvider(db::DbProvider& provider, const QDomElement& element, unsigned long flags = 0x00);
    static void ParseSeries(db::DbSeries& series, const QDomElement& element, unsigned long flags = 0x00);

    static void ParseOldDataBase(db::IADataBase& indb, const QDomElement& element);
    static void ParseOldProvider(db::DbProvider& provider, const QDomElement& element);
    static void ParseOldSeries(db::DbSeries& series, const QDomElement& element);

    static QDomElement* ToXml(const db::IADataBase& indb, const char* fileName, QDomDocument document, unsigned long flags = 0x00);
    static QDomElement* ToXml(const db::DbProvider* provider, QDomDocument document, unsigned long flags = 0x00);
    static QDomElement* ToXml(const db::DbSeries* series, QDomDocument document, unsigned long flags = 0x00);

    static QString ParseStringValue( QDomElement& element, const char *name );
    static void ParseStringValue( QDomElement& element, const char *name, char* field, size_t length );
    static int ParseIntValue( QDomElement& element, const char *name );
    static double ParseDoubleValue( QDomElement& element, const char *name );
    static bool ParseBoolValue( QDomElement& element, const char *name );

    static QDomElement* MakeStringElement( const char *name, const char * _value, QDomDocument document );
    static QDomElement* MakeIntElement( const char *name, int _value, QDomDocument document );
    static QDomElement* MakeDoubleElement( const char *name, double _value, QDomDocument document );
    static QDomElement* MakeBoolElement( const char *name, bool _value, QDomDocument document );
};
