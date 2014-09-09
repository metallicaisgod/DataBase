
#include "stdafx.h"

#include "ialang_res.h"

#define USE_GLEW
//#ifdef USE_GLEW

//#ifdef __cplusplus
//extern "C" {
//#endif
//	#include <glew.h>
//	#include "GlewStub.h"
//#ifdef __cplusplus
//}
//#endif

#include    "GlslTypes.h"
#include    "GlslProgram.h"


/*
#else
	#include <gl.h>
	#include <glext.h>
#endif
//*/
#include "RenderSysState.h"

//////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_CONTEXT_PARAMS\
	HGLRC currRC = NULL;\
	HDC currDC = NULL;\
	BOOL currContextChanged = FALSE;

#define SET_RENDER_CONTEXT(hDC, hRC)\
	currRC = wglGetCurrentContext();\
	if (currRC != hRC)\
	{\
		currDC = wglGetCurrentDC();\
		if (::wglMakeCurrent(hDC, hRC))\
			currContextChanged = TRUE;\
	}

#define RESET_RENDER_CONTEXT\
	if (currContextChanged)\
	{\
		::wglMakeCurrent(currDC, currRC);\
	}

const TCHAR* UNDEFINED_ERROR = L"UNDEFINED ERROR";

//////////////////////////////////////////////////////////////////////////////////////////////
int				RenderSystemState::m_renderMode;		
//
// for order independent transparency (OITransparency)
int				RenderSystemState::m_transparencyMode;
GLuint			RenderSystemState::m_shadowMap;
int				RenderSystemState::m_shadowMapSize	= 512;
bool			RenderSystemState::m_drawAllLayers	= false;
OITBlendingMode	RenderSystemState::m_blendingMode	= SCENE_VIEWER_BLEND_NORMAL;

float			RenderSystemState::m_lowTransparency	= 0.85f;
float			RenderSystemState::m_mediumTransparency	= 0.55f;
float			RenderSystemState::m_highTransparency	= 0.25f;
int				RenderSystemState::m_layers				= 5;
int				RenderSystemState::m_transpRotate		= 0;

static			GlslProgram		s_program;

