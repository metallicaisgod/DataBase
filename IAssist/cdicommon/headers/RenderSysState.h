#pragma once

#include <windows.h>

//////////////////////////////////////////////////////////////////////////////////////////////
#define		STRING_INFO_SIZE		64

//////////////////////////////////////////////////////////////////////////////////////////////
// VBO buffer description
typedef struct
{
	GLvoid*			vertices;
	unsigned int	verticesSize;
	GLvoid*			normals;
	unsigned int	normalsSize;
	GLvoid*			colors;
	unsigned int	colorsSize;
	unsigned int*	indexes;
	unsigned int	indexesSize;
	unsigned int	verBuffId;
	unsigned int	indBuffId;
	GLint			maxIndicesPart;
	BOOL			availableGlew;
	BOOL			isChanged;
}VBOBufferDescr;

// Render System Information
typedef struct
{
	char	glVendorName[STRING_INFO_SIZE];
	char	glRendererName[STRING_INFO_SIZE];
	char	glVersion[STRING_INFO_SIZE];
}RenderSystemInfo;

//////////////////////////////////////////////////////////////////////////////////////////////
// for order independent transparency (OITransparency)
typedef void* GLSL_PROGRAM;
typedef struct
{
	GLuint			zTextureID;
	GLuint*			rgbaTextureID;
	int				viewportWidth;
	int				viewportHeight;
	int				usingStencilOverlay;
	int				layersNumber;
	int				maxLayers;
	GLuint*			zBuffer;	
	GLenum			depthFormat;
	GLuint			shadowMap;
	GLSL_PROGRAM	program;
	bool			notNeedInit;
	bool			notNeedReshape;
}OITransparencyData;

typedef enum
{
	SCENE_VIEWER_BLEND_NONE,
	SCENE_VIEWER_BLEND_NORMAL,
	SCENE_VIEWER_BLEND_TRUE_ALPHA
} OITBlendingMode;


typedef struct
{
	int		code;
    QString	errInfo;
}RenderSysError;

//////////////////////////////////////////////////////////////////////////////////////////////
class RenderSystemState
{
public:
	enum RENDER_SYS_MODE
	{
		NONE_MODE,
		PRINTER_MODE,
		SOFTWARE_MODE,
		VBO_USE_MODE,
	};
	enum TRANSPARENCY_SYS_MODE
	{
		NO_TRANSPARENCY,
		SOFTWARE_TRANSPARENCY,
		HARDWARE_TRANSPARENCY,
	};

	enum RENDER_SYS_ERROR
	{
		RS_OK,
		RS_ERR,
		RS_NO_GLEW_LIB,
		RS_NO_VBO_EXT,
		RS_BAD_PARAMETERS,
		RS_NO_MEMORY,
		RS_NO_TRANSPARENCY,
		RS_NOT_INIT_OIT,
		RS_BAD_GLSL_PARAM,
	};
public:
				RenderSystemState();
				~RenderSystemState();
static	BOOL	SetLastError(int code, RenderSysError* renderError);
static	BOOL	GetRenderSystemInfo(HGLRC hRC, HDC hDC, RenderSystemInfo& sysInfo);
static	BOOL	isExtensionAvailable(HGLRC hRC, HDC hDC);
static	void	SetRenderSystemMode(RENDER_SYS_MODE mode){m_renderMode = mode;};
static	int		GetRenderSystemMode(){return m_renderMode;};
static	int		InitVBO(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr);
static	void	ReleaseVBO(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr);
static	BOOL	isVBOAvailable(HGLRC hRC, HDC hDC, VBOBufferDescr& buffDescr);
static	HGLRC	CreateRenderContext(HDC hDC, int renderMode);
static	BOOL	DeleteRenderContext(HGLRC hRC);
static	int		SetObjectTransparancy(float alpha, size_t objVertexOffset, size_t objVertexSize, VBOBufferDescr& buffDescr);

protected:
static	BOOL	SetPixelFormat(HDC hDC, int renderMode);

protected:
static	int				m_renderMode;

//-----------------------------------------------------------------------------
// for transparency
protected:
static	int				m_transpRotate;
static	int				m_layers;
static	float			m_lowTransparency;
static	float			m_mediumTransparency;
static	float			m_highTransparency;
public:
static	int		GetTranspRotate(){return m_transpRotate;};
static	void	SetTranspRotate(int transpRotate){m_transpRotate = transpRotate;};
static	int		GetTransparencyLayers(){return m_layers;};
static	void	SetTransparencyLayers(int layers){m_layers = layers;};
static	float	GetTransparencyLevel(int opacityLevel);
static	void	SetTransparencyLevel(int opacityLevel, float transparencyLevel);

//-----------------------------------------------------------------------------
// for order independent transparency (OITransparency)
public:
static	int		ClearOITransparency(OITransparencyData* oitData);
static	void	ReleaseGLSL(OITransparencyData* oitData);
static	int		BindGLSL(OITransparencyData* oitData);
static	int		UnbindGLSL(OITransparencyData* oitData);
static	int		SetUniformFloat(const char* name, float value, OITransparencyData* oitData);
static	int		SetUniformInt(const char* name, int value, OITransparencyData* oitData);
static	int		GetUniformInt(const char* name, int* value, OITransparencyData* oitData);
static	int		GetAttribLocation(const char* name, OITransparencyData* oitData);
static	int		SetLayerIndex(int layerIndex, OITransparencyData* oitData);
static	int		InitOITransparency(HGLRC hRC, HDC hDC, OITransparencyData** oitData, int width, int height, int layers);
static	int		ReleaseOITransparency(OITransparencyData** oitData);
static	int		OITReshape(OITransparencyData* oitData, int width, int height, int layers);
static	int		OITDraw(OITransparencyData* oitData, int layers);

static	void	SetTransparencyMode(TRANSPARENCY_SYS_MODE mode){m_transparencyMode = mode;};
static	int		GetTransparencyMode(){return m_transparencyMode;};
static	void	SetNeedOITInit(OITransparencyData* oitData)
				{
					if (!oitData)
						return;
					oitData->notNeedInit = false;
				};
static	bool	isNeedOITInit(OITransparencyData* oitData, int layers)
				{
					if (!oitData || !oitData->notNeedInit || oitData->layersNumber != layers + 1)
						return true;
					else
						return false;
				};
static	bool	isNeedOITInit(OITransparencyData* oitData)
				{
					if (!oitData || !oitData->notNeedInit || oitData->layersNumber != m_layers + 1)
						return true;
					else
						return false;
				};
static	void	SetNeedOITReshape(OITransparencyData* oitData)
				{
					if (!oitData)
						return;
					oitData->notNeedReshape = false;
				};
static	bool	isNeedOITReshape(OITransparencyData* oitData)
				{
					if (!oitData)
						return false;
					if (!oitData->notNeedReshape)
						return true;
					else
						return false;
				};

protected:
static	int		InitGLSL(OITransparencyData* oitData);

protected:
static	int				m_transparencyMode;

public:
static	GLuint			m_shadowMap;
static	int				m_shadowMapSize;
static	bool			m_drawAllLayers;
//static	bool			m_isShaders;
static	OITBlendingMode	m_blendingMode;

};

//void __cdecl main(void)
//GLEWAPI GLenum __cdecl glewInit();

//////////////////////////////////////////////////////////////////////////////////////////////

