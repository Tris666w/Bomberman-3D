#pragma once
#include <GameObject.h>

#include "BombermanGameSettings.h"
#include "ContentManager.h"
#include "SoundManager.h"
#include "SpriteFont.h"
#include "TextRenderer.h"


template <typename F, void(F::* Func)(void*)>
class UiButton final : public GameObject
{
public:
	UiButton(const std::wstring& fontPath, const std::wstring& soundPath, DirectX::Rect boxDimensions, std::wstring buttonText, F* pObj)
		:GameObject(),
		m_pFont(nullptr),
		m_FontPath(fontPath),
		m_ButtonText(std::move(buttonText)),
		m_CharacterSize(32),
		m_DrawRect(boxDimensions),
		m_MouseClickId(-1),
		m_Obj(pObj),
		m_SoundPath(soundPath)
	{
	}

	UiButton(const std::wstring& fontPath, const std::wstring& soundPath, DirectX::XMFLOAT2 drawPos, std::wstring buttonText, F* pObj, int charSize = 32)
		:GameObject(),
		m_pFont(nullptr),
		m_FontPath(fontPath),
		m_ButtonText(std::move(buttonText)),
		m_CharacterSize(charSize),
		m_DrawRect(0, 0, 0, 0),
		m_MouseClickId(-1),
		m_Obj(pObj),
		m_SoundPath(soundPath)

	{
		m_DrawRect.x = static_cast<size_t>(drawPos.x);
		m_DrawRect.y = static_cast<size_t>(drawPos.y);
	}

	void CheckForActivation(const GameContext& gameContext, void* pData = nullptr)
	{
		if (IsPointInRect(gameContext.pInput->GetMousePosition(), m_DrawRect))
		{

			if (gameContext.pInput->IsActionTriggered(m_MouseClickId))
			{
				(m_Obj->*Func)(pData);

				SoundManager::GetInstance()->GetSystem()->playSound(m_pSound, nullptr, false, &m_pChannel);
				m_pChannel->setVolumeRamp(false);
				m_pChannel->setVolume(BombermanGameSettings::GetInstance()->GetSoundVolume());
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
		auto const inputAction = InputAction(m_MouseClickId, InputTriggerState::Pressed, -1, VK_LBUTTON, WORD(), GamepadIndex::PlayerOne);
		gameContext.pInput->AddInputAction(inputAction);

		auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();

		auto const fmodResult = pFmodSystem->createStream(WstringToString(m_SoundPath).c_str(), FMOD_DEFAULT, 0, &m_pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);
		m_pSound->setMode(FMOD_LOOP_OFF);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);
	}

	void Draw(const GameContext& gameContext) override
	{
		if (m_pFont->GetFontName() != L"")
		{
			if (!IsPointInRect(gameContext.pInput->GetMousePosition(), m_DrawRect))
				TextRenderer::GetInstance()->DrawText(m_pFont, m_ButtonText, { static_cast<float>(m_DrawRect.x), static_cast<float>(m_DrawRect.y) },
					m_NormalColor);

			else
				TextRenderer::GetInstance()->DrawText(m_pFont, m_ButtonText, { static_cast<float>(m_DrawRect.x), static_cast<float>(m_DrawRect.y) },
					m_HoverColor);
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
	DirectX::XMFLOAT4 m_NormalColor = BombermanGameSettings::button_color_passive;
	DirectX::XMFLOAT4 m_HoverColor = BombermanGameSettings::button_color_hover;
	std::wstring m_SoundPath;
	FMOD::Channel* m_pChannel = nullptr;
	FMOD::Sound* m_pSound = nullptr;
};