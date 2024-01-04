#include "GLViewTestDeferredRenderer.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"
#include "AftrGLRendererDeferred.h"
#include "WOQuad.h"
#include "GLSLShaderDefaultGL32Deferred.h"

using namespace Aftr;

GLViewTestDeferredRenderer* GLViewTestDeferredRenderer::New( const std::vector< std::string >& args )
{
   GLViewTestDeferredRenderer* glv = new GLViewTestDeferredRenderer( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewTestDeferredRenderer::GLViewTestDeferredRenderer( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewTestDeferredRenderer::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewTestDeferredRenderer::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewTestDeferredRenderer::onCreate()
{
   //GLViewTestDeferredRenderer::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewTestDeferredRenderer::~GLViewTestDeferredRenderer()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewTestDeferredRenderer::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
}


void GLViewTestDeferredRenderer::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewTestDeferredRenderer::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewTestDeferredRenderer::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewTestDeferredRenderer::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewTestDeferredRenderer::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   //G-Buffer containing
    //1 - VertexES
    //2 - NormalES
    //3 - Tex
    //4 - Diffuse
    //5 - Ambient
    //6 - Specular
   static bool b = false;
   if( key.keysym.sym == SDLK_1 )
   {
      if (d == nullptr)
      {
         this->r = this->glRenderer;
         this->d = AftrGLRendererDeferred::New();
      }
      static bool b = true;
      if (b)
      {
         this->glRenderer = d;
         b = false;
         for (size_t i = 0; i < deferred.size(); i++)
            deferred[i]->isVisible = true;
         for (size_t i = 0; i < normal.size(); i++)
            normal[i]->isVisible = false;
      }
      else
      {
         this->glRenderer = r;
         b = true;
         for (size_t i = 0; i < deferred.size(); i++)
            deferred[i]->isVisible = false;
         for (size_t i = 0; i < normal.size(); i++)
            normal[i]->isVisible = true;
      }
   }
   if (key.keysym.sym == SDLK_2)
   {
      ((AftrGLRendererDeferred*) this->d)->renderType = AftrGLRendererDeferred::renderType::FINAL;
   }
   if (key.keysym.sym == SDLK_3)
   {
      ((AftrGLRendererDeferred*)this->d)->renderType = AftrGLRendererDeferred::renderType::POSITION;
   }
   if (key.keysym.sym == SDLK_4)
   {
      ((AftrGLRendererDeferred*)this->d)->renderType = AftrGLRendererDeferred::renderType::COLOR;
   }
   if (key.keysym.sym == SDLK_5)
   {
      ((AftrGLRendererDeferred*)this->d)->renderType = AftrGLRendererDeferred::renderType::DIFFUSE;
   }
   if (key.keysym.sym == SDLK_6)
   {
      ((AftrGLRendererDeferred*)this->d)->renderType = AftrGLRendererDeferred::renderType::AMBIENT;
   }
   if (key.keysym.sym == SDLK_7)
   {
      ((AftrGLRendererDeferred*)this->d)->renderType = AftrGLRendererDeferred::renderType::SPECULAR;
   }
   if (key.keysym.sym == SDLK_8)
   {
      ((AftrGLRendererDeferred*)this->d)->renderType = AftrGLRendererDeferred::renderType::DEPTH;
   }
   if (key.keysym.sym == SDLK_9)
   {
      ((AftrGLRendererDeferred*)this->d)->renderType = AftrGLRendererDeferred::renderType::NORMAL;
   }
   if (key.keysym.sym == SDLK_0)
   {
     
   }
}


void GLViewTestDeferredRenderer::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewTestDeferredRenderer::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,10 );

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );

   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( false );
   light->setPosition( Vector( 0, 0, 100 ) );
   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0 ) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );
   wo->getModel()->getSkin().setShader(GLSLShaderDefaultGL32Deferred::New());
   deferred.push_back(wo);

    wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
   wo->setPosition(Vector(0, 0, 0));
   wo->setLabel("Sky Box");
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back(wo);
   normal.push_back(wo);

   ////Create the infinite grass plane (the floor)
   wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   wo->setPosition( Vector( 0, 0, 0 ) );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   wo->upon_async_model_loaded([wo]() {
      ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
      grassSkin.getMultiTextureSet().at(0).setTexRepeats(5.0f);
      grassSkin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Color of object when it is not in any light
      grassSkin.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
      grassSkin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
      grassSkin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
      wo->getModel()->getSkin().setShader(GLSLShaderDefaultGL32Deferred::New());
      wo->getModel()->isUsingBlending(false);
      } );
   wo->setLabel( "Grass" );
   worldLst->push_back( wo );
   deferred.push_back(wo);

   wo = WO::New(grass, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
   wo->setPosition(Vector(0, 0, 0));
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   wo->upon_async_model_loaded([wo] {
      ModelMeshSkin& grassSkin2 = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
      grassSkin2.getMultiTextureSet().at(0).setTexRepeats(5.0f);
      grassSkin2.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Color of object when it is not in any light
      grassSkin2.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
      grassSkin2.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
      grassSkin2.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
      wo->getModel()->isUsingBlending(false);
      });
   wo->setLabel("Grass");
   worldLst->push_back(wo);
   //wo->getModel()->getSkin().setShader(GLSLShaderDefaultGL32Deferred::New());
   normal.push_back(wo);

   wo = WO::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   wo->setPosition( Vector(0,0,20.0f) );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   wo->setLabel( "Cube" );
   worldLst->push_back( wo );
   wo->upon_async_model_loaded([wo]() {
      ModelMeshSkin& grassSkin2 = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
      grassSkin2.setSpecular(aftrColor4f(1.0f, 0.0f, 0.0f, 1.0f));
      grassSkin2.setSpecularCoefficient(20);
      wo->getModel()->getSkin().setShader(GLSLShaderDefaultGL32Deferred::New());
      wo->getModel()->isUsingBlending(false);
      });
   deferred.push_back(wo);

   wo = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
   wo->setPosition(Vector(0, 0, 20.0f));
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   wo->setLabel("Grass");
   worldLst->push_back(wo);
   normal.push_back(wo);

   for (int i = 0; i < deferred.size(); i++)
      deferred[i]->isVisible = false;
 
   createTestDeferredRendererWayPoints();
}


void GLViewTestDeferredRenderer::createTestDeferredRendererWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWayPointSpherical::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   wayPt->getModel()->getSkin().setShader(GLSLShaderDefaultGL32Deferred::New());
   worldLst->push_back( wayPt );
}
