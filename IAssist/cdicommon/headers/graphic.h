#pragma once
#include "glh_linear.h"

#define EXCH_RB(_A_) ((0xff&(_A_>>16)) | (0xff00&_A_) | (0xff0000&(_A_<<16)))


namespace CDICommon
{
////////////////////////////////////////////////////////////////////////
// Points
template <class _X> class Point2
{
public:
	_X x;
	_X y;
	Point2(): x(0), y(0){};
	Point2(_X inX, _X inY): x(inX), y(inY){};
	Point2&	operator = (const Point2& in)
	{
		x = in.x;
		y = in.y;
		return *this;
	}
	Point2&	operator += (const Point2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Point2&	operator *= (double v)
	{
		x *= v;
		y *= v;
		return *this;
	}
	Point2	operator * (double v)
	{
	
		return Point2(x*v, y*v);
	}
	Point2	operator / (double v)
	{
	
		return Point2(x/v, y/v);
	}
	Point2	operator - (const Point2& v)
	{
		return Point2(x-v.x, y-v.y);
	}
	Point2	operator + (const Point2& v)
	{
		return Point2(x+v.x, y+v.y);
	}
	Point2&	operator -= (const Point2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Point2	operator - () const
	{
		return Point2(-x, -y);
	}
};

typedef Point2<unsigned short> USPoint2;
template <class _X> class Point3
{
public:
	_X x;
	_X y;
	_X z;
	virtual void SetU(_X in_u){}
	virtual _X GetU(){return 0;}
	virtual _X GetV(){return 0;}
	virtual ~Point3(){};
	Point3(): x(0), y(0), z(0) {};
	Point3(_X inX, _X inY, _X inZ): x(inX), y(inY), z(inZ) {};
	Point3&	operator = (const Point3& in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
		return *this;
	}
	Point3&	operator += (const Point3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}
	Point3&	operator *= (double v)
	{
		x *= v;
		y *= v;
		z *= v;

		return *this;
	}
	Point3	operator * (double v)
	{
	
		return Point3(x*v, y*v, z*v);
	}
	Point3	operator / (double v)
	{
	
		return Point3(x/v, y/v, z/v);
	}
	Point3	operator - (const Point3& v)
	{
		return Point3(x-v.x, y-v.y, z-v.z);
	}
	Point3	operator + (const Point3& v)
	{
		return Point3(x+v.x, y+v.y, z+v.z);
	}
	Point3&	operator -= (const Point3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}
	Point3	operator - () const
	{
		return Point3(-x, -y, -z);
	}
	void Set(_X inX, _X inY, _X inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}
	bool	operator == (const Point3& in)
	{
		return (x==in.x && y==in.y && z==in.z);
	}
};

////////////////////////////////////////////////////////////////////////
// Point5
template <class _X> class	Point5: public Point3<_X> 
{
public:
	_X u;
	_X v;
	Point5(): Point3<_X>(), u(0), v(0){};
	Point5(_X inX, _X inY, _X inZ, _X inU, _X inV):  Point3<_X>(inX, inY, inZ), u(inU), v(inV) {};
	Point5&	operator = (const Point5& in)
    {
        this->x = in.x;
        this->y = in.y;
        this->z = in.z;
		u = in.u;
		v = in.v;
		return *this;
	}
	virtual ~Point5(){};
	virtual void SetU(_X in_u){u = in_u;}
	virtual _X GetU(){return u;}
	virtual _X GetV(){return v;}
	void Set(_X inX, _X inY, _X inZ, _X inU, _X inV)
	{
        this->x = inX;
        this->y = inY;
        this->z = inZ;
		u = inU;
		v = inV;
	}
};

typedef Point3<int> IPoint ;

typedef Point3<double> DPoint ;
typedef Point2<double> DPoint2 ;
typedef Point5<double> DPoint5 ;

////////////////////////////////////////////////////////////////////////
// Segments
template <class _X> class Segment3D
{
public:
	Point3<_X> p1;
	Point3<_X> p2;

	Segment3D():p1(0,0,0), p2(0,0,0) 
	{
	}
	Segment3D(Point3<_X> in_p1, Point3<_X> in_p2)
	{
		p1 = in_p1; p2 = in_p2;
	}
};

template <class _X> class Segment5D
{
public:
	Point5<_X> p1;
	Point5<_X> p2;

	Segment5D():p1(), p2() 
	{
	}
	Segment5D(Point5<_X> in_p1, Point5<_X> in_p2)
	{
		p1 = in_p1; p2 = in_p2;
	}
};

template <class _X> class Segment3DEx
{
public:
	Point3<_X> p1;
	Point3<_X> p2;
	Point3<_X> ct;
	Point3<_X> n1;
	Point3<_X> n2;
	bool flag;

	Segment3DEx():p1(0,0,0), p2(0,0,0),ct(0,0,0),n1(0,0,0),n2(0,0,0),flag(true)
	{
	}
	Segment3DEx(Point3<_X> in_p1, Point3<_X> in_p2):
	ct(0,0,0),n1(0,0,0),n2(0,0,0),flag(true)
	{
		p1 = in_p1; p2 = in_p2;
	}
};
typedef Segment3D<double> DSegment ;
typedef Segment5D<double> DSegment5 ;
typedef Segment3DEx<double> DSegmentEx ;

///////////////////////////////////////////////////////////////////
// Direction
typedef enum 
{
	Left=0,
	Right=1,
	Top=2,
	Bottom=3,
	Anterior=4,
	Posterior=5,
	Lingual=6,
	Bukkal=7
} Direction;

/////////////////////////////////////////////////////////////////////
// 3D Vector
template <class _X>
class Vector3D : public Segment3D<_X>
{
public:
	Vector3D():Segment3D<_X>()
	{
    }
	Vector3D(Point3<_X> in_p):Segment3D<_X>(Point3<_X>(0.,0.,0.), in_p)
	{
    }
	Vector3D(Point3<_X> in_p1, Point3<_X> in_p2):Segment3D<_X>(in_p1, in_p2)
	{
    }
	void Move(Point3<_X> in)
	{
        this->p1 += in;	this->p2 += in;
    }
	double GetLength() 
	{ 
        return sqrt((this->p1.x-this->p2.x)*(this->p1.x-this->p2.x) + (this->p1.y-this->p2.y)*(this->p1.y - this->p2.y) +(this->p1.z-this->p2.z)*(this->p1.z - this->p2.z));
	}
	double ScalarProd(Vector3D<_X>& inV)
	{
		Point3<_X> a,b;
        a = this->p2 - this->p1;
		b = inV.p2-inV.p1;
		return a.x * b.x + a.y * b.y + a.z * b.z; 
	}
	Vector3D<_X>	VectProd(Vector3D<_X>& inV)
	{
		Vector3D<_X> outVect;
		Point3<_X> a,b;
        a = this->p2 - this->p1;
		b = inV.p2-inV.p1;

		outVect.p2.x = a.y * b.z - a.z * b.y; 
		outVect.p2.y = a.z * b.x - a.x * b.z; 
		outVect.p2.z = a.x * b.y - a.y * b.x;
		return outVect;
	}
	
	void Normalize()
	{
		double	length = GetLength();
        Move(-(this->p1));
        this->p2.x /= length;
        this->p2.y /= length;
        this->p2.z /= length;
	}

	Vector3D<_X> GetNormalized()
	{
		Vector3D<_X> out = *this;
		out.Normalize();
		return out;
	}

	Vector3D<_X>&	operator *= (double v)
	{
        DPoint p_1 = this->p1;
        this->p1 *= v;
        this->p2 *= v;
        Move(p_1 - this->p1);
		return *this;
	}

	double GetCosine(Vector3D<_X>& inV)
	{
		Vector3D<_X> v1 = GetNormalized();
		Vector3D<_X> v2 = inV.GetNormalized();

		double prod = v1.ScalarProd(v2);
		if(prod > 0)
		{
			if(fabs(1.0 - prod)< 0.000001) prod = 1;
		}
		else
		{
			if(fabs(-1.0 - prod)< 0.000001)prod = -1;
		}
		return prod;
	}
	double GetSine(Vector3D<_X>& inV)
	{
		Vector3D<_X> v1 = GetNormalized();
		Vector3D<_X> v2 = inV.GetNormalized();

		Vector3D<_X> v_prod = v1.VectProd(v2);
			
		double sine=v_prod.GetLength()/(v1.GetLength()*v2.GetLength());
		return sine;
	}
	double GetSineSign(Vector3D<_X>& inV)
	{
		Vector3D<_X> v1 = GetNormalized();
		Vector3D<_X> v2 = inV.GetNormalized();

		Vector3D<_X> v_prod = v1.VectProd(v2);
			
		double sine=v_prod.GetLength()/(v1.GetLength()*v2.GetLength());
        if(abs(sine) < 0.000001)
			return 0.;

		
		//Vector3D<_X> v_prod_1 = v1.VectProd(v_prod);
		//float scp = v_prod_1.ScalarProd(v2);

		//if(scp<0.)
		//	return -sine;
		//return sine;

//////////////////////////////////////////////////
		glh::vec3f ppp,p0,p1,p2;
		p0[0]=0.; p0[1]=0.; p0[2]=0.;
		p1[0]=(float)v1.p2.x; p1[1]=(float)v1.p2.y; p1[2]=(float)v1.p2.z;
		p2[0]=(float)v2.p2.x; p2[1]=(float)v2.p2.y; p2[2]=(float)v2.p2.z;
		glh::planef pln(p0,p1,p2);
		v_prod.Normalize();
		ppp[0]=(float)v_prod.p2.x; ppp[1]=(float)v_prod.p2.y; ppp[2]=(float)v_prod.p2.z;

		bool hsp = pln.is_in_half_space(ppp);
		if(!hsp)
			return -sine;
		return sine;

		float dist = pln.distance(ppp);
		if(dist<0.)
			return -sine;
		return sine;
	}


	Point3<_X> ProjectPoint(Point3<_X>& inP)
	{
		Vector3D<_X> vec1 = GetNormalized();
        Vector3D<_X> vec2(this->p1, inP);
		double val = vec2.GetLength()*GetCosine(vec2);
		vec1*=val;
        vec1.Move(this->p1);
		return vec1.p2;
	}

	Vector3D<_X>	operator + (Vector3D<_X> in)
	{
		Vector3D<_X> res =*this;
		res.p1 += in.p1;
		res.p2 += in.p2;
		return res;
	}

};

typedef Vector3D<double> DVector;

////////////////////////////////////////////////////////////////////////////////
// Plane
/*
template <class _T>
class Plane
{
public:
	Plane(const _T* plane): m_a(plane[0]), m_b(plane[1]), m_c(plane[2]), m_d(plane[3]) {};
	Plane(_T _a, _T _b, _T _c, _T _d): m_a(_a), m_b(_b), m_c(_c), m_d(_d) {};
	Plane(Point3<_T> p1, Point3<_T> p2, Point3<_T> p3)
	{
		//A = y1 (z2 - z3) + y2 (z3 - z1) + y3 (z1 - z2) 
		//B = z1 (x2 - x3) + z2 (x3 - x1) + z3 (x1 - x2) 
		//C = x1 (y2 - y3) + x2 (y3 - y1) + x3 (y1 - y2) 
		//-D = x1 (y2 z3 - y3 z2) + x2 (y3 z1 - y1 z3) + x3 (y1 z2 - y2 z1)
		m_a =  p1.y*(p2.z-p3.z) + p2.y*(p3.z-p1.z) + p3.y*(p1.z-p2.z);
		m_b =  p1.z*(p2.x-p3.x) + p2.z*(p3.x-p1.x) + p3.z*(p1.x-p2.x);
		m_c =  p1.x*(p2.y-p3.y) + p2.x*(p3.y-p1.y) + p3.x*(p1.y-p2.y);
		m_d = -p1.x*(p2.y*p3.z - p3.y*p2.z) - p2.x*(p3.y*p1.z - p1.y*p3.z) - p3.x*(p1.y*p2.z - p2.y*p1.z)
	};

	// Distance from point to plain
	_T distance(_T x, _T y, _T z) const { return m_a*x + m_b*y + m_c*z + m_d; };
	_T distance(Point3<_T>& inP) const { return m_a*inP.x + m_b*inP.y + m_c*inP.z + m_d; };

    // Calculates edge intersection point
	void calcVert(_T x1, _T y1, _T z1,	_T x2, _T y2, _T z2, _T* outVert)
	{
		const _T dx = x2 - x1;
		const _T dy = y2 - y1;
		const _T dz = z2 - z1;
		const _T w = m_a * dx + m_b * dy + m_c * dz;
		if (w == 0)
		{
			outVert[0] = x1;
			outVert[1] = y1;
			outVert[2] = z1;
		}
		else
		{
			const _T u = (m_a * x1 + m_b * y1 + m_c * z1 + d) / -w;
			outVert[0] = x1 + u * dx;
			outVert[1] = y1 + u * dy;
			outVert[2] = z1 + u * dz;
		}
	}

	// vert1 - first vertex (float[3])
	// vert2 - second vertex (float[3])
	// vert3 - third vertex (float[3])
	// outVert1 - first output vertex (float[3])
	// outVert2 - second output vertex (float[3])
	bool intersect_triangle(const _T* vert1, const _T* vert2, const _T* vert3,
				_T* outVert1, _T* outVert2) 
	{
		// Distances to the plane.
		const _T dv1 = distance(vert1[0], vert1[1], vert1[2]);
		const _T dv2 = distance(vert2[0], vert2[1], vert2[2]);
		const _T dv3 = distance(vert3[0], vert3[1], vert3[2]);

		// Check whether the plane intersects the triangle
		int vertsInPlane = 3;
		if (dv1 < 0)
			--vertsInPlane;
		if (dv2 < 0)
			--vertsInPlane;
		if (dv3 < 0)
			--vertsInPlane;
		if (vertsInPlane == 3 || vertsInPlane == 0)
			return false;

		bool firstVertFound = false;

		// Check first edge (vert1, vert2)
		if ((dv1 < 0 && dv2 >= 0) || (dv1 >= 0 && dv2 < 0))
		{
			calcVert(vert1[0], vert1[1], vert1[2], vert2[0], vert2[1], vert2[2], outVert1);
			firstVertFound = true;
		}

		// Check second edge (vert2, vert3)
		if ((dv2 < 0 && dv3 >= 0) || (dv2 >= 0 && dv3 < 0))
		{
			calcVert(vert2[0], vert2[1], vert2[2],	vert3[0], vert3[1], vert3[2], firstVertFound ? outVert2 : outVert1);
			firstVertFound = true;
		}

		// Check third edge (vert3, vert1)
		if ((dv3 < 0 && dv1 >= 0) || (dv3 >= 0 && dv1 < 0))
		{
			// assert(firstVertFound);
			calcVert(vert3[0], vert3[1], vert3[2], vert1[0], vert1[1], vert1[2], outVert2);
		}

		return true;

	};


	
private:
	 _T m_a, m_b, m_c, m_d;
};

typedef Plane<float> FPlane;
typedef Plane<double> DPlane;

*/
} // end of namespace CDICommon
