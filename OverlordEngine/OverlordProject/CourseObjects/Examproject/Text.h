#pragma once
#include <GameObject.h>
class SpriteFont;

class Text final:  public GameObject
{
public:
	Text(const std::wstring& text, DirectX::XMFLOAT2 drawPos, DirectX::XMFLOAT4 drawColor, const std::wstring& fontPath);
	~Text() override;

	void SetText(const std::wstring& newText);
	void SetDrawPos(const DirectX::XMFLOAT2& newPos);
	void SetDrawColor(const DirectX::XMFLOAT4& newColor);
	void SetFont(const std::wstring& fontPath);


	[[nodiscard]]std::wstring GetText()const;
	[[nodiscard]]DirectX::XMFLOAT2 GetDrawPos()const;
	[[nodiscard]]DirectX::XMFLOAT4 GetDrawColor()const;
	[[nodiscard]]std::wstring GetFontName()const;

protected:
	void Initialize(const GameContext&) override;
	void Draw(const GameContext&) override;
	void Update(const GameContext&) override;

private:
	std::wstring m_Text;
	DirectX::XMFLOAT2 m_DrawPos;
	DirectX::XMFLOAT4 m_DrawColor;
	
	std::wstring m_FontPath;
	SpriteFont* m_pFont;
	
};

