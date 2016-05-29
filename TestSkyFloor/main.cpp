#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Ogre.h>
#include <OIS/OIS.h>



using namespace Ogre;



class KeyboardListener : public FrameListener{
	OIS::Keyboard *mKeyboard;

public:
	KeyboardListener(OIS::Keyboard *keyboard) : mKeyboard(keyboard) {}
	bool frameStarted(const FrameEvent &evt)
	{
		mKeyboard->capture();
		return !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
	}
};


class MainListener : public FrameListener {
	OIS::Keyboard *mKeyboard;
	Root* mRoot;
	SceneNode *mBicycleNode;
	Ogre::Entity *mBicycleEntity;
	Ogre::AnimationState* mAnimationState;

	std::deque<Vector3> mWalkList;
	Real mWalkSpeed;
	Vector3 mDirection;
	Real mDistance;
	Vector3 mDestination;

public:
	MainListener(Root* root, OIS::Keyboard *keyboard) : mKeyboard(keyboard), mRoot(root)
	{

		mBicycleNode = mRoot->getSceneManager("main")->getSceneNode("Bicycle");
		mBicycleEntity = mRoot->getSceneManager("main")->getEntity("Bicycle");


	}

	bool frameStarted(const FrameEvent &evt)
	{

		return true;
	}
	
};

class LectureApp {

	Root* mRoot;
	RenderWindow* mWindow;
	SceneManager* mSceneMgr;
	MaterialManager* mMaterialMgr;
	Camera* mCamera;
	Viewport* mViewport;
	OIS::Keyboard* mKeyboard;
	OIS::InputManager *mInputManager;

	MainListener* mMainListener;
	KeyboardListener* mKeyboardListener;

public:

	LectureApp() {}

	~LectureApp() {}