//===========================================================================
RenderSystemState::RenderSystemState()
{
	m_renderMode		= SOFTWARE_MODE;
	m_transparencyMode	= SOFTWARE_TRANSPARENCY;
}
//===========================================================================
RenderSystemState::~RenderSystemState()
{
	m_renderMode	= NONE_MODE;
}
//===========================================================================
BOOL RenderSystemState::SetLastError(int code, RenderSysError* renderError)
{
	if (!renderError)
		return FALSE;

	renderError->code = code;
	switch (code)
	{
	case RS_OK:
        renderError->errInfo = "";
		break;
	case RS_NO_GLEW_LIB:
		renderError->errInfo.LoadString(IDS_RE_GLEW_ERROR);
		break;
	case RS_BAD_GLSL_PARAM:
		renderError->errInfo.LoadString(IDS_RE_GLSL_ERROR);
		break;
	case RS_NOT_INIT_OIT:
		renderError->errInfo.LoadString(IDS_RE_OIT_ERROR);
		break;
	case RS_NO_VBO_EXT:
		renderError->errInfo.LoadString(IDS_RE_VBO_EROR);
		break;
	default:
		renderError->errInfo = UNDEFINED_ERROR;
	}

	return TRUE;
}
//===========================================================================
BOOL RenderSystemState::GetRenderSystemInfo(HGLRC hRC, HDC hDC, RenderSystemInfo& sysInfo)
{
	DECLARE_CONTEXT_PARAMS;

	if ( !hRC || !hDC)
	{
		return FALSE;
	}
	SET_RENDER_CONTEXT(hDC, hRC);
	size_t len = 0;
	const char* tmpStr = (const char*)glGetString(GL_VENDOR);
	if (tmpStr)
	{
		::strncpy(sysInfo.glVendorName, tmpStr, STRING_INFO_SIZE);
	}
	else
	{
		GLenum err = glGetError();
		if (GL_INVALID_OPERATION  == err)
		{
			TRACE("GetRenderSystemInfo failed: GL_INVALID_OPERATION 0x%X\r\n",err) ;
		}
		else if (GL_INVALID_ENUM  == err)
		{
			TRACE("GetRenderSystemInfo failed: GL_INVALID_OPERATION 0x%X\r\n",err) ;
		}
		else
		{
			TRACE("GetRenderSystemInfo failed: glError 0x%X\r\n",err) ;
		}
		RESET_RENDER_CONTEXT;
		return FALSE;
	}

	tmpStr = (const char*)glGetString(GL_RENDERER);
	if (tmpStr)
	{
		::strncpy(sysInfo.glRendererName, tmpStr, STRING_INFO_SIZE);
	}

	tmpStr = (const char*)glGetString(GL_VERSION);
	if (tmpStr)
	{
		::strncpy(sysInfo.glVersion, tmpStr, STRING_INFO_SIZE);
	}

	RESET_RENDER_CONTEXT;

	return TRUE;
}
//===========================================================================
BOOL RenderSystemState::isExtensionAvailable(HGLRC hRC, HDC hDC)
{
	DECLARE_CONTEXT_PARAMS;
	if ( !hRC || !hDC)
	{
		TRACE("Bad parameters of isExtensionAvailable: hRC=0x%X; hDC=0x%X\r\n", hRC, hDC) ;
		return FALSE;
	}
	SET_RENDER_CONTEXT(hDC, hRC);
	BOOL availableGlew = glewInitStub();
	RESET_RENDER_CONTEXT;

	if (availableGlew && GLEW_ARB_vertex_buffer_object)
		return TRUE;
	else
		return FALSE;
}
//===========================================================================
BOOL RenderSystemState::SetPixelFormat(HDC hDC, int renderMode)
{
	BOOL res = TRUE;
	ASSERT(hDC != NULL);
	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR)) ;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); 
	pfd.nVersion = 1;							// Version number
	switch (renderMode)
	{
	case VBO_USE_MODE:
		pfd.dwFlags =  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
		pfd.cColorBits = 24;				// (BYTE)bmInfo.bmBitsPixel;
		pfd.cAlphaBits = 8;
		break;
	case PRINTER_MODE:
		{
			CDC *cDC = CDC::FromHandle(hDC);
			ASSERT(cDC != NULL);
			CBitmap* pBitmap = cDC->GetCurrentBitmap();
			ASSERT(pBitmap != NULL);
			BITMAP bmInfo;
			ZeroMemory(&bmInfo, sizeof(BITMAP));
			ASSERT(0 != pBitmap->GetObject(sizeof(BITMAP), &bmInfo));
			pfd.dwFlags =  PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI | PFD_DRAW_TO_BITMAP;
			pfd.cColorBits = (BYTE)bmInfo.bmBitsPixel;
		}
		break;
	case SOFTWARE_MODE:
		{
			pfd.dwFlags =  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
			pfd.cColorBits = 24;				// (BYTE)bmInfo.bmBitsPixel;
			pfd.cAlphaBits = 8;
		}
		break;
	default:
		{
			TRACE("Render Mode failed %d\r\n",renderMode) ;
			return FALSE;
		}
	}
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cDepthBits = 32;				// 32-bit depth buffer
	pfd.iLayerType = PFD_MAIN_PLANE;	// Layer type

// Choose the pixel format.
	int nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (nPixelFormat == 0)
	{
		TRACE("ChoosePixelFormat failed: err = 0x%X\r\n",GetLastError()) ;
		return FALSE;
	}
	TRACE("Pixel Format %d\r\n", nPixelFormat) ;

