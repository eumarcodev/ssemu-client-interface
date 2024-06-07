#pragma once
constexpr int MAX_LOG_TEXT_LINE = 30;
constexpr int MAX_LOG_TEXT_SIZE = 80;

enum class eLogColor
{
    LOG_BLACK = 0,
	LOG_RED = 1,
	LOG_GREEN = 2,
	LOG_BLUE = 3,
};

class LogDisplayInfo
{
public:
	LogDisplayInfo() : color(eLogColor::LOG_BLACK) {  }

	const std::string& getText() const { return m_text; }
	void setText(const std::string& t) { m_text = t; }

	eLogColor getColor() const { return color; }
	void setColor(eLogColor c) { color = c; }
private:
	std::string m_text;
	eLogColor color;
};