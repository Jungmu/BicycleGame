#pragma once
#include "GameState.h"
#include <fstream>
#include <vector>
#include <string>
class OptionState : public GameState
{
public:
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
	void _setOverlay(void);
	static OptionState* getInstance() { return &mOptionState; }

private:
	Ogre::OverlaySystem* mOverlaySystem;
	Ogre::OverlayManager*    mOverlayMgr;
	Ogre::Overlay*           mTextOverlay;
	Ogre::Overlay*           mLogoOverlay;
	Ogre::Overlay*           mDebugOverlay;
	Ogre::OverlayContainer*  mPanel;
	Ogre::Root *mRoot;
	static OptionState mOptionState;
	bool mContinue;
};