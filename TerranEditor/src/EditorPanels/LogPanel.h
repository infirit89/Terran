#pragma once

#include <string>
#include <vector>

#include "Terran.h"

namespace TerranEditor 
{
	using namespace TerranEngine;

	enum class LogMessageLevel 
	{
		Trace = 0,
		Info,
		Warn,
		Error
	};

	struct LogMessage 
	{
		std::string Message;
		LogMessageLevel MessageLevel;
	};

	class LogPanel 
	{
	public:
		LogPanel();
		~LogPanel();

		void AddLog(LogMessage logMessage);
		void ImGuiRender();
		void ClearMessageBuffer();

		void SetOpen(bool open) { m_Open = open; }

		static LogPanel* GetInstance() { return s_Instance; }

	private:
		std::vector<LogMessage> m_TextBuffer;
		bool m_Open = true;
		static LogPanel* s_Instance;
		Shared<Texture> m_ErrorTexture;
		Shared<Texture> m_WarnTexture;
		Shared<Texture> m_InfoTexture;
	};
}