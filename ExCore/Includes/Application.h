#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "pch.h"
#include "Hardware.h"
#include "InputEvent.h"
#include "Cursor.h"

namespace ExCore
{
	class EXILE_CORE_API Application
	{
	private:
		static void error_callback(int error, const char* description);
		static void window_resize_callback(GLFWwindow* window, int width, int height);

	public:
		static int									width;
		static int									height;
		static const char*							title;
		static GLFWwindow*							window;
		static ExCore::RenderDevice::Properties		rdp;

		static void Create(int width, int height, const char* title, bool maximise, bool fullscreen, bool showCursor);
		static void Destroy();
		static void PollEvents();
		static void Swap();

		static bool isRunning();
	};
}

#endif