// Set the pixel format.
	res = ::SetPixelFormat(hDC, nPixelFormat, &pfd);
	if (!res)
		TRACE("SetPixelFormat failed: err = 0x%X\r\n",GetLastError()) ;
	return res;
}
//===========================================================================
HGLRC RenderSystemState::CreateRenderContext(HDC hDC, int renderMode)
{
	ASSERT(hDC != NULL);
	
	if (!RenderSystemState::SetPixelFormat(hDC, renderMode))
		return NULL;
//
// Create a rendering context.
	HGLRC hRC = ::wglCreateContext(hDC);
	if (!hRC)
	{
		TRACE("wglCreateContext failed: err = 0x%X\r\n",GetLastError()) ;
		return NULL;
	}

	if (!::wglMakeCurrent(hDC, hRC))
	{
		::wglDeleteContext(hRC);
		TRACE("wglMakeCurrent failed: err = 0x%X\r\n",GetLastError()) ;
		return NULL;
	}
	
	::glShadeModel(GL_SMOOTH);
//
// Select The Modelview Matrix
	::glMatrixMode(GL_MODELVIEW);							
	::glLoadIdentity();
			
	::glEnable(GL_COLOR_MATERIAL);
	::glEnable(GL_LIGHTING);
	::glEnable(GL_LIGHT0);
	::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	GLfloat amblight[4] = {0.1f, 0.1f, 0.1f, 1.0f};
	::glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amblight );

	GLfloat diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
	::glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	
	wglMakeCurrent(NULL,NULL);

	return hRC;
}
//===========================================================================
BOOL RenderSystemState::DeleteRenderContext(HGLRC hRC)
{
	if (!hRC)
		return TRUE;

	HGLRC currDC = wglGetCurrentContext();
	if (currDC == hRC)
		::wglMakeCurrent(NULL, NULL);  

	BOOL retVal = ::wglDeleteContext(hRC);
	if (!retVal)
	{
		TRACE("wglDeleteContext failed: error 0x%X\r\n", ::GetLastError()) ;
	}
	return retVal;
}
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_GLEW
//===========================================================================
int RenderSystemState::SetObjectTransparancy(float alpha, size_t objVertexOffset, size_t objVertexSize, VBOBufferDescr& buffDescr)
{
	int retVal = RS_OK;
	size_t verBuffSize = buffDescr.verticesSize * sizeof(float) * 3;
	size_t norBuffSize = buffDescr.normalsSize * sizeof(float) * 3;
	size_t colBuffSize = objVertexSize * sizeof(float) * 4;
	size_t offsetColor = objVertexOffset * sizeof(float) * 4;
	BYTE* objColors = (BYTE*)buffDescr.colors + offsetColor;

	float* tmp = (float*)(objColors + sizeof(float) * 3);
	for (size_t i=0; i<objVertexSize; i++)
	{
		*tmp = alpha;
		tmp += 4;
	}

	glBindBufferARB(GL_ARRAY_BUFFER, buffDescr.verBuffId);
	glBufferSubDataARB(GL_ARRAY_BUFFER, verBuffSize + norBuffSize + offsetColor, colBuffSize, objColors);
	glBindBufferARB(GL_ARRAY_BUFFER, 0);

	return retVal;
}
//===========================================================================
int RenderSystemState::InitVBO(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr)
{
	DECLARE_CONTEXT_PARAMS

	int retVal = RS_OK;
	if (!buffDescr.colorsSize || !buffDescr.indexesSize 
		|| !buffDescr.normalsSize || !buffDescr.verticesSize
		|| !hRC || !hDC)
	{
		retVal = RS_BAD_PARAMETERS;
		goto exit0;
	}

	if (buffDescr.indBuffId != 0 && buffDescr.verBuffId != 0)
	{
		retVal = RS_OK;
		goto exit0;
	}
	
	SET_RENDER_CONTEXT(hDC, hRC);

	if (!buffDescr.availableGlew)
	{
		if (glewInitStub())
			buffDescr.availableGlew	= TRUE;
		else
		{
			retVal = RS_NO_GLEW_LIB;
			goto exit0;
		}
	}

	if (GLEW_ARB_vertex_buffer_object)
	{		
		size_t verBuffSize = buffDescr.verticesSize * sizeof(float) * 3;
		size_t norBuffSize = buffDescr.normalsSize * sizeof(float) * 3;
		size_t colBuffSize = buffDescr.colorsSize * sizeof(float) * 4;
		size_t size = verBuffSize + norBuffSize + colBuffSize;
		
		if (!glIsBufferARB(buffDescr.verBuffId))
		{
// create vertex data VBO
			glGenBuffersARB(1, &buffDescr.verBuffId);
			glBindBufferARB(GL_ARRAY_BUFFER, buffDescr.verBuffId);
			glBufferDataARB(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);
// copy vertices starting from 0 offest
// typedef void (GLAPIENTRY * PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid* data);
			glBufferSubDataARB(GL_ARRAY_BUFFER, 0, verBuffSize, buffDescr.vertices);
// copy normals after vertices
			glBufferSubDataARB(GL_ARRAY_BUFFER, verBuffSize, norBuffSize, buffDescr.normals);
// copy colours after normals
			glBufferSubDataARB(GL_ARRAY_BUFFER, verBuffSize + norBuffSize, colBuffSize, buffDescr.colors);
		}
		int buffSize = 0;
		glGetBufferParameterivARB(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffSize);
		if (size != buffSize)
		{
			ReleaseVBO(hRC, hDC, buffDescr);
			retVal = RS_NO_MEMORY;
			goto exit0;
		}

		if (!glIsBufferARB(buffDescr.indBuffId))
		{
// create index data VBO
			glGenBuffersARB(1, &buffDescr.indBuffId);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, buffDescr.indBuffId);
			size = buffDescr.indexesSize * sizeof(int);
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);
// copy indexes
			glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER, 0, size, buffDescr.indexes);
		}
		buffSize = 0;
		glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffSize);
		if (size != buffSize)
		{
			ReleaseVBO(hRC, hDC, buffDescr);
			retVal = RS_NO_MEMORY;
			goto exit0;
		}
		buffDescr.maxIndicesPart = 0;
		glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &buffDescr.maxIndicesPart);
		buffDescr.maxIndicesPart = (buffDescr.maxIndicesPart / 3) * 3; 
		retVal = RS_OK;
	}
	else 
	{
		retVal = RS_NO_VBO_EXT;
		goto exit0;
	}

