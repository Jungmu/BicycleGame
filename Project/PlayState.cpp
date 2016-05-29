#include "PlayState.h"
#include "TitleState.h"
#include "OptionState.h"


using namespace Ogre;

PlayState PlayState::mPlayState;


void PlayState::enter(void)
{
	
	BicycleSpeed = 0.0f;

	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");
	mCamera = mSceneMgr->getCamera("main");
	mCamera->setPosition(Ogre::Vector3::ZERO);

	_drawGridPlane();
	_setLights();
	_drawGroundPlane();
	_drawBuilding();

	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show();

	mCharacterRoot = mSceneMgr->getRootSceneNode()->createChildSceneNode("ProfessorRoot");
	mCharacterYaw = mCharacterRoot->createChildSceneNode("ProfessorYaw");

	mCameraYaw = mCharacterRoot->createChildSceneNode("CameraYaw", Vector3(0.0f, 120.0f, 0.0f));
	mCameraPitch = mCameraYaw->createChildSceneNode("CameraPitch");
	mCameraHolder = mCameraPitch->createChildSceneNode("CameraHolder", Vector3(0.0f, 80.0f, 500.0f));

	mCharacterEntity = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
	mCharacterYaw->attachObject(mCharacterEntity);
	mCharacterEntity->setCastShadows(true);

	mCameraHolder->attachObject(mCamera);
	mCamera->lookAt(mCameraYaw->getPosition());

	mAnimationState = mCharacterEntity->getAnimationState("Idle");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
}

void PlayState::BicycleTurn(GameManager* game, const FrameEvent &evt)
{
	if (game->mDatacollector->roll_w > 10)
	{
		mCharacterYaw->yaw(Degree(90 * evt.timeSinceLastFrame));
	}
	else if (game->mDatacollector->roll_w < 8)
	{
		mCharacterYaw->yaw(Degree(-90 * evt.timeSinceLastFrame));
	}
}
void PlayState::BicycleRun(GameManager* game, const FrameEvent &evt)
{
	if (game->mDatacollector->currentPose == myo::Pose::fingersSpread)
	{
		if (mAnimationState->getAnimationName() == "Idle")
		{
			mAnimationState = mCharacterEntity->getAnimationState("Run");
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(true);
		}
		BicycleSpeed += 1.0f;
	}
	else
	{
		if (BicycleSpeed > 0)
		{
			BicycleSpeed -= 1.0f;
		}
		else if (BicycleSpeed < 0)
		{
			BicycleSpeed = 0.0f;
		}
		if (BicycleSpeed == 0)
		{
			mAnimationState = mCharacterEntity->getAnimationState("Idle");
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(true);
		}
	}
	mCharacterYaw->translate(0.0f, 0.0f, BicycleSpeed * evt.timeSinceLastFrame, Node::TS_LOCAL);
}

void PlayState::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
	mInformationOverlay->hide();

	// ---------------------------------------
}

void PlayState::pause(void)
{
}

void PlayState::resume(void)
{
	mAnimationState->setEnabled(false);
	mAnimationState = mCharacterEntity->getAnimationState(anim);
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	mAnimationState->addTime(evt.timeSinceLastFrame);
	BicycleTurn(game,evt);
	BicycleRun(game,evt);
	return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	static Ogre::DisplayString currFps = L"Cur FPS: ";
	static Ogre::DisplayString avgFps = L"Avg FPS: ";
	static Ogre::DisplayString bestFps = L"Best FPS: ";
	static Ogre::DisplayString worstFps = L"Worst FPS: ";

	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");

	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();

	guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
	guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
	guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
	guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS));

	return true;
}


bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_O:

		game->pushState(OptionState::getInstance());
		break;
	case OIS::KC_ESCAPE:
		game->changeState(TitleState::getInstance());
		break;
	default:
		break;
	}
	// -----------------------------------------------------

	return true;
}

bool PlayState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	mCameraYaw->yaw(Degree(-e.state.X.rel));
	mCameraPitch->pitch(Degree(-e.state.Y.rel));

	mCameraHolder->translate(Ogre::Vector3(0, 0, -e.state.Z.rel * 0.1f));
	return true;
}



void PlayState::_setLights(void)
{
	mSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_DIRECTIONAL);
	mLightD->setDirection(Vector3(1, -2.0f, -1));
	mLightD->setVisible(true);
}
void PlayState::_drawBuilding(void)
{
	Ogre::SceneNode* BuildingNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode", Vector3(3000.0f, 500.0f, 0.0f));
	Ogre::Entity* Building = mSceneMgr->createEntity("Building", "Box01.mesh");
	BuildingNode->attachObject(Building);
	Building->setCastShadows(true);
	mSceneMgr->getSceneNode("BuildingNode")->setScale(Ogre::Vector3(10, 10, 10));
}
void PlayState::_drawGroundPlane(void)
{
	//---------------------------

	Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"Ground",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane1,
		9000, 5000, 20, 20,
		true,
		1, 90, 50,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* Ground = mSceneMgr->createEntity("Ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 0, -1000))->attachObject(Ground);
	Ground->setCastShadows(false);
	Ground->setMaterialName("Examples/Rockwall");

	//---------------------------

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
	mSceneMgr->getRootSceneNode()->createChildSceneNode("coner4", Ogre::Vector3(5000, 0, -4000))->attachObject(coner4);
	mSceneMgr->getSceneNode("coner4")->yaw(Degree(180));
	coner4->setCastShadows(false);
	coner4->setMaterialName("road0");

	//--------------------------

	Ogre::Entity* coner5 = mSceneMgr->createEntity("coner");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("coner5", Ogre::Vector3(-5000, 0, -4000))->attachObject(coner5);
	mSceneMgr->getSceneNode("coner5")->yaw(Degree(-90));
	coner5->setCastShadows(false);
	coner5->setMaterialName("road0");
	//--------------------------

	Ogre::Entity* coner6 = mSceneMgr->createEntity("coner");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("coner6", Ogre::Vector3(5000, 0, 4000))->attachObject(coner6);
	mSceneMgr->getSceneNode("coner6")->yaw(Degree(90));
	coner6->setCastShadows(false);
	coner6->setMaterialName("road0");
}

void PlayState::_drawGridPlane(void)
{
	// ÁÂÇ¥Ãà Ç¥½Ã
	Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
	mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

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