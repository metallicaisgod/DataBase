//
// Class to encapsulate GLSL program and shader objects and working with them
//
#pragma once

#ifndef __GLSL_PROGRAM__
#define __GLSL_PROGRAM__


#include    <string>

using namespace std;

class   Data;

class   GlslProgram
{
protected:
    GLhandleARB program;                            // program object handle
    GLhandleARB vertexShader;
    GLhandleARB fragmentShader;
    bool        ok;                                 // whether program is loaded and ready to be used
    string      glError;
    string      log;

public:
    GlslProgram  ();
    ~GlslProgram ();

                                                    // load shaders
    bool    loadShaders ( const char * vertexFileName, const char * fragmentFileName );
    bool    loadShaders (Data* vertexShaderData, Data* fragmentShaderData);

                                                    // remove all shaders and free all objects
    void    clear ();


    string  getLog     () const                         // get current log
    {
        return log;
    }

    bool    isOk () const                               // whether shader is ok
    {
        return ok;
    }

    string  getGlError () const
    {
        return glError;
    }

    void    bind   ();
    void    unbind ();

                                                        // uniform variables handling methods
    bool        setUniformGlslVector  ( const char * name, const GlslVector4D& value  );
    bool        setUniformGlslVector  ( int loc,            const GlslVector4D& value );
    bool        setUniformGlslVector  ( const char * name, const GlslVector3D& value  );
    bool        setUniformGlslVector  ( int loc,            const GlslVector3D& value );
    bool        setUniformGlslVector  ( const char * name, const GlslVector2D& value  );
    bool        setUniformGlslVector  ( int loc,            const GlslVector2D& value );
    bool        setUniformFloat   ( const char * name, float value            );
    bool        setUniformFloat   ( int loc,            float value           );
    bool        setUniformMatrix  ( const char * name, const GlslMatrix4x4& value );
    bool        setUniformMatrix  ( const char * name, const GlslMatrix3D&  value );
    bool        setUniformMatrix  ( const char * name, float value [16]       );
    bool        setUniformInt     ( const char * name, int value              );
    bool        setUniformInt     ( int loc,            int value             );
	bool		getUniformInt		(const char * name, int* val);
    GlslVector4D    getUniformGlslVector  ( const char * name );
    GlslVector4D    getUniformGlslVector  ( int loc            );
    int         locForUniformName ( const char * name );

                                                        // attribute variables handling methods
    bool        setAttribute     ( const char * name, const GlslVector4D& value );
    bool        setAttribute     ( int index,         const GlslVector4D& value );
    GlslVector4D    getAttribute     ( const char * name );
    GlslVector4D    getAttribute     ( int index );
    int         indexForAttrName ( const char * name );
    bool        bindAttributeTo  ( int no, const char * name );

    bool    	setTexture ( const char * name, int texUnit );
    bool    	setTexture ( int loc,           int texUnit );

	bool		bindFragOut ( const char * name, int no )
	{
        glBindFragDataLocationEXT ( program, no, name );
		
		return true;
	}
                                                        // check whether there is a support for GLSL
    static  bool    isSupported ();
    static  string  version     ();
                                                        // some limitations on program
    static  int maxVertexUniformComponents   ();
    static  int maxVertexAttribs             ();
    static  int maxFragmentTextureUnits      ();
    static  int maxVertexTextureUnits        ();
    static  int maxCombinedTextureUnits      ();
    static  int maxVaryingFloats             ();
    static  int maxFragmentUniformComponents ();
    static  int maxTextureCoords             ();

protected:
    bool    loadShader   ( GLhandleARB shader, Data * data );
    bool    checkGlError ();
    void    loadLog      ( GLhandleARB object );
};

#endif
