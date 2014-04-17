#pragma once
#include "GuideDataBase.h"
#include "tinyxml.h"
#include "ticpp.h"

class GuideHelper
{
public:
	GuideHelper(void);

	static TiXmlElement* ToXml(const db::GuideDataBase& indb, unsigned long flags = 0x00);
	static TiXmlElement* ToXml(const db::DbGuideProvider* provider, unsigned long flags = 0x00);
	static TiXmlElement* ToXml(const db::DbGuideSeries* series, unsigned long flags = 0x00);
	static TiXmlElement* ToXml(const db::ProductionBase* prodbase);

	static TiXmlElement* MakeStringElement( const char *name, const char * _value );
	static TiXmlElement* MakeIntElement( const char *name, int _value );
	static TiXmlElement* MakeDoubleElement( const char *name, double _value );
	static TiXmlElement* MakeBoolElement( const char *name, bool _value );

    static void ParseDataBase(db::GuideDataBase& indb, const char* fileName, const ticpp::Element* element, unsigned long flags = 0x00);
	static void ParseProvider(db::DbGuideProvider& provider, const ticpp::Element& element, unsigned long flags = 0x00);
	static void ParseSeries(db::DbGuideSeries& series, const ticpp::Element& element, unsigned long flags = 0x00);
	static void ParseSegments(db::ProductionBase& prodbase, const ticpp::Element* element);

	static void ParseOldDataBase(db::GuideDataBase& indb, const ticpp::Element* element);
	static void ParseOldProvider(db::DbGuideProvider& provider, const ticpp::Element& element);
	static void ParseOldSeries(db::DbGuideSeries& series, const ticpp::Element& element);
	static void ParseOldSegments(db::ProductionBase& prodbase, const ticpp::Element* element);

	static std::string ParseStringValue( ticpp::Element* element, const char *name );
	static void ParseStringValue( ticpp::Element* element, const char *name, char* field, size_t length );
	static int ParseIntValue( ticpp::Element* element, const char *name );
	static double ParseDoubleValue( ticpp::Element* element, const char *name );
	static bool ParseBoolValue( ticpp::Element* element, const char *name );
};
