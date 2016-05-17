#include "OptionState.h"
#include "TitleState.h"
#include "PlayState.h"
using namespace Ogre;

OptionState OptionState::mOptionState;


void OptionState::enter(void)
{
}

void OptionState::exit(void)
{
}


void OptionState::pause(void)
{
}

void OptionState::resume(void)
{
}

bool OptionState::frameStarted(GameManager* game, const FrameEvent& evt)
{

	return true;
}

bool OptionState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	return true;
}


bool OptionState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	return true;
}

bool OptionState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_W:
		PlayState::getInstance()->anim = "Walk";
		game->popState();
		break;
	case OIS::KC_R:
		PlayState::getInstance()->anim = "Run";
		game->popState();
		break;
	default:
		break;
	}
	// -----------------------------------------------------

	return true;
}

bool OptionState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool OptionState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}


bool OptionState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	return true;
}

