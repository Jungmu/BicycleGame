#include "PlayState.h"
#include "TitleState.h"
#include "OptionState.h"


using namespace Ogre;

PlayState PlayState::mPlayState;


void PlayState::enter(void)
{
	angleLock = true;
	keyRight = false;
	keyLeft = false;
	keyRun = false;
	BicycleSpeed = 0.0f;

	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");
	mCamera = mSceneMgr->getCamera("main");
	mCamera->setPosition(Ogre::Vector3::ZERO);

	mMapCamera = mSceneMgr->getCamera("MapCamera");
	mMapCamera->setVisible(true);
	_drawMapPlane();
	_drawGridPlane();
	_setLights();
	_drawGroundPlane();
	_drawBuilding();

	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show();

	mCharacterRoot = mSceneMgr->getRootSceneNode()->createChildSceneNode("ProfessorRoot");
	mCharacterYaw = mCharacterRoot->createChildSceneNode("ProfessorYaw",Vector3(5000.0f,0.0f,0.0f));
	mMapCharector = mSceneMgr->getRootSceneNode()->createChildSceneNode("MapCharectorNode", Vector3(50000.0f, 0.0f, 0.0f));

	mCameraYaw = mCharacterYaw->createChildSceneNode("CameraYaw", Vector3(0.0f, 120.0f, 0.0f));
	mCameraPitch = mCameraYaw->createChildSceneNode("CameraPitch");
	mCameraHolder = mCameraPitch->createChildSceneNode("CameraHolder", Vector3(0.0f, 80.0f, -500.0f));

	mMapCharctorEntity = mSceneMgr->createEntity("MapCharector", "DustinBody.mesh");
	mCharacterEntity = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
	mMapCharector->attachObject(mMapCharctorEntity);
	mCharacterYaw->attachObject(mCharacterEntity);
	
	mMapCharctorEntity->setCastShadows(false);
	mMapCharector->scale(Vector3(10));
	mMapCharector->setInheritOrientation(false);
	mMapCharector->setInheritScale(false);
	mCharacterEntity->setCastShadows(true);

	mCameraHolder->attachObject(mCamera);
	mCamera->lookAt(Vector3(5000.0f, 120.0f, 0.0f));

	mMapCamera->setPosition(50000.0f, 2000.0f, 1.0f);
	mMapCamera->lookAt(50000.0f, -10000.0f, 0.0f);
	

	mAnimationState = mCharacterEntity->getAnimationState("Idle");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
}

