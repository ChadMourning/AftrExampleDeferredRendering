#pragma once

#include "GLView.h"

namespace Aftr
{
   class Camera;
   class AftrGLRendererDeferred;

/**
   \class GLViewTestDeferredRenderer
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewTestDeferredRenderer : public GLView
{
public:
   static GLViewTestDeferredRenderer* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewTestDeferredRenderer();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createTestDeferredRendererWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

   std::vector< WO* > normal;
   std::vector< WO* > deferred;

   AftrGLRendererDeferred* d = nullptr;
   AftrGLRendererBase* r = nullptr;

protected:
   GLViewTestDeferredRenderer( const std::vector< std::string >& args );
   virtual void onCreate();   
};

/** \} */

} //namespace Aftr