exit0:
//
// unbind buffers
	if (GLEW_ARB_vertex_buffer_object)
	{
		glBindBufferARB(GL_ARRAY_BUFFER, 0);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (RS_OK == retVal)
		buffDescr.isChanged = FALSE;

	RESET_RENDER_CONTEXT;

	return retVal;
}
//===========================================================================
void RenderSystemState::ReleaseVBO(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr)
{
	DECLARE_CONTEXT_PARAMS;
	if ( !hRC || !hDC)
		return;
	SET_RENDER_CONTEXT(hDC, hRC);

	if (buffDescr.availableGlew && GLEW_ARB_vertex_buffer_object)
	{
		glDeleteBuffersARB(1, &buffDescr.verBuffId);
		glDeleteBuffersARB(1, &buffDescr.indBuffId);
	}
	buffDescr.verBuffId = buffDescr.indBuffId = 0;
	buffDescr.maxIndicesPart = 0;
	RESET_RENDER_CONTEXT;
}
//===========================================================================
BOOL RenderSystemState::isVBOAvailable(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr)
{
	DECLARE_CONTEXT_PARAMS;
	if ( !hRC || !hDC)
		return FALSE;
	SET_RENDER_CONTEXT(hDC, hRC);
	if (!buffDescr.availableGlew || !GLEW_ARB_vertex_buffer_object ||
		!glIsBufferARB(buffDescr.verBuffId) || !glIsBufferARB(buffDescr.indBuffId))
	{
		buffDescr.verBuffId = 0;
		buffDescr.indBuffId = 0;
		buffDescr.maxIndicesPart = 0;
		RESET_RENDER_CONTEXT;
		return FALSE;
	}
	else
	{
		RESET_RENDER_CONTEXT;
		return TRUE;
	}
}
//===========================================================================
int RenderSystemState::InitGLSL(OITransparencyData* oitData)
{
	int retVal = RS_OK;

	GlslProgram* program = (GlslProgram*)oitData->program;
	if (program)
	{
		program->clear();
	}

	program = new GlslProgram;
	if (!program)
		return FALSE;
	oitData->program = program;

	BOOL availableGlew = glewInitStub();
	if (!availableGlew || !GlslProgram::isSupported())
	{
		TRACE("GLSL not supported.\n");
		return RS_NO_TRANSPARENCY;
	}
	
	if (!program->loadShaders("Lighting.vsh", "Lighting.fsh"))
	{
		TRACE("Error loading shaders:\n%s\n", program->getLog().c_str());
		return RS_NO_TRANSPARENCY;
	}
    program->bind();
	program->setTexture("depthMap", 1);
	program->unbind();
	return retVal;
}
//===========================================================================
int RenderSystemState::BindGLSL(OITransparencyData* oitData)
{
	ASSERT(oitData != NULL);
	GlslProgram* program = (GlslProgram*)oitData->program;
	ASSERT(program != NULL);
    program->bind();
	return RS_OK;
}
//===========================================================================
int RenderSystemState::UnbindGLSL(OITransparencyData* oitData)
{
	ASSERT(oitData != NULL);
	GlslProgram* program = (GlslProgram*)oitData->program;
	ASSERT(program != NULL);
	program->unbind();
	return RS_OK;
}
//===========================================================================
int RenderSystemState::SetUniformFloat(const char* name, float value, OITransparencyData* oitData)
{
	ASSERT(oitData != NULL);
	GlslProgram* program = (GlslProgram*)oitData->program;
	ASSERT(program != NULL);
	if (program->setUniformFloat(name, value))
		return RS_OK;
	else
		return RS_BAD_GLSL_PARAM;
}
//===========================================================================
int RenderSystemState::SetUniformInt(const char* name, int value, OITransparencyData* oitData)
{
	ASSERT(oitData != NULL);
	GlslProgram* program = (GlslProgram*)oitData->program;
	ASSERT(program != NULL);
	if (program->setUniformInt(name, value))
		return RS_OK;
	else
		return RS_BAD_GLSL_PARAM;
}
//===========================================================================
int RenderSystemState::GetUniformInt(const char* name, int* value, OITransparencyData* oitData)
{
	ASSERT(oitData != NULL);
	GlslProgram* program = (GlslProgram*)oitData->program;
	ASSERT(program != NULL);
	if (program->getUniformInt(name, value))
		return RS_OK;
	else
		return RS_BAD_GLSL_PARAM;
}
//===========================================================================
int RenderSystemState::GetAttribLocation(const char* name, OITransparencyData* oitData)
{
	ASSERT(oitData != NULL);
	GlslProgram* program = (GlslProgram*)oitData->program;
	ASSERT(program != NULL);
	return program->indexForAttrName(name);
}
//===========================================================================
int RenderSystemState::SetLayerIndex(int layerIndex, OITransparencyData* oitData)
{
	ASSERT(oitData != NULL);
	GlslProgram* program = (GlslProgram*)oitData->program;
	ASSERT(program != NULL);
	if (program->setUniformInt("layerIndex", layerIndex))
		return RS_OK;
	else
		return RS_BAD_GLSL_PARAM;
}
//===========================================================================
void RenderSystemState::ReleaseGLSL(OITransparencyData* oitData)
{
	if (oitData == NULL)
		return;
	GlslProgram* program = (GlslProgram*)oitData->program;
	if (program == NULL)
		return;
	program->clear();
	delete program;
	program = NULL;
}
//===========================================================================
int RenderSystemState::InitOITransparency(HGLRC hRC, HDC hDC, OITransparencyData** oitData, int width, int height, int layers)
{
	if ( !hRC || !hDC || !oitData)
	{
		TRACE("Bad parameters of InitOITransparency: hRC=0x%X; hDC=0x%X; oitData=0x%X\r\n", hRC, hDC, oitData) ;
		return RS_BAD_GLSL_PARAM;
	}
	DECLARE_CONTEXT_PARAMS;
	SET_RENDER_CONTEXT(hDC, hRC);

	int usingStencilOverlay = 0;
	OITransparencyData* tmpData = *oitData;
	if (!tmpData)
	{
		tmpData = new OITransparencyData;
		if (!tmpData)
		{
			RESET_RENDER_CONTEXT;
			return RS_NO_MEMORY;
		}
		::memset(tmpData, 0, sizeof(OITransparencyData));
	}
	else
	{
		ClearOITransparency(tmpData);
	}

	if (RS_OK != InitGLSL(tmpData))
	{
		RESET_RENDER_CONTEXT;
		return RS_NO_TRANSPARENCY;
	}
	tmpData->zTextureID = 0;
	tmpData->rgbaTextureID = (GLuint *)NULL;
	tmpData->viewportWidth = width;
	tmpData->viewportHeight = height;
//	Need one more layer for the background
	tmpData->layersNumber = layers + 1;
	tmpData->usingStencilOverlay = usingStencilOverlay;
	tmpData->maxLayers = 0;
	tmpData->zBuffer = (GLuint *)NULL;
//
// Get size of alpha [blending] buffer.
// glGetIntegerv(GL_ALPHA_BITS,&alpha_bits);
// turn on alpha
	switch(m_blendingMode)
	{
		default:
			m_blendingMode = SCENE_VIEWER_BLEND_NORMAL;
		case SCENE_VIEWER_BLEND_NORMAL:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
		case SCENE_VIEWER_BLEND_NONE:
			glDisable(GL_BLEND);
			 break;
//#if defined GL_VERSION_1_4
		case SCENE_VIEWER_BLEND_TRUE_ALPHA:
// This function is protected at runtime by testing in the set blending mode function
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			break;
//#endif // defined GL_VERSION_1_4 
	}

	int res = 1;
	GLint depthBits;
	glEnable(GL_DEPTH_TEST);
	glGetIntegerv(GL_DEPTH_BITS, &depthBits);
	switch (depthBits)
	{
		case 16:
			tmpData->depthFormat = GL_DEPTH_COMPONENT16_ARB;
			break;
		case 24:
			tmpData->depthFormat = GL_DEPTH_COMPONENT24_ARB;
			break;
		default:
			//display_message(ERROR_MESSAGE, "order_independent_initialise.  "
			//	"Unsupported depth format for order independent transparency");
			res = 0;
	}

	if (res)
	{
		GLint alphaBits;
		glEnable(GL_ALPHA_TEST);
		glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
		if (alphaBits < 8)
		{
			//display_message(ERROR_MESSAGE, "order_independent_initialise.  "
			//	"This extension requires alpha planes to work, alpha_bits = %d",
			//	alpha_bits);
			res = 0;
		}
	}

	if (!res)
	{
		ReleaseOITransparency(&tmpData);
	}
	else
	{
		glGenTextures(1, &tmpData->zTextureID);
		glActiveTextureARB(GL_TEXTURE3_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tmpData->zTextureID);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_COMPARE_FUNC, GL_GREATER);

//		glEnable(GL_DEPTH_TEST);

		glGenTextures   ( 1, &tmpData->shadowMap );
		glBindTexture   ( GL_TEXTURE_2D, tmpData->shadowMap );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		glTexParameteri  ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB );
		glTexParameteri  ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL );

		glTexImage2D     ( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, tmpData->viewportWidth, tmpData->viewportHeight, 0,
						   GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );
		
		if (RS_OK != OITReshape(tmpData, tmpData->viewportWidth, tmpData->viewportHeight, layers))
		{
			ReleaseOITransparency(&tmpData);
		}
		else
			tmpData->notNeedInit = true;

		*oitData = tmpData;
	}

	RESET_RENDER_CONTEXT;

	return (tmpData) ? RS_OK : RS_NOT_INIT_OIT;
}
//===========================================================================
int RenderSystemState::ReleaseOITransparency(OITransparencyData** oitData)
{
	if (!oitData || !*oitData)
		return RS_OK;

	OITransparencyData* tmpData = *oitData;
	ClearOITransparency(tmpData);
	delete tmpData;
	(*oitData) = NULL;

	return RS_OK;
}
//===========================================================================
int RenderSystemState::ClearOITransparency(OITransparencyData* oitData)
{
	if (!oitData)
		return RS_OK;

	glDeleteTextures(1, &oitData->shadowMap);
	glDeleteTextures(1, &oitData->zTextureID);
	if (oitData->rgbaTextureID)
	{
		for (int i=0; i<oitData->layersNumber; i++)
		{
			glDeleteTextures(1, &(oitData->rgbaTextureID[i]));
		}
		delete [] oitData->rgbaTextureID;
	}
	delete [] oitData->zBuffer;
	ReleaseGLSL(oitData);
	::memset(oitData, 0, sizeof(OITransparencyData));

	return RS_OK;
}
//===========================================================================
int RenderSystemState::OITReshape(OITransparencyData* oitData, int width, int height, int layers)
{
	if (!oitData || !oitData->zTextureID)
	{
		return RS_NO_TRANSPARENCY;
	}

	int i;
// We need one more layer for the background
	layers++;
	if (oitData->zTextureID && oitData->zBuffer && (oitData->viewportWidth == width)
		&& (oitData->viewportHeight == height) && (layers <= oitData->maxLayers))
	{
		oitData->layersNumber = layers;
	}
	else
	{
		glActiveTextureARB(GL_TEXTURE3_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, oitData->zTextureID);
		
		delete [] oitData->zBuffer;
		oitData->zBuffer = new GLuint[width * height];
		if (!oitData->zBuffer)
			return RS_NO_MEMORY;

		memset(oitData->zBuffer, 0, sizeof(GLuint) * width * height);
		oitData->viewportWidth = width;
		oitData->viewportHeight = height;
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, oitData->depthFormat, 
			width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, oitData->zBuffer);
		glActiveTextureARB(GL_TEXTURE0);

		if (!oitData->rgbaTextureID || layers > oitData->maxLayers)
		{
			if (oitData->rgbaTextureID)
			{
				for (i=0; i<oitData->layersNumber; i++)
				{
					glDeleteTextures(1, &(oitData->rgbaTextureID[i]));
				}
				delete [] oitData->rgbaTextureID;
			}
			oitData->rgbaTextureID = new GLuint[layers];
			if (!oitData->rgbaTextureID)
				return RS_NO_MEMORY;
			for (i = oitData->maxLayers; i<layers ; i++)
			{
				glGenTextures(1, &(oitData->rgbaTextureID[i]));
			}
			oitData->maxLayers = layers;
		}

		oitData->layersNumber = layers;
		for (i=0 ; i<oitData->maxLayers; i++)
		{
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, oitData->rgbaTextureID[i]);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, width, height, 0, 
				GL_RGBA, GL_UNSIGNED_BYTE, oitData->zBuffer);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	oitData->notNeedReshape = true;
	return RS_OK;
} 
static int tmpI = 1; 
//===========================================================================
int RenderSystemState::OITDraw(OITransparencyData* oitData, int layers)
{
	int i;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, oitData->viewportWidth, 0, oitData->viewportHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	glActiveTextureARB(GL_TEXTURE0);

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);

	if (oitData->usingStencilOverlay)
	{
// disable stencil buffer to get the overlay in the back plane
		glDisable(GL_STENCIL_TEST);
	}

	switch(m_blendingMode)
	{
		default:
		case SCENE_VIEWER_BLEND_NORMAL:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
		case SCENE_VIEWER_BLEND_NONE:
			glDisable(GL_BLEND);
			break;
#if defined GL_VERSION_1_4
		case SCENE_VIEWER_BLEND_TRUE_ALPHA:
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
				GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			break;
#endif /* defined GL_VERSION_1_4 */
	}

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//*
	for (i=layers-1; i>=0; i--)
