#pragma once
#include "IADataBase.h"
#include "ticpp.h"

class ParserHelper
{
public:
	ParserHelper(void);

    static void ParseDataBase(db::IADataBase& indb, const char* fileName, const ticpp::Element* element, unsigned long flags = 0x00);
	static void ParseProvider(db::DbProvider& provider, const ticpp::Element& element, unsigned long flags = 0x00);
	static void ParseSeries(db::DbSeries& series, const ticpp::Element& element, unsigned long flags = 0x00);

	static void ParseOldDataBase(db::IADataBase& indb, const ticpp::Element* element);
	static void ParseOldProvider(db::DbProvider& provider, const ticpp::Element& element);
	static void ParseOldSeries(db::DbSeries& series, const ticpp::Element& element);

    static TiXmlElement* ToXml(const db::IADataBase& indb, const char* fileName, unsigned long flags = 0x00);
	static TiXmlElement* ToXml(const db::DbProvider* provider, unsigned long flags = 0x00);
	static TiXmlElement* ToXml(const db::DbSeries* series, unsigned long flags = 0x00);

	static std::string ParseStringValue( ticpp::Element* element, const char *name );
	static void ParseStringValue( ticpp::Element* element, const char *name, char* field, size_t length );
	static int ParseIntValue( ticpp::Element* element, const char *name );
	static double ParseDoubleValue( ticpp::Element* element, const char *name );
	static bool ParseBoolValue( ticpp::Element* element, const char *name );

	static TiXmlElement* MakeStringElement( const char *name, const char * _value );
	static TiXmlElement* MakeIntElement( const char *name, int _value );
	static TiXmlElement* MakeDoubleElement( const char *name, double _value );
	static TiXmlElement* MakeBoolElement( const char *name, bool _value );
};
