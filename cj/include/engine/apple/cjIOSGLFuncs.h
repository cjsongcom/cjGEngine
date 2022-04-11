
/////////////////////////////////////////////////////////////////
// EAGL 2.0 - Apple's Embedded Graphics Library (OPENGL ES 2.0)
////////////////////////////////////////////////////////////////


/*
 GL_OES_depth_texture GL_OES_depth24 GL_OES_element_index_uint GL_OES_fbo_render_mipmap GL_OES_mapbuffer GL_OES_packed_depth_stencil GL_OES_rgb8_rgba8 GL_OES_standard_derivatives GL_OES_texture_float GL_OES_texture_half_float GL_OES_texture_half_float_linear GL_OES_vertex_array_object GL_EXT_blend_minmax GL_EXT_color_buffer_half_float GL_EXT_debug_label GL_EXT_debug_marker GL_EXT_discard_framebuffer GL_EXT_draw_instanced GL_EXT_instanced_arrays GL_EXT_map_buffer_range GL_EXT_occlusion_query_boolean GL_EXT_pvrtc_sRGB GL_EXT_read_format_bgra GL_EXT_separate_shader_objects GL_EXT_shader_framebuffer_fetch GL_EXT_shader_texture_lod GL_EXT_shadow_samplers GL_EXT_sRGB GL_EXT_texture_filter_anisotropic GL_EXT_texture_rg GL_EXT_texture_storage GL_APPLE_copy_texture_levels GL_APPLE_framebuffer_multisample GL_APPLE_rgb_422 GL_APPLE_sync GL_APPLE_texture_format_BGRA8888 GL_APPLE_texture_max_level GL_IMG_read_format GL_IMG_texture_compression_pvrtc " "GL_OES_depth_texture GL_OES_depth24 GL_OES_element_index_uint GL_OES_fbo_render_mipmap GL_OES_mapbuffer GL_OES_packed_depth_stencil GL_OES_rgb8_rgba8 GL_OES_standard_derivatives GL_OES_texture_float GL_OES_texture_half_float GL_OES_texture_half_float_linear GL_OES_vertex_array_object GL_EXT_blend_minmax GL_EXT_color_buffer_half_float GL_EXT_debug_label GL_EXT_debug_marker GL_EXT_discard_framebuffer GL_EXT_draw_instanced GL_EXT_instanced_arrays GL_EXT_map_buffer_range GL_EXT_occlusion_query_boolean GL_EXT_pvrtc_sRGB GL_EXT_read_format_bgra GL_EXT_separate_shader_objects GL_EXT_shader_framebuffer_fetch GL_EXT_shader_texture_lod GL_EXT_shadow_samplers GL_EXT_sRGB GL_EXT_texture_filter_anisotropic GL_EXT_texture_rg GL_EXT_texture_storage GL_APPLE_copy_texture_levels GL_APPLE_framebuffer_multisample GL_APPLE_rgb_422 GL_APPLE_sync GL_APPLE_texture_format_BGRA8888 GL_APPLE_texture_max_level GL_IMG_read_format GL_IMG_texture_compression_pvrtc "
 */


CJGLCAPS(cjGL)