	void go(void)
	{
#if !defined(_DEBUG)
		mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
		mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif


		if (!mRoot->restoreConfig()) {
			if (!mRoot->showConfigDialog()) return;
		}

		mWindow = mRoot->initialise(true, "Walking Around Bicycle : Copyleft by Dae-Hyun Lee");


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


		mCamera->setPosition(0.0f, 10000.0f, -1000.0f);
		mCamera->lookAt(0.0f, 0.0f, 0.0f);

		mCamera->setNearClipDistance(5.0f);

		mViewport = mWindow->addViewport(mCamera);
		mViewport->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.5f));
		mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));


		ResourceGroupManager::getSingleton().addResourceLocation("resource.zip", "Zip");
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));


		Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
		mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
		mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

		_drawGridPlane();

		//---------------------------

		//Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, 0);
		//Ogre::MeshManager::getSingleton().createPlane(
		//	"Ground",
		//	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		//	plane1,
		//	9000, 5000, 20, 20,
		//	true,
		//	1, 90, 50,
		//	Ogre::Vector3::UNIT_Z);
		//Ogre::Entity* Ground = mSceneMgr->createEntity("Ground");
		//mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 0, -1000))->attachObject(Ground);
		//Ground->setCastShadows(false);
		//Ground->setMaterialName("Examples/Rockwall");
		////---------------------------

		Ogre::Plane plane2(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane(
			"Ground2",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane2,
			5000, 2000, 20, 20,
			true,
			1, 50, 20,
			Ogre::Vector3::UNIT_Z);
		Ogre::Entity* Ground2 = mSceneMgr->createEntity("Ground2");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(2000, 0, 2500))->attachObject(Ground2);
		Ground2->setCastShadows(false);
		Ground2->setMaterialName("Examples/Rockwall");

		//---------------------------

		Ogre::Plane vplane1(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane(
			"VerticalRoad1",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			vplane1,
			1000, 7000, 20, 20,
			true,
			1, 1, 7,
			Ogre::Vector3::UNIT_Z);
		Ogre::Entity* VerticalRoad1 = mSceneMgr->createEntity("VerticalRoad1");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(5000, 0, 0))->attachObject(VerticalRoad1);
		VerticalRoad1->setCastShadows(false);
		VerticalRoad1->setMaterialName("road1");

		//--------------------------

		Ogre::Plane vplane2(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane(
			"VerticalRoad2",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			vplane2,
			1000, 5000, 20, 20,
			true,
			1, 1, 5,
			Ogre::Vector3::UNIT_Z);
		Ogre::Entity* VerticalRoad2 = mSceneMgr->createEntity("VerticalRoad2");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-5000, 0, -1000))->attachObject(VerticalRoad2);
		VerticalRoad2->setCastShadows(false);
		VerticalRoad2->setMaterialName("road1");

		//--------------------------

		Ogre::Plane vplane3(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane(
			"VerticalRoad3",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			vplane3,
			1000, 1000, 20, 20,
			true,
			1, 1, 1,
			Ogre::Vector3::UNIT_Z);
		Ogre::Entity* VerticalRoad3 = mSceneMgr->createEntity("VerticalRoad3");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-1000, 0, 3000))->attachObject(VerticalRoad3);
		VerticalRoad3->setCastShadows(false);
		VerticalRoad3->setMaterialName("road1");


		//--------------------------

		Ogre::Plane hplane1(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane(
			"HorizontalRoad1",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			hplane1,
			9000, 1000, 20, 20,
			true,
			1, 9, 1,
			Ogre::Vector3::UNIT_Z);
		Ogre::Entity* HorizontalRoad1 = mSceneMgr->createEntity("HorizontalRoad1");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 0, -4000))->attachObject(HorizontalRoad1);
		HorizontalRoad1->setCastShadows(false);
		HorizontalRoad1->setMaterialName("road2");


		//--------------------------

		Ogre::Plane hplane2(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane(
			"HorizontalRoad2",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			hplane2,
			3000, 1000, 20, 20,
			true,
			1, 3, 1,
			Ogre::Vector3::UNIT_Z);
		Ogre::Entity* HorizontalRoad2 = mSceneMgr->createEntity("HorizontalRoad2");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-3000, 0, 2000))->attachObject(HorizontalRoad2);
		HorizontalRoad2->setCastShadows(false);
		HorizontalRoad2->setMaterialName("road2");

		//--------------------------

		Ogre::Plane hplane3(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane(
			"HorizontalRoad3",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			hplane3,
			5000, 1000, 20, 20,
			true,
			1, 5, 1,
			Ogre::Vector3::UNIT_Z);
		Ogre::Entity* HorizontalRoad3 = mSceneMgr->createEntity("HorizontalRoad3");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(2000, 0, 4000))->attachObject(HorizontalRoad3);
		HorizontalRoad3->setCastShadows(false);
		HorizontalRoad3->setMaterialName("road2");

		//--------------------------

		Ogre::Plane cplane(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane(
			"coner",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			cplane,
			1000, 1000, 20, 20,
			true,
			1, 1, 1,
			Ogre::Vector3::UNIT_Z);
		Ogre::Entity* coner1 = mSceneMgr->createEntity("coner");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-5000, 0, 2000))->attachObject(coner1);
		coner1->setCastShadows(false);
		coner1->setMaterialName("road0");

		

		//--------------------------
	
		Ogre::Entity* coner2 = mSceneMgr->createEntity("coner");
		mSceneMgr->getRootSceneNode()->createChildSceneNode("coner2", Ogre::Vector3(-1000, 0, 2000))->attachObject(coner2);
		mSceneMgr->getSceneNode("coner2")->yaw(Degree(180));
		coner2->setCastShadows(false);
		coner2->setMaterialName("road0");

		//--------------------------

		Ogre::Entity* coner3 = mSceneMgr->createEntity("coner");
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-1000, 0, 4000))->attachObject(coner3);
		coner3->setCastShadows(false);
		coner3->setMaterialName("road0");

		//--------------------------

		Ogre::Entity* coner4 = mSceneMgr->createEntity("coner");
		mSceneMgr->getRootSceneNode()->createChildSceneNode("coner4",Ogre::Vector3(5000, 0, -4000))->attachObject(coner4);
		mSceneMgr->getSceneNode("coner4")->yaw(Degree(180));
		coner4->setCastShadows(false);
		coner4->setMaterialName("road0");

		//--------------------------

		Ogre::Entity* coner5 = mSceneMgr->createEntity("coner");
		mSceneMgr->getRootSceneNode()->createChildSceneNode("coner5",Ogre::Vector3(-5000, 0, -4000))->attachObject(coner5);
		mSceneMgr->getSceneNode("coner5")->yaw(Degree(-90));
		coner5->setCastShadows(false);
		coner5->setMaterialName("road0");
		//--------------------------

		Ogre::Entity* coner6 = mSceneMgr->createEntity("coner");
		mSceneMgr->getRootSceneNode()->createChildSceneNode("coner6", Ogre::Vector3(5000, 0, 4000))->attachObject(coner6);
		mSceneMgr->getSceneNode("coner6")->yaw(Degree(90));
		coner6->setCastShadows(false);
		coner6->setMaterialName("road0");


		mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

		///// -----------------
		Entity* entity1 = mSceneMgr->createEntity("Bicycle", "ninja.mesh");
		SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Bicycle", Vector3(0.0f, 0.0f, 0.0f));
		node1->yaw(Degree(180));
		node1->attachObject(entity1);

		///// -----------------
		Entity* building1 = mSceneMgr->createEntity("Building", "Box01.mesh");
		SceneNode* buildingNode1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Building", Vector3(100.0f, 100.0f, 0.0f));
		buildingNode1->attachObject(building1);

		mKeyboardListener = new KeyboardListener(mKeyboard);
		mRoot->addFrameListener(mKeyboardListener);

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
		Ogre::ManualObject* gridPlane = mSceneMgr->createManualObject("GridPlane");
		Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode");

		Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial", "General");
		gridPlaneMaterial->setReceiveShadows(false);
		gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true);
		gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1, 1, 1, 0);
		gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1, 1, 1);
		gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1, 1, 1);

		gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST);
		for (int i = 0; i < 21; i++)
		{
			gridPlane->position(-500.0f, 0.0f, 500.0f - i * 50);
			gridPlane->position(500.0f, 0.0f, 500.0f - i * 50);

			gridPlane->position(-500.f + i * 50, 0.f, 500.0f);
			gridPlane->position(-500.f + i * 50, 0.f, -500.f);
		}

		gridPlane->end();

		gridPlaneNode->attachObject(gridPlane);
	}
};


#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char *argv[])
#endif
	{
		LectureApp app;

		try {

			app.go();

		}
		catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
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

