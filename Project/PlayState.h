#pragma once
#include "DataCollector.h"
#include "GameState.h"

class PlayState : public GameState
{
public:
	bool keyRun,keyLeft,keyRight,angleLock;
	float BicycleSpeed;
	const int buildingNum = 47;
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


	static PlayState* getInstance() { return &mPlayState; }

	std::string anim;

private:

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

};