CJGLPROC(cjGL,void,glActiveTexture,glActiveTexture,(GLenum texture))
CJGLPROC(cjGL,void,glAttachShader,glAttachShader,(GLuint program, GLuint shader))
CJGLPROC(cjGL,void,glBindAttribLocation,glBindAttribLocation,(GLuint program, GLuint index, const GLchar* name))
CJGLPROC(cjGL,void,glBindBuffer,glBindBuffer,(GLenum target, GLuint buffer))
CJGLPROC(cjGL,void,glBindFramebuffer,glBindFramebuffer,(GLenum target, GLuint framebuffer))
CJGLPROC(cjGL,void,glBindRenderbuffer,glBindRenderbuffer,(GLenum target, GLuint renderbuffer))
CJGLPROC(cjGL,void,glBindTexture,glBindTexture,(GLenum target, GLuint texture))
CJGLPROC(cjGL,void,glBlendColor,glBlendColor,(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha))
CJGLPROC(cjGL,void,glBlendEquation,glBlendEquation,(GLenum mode))
CJGLPROC(cjGL,void,glBlendEquationSeparate,glBlendEquationSeparate,(GLenum modeRGB, GLenum modeAlpha))
CJGLPROC(cjGL,void,glBlendFunc,glBlendFunc,(GLenum sfactor, GLenum dfactor))
CJGLPROC(cjGL,void,glBlendFuncSeparate,glBlendFuncSeparate,(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha))
CJGLPROC(cjGL,void,glBufferData,glBufferData,(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage))
CJGLPROC(cjGL,void,glBufferSubData,glBufferSubData,(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data))
CJGLPROC(cjGL,GLenum,glCheckFramebufferStatus,glCheckFramebufferStatus,(GLenum target))
CJGLPROC(cjGL,void,glClear,glClear,(GLbitfield mask))
CJGLPROC(cjGL,void,glClearColor,glClearColor,(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha))
CJGLPROC(cjGL,void,glClearDepthf,glClearDepthf,(GLclampf depth))
CJGLPROC(cjGL,void,glClearStencil,glClearStencil,(GLint s))
CJGLPROC(cjGL,void,glColorMask,glColorMask,(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha))
CJGLPROC(cjGL,void,glCompileShader,glCompileShader,(GLuint shader))
CJGLPROC(cjGL,void,glCompressedTexImage2D,glCompressedTexImage2D,(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data))
CJGLPROC(cjGL,void,glCompressedTexSubImage2D,glCompressedTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data))
CJGLPROC(cjGL,void,glCopyTexImage2D,glCopyTexImage2D,(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border))
CJGLPROC(cjGL,void,glCopyTexSubImage2D,glCopyTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height))
CJGLPROC(cjGL,GLuint,glCreateProgram,glCreateProgram,(void))
CJGLPROC(cjGL,GLuint,glCreateShader,glCreateShader,(GLenum type))
CJGLPROC(cjGL,void,glCullFace,glCullFace,(GLenum mode))
CJGLPROC(cjGL,void,glDeleteBuffers,glDeleteBuffers,(GLsizei n, const GLuint* buffers))
CJGLPROC(cjGL,void,glDeleteFramebuffers,glDeleteFramebuffers,(GLsizei n, const GLuint* framebuffers))
CJGLPROC(cjGL,void,glDeleteProgram,glDeleteProgram,(GLuint program))
CJGLPROC(cjGL,void,glDeleteRenderbuffers,glDeleteRenderbuffers,(GLsizei n, const GLuint* renderbuffers))
CJGLPROC(cjGL,void,glDeleteShader,glDeleteShader,(GLuint shader))
CJGLPROC(cjGL,void,glDeleteTextures,glDeleteTextures,(GLsizei n, const GLuint* textures))
CJGLPROC(cjGL,void,glDepthFunc,glDepthFunc,(GLenum func))
CJGLPROC(cjGL,void,glDepthMask,glDepthMask,(GLboolean flag))
CJGLPROC(cjGL,void,glDepthRangef,glDepthRangef,(GLclampf zNear, GLclampf zFar))
CJGLPROC(cjGL,void,glDetachShader,glDetachShader,(GLuint program, GLuint shader))
CJGLPROC(cjGL,void,glDisable,glDisable,(GLenum cap))
CJGLPROC(cjGL,void,glDisableVertexAttribArray,glDisableVertexAttribArray,(GLuint index))
CJGLPROC(cjGL,void,glDrawArrays,glDrawArrays,(GLenum mode, GLint first, GLsizei count))
CJGLPROC(cjGL,void,glDrawElements,glDrawElements,(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices))
CJGLPROC(cjGL,void,glEnable,glEnable,(GLenum cap))
CJGLPROC(cjGL,void,glEnableVertexAttribArray,glEnableVertexAttribArray,(GLuint index))
CJGLPROC(cjGL,void,glFinish,glFinish,(void))
CJGLPROC(cjGL,void,glFlush,glFlush,(void))
CJGLPROC(cjGL,void,glFramebufferRenderbuffer,glFramebufferRenderbuffer,(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer))
CJGLPROC(cjGL,void,glFramebufferTexture2D,glFramebufferTexture2D,(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level))
CJGLPROC(cjGL,void,glFrontFace,glFrontFace,(GLenum mode))
CJGLPROC(cjGL,void,glGenBuffers,glGenBuffers,(GLsizei n, GLuint* buffers))
CJGLPROC(cjGL,void,glGenerateMipmap,glGenerateMipmap,(GLenum target))
CJGLPROC(cjGL,void,glGenFramebuffers,glGenFramebuffers,(GLsizei n, GLuint* framebuffers))
CJGLPROC(cjGL,void,glGenRenderbuffers,glGenRenderbuffers,(GLsizei n, GLuint* renderbuffers))
CJGLPROC(cjGL,void,glGenTextures,glGenTextures,(GLsizei n, GLuint* textures))
CJGLPROC(cjGL,void,glGetActiveAttrib,glGetActiveAttrib,(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name))
CJGLPROC(cjGL,void,glGetActiveUniform,glGetActiveUniform,(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name))
CJGLPROC(cjGL,void,glGetAttachedShaders,glGetAttachedShaders,(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders))
CJGLPROC(cjGL,int,glGetAttribLocation,glGetAttribLocation,(GLuint program, const GLchar* name))
CJGLPROC(cjGL,void,glGetBooleanv,glGetBooleanv,(GLenum pname, GLboolean* params))
CJGLPROC(cjGL,void,glGetBufferParameteriv,glGetBufferParameteriv,(GLenum target, GLenum pname, GLint* params))
CJGLPROC(cjGL,GLenum,glGetError,glGetError,(void))
CJGLPROC(cjGL,void,glGetFloatv,glGetFloatv,(GLenum pname, GLfloat* params))
CJGLPROC(cjGL,void,glGetFramebufferAttachmentParameteriv,glGetFramebufferAttachmentParameteriv,(GLenum target, GLenum attachment, GLenum pname, GLint* params))
CJGLPROC(cjGL,void,glGetIntegerv,glGetIntegerv,(GLenum pname, GLint* params))
CJGLPROC(cjGL,void,glGetProgramiv,glGetProgramiv,(GLuint program, GLenum pname, GLint* params))
CJGLPROC(cjGL,void,glGetProgramInfoLog,glGetProgramInfoLog,(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog))
CJGLPROC(cjGL,void,glGetRenderbufferParameteriv,glGetRenderbufferParameteriv,(GLenum target, GLenum pname, GLint* params))
CJGLPROC(cjGL,void,glGetShaderiv,glGetShaderiv,(GLuint shader, GLenum pname, GLint* params))
CJGLPROC(cjGL,void,glGetShaderInfoLog,glGetShaderInfoLog,(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog))
CJGLPROC(cjGL,void,glGetShaderPrecisionFormat,glGetShaderPrecisionFormat,(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision))
CJGLPROC(cjGL,void,glGetShaderSource,glGetShaderSource,(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source))
CJGLPROC(cjGL,const GLubyte*,glGetString,glGetString,(GLenum name))
CJGLPROC(cjGL,void,glGetTexParameterfv,glGetTexParameterfv,(GLenum target, GLenum pname, GLfloat* params))
CJGLPROC(cjGL,void,glGetTexParameteriv,glGetTexParameteriv,(GLenum target, GLenum pname, GLint* params))
CJGLPROC(cjGL,void,glGetUniformfv,glGetUniformfv,(GLuint program, GLint location, GLfloat* params))
CJGLPROC(cjGL,void,glGetUniformiv,glGetUniformiv,(GLuint program, GLint location, GLint* params))
CJGLPROC(cjGL,int,glGetUniformLocation,glGetUniformLocation,(GLuint program, const GLchar* name))
CJGLPROC(cjGL,void,glGetVertexAttribfv,glGetVertexAttribfv,(GLuint index, GLenum pname, GLfloat* params))
CJGLPROC(cjGL,void,glGetVertexAttribiv,glGetVertexAttribiv,(GLuint index, GLenum pname, GLint* params))
CJGLPROC(cjGL,void,glGetVertexAttribPointerv,glGetVertexAttribPointerv,(GLuint index, GLenum pname, GLvoid** pointer))
CJGLPROC(cjGL,void,glHint,glHint,(GLenum target, GLenum mode))
CJGLPROC(cjGL,GLboolean,glIsBuffer,glIsBuffer,(GLuint buffer))
CJGLPROC(cjGL,GLboolean,glIsEnabled,glIsEnabled,(GLenum cap))
CJGLPROC(cjGL,GLboolean,glIsFramebuffer,glIsFramebuffer,(GLuint framebuffer))
CJGLPROC(cjGL,GLboolean,glIsProgram,glIsProgram,(GLuint program))
CJGLPROC(cjGL,GLboolean,glIsRenderbuffer,glIsRenderbuffer,(GLuint renderbuffer))
CJGLPROC(cjGL,GLboolean,glIsShader,glIsShader,(GLuint shader))
CJGLPROC(cjGL,GLboolean,glIsTexture,glIsTexture,(GLuint texture))
CJGLPROC(cjGL,void,glLineWidth,glLineWidth,(GLfloat width))
CJGLPROC(cjGL,void,glLinkProgram,glLinkProgram,(GLuint program))
CJGLPROC(cjGL,void,glPixelStorei,glPixelStorei,(GLenum pname, GLint param))
CJGLPROC(cjGL,void,glPolygonOffset,glPolygonOffset,(GLfloat factor, GLfloat units))
CJGLPROC(cjGL,void,glReadPixels,glReadPixels,(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels))
CJGLPROC(cjGL,void,glReleaseShaderCompiler,glReleaseShaderCompiler,(void))
CJGLPROC(cjGL,void,glRenderbufferStorage,glRenderbufferStorage,(GLenum target, GLenum internalformat, GLsizei width, GLsizei height))
CJGLPROC(cjGL,void,glSampleCoverage,glSampleCoverage,(GLclampf value, GLboolean invert))
CJGLPROC(cjGL,void,glScissor,glScissor,(GLint x, GLint y, GLsizei width, GLsizei height))
CJGLPROC(cjGL,void,glShaderBinary,glShaderBinary,(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length))
CJGLPROC(cjGL,void,glShaderSource,glShaderSource,(GLuint shader, GLsizei count, const GLchar* const *string, const GLint* length))
CJGLPROC(cjGL,void,glStencilFunc,glStencilFunc,(GLenum func, GLint ref, GLuint mask))
CJGLPROC(cjGL,void,glStencilFuncSeparate,glStencilFuncSeparate,(GLenum face, GLenum func, GLint ref, GLuint mask))
CJGLPROC(cjGL,void,glStencilMask,glStencilMask,(GLuint mask))
CJGLPROC(cjGL,void,glStencilMaskSeparate,glStencilMaskSeparate,(GLenum face, GLuint mask))
CJGLPROC(cjGL,void,glStencilOp,glStencilOp,(GLenum fail, GLenum zfail, GLenum zpass))
CJGLPROC(cjGL,void,glStencilOpSeparate,glStencilOpSeparate,(GLenum face, GLenum fail, GLenum zfail, GLenum zpass))
CJGLPROC(cjGL,void,glTexImage2D,glTexImage2D,(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels))
CJGLPROC(cjGL,void,glTexParameterf,glTexParameterf,(GLenum target, GLenum pname, GLfloat param))
CJGLPROC(cjGL,void,glTexParameterfv,glTexParameterfv,(GLenum target, GLenum pname, const GLfloat* params))
CJGLPROC(cjGL,void,glTexParameteri,glTexParameteri,(GLenum target, GLenum pname, GLint param))
CJGLPROC(cjGL,void,glTexParameteriv,glTexParameteriv,(GLenum target, GLenum pname, const GLint* params))
CJGLPROC(cjGL,void,glTexSubImage2D,glTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels))
CJGLPROC(cjGL,void,glUniform1f,glUniform1f,(GLint location, GLfloat x))
CJGLPROC(cjGL,void,glUniform1fv,glUniform1fv,(GLint location, GLsizei count, const GLfloat* v))
CJGLPROC(cjGL,void,glUniform1i,glUniform1i,(GLint location, GLint x))
CJGLPROC(cjGL,void,glUniform1iv,glUniform1iv,(GLint location, GLsizei count, const GLint* v))
CJGLPROC(cjGL,void,glUniform2f,glUniform2f,(GLint location, GLfloat x, GLfloat y))
CJGLPROC(cjGL,void,glUniform2fv,glUniform2fv,(GLint location, GLsizei count, const GLfloat* v))
CJGLPROC(cjGL,void,glUniform2i,glUniform2i,(GLint location, GLint x, GLint y))
CJGLPROC(cjGL,void,glUniform2iv,glUniform2iv,(GLint location, GLsizei count, const GLint* v))
CJGLPROC(cjGL,void,glUniform3f,glUniform3f,(GLint location, GLfloat x, GLfloat y, GLfloat z))
CJGLPROC(cjGL,void,glUniform3fv,glUniform3fv,(GLint location, GLsizei count, const GLfloat* v))
CJGLPROC(cjGL,void,glUniform3i,glUniform3i,(GLint location, GLint x, GLint y, GLint z))
CJGLPROC(cjGL,void,glUniform3iv,glUniform3iv,(GLint location, GLsizei count, const GLint* v))
CJGLPROC(cjGL,void,glUniform4f,glUniform4f,(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w))
CJGLPROC(cjGL,void,glUniform4fv,glUniform4fv,(GLint location, GLsizei count, const GLfloat* v))
CJGLPROC(cjGL,void,glUniform4i,glUniform4i,(GLint location, GLint x, GLint y, GLint z, GLint w))
CJGLPROC(cjGL,void,glUniform4iv,glUniform4iv,(GLint location, GLsizei count, const GLint* v))
CJGLPROC(cjGL,void,glUniformMatrix2fv,glUniformMatrix2fv,(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value))
CJGLPROC(cjGL,void,glUniformMatrix3fv,glUniformMatrix3fv,(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value))
CJGLPROC(cjGL,void,glUniformMatrix4fv,glUniformMatrix4fv,(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value))
CJGLPROC(cjGL,void,glUseProgram,glUseProgram,(GLuint program))
CJGLPROC(cjGL,void,glValidateProgram,glValidateProgram,(GLuint program))
CJGLPROC(cjGL,void,glVertexAttrib1f,glVertexAttrib1f,(GLuint indx, GLfloat x))
CJGLPROC(cjGL,void,glVertexAttrib1fv,glVertexAttrib1fv,(GLuint indx, const GLfloat* values))
CJGLPROC(cjGL,void,glVertexAttrib2f,glVertexAttrib2f,(GLuint indx, GLfloat x, GLfloat y))
CJGLPROC(cjGL,void,glVertexAttrib2fv,glVertexAttrib2fv,(GLuint indx, const GLfloat* values))
CJGLPROC(cjGL,void,glVertexAttrib3f,glVertexAttrib3f,(GLuint indx, GLfloat x, GLfloat y, GLfloat z))
CJGLPROC(cjGL,void,glVertexAttrib3fv,glVertexAttrib3fv,(GLuint indx, const GLfloat* values))
CJGLPROC(cjGL,void,glVertexAttrib4f,glVertexAttrib4f,(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w))
CJGLPROC(cjGL,void,glVertexAttrib4fv,glVertexAttrib4fv,(GLuint indx, const GLfloat* values))
CJGLPROC(cjGL,void,glVertexAttribPointer,glVertexAttribPointer,(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr))
CJGLPROC(cjGL,void,glViewport,glViewport,(GLint x, GLint y, GLsizei width, GLsizei height))

