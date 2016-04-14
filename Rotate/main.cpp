﻿#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Ogre.h>
#include <OIS/OIS.h>


using namespace Ogre;


class ESCListener : public FrameListener {
	OIS::Keyboard *mKeyboard;

public:
	ESCListener(OIS::Keyboard *keyboard) : mKeyboard(keyboard) {}
  bool frameStarted(const FrameEvent &evt)
  {
    mKeyboard->capture();
	return !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
  }
};


class MainListener : public FrameListener {
  OIS::Keyboard *mKeyboard;
  Root* mRoot;
  SceneNode *mProfessorNode, *mFishNode, *mEmptyNode;

  const Vector3 movingSpeed = Vector3(0.0f, 0.0f, 2.0f);
  const Degree rotationSpeed = Degree(1.0f);
  const Degree fishRotationSpeed = Degree(-1.0f);
  const float professorMaxPosition_z = 250.0f;
  
  float rotationCount = 0.0f;

public:
  MainListener(Root* root, OIS::Keyboard *keyboard) : mKeyboard(keyboard), mRoot(root) 
  {
    mProfessorNode = mRoot->getSceneManager("main")->getSceneNode("Professor");
	// 빈노드는 물고기의 부모
	mEmptyNode = mRoot->getSceneManager("main")->getSceneNode("Empty");
    mFishNode = mRoot->getSceneManager("main")->getSceneNode("Fish");
  }

  bool frameStarted(const FrameEvent &evt)
  {
    // Fill Here ----------------------------------------------
	 
	  // Professor move
	  if (mProfessorNode->getPosition().z < -1 * professorMaxPosition_z || mProfessorNode->getPosition().z > professorMaxPosition_z)
	  {
		  mProfessorNode->yaw(rotationSpeed);
		  rotationCount += 1.0f;
		  if (rotationCount >= 180 )
		  {
			  float professorPos_x = mProfessorNode->getPosition().x;
			  float professorPos_y = mProfessorNode->getPosition().y;
			  float professorPos_z = mProfessorNode->getPosition().z > 0 ? 250.0f : -1 * 250.0f;
			  Vector3 pos = Vector3(professorPos_x, professorPos_y, professorPos_z);
			  mProfessorNode->setPosition(pos);

			  mProfessorNode->translate(movingSpeed, Node::TransformSpace::TS_LOCAL);
			  rotationCount = 0;
		  }
		  
	  }
	  else
	  {
		  mProfessorNode->translate(movingSpeed, Node::TransformSpace::TS_LOCAL);
		  
	  }

	  //--- Fish rotate

		  mEmptyNode->yaw(fishRotationSpeed);
		  
		  float fishPos_x = mProfessorNode->getPosition().x;
		  float fishPos_y = mProfessorNode->getPosition().y;
		  float fishPos_z = mProfessorNode->getPosition().z;
		  
		  Vector3 pos = Vector3(fishPos_x, fishPos_y, fishPos_z);
		  mEmptyNode->setPosition(pos);
		  
    // --------------------------------------------------------

    return true;
  }

};

class LectureApp {

  Root* mRoot;
  RenderWindow* mWindow;
  SceneManager* mSceneMgr;
  Camera* mCamera;
  Viewport* mViewport;
  OIS::Keyboard* mKeyboard;
  OIS::InputManager *mInputManager;

  MainListener* mMainListener;
  ESCListener* mESCListener;



public:

  LectureApp() {}

  ~LectureApp() {}

  void go(void)
  {
    // OGREÀÇ ¸ÞÀÎ ·çÆ® ¿ÀºêÁ§Æ® »ý¼º
#if !defined(_DEBUG)
    mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
    mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif


    // ÃÊ±â ½ÃÀÛÀÇ ÄÁÇÇ±Ô·¹ÀÌ¼Ç ¼³Á¤ - ogre.cfg ÀÌ¿ë
    if (!mRoot->restoreConfig()) {
      if (!mRoot->showConfigDialog()) return;
    }

    mWindow = mRoot->initialise(true, "Rotate : Copyleft by Dae-Hyun Lee");


    // ESC key¸¦ ´­·¶À» °æ¿ì, ¿À¿ì°Å ¸ÞÀÎ ·»´õ¸µ ·çÇÁÀÇ Å»ÃâÀ» Ã³¸®
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    OIS::ParamList pl;
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    mInputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false));


    mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "main");
    mCamera = mSceneMgr->createCamera("main");


    mCamera->setPosition(0.0f, 100.0f, 700.0f);
    mCamera->lookAt(0.0f, 100.0f, 0.0f);

    mCamera->setNearClipDistance(5.0f);

    mViewport = mWindow->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.0f,0.0f,0.5f));
    mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));


    ResourceGroupManager::getSingleton().addResourceLocation("resource.zip", "Zip");
	ResourceGroupManager::getSingleton().addResourceLocation("fish.zip", "Zip");
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

    // ÁÂÇ¥Ãà Ç¥½Ã
    Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
    mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode",Ogre::Vector3(0,0,0))->attachObject(mAxesEntity);
    mSceneMgr->getSceneNode("AxesNode")->setScale(6, 6, 6);

    _drawGridPlane();


    Entity* entity1 = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
    SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Professor", Vector3(0.0f, 0.0f, 0.0f));
    node1->attachObject(entity1);

	Entity* entity2 = mSceneMgr->createEntity("Empty","fish.mesh");
	SceneNode* node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Empty", Vector3(0.0f, 0.0f, 0.0f));
	node2->attachObject(entity2);
	mSceneMgr->getSceneNode("Empty")->setScale(0.1, 0.1, 0.1);
	
    Entity* entity3 = mSceneMgr->createEntity("Fish", "fish.mesh");
    SceneNode* node3 = node2->createChildSceneNode("Fish", Vector3(0.0f, 0.0f, 1000.0f));
    node3->attachObject(entity3);
	mSceneMgr->getSceneNode("Fish")->setScale(60, 60, 60);


    mESCListener =new ESCListener(mKeyboard);
    mRoot->addFrameListener(mESCListener);

    mMainListener = new MainListener(mRoot, mKeyboard);
    mRoot->addFrameListener(mMainListener);


    mRoot->startRendering();

    mInputManager->destroyInputObject(mKeyboard);
    OIS::InputManager::destroyInputSystem(mInputManager);

    delete mRoot;
  }

private:
  void _drawGridPlane(void)
  {
    Ogre::ManualObject* gridPlane =  mSceneMgr->createManualObject("GridPlane"); 
    Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode"); 

    Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial","General"); 
    gridPlaneMaterial->setReceiveShadows(false); 
    gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1,1,1,0); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1,1,1); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1,1,1); 

    gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST); 
    for(int i=0; i<21; i++)
    {
      gridPlane->position(-500.0f, 0.0f, 500.0f-i*50);
      gridPlane->position(500.0f, 0.0f, 500.0f-i*50);

      gridPlane->position(-500.f+i*50, 0.f, 500.0f);
      gridPlane->position(-500.f+i*50, 0.f, -500.f);
    }

    gridPlane->end(); 

    gridPlaneNode->attachObject(gridPlane);
  }
};


#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
  int main(int argc, char *argv[])
#endif
  {
    LectureApp app;

    try {

      app.go();

    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      std::cerr << "An exception has occured: " <<
        e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
  }

#ifdef __cplusplus
}
#endif

