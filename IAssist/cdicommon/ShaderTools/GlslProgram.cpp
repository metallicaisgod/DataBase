//
// Class to encapsulate GLSL program and shader objects and working with them
//

#include "stdafx.h"

#include    "GlslTypes.h"
#include    "GlslProgram.h"
#include    "Data.h"

GlslProgram :: GlslProgram ()
{
    program        = 0;
    vertexShader   = 0;
    fragmentShader = 0;
    ok             = false;
}

GlslProgram :: ~GlslProgram ()
{
    clear ();
}

bool    GlslProgram :: loadShaders ( const char * vertexFileName, const char * fragmentFileName )
{
    ok = false;

    Data* fragmentShaderData	= NULL;
    Data* vertexShaderData		= getFileData(vertexFileName);

	if (vertexShaderData == NULL)
	{
		log += "Cannot open \"";
		log += vertexFileName;
		log += "\"";
		goto load_exit;
	}
	else if ( !vertexShaderData -> isOk () || vertexShaderData -> isEmpty () )
    {
    	log += "Error loading vertex shader";
		releaseFileData(&vertexShaderData);
		goto load_exit;
    }

    fragmentShaderData = getFileData(fragmentFileName);

	if (fragmentShaderData == NULL)
	{
		log += "Cannot open \"";
		log += fragmentFileName;
		log += "\"";
		goto load_exit;
	}
	else if ( !fragmentShaderData -> isOk () || fragmentShaderData -> isEmpty () )
    {
    	log += "Error loading fragment shader";
		releaseFileData(&fragmentShaderData);
		goto load_exit;
    }

    ok = loadShaders(vertexShaderData, fragmentShaderData);

load_exit:
	releaseFileData(&vertexShaderData);
	releaseFileData(&fragmentShaderData);

	return ok;
}

bool    GlslProgram :: loadShaders(Data* vertexShaderData, Data* fragmentShaderData)
{
    ok = false;
                                // check whether we should create program object
    if ( program == 0 )
        program = glCreateProgramObjectARB ();

                                // check for errors
//    if ( !checkGlError () )
    if (!program)
        return false;
                                // create a vertex shader object and a fragment shader object
    if ( vertexShaderData != NULL )
    {
	    vertexShader = glCreateShaderObjectARB ( GL_VERTEX_SHADER_ARB   );

	    log += "Loading vertex shader\n";

    	if (!loadShader ( vertexShader, vertexShaderData))
        	return false;

                                // attach shaders to program object
	    glAttachObjectARB ( program, vertexShader );
    }

    if (fragmentShaderData != NULL)
    {
	    fragmentShader = glCreateShaderObjectARB ( GL_FRAGMENT_SHADER_ARB );

	    log += "Loading fragment shader\n";

    	if (!loadShader(fragmentShader, fragmentShaderData))
        	return false;
                                // attach shaders to program object
	    glAttachObjectARB ( program, fragmentShader );
    }
/*
	glBindAttribLocationARB(program, 7, "vertexColor");
    if ( !checkGlError () )
        return false;
//*/
    GLint   linked;

    log += "Linking programs\n";

                                // link the program object and print out the info log
    glLinkProgramARB ( program );

/**/  if ( !checkGlError () )     // check for errors
/**/      return false;

    glGetObjectParameterivARB ( program, GL_OBJECT_LINK_STATUS_ARB, &linked );
    if ( !linked )
        return false;

	glBindAttribLocationARB(program, 7, "vertexColor");
    if ( !checkGlError () )
        return false;


    loadLog ( program );

	return ok = true;
}

void    GlslProgram :: bind ()
{
    glUseProgramObjectARB ( program );
}

void    GlslProgram :: unbind ()
{
    glUseProgramObjectARB ( 0 );
}

bool    GlslProgram :: loadShader ( GLhandleARB shader, Data * data )
{
    const char * body = (const char *) data -> getPtr ( 0 );
    GLint		 len  = data -> getLength ();
    GLint        compileStatus;

    glShaderSourceARB ( shader, 1, &body,  &len );

                                        // compile the particle vertex shader, and print out
    glCompileShaderARB ( shader );

    //if ( !checkGlError() )              // check for OpenGL errors
    //    return false;

    glGetObjectParameterivARB ( shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus );

    loadLog ( shader );

    return compileStatus != 0;

}

bool    GlslProgram :: checkGlError ()
{
    bool    retCode = true;
 	GLenum  glErr   = glGetError();

    if ( glErr == GL_NO_ERROR )
    	return retCode;

    glError = (const char *) gluErrorString ( glErr );		// XXX: what about gluErrorString errors ?

    return false;
}

