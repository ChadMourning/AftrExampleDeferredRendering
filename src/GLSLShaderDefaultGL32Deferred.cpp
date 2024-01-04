#include "GLSLShaderDefaultGL32Deferred.h"
#include "ManagerShader.h"
#include "ManagerEnvironmentConfiguration.h"
#include "GLSLUniform.h"
#include "GLSLAttribute.h"
#include "Model.h"
#include "GLView.h"
#include "Camera.h"
using namespace Aftr;


GLSLShaderDefaultGL32Deferred* GLSLShaderDefaultGL32Deferred::New()
{
   std::string vert = ManagerEnvironmentConfiguration::getSMM() + "shaders/defaultGL32.vert";
   std::string frag = ManagerEnvironmentConfiguration::getSMM() + "shaders/defaultGL32Deferred.frag";

   GLSLShaderDataShared* shdrData = ManagerShader::loadShaderDataShared( vert, frag );
   if( shdrData == nullptr )
      return nullptr;

   GLSLShaderDefaultGL32Deferred* shdr = new GLSLShaderDefaultGL32Deferred( shdrData );
   return shdr;
}

GLSLShaderDefaultGL32Deferred* GLSLShaderDefaultGL32Deferred::New( GLSLShaderDataShared* shdrData )
{   
   GLSLShaderDefaultGL32Deferred* shdr = new GLSLShaderDefaultGL32Deferred( shdrData );
   return shdr;
}

GLSLShaderDefaultGL32Deferred::GLSLShaderDefaultGL32Deferred(GLSLShaderDataShared* dataShared ) : GLSLShaderDefaultGL32( dataShared )
{
}

GLSLShaderDefaultGL32Deferred::GLSLShaderDefaultGL32Deferred( const GLSLShaderDefaultGL32Deferred& toCopy ) : GLSLShaderDefaultGL32( toCopy.dataShared )
{
   *this = toCopy;
}

GLSLShaderDefaultGL32Deferred::~GLSLShaderDefaultGL32Deferred()
{
   //Parent destructor deletes all uniforms and attributes
}

GLSLShaderDefaultGL32Deferred& Aftr::GLSLShaderDefaultGL32Deferred::operator =( const GLSLShaderDefaultGL32Deferred& shader )
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

GLSLShader* GLSLShaderDefaultGL32Deferred::getCopyOfThisInstance()
{
   GLSLShaderDefaultGL32Deferred* copy = new GLSLShaderDefaultGL32Deferred( *this );
   return copy;
}
