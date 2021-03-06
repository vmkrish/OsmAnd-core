#ifndef _OSMAND_CORE_GPU_API_OPENGLES2_H_
#define _OSMAND_CORE_GPU_API_OPENGLES2_H_

#include "stdlib_common.h"
#include <array>

#include "QtExtensions.h"
#include <QString>
#include <QHash>

#include <glm/glm.hpp>

#include "OsmAndCore.h"
#include "CommonTypes.h"
#include "OpenGL/GPUAPI_OpenGL.h"

namespace OsmAnd
{
    class GPUAPI_OpenGLES2 : public GPUAPI_OpenGL
    {
    public:
#if !defined(OSMAND_TARGET_OS_ios)
        typedef void (GL_APIENTRYP P_glTexStorage2DEXT_PROC)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
        static P_glTexStorage2DEXT_PROC glTexStorage2DEXT;

        static PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
        static PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
        static PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;

        typedef void (GL_APIENTRYP PFNGLPOPGROUPMARKEREXTPROC)(void);
        typedef void (GL_APIENTRYP PFNGLPUSHGROUPMARKEREXTPROC)(GLsizei length, const GLchar* marker);
        static PFNGLPOPGROUPMARKEREXTPROC glPopGroupMarkerEXT;
        static PFNGLPUSHGROUPMARKEREXTPROC glPushGroupMarkerEXT;
#endif // !OSMAND_TARGET_OS_ios
    private:
        bool _isSupported_EXT_unpack_subimage;
        bool _isSupported_EXT_texture_storage;
        bool _isSupported_APPLE_texture_max_level;
        bool _isSupported_OES_vertex_array_object;
        bool _isSupported_OES_rgb8_rgba8;
        bool _isSupported_OES_texture_float;
        bool _isSupported_EXT_texture_rg;
        bool _isSupported_EXT_shader_texture_lod;

        void preprocessShader(QString& code, const QString& extraHeader = QString());

        QHash<GLenum, SamplerType> _textureBlocksSamplers;

        virtual void glPushGroupMarkerEXT_wrapper(GLsizei length, const GLchar* marker);
        virtual void glPopGroupMarkerEXT_wrapper();
    protected:
    public:
        GPUAPI_OpenGLES2();
        virtual ~GPUAPI_OpenGLES2();

        virtual bool initialize();
        virtual bool release();

        const bool& isSupported_EXT_unpack_subimage;
        const bool& isSupported_EXT_texture_storage;
        const bool& isSupported_APPLE_texture_max_level;
        const bool& isSupported_OES_vertex_array_object;
        const bool& isSupported_OES_rgb8_rgba8;
        const bool& isSupported_OES_texture_float;
        const bool& isSupported_EXT_texture_rg;
        const bool& isSupported_EXT_shader_texture_lod;

        virtual GLenum validateResult();

        virtual TextureFormat getTextureFormat(const std::shared_ptr< const MapTile >& tile);
        virtual TextureFormat getTextureFormat(const std::shared_ptr< const MapSymbol >& symbol);
        virtual SourceFormat getSourceFormat(const std::shared_ptr< const MapTile >& tile);
        virtual SourceFormat getSourceFormat(const std::shared_ptr< const MapSymbol >& symbol);
        virtual void allocateTexture2D(GLenum target, GLsizei levels, GLsizei width, GLsizei height, const TextureFormat format);
        virtual void uploadDataToTexture2D(GLenum target, GLint level,
            GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
            const GLvoid *data, GLsizei dataRowLengthInElements, GLsizei elementSize,
            const SourceFormat sourceFormat);
        virtual void setMipMapLevelsLimit(GLenum target, const uint32_t mipmapLevelsCount);

        virtual void glGenVertexArrays_wrapper(GLsizei n, GLuint* arrays);
        virtual void glBindVertexArray_wrapper(GLuint array);
        virtual void glDeleteVertexArrays_wrapper(GLsizei n, const GLuint* arrays);

        virtual void preprocessVertexShader(QString& code);
        virtual void preprocessFragmentShader(QString& code);
        virtual void optimizeVertexShader(QString& code);
        virtual void optimizeFragmentShader(QString& code);

        virtual void setTextureBlockSampler(const GLenum textureBlock, const SamplerType samplerType);
        virtual void applyTextureBlockToTexture(const GLenum texture, const GLenum textureBlock);
    };
}

#endif // !defined(_OSMAND_CORE_GPU_API_OPENGLES2_H_)