void    GlslProgram :: clear ()
{
	if (program)
		glDeleteObjectARB(program);
	if (vertexShader)
		glDeleteObjectARB(vertexShader);
	if (fragmentShader)
		glDeleteObjectARB(fragmentShader);

    program			= 0;
	vertexShader	= 0;
	fragmentShader	= 0;
    ok				= false;
}

void    GlslProgram :: loadLog ( GLhandleARB object )
{
    GLint       logLength     = 0;
    GLsizei     charsWritten  = 0;
    GLcharARB   buffer [2048];
    GLcharARB * infoLog;

    glGetObjectParameterivARB ( object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength );

    if ( !checkGlError() )          // check for OpenGL errors
        return;

    if ( logLength < 1 )
        return;
                                    // try to avoid allocating buffer
    if ( logLength > (int) sizeof ( buffer ) )
    {
        infoLog = (GLcharARB*) malloc ( logLength );

        if ( infoLog == NULL )
        {
            log = "ERROR: Could not allocate log buffer";

            return;
        }
    }
    else
        infoLog = buffer;

    glGetInfoLogARB ( object, logLength, &charsWritten, infoLog );

    log += infoLog;

    if ( infoLog != buffer )
        free ( infoLog );
}

bool    GlslProgram :: setUniformGlslVector ( const char * name, const GlslVector4D& value )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc < 0 )
        return false;

	glUniform4fvARB ( loc, 1, &value.x );

    return true;
}

bool    GlslProgram :: setUniformGlslVector  ( int loc, const GlslVector4D& value )
{
    glUniform4fvARB ( loc, 1, &value.x );

    return true;
}

bool    GlslProgram :: setUniformGlslVector ( const char * name, const GlslVector3D& value )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc < 0 )
        return false;

	glUniform3fvARB ( loc, 1, &value.x );

    return true;
}

bool    GlslProgram :: setUniformGlslVector  ( int loc, const GlslVector3D& value )
{
    glUniform3fvARB ( loc, 1, &value.x );

    return true;
}

bool    GlslProgram :: setUniformGlslVector ( const char * name, const GlslVector2D& value )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc < 0 )
        return false;

    glUniform2fvARB ( loc, 1, &value.x );

    return true;
}

bool    GlslProgram :: setUniformGlslVector  ( int loc, const GlslVector2D& value )
{
    glUniform2fvARB ( loc, 1, &value.x );

    return true;
}

bool    GlslProgram :: setUniformFloat ( const char * name, float value )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc < 0 )
        return false;

    glUniform1fARB ( loc, value );

    return true;
}

bool    GlslProgram :: setUniformFloat ( int loc, float value )
{
    glUniform1fARB ( loc, value );

    return true;
}

bool    GlslProgram :: setUniformInt ( const char * name, int value )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc < 0 )
        return false;

    glUniform1iARB ( loc, value );

    return true;
}

bool    GlslProgram :: setUniformInt ( int loc, int value )
{
    glUniform1iARB ( loc, value );

    return true;
}

bool    GlslProgram :: setUniformMatrix  ( const char * name, const GlslMatrix4x4& value )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc < 0 )
        return false;

    glUniformMatrix4fvARB ( loc, 1, GL_FALSE, value.x[0] );

    return true;
}

bool    GlslProgram :: setUniformMatrix  ( const char * name, float value [16] )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc < 0 )
        return false;

    glUniformMatrix4fvARB ( loc, 1, GL_FALSE, value );

    return true;
}

bool    GlslProgram :: setUniformMatrix  ( const char * name, const GlslMatrix3D& value )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc < 0 )
        return false;

    glUniformMatrix3fvARB ( loc, 1, GL_FALSE, value.x[0] );

    return true;
}

int     GlslProgram :: locForUniformName ( const char * name )
{
    return glGetUniformLocationARB ( program, name );
}
bool GlslProgram :: getUniformInt( const char * name, int* val)
{
    int loc = glGetUniformLocationARB ( program, name );
    if ( loc < 0 )
        return false;
	glGetUniformivARB(program, loc, val);
	return true;
}

GlslVector4D    GlslProgram :: getUniformGlslVector ( const char * name )
{
//    float   values [4];
	GlslVector4D value = {0, 0, 0, 0};

    int loc = glGetUniformLocationARB ( program, name );

	if ( loc < 0 )
	{
        return value;
	}
	glGetUniformfvARB ( program, loc, &value.x );

    return value;  // GlslVector4D ( values [0], values [1], values [2], values [3] );
}

