

#include "AftrConfig.h"
//#ifdef AFTR_CONFIG_USE_OPEN_VR

#include "AftrGLRendererDeferred.h"
#include "WorldContainer.h"
#include "Axes.h"
#include "AftrFrameBufferObject.h"
#include "AftrUtilities.h"
#include "ManagerShader.h"
#include "GLSLShaderDefaultLoadingScreenGL32.h"
#include "ModelMeshSkin.h"
#include "GLSLShaderDefaultOrthoWOGUI_GL32.h"
#include "GLSLShaderDefaultOrthoWOGUIDepthMapGL32.h"
#include "GLSLShaderDefaultGL32DeferredFinal.h"
#include "AftrUtil_matrix.h"
#include "IndexedGeometryQuad.h"
using namespace Aftr;


Aftr::AftrGLRendererDeferred* Aftr::AftrGLRendererDeferred::New()
{
   AftrGLRendererDeferred* r = new AftrGLRendererDeferred();
    r->onCreate();
    return r;
}

Aftr::AftrGLRendererDeferred::AftrGLRendererDeferred() : AftrGLRendererBase()
{
    std::cout << "Aftr::AftrGLRendererOVR::AftrGLRendererOVR()...\n";
    //maybe do ManagerOpenVr init here and skip in GLView::init()?
    //G-Buffer containing
    //1 - VertexES
    //2 - NormalES
    //3 - Tex
    //4 - Diffuse
    //5 - Ambient
    //6 - Specular
    std::vector< GLenum > internalFormats;
    internalFormats.push_back(GL_RGBA32F);//breaks at a distance if not F
    internalFormats.push_back(GL_RGBA32F);//breaks if not F
    internalFormats.push_back(GL_RGBA);
    internalFormats.push_back(GL_RGBA);
    internalFormats.push_back(GL_RGBA);
    internalFormats.push_back(GL_RGBA32F);//should probably pull out specular coefficient into a single channel, but I'm done here
    fbo = AftrFrameBufferObject::New(800, 600, internalFormats, true, false, false, true, 1);
    
    tex = fbo->generateTexFromFBO();
    this->CaptureNextFrame = false;
    this->CaptureNextFrameToFile = false;

    
       skinFinal.setShader(GLSLShaderDefaultGL32DeferredFinal::New());
    skinDepth.setShader(GLSLShaderDefaultOrthoWOGUIDepthMapGL32::New());
    skinOrtho.setShader(GLSLShaderDefaultOrthoWOGUI_GL32::New());
    //skin.getMultiTextureSet().at(0) = tex;
    ManagerOpenGLState::blend = false;
}

void Aftr::AftrGLRendererDeferred::onCreate()
{
   
}

Aftr::AftrGLRendererDeferred::~AftrGLRendererDeferred()
{
};

void Aftr::AftrGLRendererDeferred::renderScene(const Camera& cam, WorldContainer& wList, bool clearColorDepthStencilBuffersPriorToRender)
{
   glDisable(GL_BLEND);
    if (clearColorDepthStencilBuffersPriorToRender)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    ManagerLight::synchronizeLights(cam);
   
    wList.update(cam);

    if (Axes::isVisible)
        Axes::showAxes(cam);

    //Render the visible world
    wList.renderWorld(cam);

    

   

    if (this->CaptureNextFrame)//capture frames
    {
        ManagerFrameCapture::captureFrameToBuffer();
        this->CaptureNextFrame = false;
    }

    if (this->CaptureNextFrameToFile)
    {
        ManagerFrameCapture::captureFrameToFile(this->captureFilename);
        this->CaptureNextFrameToFile = false;
        this->captureFilename = "";
    }
}

void Aftr::AftrGLRendererDeferred::render(Camera& cam, WorldContainer& wList)
{
    this->setUpSingleCamera( cam );
    this->renderSplit(cam, wList);//always assume stereo
}

void Aftr::AftrGLRendererDeferred::renderSingular(Camera& cam, WorldContainer& wList)
{
    setUpSingleCamera(cam);
    this->renderScene(cam, wList);
}

void Aftr::AftrGLRendererDeferred::renderStereo(Camera& cam, WorldContainer& wList)
{
    std::cout << "Does nothing for OpenVR, call renderSplit for Stereoscopic vertical split rendering...\n";
}

