#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
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
	  static float rotationCount = 0.0f;
	  static float movingSpeed = 1.0f;
	  static float rotationSpeed = 180.0f;
	  static float fishRotationSpeed = -180.0f;
	  static float professorMaxPosition_z = 250.0f;
	  
	  // 범위를 벗어나면 교수는 회전한 뒤 이동한다.
	  if (mProfessorNode->getPosition().z < -1 * professorMaxPosition_z || mProfessorNode->getPosition().z > professorMaxPosition_z)
	  {
		  mProfessorNode->yaw(Degree(rotationSpeed * evt.timeSinceLastFrame));
		  rotationCount += rotationSpeed * evt.timeSinceLastFrame;
		  
		  Vector3 professorPos = mProfessorNode->getPosition();
		  // 회전을 다해서 누적 회전각이 180을 넘어가면 앞으로 전진하여 범위 안으로 들어오게 된다.
		  if (rotationCount >= 180 )
		  {			  
			  // 250범위를 약간 넘어갈 수 있기 때문에 이를 250.0f 라는 정확한 좌표로 지정을 해준다.
			  professorPos.z = mProfessorNode->getPosition().z > 0 ? 250.0f : -1 * 250.0f;
			  Vector3 pos = professorPos;
			  mProfessorNode->setPosition(pos);
			  // 회전각이 evt.timeSinceLastFrame 과 곱해지기때문에 정확히 180도를 돌지 않게된다. 
			  // 그래서 180도를 돈 상태로 만들어준다.
			  if (250.0f == professorPos.z)
			  {
				  mProfessorNode->resetOrientation();
				  mProfessorNode->yaw(Degree(180));
			  }
			  else
			  {
				  mProfessorNode->resetOrientation();
			  }
			  // 회전이 이루어 졌으므로 한걸음 앞으로 나아가서 범위 안으로 들어간다.
			  mProfessorNode->translate(0.0f, 0.0f, movingSpeed * evt.timeSinceLastFrame, Node::TransformSpace::TS_LOCAL);
			  rotationCount = 0;
		  }		  
	  }
	  else // 범위를 벗어나지 않았다면 교수는 그저 앞으로 전진한다.
	  {
		  mProfessorNode->translate(0.0f, 0.0f, movingSpeed, Node::TransformSpace::TS_LOCAL);
	  }
	  // 물고기는 일정한 속도로 교수 주변을 계속 회전하며 ( 물고기의 부모 노드가 회전하는 것 )
	  // 물고기의 부모 노드는 항상 교수노드와 같은 위치에 존재한다.
	  mEmptyNode->yaw(Degree(fishRotationSpeed * evt.timeSinceLastFrame));
	  mEmptyNode->setPosition(mProfessorNode->getPosition());

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