/*/
	i = tmpI;
	if (-1 != i)
//*/
	{
		if (oitData->layersNumber - 2 == i)
		{
			if (oitData->usingStencilOverlay)
			{
				glEnable(GL_STENCIL_TEST);
			}
		}

		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, oitData->rgbaTextureID[i]);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		glTexCoord2f(0, (GLfloat)oitData->viewportHeight);
		glVertex2f(0, (GLfloat)oitData->viewportHeight);
		glTexCoord2f((GLfloat)oitData->viewportWidth, (GLfloat)oitData->viewportHeight);
		glVertex2f((GLfloat)oitData->viewportWidth, (GLfloat)oitData->viewportHeight);
		glTexCoord2f((GLfloat)oitData->viewportWidth, 0);
		glVertex2f((GLfloat)oitData->viewportWidth, 0);
		glEnd();
	}

	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	return RS_OK;
}

//===========================================================================
float RenderSystemState::GetTransparencyLevel(int opacityLevel)
{
	switch (opacityLevel)
	{
	case 2:
		return m_lowTransparency;
		break;
	case 1:
		return m_mediumTransparency;
		break;
	case 0:
		return m_highTransparency;
		break;
	default:
		return 1.0f;
	}
}
//===========================================================================
void RenderSystemState::SetTransparencyLevel(int opacityLevel, float transparencyLevel)
{
	switch (opacityLevel)
	{
	case 2:
		m_lowTransparency = transparencyLevel;
		break;
	case 1:
		m_mediumTransparency = transparencyLevel;
		break;
	case 0:
		m_highTransparency = transparencyLevel;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
#else
//===========================================================================
int RenderSystemState::InitVBO(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr)
{
	size_t verBuffSize = buffDescr.verticesSize * sizeof(float) * 3;
	size_t norBuffSize = buffDescr.normalsSize * sizeof(float) * 3;
	size_t colBuffSize = buffDescr.colorsSize * sizeof(float) * 4;
	size_t size = verBuffSize + norBuffSize + colBuffSize;
	
	if (!glIsBuffer(buffDescr.verBuffId))
	{
// create vertex data VBO
		glGenBuffers(1, &buffDescr.verBuffId);
		glBindBuffer(GL_ARRAY_BUFFER, buffDescr.verBuffId);
		glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);
// copy vertices starting from 0 offest
		glBufferSubData(GL_ARRAY_BUFFER, 0, verBuffSize, buffDescr.vertices);
// copy normals after vertices
		glBufferSubData(GL_ARRAY_BUFFER, verBuffSize, norBuffSize, buffDescr.normals);
// copy colours after normals
		glBufferSubData(GL_ARRAY_BUFFER, verBuffSize + norBuffSize, colBuffSize, buffDescr.colors);
	}
	int buffSize = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffSize);
	if (size != buffSize)
	{
		ReleaseVBO(buffDescr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return RS_NO_MEMORY;
	}

	if (!glIsBuffer(buffDescr.indBuffId))
	{
// create index data VBO
		glGenBuffers(1, &buffDescr.indBuffId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffDescr.indBuffId);
		size = buffDescr.indexesSize * sizeof(int);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);
// copy indexes
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, buffDescr.indexes);
	}
	buffSize = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffSize);
	if (size != buffSize)
	{
		ReleaseVBO(buffDescr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return RS_NO_MEMORY;
	}
//
// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return RS_OK;
}
//===========================================================================
void RenderSystemState::ReleaseVBO(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr)
{
	glDeleteBuffers(1, &buffDescr.verBuffId);
	glDeleteBuffers(1, &buffDescr.indBuffId);
}
//===========================================================================
BOOL RenderSystemState::isVBOAvailable(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr)
{
	if (!glIsBuffer(buffDescr.verBuffId) || !glIsBuffer(buffDescr.indBuffId))
	{
		buffDescr.verBuffId = 0;
		buffDescr.indBuffId = 0;
		return FALSE;
	}
	else
		return TRUE;
}
#endif
//////////////////////////////////////////////////////////////////////////////////////////////

