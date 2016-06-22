#include "OptionState.h"
#include "TitleState.h"
#include "PlayState.h"
#include <MMSystem.h> 
#pragma comment(lib,"Winmm.lib")
using namespace Ogre;

OptionState OptionState::mOptionState;


void OptionState::enter(void)
{
	sndPlaySoundA("Impact_Prelude.wav", SND_ASYNC | SND_NODEFAULT | SND_LOOP);
	mContinue = true;
	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();
	if (mTextOverlay)
	{
		mTextOverlay->show();
	}
	else
	{
		_setOverlay();
	}

	mPanel = static_cast<Ogre::OverlayContainer*>(OverlayManager::getSingletonPtr()->getOverlayElement("container1"));
}

void OptionState::exit(void)
{
	mTextOverlay->hide();
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
	return mContinue;
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
	case OIS::KC_ESCAPE:
		mContinue = false;
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

void OptionState::_setOverlay(void)
{
	ifstream in("Rank.txt");
	string Rank[7];
	for (int i = 0; i < 7; i++)
	{
		in >> Rank[i];
	}
	// fill here ================================================================================================
	mOverlayMgr = OverlayManager::getSingletonPtr();
	mTextOverlay = mOverlayMgr->create("TextOverlay");

	mPanel = static_cast<Ogre::OverlayContainer*>(mOverlayMgr->createOverlayElement("Panel", "container1"));
	mPanel->setDimensions(1, 1);
	mPanel->setPosition(0.2f, 0.2f);

	OverlayElement* textBox = mOverlayMgr->createOverlayElement("TextArea", "TextID");
	textBox->setMetricsMode(Ogre::GMM_PIXELS);
	textBox->setPosition(10, 10);
	textBox->setWidth(100);
	textBox->setHeight(20);
	textBox->setParameter("font_name", "Font/NanumBold18");
	textBox->setParameter("char_height", "40");
	textBox->setColour(Ogre::ColourValue::White);
	textBox->setCaption(L"현재 기록 : " + Rank[6] + "\n\tTop Ranking 5 \n\n1. " + Rank[0] + "\n2. " + Rank[1] + "\n3. " + Rank[2] + "\n4. " + Rank[3] + "\n5. " + Rank[4]);
	mPanel->addChild(textBox);
	
	mTextOverlay->add2D(mPanel);
	mTextOverlay->show();
	in.close();
	// ==========================================================================================================
}