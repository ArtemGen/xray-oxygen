#pragma once
#include "../xrEngine/PS_instance.h"
#include "../xrEngine/Render.h"

PARTICLES_API extern const Fvector zero_vel;

class PARTICLES_API CParticlesObject : public CPS_Instance
{
	typedef CPS_Instance	inherited;

	u32 dwLastTime;
	void Init(LPCSTR p_name, IRender_Sector* S, BOOL bAutoRemove);
	void UpdateSpatial();

protected:
	bool m_bLooped;			//флаг, что система зациклена
	bool m_bStopping;		//вызвана функция Stop()
	DWORD m_lastUpdatedFrame = 0;
	static xr_list<CParticlesObject*> AllParticleObjects;

protected:
	virtual ~CParticlesObject();

public:
	CParticlesObject(LPCSTR p_name, BOOL bAutoRemove, bool destroy_on_game_load);

	virtual bool shedule_Needed() { return true; };
	virtual float shedule_Scale();
	virtual void shedule_Update(u32 dt);
	virtual	shared_str shedule_Class_Name() const override { return shared_str("particle_object"); };
	virtual void renderable_Render();
	void PerformAllTheWork(u32 dt);

	Fvector& Position();
	void SetXFORM(const Fmatrix& m);

	void UpdateParent(const Fmatrix& m, const Fvector& vel);

	void play_at_pos(const Fvector& pos, BOOL xform = FALSE);
	virtual void Play(bool bHudMode);
	void Stop(BOOL bDefferedStop = TRUE);

	bool IsLooped() { return m_bLooped; }
	bool IsAutoRemove();
	bool IsPlaying();
	void SetAutoRemove(bool auto_remove);

	const shared_str Name();

	inline	Fmatrix& XFORM() { return renderable.xform; }

	static void UpdateAllAsync();
	static void WaitForParticles();

public:
	static CParticlesObject* Create(LPCSTR p_name, BOOL bAutoRemove = TRUE, bool remove_on_game_load = true)
	{
		return xr_new<CParticlesObject>(p_name, bAutoRemove, remove_on_game_load);
	}
	static void Destroy(CParticlesObject*& p)
	{
		if (p)
		{
			p->PSI_destroy();
			p = 0;
		}
	}
};
