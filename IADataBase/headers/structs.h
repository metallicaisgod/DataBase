#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#define NAME_SIZE 0x100
#define ARTIKUL_SIZE 0x20
namespace db
{
	struct ObjState
	{
		enum State
		{
			Nonactive = 0,
			Active = 1,
			Undefined = 2,
		};
	};

	enum CutterType
	{
		Pilot = 0,
		Finish = 1,
		Intermediate = 2,
		Intermediate1 = 3,
		Intermediate2 = 4,
		Intermediate3 = 5,
		Intermediate4 = 6,
		Intermediate5 = 7,
	};
	

    //////////////////////////////////////////////////////////////////////////
    // Implants providers
    //////////////////////////////////////////////////////////////////////////
    struct Provider
    {
        char name[NAME_SIZE];
        char logo[_MAX_PATH];
    };

    struct Series
    {
        char name[NAME_SIZE];
    };

    struct CompatibleSeries
    {
        char prov[NAME_SIZE];
        char ser[NAME_SIZE];
    };

    struct Segment
    {
        double D1;
        double D2;
        double L;

        Segment()
        {
            D1 = D2 = L = 0.0;
        }
        Segment(const Segment& src)
        {
            D1 = src.D1;
            D2 = src.D2;
            L = src.L;
        }
        Segment& operator=(const Segment& src)
        {
            D1 = src.D1;
            D2 = src.D2;
            L = src.L;
            return *this;
        }
    };

    struct ProductionBase
    {
        ProductionBase()
        {
            name[0] = '\0';
            artikul[0] = '\0';
            szModelName[0] = '\0';
            defcolor[0] = '\0';
            szCompatibility[0] = '\0';
            editable = true;
            Segments.clear();
            Segment seg;
            Segments.push_back(seg);
            state = ObjState::Nonactive;
        }
        char name[NAME_SIZE];
        char artikul[ARTIKUL_SIZE];
        char defcolor[ARTIKUL_SIZE];
        ObjState::State state;
        bool editable;
        char szModelName[_MAX_PATH];
        char szCompatibility[_MAX_PATH];
        std::vector<Segment> Segments;
    };
    //////////////////////////////////////////////////////////////////////////////////////
    // Object for QT DB Editor
    struct ImplantData : public ProductionBase
    {
        ImplantData()
        {
            D1 = L1 = D2 = L2 = Le = 0.0;
        }
        double D1;   // diameter of cylindric part, mm. Range: (0...100);
        double L1;   // length of cylindric part, mm. Range: [0...100);
        double D2;   // diameter of conic part, mm. Range: (0...100);
        double L2;   // length of conic part, mm. Range: [0...100);
        double Le;   // length of endossal part, mm. Range: [0...100);
    };

    struct AbutmentData : public ProductionBase
    {
        AbutmentData()
        {
            D1 = L1 = Alpha = 0.0;
        }
        double D1;     // diameter, mm. Range: (0...100);
        double L1;     // length, mm. Range: [0...100);
        double Alpha;  // angle, degrees. Range [0...90)
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    struct ScrewData : public ProductionBase
    {
        ScrewData()
        {
            D = Dh = L = Lh = Lt = 0.0;
        }
        double D;     // diameter, mm. Range: (0...100);
        double Dh;    // diameter of head, mm. Range: (0...100;
        double L;     // length, mm (0...100
        double Lh;    // length of head, mm (0...100
        double Lt;    // length of thread, mm (0...100
    };

    struct CutterData : public ProductionBase
    {
        CutterData()
        {
            D = Dw = Ds = L = Ls = Lw = 0.0;
        }
        double D;     // diameter, mm. Range: (0...100);
        double Dw;    // diameter of working part, mm. Range: (0...100;
        double Ds;    // diameter of stop, mm. Range: [0...100);
        double L;     // total length, mm (0...100
        double Ls;    // length of stop , mm (0...100
        double Lw;    // length of working part , mm (0...100
        CutterType type;  // 0-pilot, 1-finish, others-intermedeiate
    };

    struct SleeveData : public ProductionBase
    {
        SleeveData()
        {
            D = Do = Dh = Lh= Lw = Df = 0.0;
        }
        double D;     // diameter, mm. Range: (0...100);
        double Do;    // diameter of opening, mm. Range: (0...100;
        double Dh;    // diameter of head, mm. Range: [0...100);
        double Df;    // diameter of flat
        double Lw;    // length of working part, mm (0...100)
        double Lh;    // length of head, mm (0...100)
    };

} // namespace db;

#endif //__STRUCT_H__