void Aftr::AftrGLRendererDeferred::renderSplit(Camera& cam, WorldContainer& wList)
{

   ManagerLight::synchronizeLights(cam);

    fbo->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render list
    wList.renderWorld(cam);

    fbo->unbind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4 modelview;
    Mat4 proj;
    aftrComputeViewMatrix({ 5.0f, 0, 0 }, { 0, 0, 0 }, { 0, 0, 1 }, modelview.getPtr());
    aftrGluOrtho2DNoGLStateModification(-1, 1, -1, 1, -12.0,12.0f, proj.getPtr());
    Mat4 MVPMatrix = proj * modelview;

    std::unique_ptr< IndexedGeometryQuad > quad(IndexedGeometryQuad::New(Aftr::QuadOrientation::qoYZ, 2.0f, 2.0f, Vector(1, 0, 1), true, false, true));
   
    //G-Buffer containing
    //1 - VertexES
    //2 - NormalES
    //3 - Tex
    //4 - Diffuse
    //5 - Ambient
    //6 - Specular

    if (renderType == FINAL)
    {
       skinFinal.getShaderT<GLSLShaderDefaultGL32DeferredFinal>()->bind(MVPMatrix, skinFinal);
       skinFinal.bind();
       for (int i = 0; i < 6; i++)
       {
          glActiveTexture(GL_TEXTURE0 + i);
          fbo->bindMyTexture(i);
       }
       glActiveTexture(GL_TEXTURE6);
       tex.bind();
    }
    else if (renderType == NORMAL)
    {
       skinOrtho.getShaderT<GLSLShaderDefaultOrthoWOGUI_GL32>()->bind(MVPMatrix, skinFinal);
       fbo->bindMyTexture(1);
    }
    else if (renderType == COLOR)
    {
       skinOrtho.getShaderT<GLSLShaderDefaultOrthoWOGUI_GL32>()->bind(MVPMatrix, skinFinal);
       fbo->bindMyTexture(2);
    }
    else if (renderType == POSITION)
    {
       skinOrtho.getShaderT<GLSLShaderDefaultOrthoWOGUI_GL32>()->bind(MVPMatrix, skinFinal);
       fbo->bindMyTexture(0);
    }
    else if (renderType == AMBIENT)
    {
       skinOrtho.getShaderT<GLSLShaderDefaultOrthoWOGUI_GL32>()->bind(MVPMatrix, skinFinal);
       fbo->bindMyTexture(4);
    }
    else if (renderType == DIFFUSE)
    {
       skinOrtho.getShaderT<GLSLShaderDefaultOrthoWOGUI_GL32>()->bind(MVPMatrix, skinFinal);
       fbo->bindMyTexture(3);
    }
    else if (renderType == SPECULAR)
    {
       skinOrtho.getShaderT<GLSLShaderDefaultOrthoWOGUI_GL32>()->bind(MVPMatrix, skinFinal);
       fbo->bindMyTexture(5);
    }
    else if (renderType == DEPTH)
    {
       skinDepth.getShaderT<GLSLShaderDefaultOrthoWOGUIDepthMapGL32>()->bind(MVPMatrix, skinFinal);
       tex.bind();
    }

   // glDisable(GL_BLEND);
    quad->render();

    skinFinal.unbind();

    glFlush();
    SDL_GL_SwapWindow(ManagerWindowing::getCurrentWindow());
}



void Aftr::AftrGLRendererDeferred::renderSelection(Camera& cam, WorldContainer& wList, SelectionQueryResult& queryResult, unsigned int pixelX, unsigned int pixelY)
{
//TODO
    /*
    printOpenGLErrors(808080, nullptr, AFTR_FILE_LINE_STR);
    //if( ManagerWindowing::usingStereoscopicRendering() )
    //{
    //   if( ManagerWindowing::usingStereoscopicRenderingSplitHorizontally() || ManagerWindowing::usingStereoscopicRenderingSplitVertically() )
    //      this->renderSelectionSplit( cam, wList );
    //   else
    //      this->renderSelectionStereo( cam, wList );
    //}
    //else

    queryResult.x = pixelX;
    queryResult.y = pixelY;

    this->renderSelectionSingular(cam, wList);

    queryResult.lastSelectedWO = this->findLastSelectedWO(pixelX, pixelY, cam, wList);
    //compute coordinate that user clicked on
    {
        Vector coord(0, 0, 0);
        if (this->findLastSelectedCoordinate(pixelX, pixelY, cam, coord))
            queryResult.setLastSelectedCoordinate(coord);
    }

    //find the selected mesh within the selected WO. This requires rendering the selected WO one more time.
    //each mesh is rendered as a separate color
    if (queryResult.lastSelectedWO != nullptr)
    {
        auto meshPtrMeshIdx = this->findLastSelectedModelMesh(pixelX, pixelY, cam, queryResult.lastSelectedWO);
        queryResult.lastSelectedModelMesh = std::get<0>(meshPtrMeshIdx);
        queryResult.lastSelectedModelMeshIdx = std::get<1>(meshPtrMeshIdx);
    }
    */
}

void Aftr::AftrGLRendererDeferred::renderSelectionSingular(Camera& cam, WorldContainer& wList)
{
    this->setUpSingleCamera(cam);
    this->renderSelectionScene(cam, wList);
}

void Aftr::AftrGLRendererDeferred::renderSelectionScene(const Camera& cam, WorldContainer& wList, bool clearColorDepthStencilBuffersPriorToRender)
{
    //TODO
    /*
    if (clearColorDepthStencilBuffersPriorToRender)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (ManagerOpenGLState::isGLContextProfileCompatibility()) glMatrixMode(GL_MODELVIEW);

    ManagerLight::synchronizeLights(cam);

    wList.update(cam);

    //Render the visible world
    wList.renderWorldSelection(cam);

    //if( Axes::isVisible )
    //   Axes::showAxes( cam );

    const_cast<Camera&>(cam).renderSelection(cam); //we have to remove the const ref for invocation of WO::render() since it is not const. However, a Camera object won't modify internally state

    //Render the GUI
    //wList.renderGUI( cam );

    glFlush();

    if (this->CaptureNextFrame)//capture frames
    {
        ManagerFrameCapture::captureFrameToBuffer();
        this->CaptureNextFrame = false;
    }

    if (this->CaptureNextFrameToFile)
    {
        ManagerFrameCapture::captureFrameToFile(this->captureFilename);
        this->CaptureNextFrameToFile = false;
        this->captureFilename = "";
    }
    */
}

//#endif //AFTR_CONFIG_USE_OCULUS_RIFT_OVR