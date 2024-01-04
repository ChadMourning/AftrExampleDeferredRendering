#include "GLSLShaderDefaultGL32DeferredFinal.h"
#include "ManagerShader.h"
#include "ManagerEnvironmentConfiguration.h"
#include "GLSLUniform.h"
#include "GLSLAttribute.h"
#include "Model.h"
#include "GLView.h"
#include "Camera.h"
using namespace Aftr;


GLSLShaderDefaultGL32DeferredFinal* GLSLShaderDefaultGL32DeferredFinal::New()
{
   std::string vert = ManagerEnvironmentConfiguration::getSMM() + "shaders/defaultOrthoMGLGUI_GL32.vert";
   std::string frag = ManagerEnvironmentConfiguration::getSMM() + "shaders/defaultGL32DeferredFinal.frag";

   GLSLShaderDataShared* shdrData = ManagerShader::loadShaderDataShared( vert, frag );
   if( shdrData == nullptr )
      return nullptr;

   GLSLShaderDefaultGL32DeferredFinal* shdr = new GLSLShaderDefaultGL32DeferredFinal( shdrData );
   return shdr;
}

GLSLShaderDefaultGL32DeferredFinal* GLSLShaderDefaultGL32DeferredFinal::New( GLSLShaderDataShared* shdrData )
{   
   GLSLShaderDefaultGL32DeferredFinal* shdr = new GLSLShaderDefaultGL32DeferredFinal( shdrData );
   return shdr;
}

GLSLShaderDefaultGL32DeferredFinal::GLSLShaderDefaultGL32DeferredFinal(GLSLShaderDataShared* dataShared ) : GLSLShaderDefaultOrthoWOGUI_GL32( dataShared )
{
}

GLSLShaderDefaultGL32DeferredFinal::GLSLShaderDefaultGL32DeferredFinal( const GLSLShaderDefaultGL32DeferredFinal& toCopy ) : GLSLShaderDefaultOrthoWOGUI_GL32( toCopy.dataShared )
{
   *this = toCopy;
}

GLSLShaderDefaultGL32DeferredFinal::~GLSLShaderDefaultGL32DeferredFinal()
{
   //Parent destructor deletes all uniforms and attributes
}

GLSLShaderDefaultGL32DeferredFinal& Aftr::GLSLShaderDefaultGL32DeferredFinal::operator =( const GLSLShaderDefaultGL32DeferredFinal& shader )
{
   if( this != &shader )
   {
      //copy all of parent info in base shader, then copy local members in this subclass instance
      GLSLShader::operator=( shader );

      //Now copy local members from this subclassed instance
      //this->mySubclassedLocalVariable = shader.mySubclassedLocalVariable
   }
   return *this;
}

GLSLShader* GLSLShaderDefaultGL32DeferredFinal::getCopyOfThisInstance()
{
   GLSLShaderDefaultGL32DeferredFinal* copy = new GLSLShaderDefaultGL32DeferredFinal( *this );
   return copy;
}
