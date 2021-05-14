#pragma once
#include <GameObject.h>

#include "ContentManager.h"
#include "SpriteFont.h"
#include "TextRenderer.h"

template <typename F, void(F::*Func)()>
class UiButton final : public GameObject
{
public:
	UiButton(const std::wstring& fontPath,DirectX::Rect boxDimensions,std::wstring buttonText,F* pObj)
	:GameObject(),
	m_pFont(nullptr),
	m_FontPath(fontPath),
	m_ButtonText(std::move(buttonText)),
	m_CharacterSize(32),
	m_DrawRect(boxDimensions),
	m_MouseClickId(-1),
	m_Obj(pObj)
	{
	}
	
	UiButton(const std::wstring& fontPath,DirectX::XMFLOAT2 drawPos,std::wstring buttonText, F* pObj,int charSize = 32)
	:GameObject(),
	m_pFont(nullptr),
	m_FontPath(fontPath),
	m_ButtonText(std::move(buttonText)),
	m_CharacterSize(charSize),
	m_DrawRect(0,0,0,0),
	m_MouseClickId(-1),
	m_Obj(pObj)
	
	{
		m_DrawRect.x = static_cast<size_t>(drawPos.x);
		m_DrawRect.y = static_cast<size_t>(drawPos.y);
	}

	//template <class... Args>
	void CheckForActivation(const GameContext& gameContext)
	{
		if (IsPointInRect(gameContext.pInput->GetMousePosition(), m_DrawRect))
		{
			if (gameContext.pInput->IsActionTriggered(m_MouseClickId))
			{
				(m_Obj->*Func)();
			}
		}
	}
protected:
	void Initialize(const GameContext& gameContext) override
	{
		m_pFont = ContentManager::Load<SpriteFont>(m_FontPath);

	if (m_DrawRect.w <= 0 || m_DrawRect.h <= 0)
	{
		m_DrawRect.w = m_ButtonText.size() * m_CharacterSize;
		m_DrawRect.h = m_CharacterSize;
	}
	m_DrawRect.w -= static_cast<size_t>(0.2 * m_CharacterSize * m_ButtonText.size());
	m_MouseClickId = gameContext.pInput->GetAvailableActionID();
	auto const inputAction = InputAction(m_MouseClickId, InputTriggerState::Pressed,-1,VK_LBUTTON,WORD(),GamepadIndex::PlayerOne);
	gameContext.pInput->AddInputAction(inputAction);
	}
	void Draw(const GameContext& gameContext) override
	{
			if (m_pFont->GetFontName() != L"")
	{
		if (IsPointInRect(gameContext.pInput->GetMousePosition(), m_DrawRect))
		{
			TextRenderer::GetInstance()->DrawText(m_pFont, m_ButtonText, { static_cast<float>(m_DrawRect.x), static_cast<float>(m_DrawRect.y) },
				static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkRed));
			
		}
		else
		{
			TextRenderer::GetInstance()->DrawText(m_pFont, m_ButtonText, { static_cast<float>(m_DrawRect.x), static_cast<float>(m_DrawRect.y) },
				static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Red));
		}
	}
	}

private:
	SpriteFont* m_pFont;
	std::wstring m_FontPath;
	std::wstring m_ButtonText;
	int m_CharacterSize;
	DirectX::Rect m_DrawRect;
	int m_MouseClickId;
	F* m_Obj;
};