CJGLPROC(cjGL,GLboolean,glIsVertexArray,glIsVertexArrayOES,(GLuint array))
CJGLPROC(cjGL,void,glBindVertexArray,glBindVertexArrayOES,(GLuint array))
CJGLPROC(cjGL,void,glDeleteVertexArrays,glDeleteVertexArraysOES,(GLsizei n, const GLuint *arrays))
CJGLPROC(cjGL,void,glGenVertexArrays,glGenVertexArraysOES,(GLsizei n, GLuint *arrays))

// there is no glPolygonMode in opengl es
CJGLPROC(cjGL,void,glPolygonMode,_nullfunc_glPolygonMode,(GLenum face, GLenum mode))


//#ifdef GL_EXT_texture_filter_anisotropic
CJGLCAPS(cjGL_EXT_texture_filter_anisotropic)
//#endif

CJGLCAPS(cjGL_multisample)

//#if GL_APPLE_framebuffer_multisample
CJGLCAPS(cjGL_APPLE_framebuffer_multisample)
//#endif
CJGLPROC(cjGL,void,glRenderbufferStorageMultisampleAPPLE,glRenderbufferStorageMultisampleAPPLE,(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height))
CJGLPROC(cjGL,void,glResolveMultisampleFramebufferAPPLE,glResolveMultisampleFramebufferAPPLE,(void))


CJGLPROC(cjGL,void*,glMapBuffer,glMapBufferOES,(GLenum target, GLenum access))
CJGLPROC(cjGL,GLboolean,glUnmapBuffer,glUnmapBufferOES,(GLenum target))





