#define CLIENT_DESCRIPTION "BicycleGame"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif



#include "GameManager.h"
#include "TitleState.h"
#include "PlayState.h"

using namespace Ogre;

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char *argv[])
#endif
	{
		DataCollector mDataCollector;

		myo::Hub hub("");

		// 마이오를 찾는 동안 대기하는 소스코드
		myo::Myo* myo = hub.waitForMyo(10000);

		// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		// 리스너 등록
		hub.addListener(&mDataCollector);

		// Fill Here ---------------------------------------------------
		GameManager game(&hub,&mDataCollector);
		try
		{
			game.init();
			game.changeState(TitleState::getInstance());
			game.go();
		}

		// --------------------------------------------------------------
		catch (Ogre::Exception& e)
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
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