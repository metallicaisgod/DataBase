//
// GlslTypes.h: Basic typedefs for GLSL data types
//
#pragma once

#ifndef	__GlslTypes__
#define	__GlslTypes__


#ifdef __cplusplus
extern "C" {
#endif
    //#include <glew.h>
	#include "GlewStub.h"
#ifdef __cplusplus
}
#endif

typedef	unsigned char		uint8;
typedef	signed	 char		int8;
typedef	unsigned short		uint16;
typedef	signed	 short		int16;
typedef	unsigned long		uint32;
typedef	signed	 long		int32;

#ifdef	_WIN32
typedef	unsigned __int64	uint64;
typedef	signed	 __int64	int64;
#else
typedef	unsigned long long	uint64;
typedef signed   long long	int64;
#endif

typedef	unsigned char		byte;
typedef	uint16				word;
typedef	uint32				dword;

typedef struct
{
	GLfloat	x;
	GLfloat	y;
}GlslVector2D;

inline GlslVector2D GetGlslVector2D(GLfloat x, GLfloat y)
{
	GlslVector2D vector;
	vector.x = x;
	vector.y = y;
	return vector;
}

typedef struct
{
	GLfloat	x;
	GLfloat	y;
	GLfloat z;
}GlslVector3D;

inline GlslVector3D GetGlslVector3D(GLfloat x, GLfloat y, GLfloat z)
{
	GlslVector3D vector;
	vector.x = x;
	vector.y = y;
	vector.z = z;
	return vector;
}

typedef struct
{
	GLfloat	x;
	GLfloat	y;
	GLfloat z;
	GLfloat w;
}GlslVector4D;

inline GlslVector4D GetGlslVector4D(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GlslVector4D vector;
	vector.x = x;
	vector.y = y;
	vector.z = z;
	vector.z = w;
	return vector;
}

typedef struct
{
	GLfloat x[3][3];
}GlslMatrix3D;

typedef struct
{
	GLfloat x[4][4];
}GlslMatrix4x4;


#endif  // __GlslTypes__ 
