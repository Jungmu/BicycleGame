#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "DataCollector.h"
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
	myo::Hub* mHub;
	DataCollector* mCollector;
	SceneNode *mBicycleNode;
	Ogre::Entity *mBicycleEntity;
	Ogre::AnimationState* mAnimationState;

	std::deque<Vector3> mWalkList;
	Real mWalkSpeed;
	Vector3 mDirection;
	Real mDistance;
	Vector3 mDestination;

public:
	MainListener(Root* root, OIS::Keyboard *keyboard, myo::Hub* hub, DataCollector* collector) : mKeyboard(keyboard), mRoot(root), mHub(hub), mCollector(collector)
	{
		mBicycleNode = mRoot->getSceneManager("main")->getSceneNode("Bicycle");
		mBicycleEntity = mRoot->getSceneManager("main")->getEntity("Bicycle");
	}

	bool frameStarted(const FrameEvent &evt)
	{
		// Myo Loop

		// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
		// 데이터를 어느정도 주기로 받아올지 정하는 소스
		// 이 값이 낮아지면 영상을 받아오는데도 딜레이가 걸리기때문에 원하는 fps를 고려해야한다.
		mHub->run(1000 / 500);
		// After processing events, we call the print() member function we defined above to print out the values we've
		// obtained from any events that have occurred.
		// 마이오 상태 모니터링 코드
		mCollector->print();

		if (mCollector->currentPose == myo::Pose::fist)
		{
			mBicycleNode->translate(0.0f, 0.1f, 0.0f);
		}
		// 마이오 루프 여기까지
		return true;
	}

};

class LectureApp {


	DataCollector collector;
	Root* mRoot;
	RenderWindow* mWindow;
	SceneManager* mSceneMgr;
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

		try
		{
			myo::Hub hub("");
			// 마이오 찾는중 ...
			std::cout << "Attempting to find a Myo..." << std::endl;

			// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
			// immediately.
			// waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
			// if that fails, the function will return a null pointer.
			// 마이오를 찾는 동안 대기하는 소스코드
			myo::Myo* myo = hub.waitForMyo(10000);

			// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
			// Hub::run() to send events to all registered device listeners.
			// 데이터를 지속적으로 받아온다.
			hub.addListener(&collector);

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


			mCamera->setPosition(0.0f, 100.0f, 500.0f);
			mCamera->lookAt(0.0f, 100.0f, 0.0f);

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


			Entity* entity1 = mSceneMgr->createEntity("Bicycle", "DustinBody.mesh");
			SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Bicycle", Vector3(0.0f, 0.0f, 0.0f));
			node1->yaw(Degree(180));
			node1->attachObject(entity1);

			mKeyboardListener = new KeyboardListener(mKeyboard);
			mRoot->addFrameListener(mKeyboardListener);

			mMainListener = new MainListener(mRoot, mKeyboard, &hub, &collector);
			mRoot->addFrameListener(mMainListener);


			mRoot->startRendering();

			mInputManager->destroyInputObject(mKeyboard);
			OIS::InputManager::destroyInputSystem(mInputManager);

			delete mRoot;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			std::cerr << "Press enter to continue.";
			std::cin.ignore();
			return;
		}
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

