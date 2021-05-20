#include "stdafx.h"
#include "Text.h"

#include "ContentManager.h"
#include "SpriteFont.h"
#include "TextRenderer.h"
#include "SpriteFont.h"

Text::Text(const std::wstring& text, DirectX::XMFLOAT2 drawPos, DirectX::XMFLOAT4 drawColor, const std::wstring& fontPath)
	:GameObject(),
	 m_Text(text),
	 m_DrawPos(drawPos),
	 m_DrawColor(drawColor),
	 m_FontPath(fontPath),
	 m_pFont(nullptr)
{}

Text::~Text()
{
}

void Text::SetText(const std::wstring& newText)
{
	m_Text = newText;
}

void Text::SetDrawPos(const DirectX::XMFLOAT2& newPos)
{
	m_DrawPos = newPos;
}

void Text::SetDrawColor(const DirectX::XMFLOAT4& newColor)
{
	m_DrawColor = newColor;
}

void Text::SetFont(const std::wstring& fontPath)
{
	m_FontPath = fontPath;
	m_pFont = ContentManager::Load<SpriteFont>(fontPath);
}

std::wstring Text::GetText() const
{
	return m_Text;
}

DirectX::XMFLOAT2 Text::GetDrawPos() const
{
	return m_DrawPos;
}

DirectX::XMFLOAT4 Text::GetDrawColor() const
{
	return m_DrawColor;
}

std::wstring Text::GetFontName() const
{
	if (m_pFont)
		return m_pFont->GetFontName();
	return L"";
}


void Text::Initialize(const GameContext&)
{
	m_pFont = ContentManager::Load<SpriteFont>(m_FontPath);
}

void Text::Draw(const GameContext&)
{
	TextRenderer::GetInstance()->DrawText(m_pFont, m_Text, { m_DrawPos.x,m_DrawPos.y },
				m_DrawColor);
}

void Text::Update(const GameContext&)
{
}