void PlayState::BicycleTurn(GameManager* game, const FrameEvent &evt)
{
	if (game->mDatacollector->onArm)
	{
		if (game->mDatacollector->roll_w > 9)
		{
			mCharacterYaw->yaw(Degree(90 * evt.timeSinceLastFrame));
		}
		else if (game->mDatacollector->roll_w < 7)
		{
			mCharacterYaw->yaw(Degree(-90 * evt.timeSinceLastFrame));
		}
	}
}
void PlayState::BicycleRun(GameManager* game, const FrameEvent &evt)
{
	const int buildingNum = 47;
	static std::string buildingStr;
	static char c;
	static Ogre::Vector3 BuildingCenter;
	if (BicycleSpeed < 500)
	{
		mAnimationState->setEnabled(false);
		mAnimationState = mCharacterEntity->getAnimationState("Walk");
		mAnimationState->setEnabled(true);
		mAnimationState->setLoop(true);
	}
	if (BicycleSpeed > 500)
	{
		mAnimationState->setEnabled(false);
		mAnimationState = mCharacterEntity->getAnimationState("Run");
		mAnimationState->setEnabled(true);
		mAnimationState->setLoop(true);
	}

	if (game->mDatacollector->currentPose == myo::Pose::fingersSpread || keyRun)
	{
		if (BicycleSpeed < 3000)
		{
			BicycleSpeed += 1.0f;
		}		
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
			mAnimationState->setEnabled(false);
			mAnimationState = mCharacterEntity->getAnimationState("Idle");
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(true);
		}
	}
	mCharacterYaw->translate(0.0f, 0.0f, BicycleSpeed * evt.timeSinceLastFrame, Node::TS_LOCAL);
	for (int i = 0; i < buildingNum; i++)
	{
		buildingStr = "BuildingNode";
		buildingStr.append(itoa(i, &c, 10));
		if (mCharacterYaw->_getWorldAABB().intersects(mSceneMgr->getSceneNode(buildingStr)->_getWorldAABB()))
		{
			mCharacterYaw->translate(0.0f, 0.0f, -1 * BicycleSpeed * evt.timeSinceLastFrame, Node::TS_LOCAL);
			mCharacterYaw->yaw(Degree(60));
			Ogre::Ray detectRay(mCharacterYaw->getPosition(), mCharacterYaw->getOrientation().zAxis());
			if (!detectRay.intersects(mSceneMgr->getSceneNode(buildingStr)->_getWorldAABB()).first)
			{
				mCharacterYaw->yaw(Degree(-55));
				if (BicycleSpeed > 5.0f)
					BicycleSpeed -= 5.0f;
			}
			else
			{
				mCharacterYaw->yaw(Degree(-120));
				detectRay.setDirection(mCharacterYaw->getOrientation().zAxis());
				if (!detectRay.intersects(mSceneMgr->getSceneNode(buildingStr)->_getWorldAABB()).first)
				{
					mCharacterYaw->yaw(Degree(55));
					if (BicycleSpeed > 5.0f)
						BicycleSpeed -= 5.0f;
				}
				else{
					mCharacterYaw->yaw(Degree(60));
					mCharacterYaw->translate(0.0f, 0.0f, -2 * BicycleSpeed * evt.timeSinceLastFrame, Node::TS_LOCAL);
					if (BicycleSpeed > 10.0f)
						BicycleSpeed -= 10.0f;
				}
			}
			
			mCharacterYaw->translate(0.0f, 0.0f, BicycleSpeed * evt.timeSinceLastFrame, Node::TS_LOCAL);
		}
	}
	mMapCharector->setPosition(mCharacterYaw->getPosition().x + 50000.0f,-10000.0f, mCharacterYaw->getPosition().z);
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
	//test code 실제 게임에서는 키보드로 캐릭터를 움직이지 않음
	if (keyRight)
	{
		if (!keyRun)
		{
			mCharacterYaw->yaw(Degree(-360 * evt.timeSinceLastFrame));
		}
		else
		{
			mCharacterYaw->yaw(Degree(-90 * evt.timeSinceLastFrame));
		}
		
	}
	else if (keyLeft)
	{
		if (!keyRun)
		{
			mCharacterYaw->yaw(Degree(360 * evt.timeSinceLastFrame));
		}
		else
		{
			mCharacterYaw->yaw(Degree(90 * evt.timeSinceLastFrame));
		}
	}
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
	switch (e.key)
	{
	case OIS::KC_UP:
		keyRun = false;
		break;
	case OIS::KC_RIGHT:
		keyRight = false;
		break;
	case OIS::KC_LEFT:
		keyLeft = false;
		break;
	default:
		break;
	}
	return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_UP:
		keyRun = true;
		break;
	case OIS::KC_RIGHT:
		keyRight = true;
		break;
	case OIS::KC_LEFT:
		keyLeft = true;
		break;
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
	angleLock = false;
	return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	angleLock = true;
	return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	if (!angleLock)
	{
		mCameraYaw->yaw(Degree(-e.state.X.rel));
		mCameraPitch->pitch(Degree(-e.state.Y.rel));
		mCameraHolder->translate(Ogre::Vector3(0, 0, -e.state.Z.rel * 0.1f));
	}	
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
	const int buildingNum = 47;
	static std::string buildingStr;
	static char c;
	Ogre::SceneNode* BuildingNode[buildingNum];
	BuildingNode[0] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode0", Vector3(-5500.0f, 500.0f, -5000.0f));
	BuildingNode[1] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode1", Vector3(-4000.0f, 500.0f, -5000.0f));
	BuildingNode[2] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode2", Vector3(-2500.0f, 500.0f, -5000.0f));
	BuildingNode[3] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode3", Vector3(-1000.0f, 500.0f, -5000.0f));
	BuildingNode[4] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode4", Vector3(500.0f, 500.0f, -5000.0f));
	BuildingNode[5] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode5", Vector3(2000.0f, 500.0f, -5000.0f));
	BuildingNode[6] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode6", Vector3(3500.0f, 500.0f, -5000.0f));
	BuildingNode[7] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode7", Vector3(5000.0f, 500.0f, -5000.0f));
	BuildingNode[8] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode8", Vector3(-4000.0f, 500.0f, -3000.0f));
	BuildingNode[9] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode9", Vector3(-2500.0f, 500.0f, -3000.0f));
	BuildingNode[27] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode27", Vector3(-1000.0f, 500.0f, -3000.0f));
	BuildingNode[10] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode10", Vector3(500.0f,500.0f, -3000.0f));
	BuildingNode[11] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode11", Vector3(2000.0f, 500.0f, -3000.0f));
	BuildingNode[12] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode12", Vector3(3500.0f, 500.0f, -3000.0f));
	BuildingNode[13] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode13", Vector3(-3500.0f, 500.0f, 1000.0f));
	BuildingNode[14] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode14", Vector3(-2000.0f, 500.0f, 1000.0f));
	BuildingNode[15] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode15", Vector3(-500.0f, 500.0f, 1000.0f));
	BuildingNode[16] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode16", Vector3(-5500.0f, 500.0f, 3000.0f));
	BuildingNode[17] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode17", Vector3(-4000.0f, 500.0f, 3000.0f));
	BuildingNode[18] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode18", Vector3(-2500.0f, 500.0f, 3000.0f));
	BuildingNode[19] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode19", Vector3(500.0f, 500.0f, 3000.0f));
	BuildingNode[20] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode20", Vector3(2000.0f, 500.0f, 3000.0f));
	BuildingNode[21] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode21", Vector3(3500.0f, 500.0f, 3000.0f));
	BuildingNode[22] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode22", Vector3(-1000.0f, 500.0f, 5000.0f));
	BuildingNode[23] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode23", Vector3(500.0f, 500.0f, 5000.0f));
	BuildingNode[24] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode24", Vector3(2000.0f, 500.0f, 5000.0f));
	BuildingNode[25] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode25", Vector3(3500.0f, 500.0f, 5000.0f));
	BuildingNode[26] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode26", Vector3(5000.0f, 500.0f, 5000.0f));
	///// 세로 건물
	BuildingNode[28] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode28", Vector3(-6000.0f, 500.0f, -4000.0f));
	BuildingNode[29] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode29", Vector3(-6000.0f, 500.0f, -2500.0f));
	BuildingNode[30] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode30", Vector3(-6000.0f, 500.0f, -1000.0f));
	BuildingNode[31] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode31", Vector3(-6000.0f, 500.0f, 500.0f));
	BuildingNode[32] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode32", Vector3(-6000.0f, 500.0f, 2000.0f));
	BuildingNode[33] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode33", Vector3(-4000.0f, 500.0f, -1500.0f));
	BuildingNode[34] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode34", Vector3(-4000.0f, 500.0f, 0.0f));
	BuildingNode[35] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode35", Vector3(0.0f, 500.0f, 2000.0f));
	BuildingNode[36] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode36", Vector3(6000.0f, 500.0f, -4000.0f));
	BuildingNode[37] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode37", Vector3(6000.0f, 500.0f, -2500.0f));
	BuildingNode[38] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode38", Vector3(6000.0f, 500.0f, -1000.0f));
	BuildingNode[39] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode39", Vector3(6000.0f, 500.0f, 500.0f));
	BuildingNode[40] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode40", Vector3(6000.0f, 500.0f, 2000.0f));
	BuildingNode[41] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode41", Vector3(6000.0f, 500.0f, 3500.0f));
	BuildingNode[42] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode42", Vector3(4000.0f, 500.0f, -1500.0f));
	BuildingNode[43] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode43", Vector3(4000.0f, 500.0f, 0.0f));
	BuildingNode[44] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode44", Vector3(4000.0f, 500.0f, 1500.0f));
	BuildingNode[45] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode45", Vector3(4000.0f, 500.0f, 3000.0f));
	BuildingNode[46] = mSceneMgr->getRootSceneNode()->createChildSceneNode("BuildingNode46", Vector3(-2000.0f, 500.0f, 4000.0f));
	
	for (int i = 28; i < 46; ++i)
	{
		BuildingNode[i]->yaw(Degree(90));
	}

	Ogre::Entity* Building[buildingNum];
	for (int i = 0; i < buildingNum; i++)
	{
		buildingStr = "BuildingNode";
		buildingStr.append(itoa(i, &c, 10));
		Building[i] = mSceneMgr->createEntity(buildingStr, "Box01.mesh");
		BuildingNode[i]->attachObject(Building[i]);
		Building[i]->setCastShadows(true);		
		mSceneMgr->getSceneNode(buildingStr)->setScale(Ogre::Vector3(10, 10, 10));
	}

}
void PlayState::_drawMapPlane(void)
{
	//---------------------------

	Ogre::Plane Mvplane1(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"MVerticalRoad1",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Mvplane1,
		1000, 7000, 20, 20,
		true,
		1, 1, 7,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* MVerticalRoad1 = mSceneMgr->createEntity("MVerticalRoad1");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(55000, -10000, 0))->attachObject(MVerticalRoad1);
	MVerticalRoad1->setCastShadows(false);
	MVerticalRoad1->setMaterialName("road1");

	//--------------------------

	Ogre::Plane Mvplane2(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"MVerticalRoad2",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Mvplane2,
		1000, 5000, 20, 20,
		true,
		1, 1, 5,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* MVerticalRoad2 = mSceneMgr->createEntity("MVerticalRoad2");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(45000, -10000, -1000))->attachObject(MVerticalRoad2);
	MVerticalRoad2->setCastShadows(false);
	MVerticalRoad2->setMaterialName("road1");

	//--------------------------

	Ogre::Plane Mvplane3(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"MVerticalRoad3",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Mvplane3,
		1000, 1000, 20, 20,
		true,
		1, 1, 1,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* MVerticalRoad3 = mSceneMgr->createEntity("MVerticalRoad3");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(49000, -10000, 3000))->attachObject(MVerticalRoad3);
	MVerticalRoad3->setCastShadows(false);
	MVerticalRoad3->setMaterialName("road1");


	//--------------------------

	Ogre::Plane Mhplane1(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"MHorizontalRoad1",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Mhplane1,
		9000, 1000, 20, 20,
		true,
		1, 9, 1,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* MHorizontalRoad1 = mSceneMgr->createEntity("MHorizontalRoad1");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(50000, -10000, -4000))->attachObject(MHorizontalRoad1);
	MHorizontalRoad1->setCastShadows(false);
	MHorizontalRoad1->setMaterialName("road2");


	//--------------------------

	Ogre::Plane Mhplane2(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"MHorizontalRoad2",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Mhplane2,
		3000, 1000, 20, 20,
		true,
		1, 3, 1,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* MHorizontalRoad2 = mSceneMgr->createEntity("MHorizontalRoad2");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(47000, -10000, 2000))->attachObject(MHorizontalRoad2);
	MHorizontalRoad2->setCastShadows(false);
	MHorizontalRoad2->setMaterialName("road2");

	//--------------------------

	Ogre::Plane Mhplane3(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"MHorizontalRoad3",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Mhplane3,
		5000, 1000, 20, 20,
		true,
		1, 5, 1,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* MHorizontalRoad3 = mSceneMgr->createEntity("MHorizontalRoad3");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(52000, -10000, 4000))->attachObject(MHorizontalRoad3);
	MHorizontalRoad3->setCastShadows(false);
	MHorizontalRoad3->setMaterialName("road2");

	//--------------------------

	Ogre::Plane Mcplane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"Mconer",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Mcplane,
		1000, 1000, 20, 20,
		true,
		1, 1, 1,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* Mconer1 = mSceneMgr->createEntity("Mconer");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(45000, -10000, 2000))->attachObject(Mconer1);
	Mconer1->setCastShadows(false);
	Mconer1->setMaterialName("road0");



	//--------------------------

	Ogre::Entity* Mconer2 = mSceneMgr->createEntity("Mconer");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("Mconer2", Ogre::Vector3(49000, -10000, 2000))->attachObject(Mconer2);
	mSceneMgr->getSceneNode("Mconer2")->yaw(Degree(180));
	Mconer2->setCastShadows(false);
	Mconer2->setMaterialName("road0");

	//--------------------------

	Ogre::Entity* Mconer3 = mSceneMgr->createEntity("Mconer");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(49000, -10000, 4000))->attachObject(Mconer3);
	Mconer3->setCastShadows(false);
	Mconer3->setMaterialName("road0");

	//--------------------------

	Ogre::Entity* Mconer4 = mSceneMgr->createEntity("Mconer");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("Mconer4", Ogre::Vector3(55000, -10000, -4000))->attachObject(Mconer4);
	mSceneMgr->getSceneNode("Mconer4")->yaw(Degree(180));
	Mconer4->setCastShadows(false);
	Mconer4->setMaterialName("road0");

	//--------------------------

	Ogre::Entity* Mconer5 = mSceneMgr->createEntity("Mconer");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("Mconer5", Ogre::Vector3(45000, -10000, -4000))->attachObject(Mconer5);
	mSceneMgr->getSceneNode("Mconer5")->yaw(Degree(-90));
	Mconer5->setCastShadows(false);
	Mconer5->setMaterialName("road0");
	//--------------------------

	Ogre::Entity* Mconer6 = mSceneMgr->createEntity("Mconer");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("Mconer6", Ogre::Vector3(55000, -10000, 4000))->attachObject(Mconer6);
	mSceneMgr->getSceneNode("Mconer6")->yaw(Degree(90));
	Mconer6->setCastShadows(false);
	Mconer6->setMaterialName("road0");
}
void PlayState::_drawGroundPlane(void)
{
	//---------------------------

	Ogre::Plane infPlane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"infGround",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		infPlane,
		50000, 50000, 20, 20,
		true,
		1, 500, 500,
		Ogre::Vector3::UNIT_Z);
	Ogre::Entity* infGround = mSceneMgr->createEntity("infGround");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, -1, 0))->attachObject(infGround);
	infGround->setCastShadows(false);
	infGround->setMaterialName("Examples/Rockwall");
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