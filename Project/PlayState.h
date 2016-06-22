#pragma once
#include "DataCollector.h"
#include "GameState.h"
#include "TitleState.h"
#include "OptionState.h"
#include <chrono>

using namespace std;
using namespace chrono;

class PlayState : public GameState
{
public:
	system_clock::time_point start;
	duration<double> time = start - start;
	bool keyRun,keyLeft = false ,keyRight = false ,angleLock;
	float BicycleSpeed;
	const int buildingNum = 47;
	float LAPS = 0;
	void enter(void);
	void exit(void);

	void pause(void);
	void resume(void);

	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

	bool mouseMoved(GameManager* game, const OIS::MouseEvent &e);
	bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);

	bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
	bool keyReleased(GameManager* game, const OIS::KeyEvent &e);

	void BicycleTurn(GameManager* game, const Ogre::FrameEvent &evt);
	void BicycleRun(GameManager* game, const Ogre::FrameEvent &evt);
	void GameEnd(GameManager* game);
	void _setOverlay(void);
	static PlayState* getInstance() { return &mPlayState; }
	Ogre::OverlayElement* textBox;
	std::string anim;

private:
	void _setItem(void);
	void _setLights(void);
	void _drawGroundPlane(void);
	void _drawGridPlane(void);
	void _drawBuilding(void);
	void _drawMapPlane(void);
	static PlayState mPlayState;

	Ogre::Root *mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;
	Ogre::Camera* mMapCamera;
	Ogre::Viewport* mMapViewport;
	Ogre::Light *mLightP, *mLightD, *mLightS;

	Ogre::SceneNode* mCharacterRoot;
	Ogre::SceneNode* mCharacterYaw;
	Ogre::SceneNode* mCameraHolder;
	Ogre::SceneNode* mCameraYaw;
	Ogre::SceneNode* mCameraPitch;
	Ogre::SceneNode* mMapCharector;

	Ogre::Entity* mCharacterEntity;
	Ogre::Entity* mMapCharctorEntity;
	Ogre::AnimationState* mAnimationState;

	Ogre::Overlay*           mInformationOverlay;
	Ogre::OverlaySystem* mOverlaySystem;
	Ogre::OverlayManager*    mOverlayMgr;
	Ogre::Overlay*           mTextOverlay;
	Ogre::Overlay*           mLogoOverlay;
	Ogre::Overlay*           mDebugOverlay;
	Ogre::OverlayContainer*  mPanel;
};