bool    GlslProgram :: setTexture ( const char * name, int texUnit )
{
    int loc = glGetUniformLocationARB ( program, name );

    if ( loc == -1 )
        return false;

    glUniform1iARB ( loc, texUnit );

    return true;
}

bool    GlslProgram :: setTexture ( int loc, int texUnit )
{
    if ( loc < 0 )
        return false;

    glUniform1iARB ( loc, texUnit );

    return true;
}

bool    GlslProgram :: bindAttributeTo  ( int loc, const char * name )
{
    glBindAttribLocationARB ( program, loc, name );

    return true;
}

bool    GlslProgram :: setAttribute ( const char * name, const GlslVector4D& value )
{
    int index = glGetAttribLocationARB ( program, name );

    if ( index < 0 )
        return false;

    glVertexAttrib4fvARB ( index, &value.x );

    return true;
}

bool    GlslProgram :: setAttribute ( int index, const GlslVector4D& value )
{
    glVertexAttrib4fvARB ( index, &value.x );

    return true;
}

int     GlslProgram :: indexForAttrName ( const char * name )
{
    return glGetAttribLocationARB ( program, name );
}

GlslVector4D    GlslProgram :: getAttribute ( const char * name )
{
    int index = glGetAttribLocationARB ( program, name );

    if ( index < 0 )
        return GetGlslVector4D(0, 0, 0, 0);

    float   buf [4];

    glGetVertexAttribfvARB ( index, GL_CURRENT_VERTEX_ATTRIB_ARB, buf );

    return GetGlslVector4D(buf [0], buf [1], buf [2], buf [3]);
}

GlslVector4D    GlslProgram :: getAttribute ( int index )
{
    float   buf [4];

    glGetVertexAttribfvARB ( index, GL_CURRENT_VERTEX_ATTRIB_ARB, buf );

    return GetGlslVector4D(buf [0], buf [1], buf [2], buf [3]);
}

bool GlslProgram::isSupported()
{
    return glewIsSupportedStub("GL_ARB_shading_language_100") &&
           glewIsSupportedStub("GL_ARB_shader_objects") &&
           glewIsSupportedStub("GL_ARB_vertex_shader") &&
           glewIsSupportedStub("GL_ARB_fragment_shader");
}

string  GlslProgram :: version ()
{
#ifdef	MACOSX
	return "1.051";
#else
    const char * slVer = (const char *) glGetString ( GL_SHADING_LANGUAGE_VERSION_ARB );

    if ( glGetError() != GL_NO_ERROR )
        return "1.051";

    return string ( slVer );
#endif
}

int     GlslProgram :: maxVertexUniformComponents ()
{
    GLint maxVertexUniformComponents;

    glGetIntegerv ( GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &maxVertexUniformComponents );

    return maxVertexUniformComponents;
}

int     GlslProgram :: maxVertexAttribs ()
{
    GLint maxVertexAttribs;

    glGetIntegerv ( GL_MAX_VERTEX_ATTRIBS_ARB, &maxVertexAttribs );

    return maxVertexAttribs;
}

int     GlslProgram :: maxFragmentTextureUnits ()
{
    GLint maxFragmentTextureUnits;

    glGetIntegerv ( GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &maxFragmentTextureUnits );

    return maxFragmentTextureUnits;
}

int     GlslProgram :: maxVertexTextureUnits ()
{
    GLint maxVertexTextureUnits;

    glGetIntegerv ( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, &maxVertexTextureUnits );

    return maxVertexTextureUnits;
}

int     GlslProgram :: maxCombinedTextureUnits ()
{
    GLint maxCombinedTextureUnits;

    glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &maxCombinedTextureUnits );

    return maxCombinedTextureUnits;
}

int     GlslProgram :: maxVaryingFloats ()
{
    GLint maxVaryingFloats;

    glGetIntegerv ( GL_MAX_VARYING_FLOATS_ARB, &maxVaryingFloats );

    return maxVaryingFloats;
}

int     GlslProgram :: maxFragmentUniformComponents ()
{
    GLint maxFragmentUniformComponents;

    glGetIntegerv ( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB, &maxFragmentUniformComponents );

    return maxFragmentUniformComponents;
}

int     GlslProgram :: maxTextureCoords ()
{
    GLint maxTextureCoords;

    glGetIntegerv ( GL_MAX_TEXTURE_COORDS_ARB, &maxTextureCoords );

    return maxTextureCoords;
}
