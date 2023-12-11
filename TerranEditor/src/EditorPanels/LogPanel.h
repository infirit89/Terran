#pragma once

#include "EditorPanel.h"

#include "Core/Base.h"
#include "Graphics/Texture.h"

#include <string>
#include <vector>

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
		LogMessageLevel MessageLevel = LogMessageLevel::Trace;
	};

	class LogPanel : public EditorPanel
	{
	public:
		LogPanel();
		~LogPanel();

		void AddLogMessage(LogMessage logMessage);
		virtual void OnRender() override;
		void ClearMessageBuffer();

		bool IsClearOnPlay() { return m_ClearOnPlay; }

		static LogPanel* GetInstance() { return s_Instance; }
		virtual const char* GetName() override { return "Log"; }

	private:
		std::vector<LogMessage> m_TextBuffer;
		bool m_AutoScroll = true;
		bool m_ClearOnPlay = true;
		static LogPanel* s_Instance;
	};
}
