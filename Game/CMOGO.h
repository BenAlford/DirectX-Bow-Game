#ifndef _CMOGO_H_
#define _CMOGO_H_

//=================================================================
//A Class for loading and displaying CMO as GameObjects
//=================================================================

#include "gameobject.h"
#include "Model.h"
#include <string>

using namespace std;
using namespace DirectX;

struct GameData;

class CMOGO : public GameObject
{
public:
	CMOGO(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF);
	virtual ~CMOGO();

	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;
	virtual void TriggerEntered(std::shared_ptr<CMOGO> other) {};

	virtual bool Intersects(const CMOGO& other) const;

	bool isTrigger() { return m_trigger; }
	void setIsTrigger(bool new_trigger) { m_trigger = new_trigger; }

	BoundingOrientedBox&		getCollider()		noexcept { return m_collider; }
	const BoundingOrientedBox&	getCollider() const noexcept { return m_collider; }

protected:
	unique_ptr<Model>  m_model;
	BoundingOrientedBox m_collider;

	bool m_trigger = false;

	//needs a slightly different raster state that the VBGOs so create one and let them all use it
	static ID3D11RasterizerState*  s_pRasterState;
	static int m_count;

	bool m_draw = true;
};

#endif
