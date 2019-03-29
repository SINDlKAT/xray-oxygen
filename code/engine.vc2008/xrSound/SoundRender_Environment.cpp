#include "stdafx.h"
#pragma hdrstop
#include <openal\efx.h>
#include "SoundRender.h"
#include "SoundRender_Environment.h"

CSoundRender_Environment::CSoundRender_Environment(void)
{
	version = sdef_env_version;
	set_default();
}

CSoundRender_Environment::~CSoundRender_Environment(void)
{
}

void CSoundRender_Environment::set_default()
{
	Environment			= 0u;
	Room				= AL_REVERB_DEFAULT_GAIN;
	RoomHF				= AL_REVERB_DEFAULT_GAINHF;
	RoomRolloffFactor	= AL_REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR;
	DecayTime			= AL_REVERB_DEFAULT_DECAY_TIME;
	DecayHFRatio		= AL_REVERB_DEFAULT_DECAY_HFRATIO;
	Reflections			= AL_REVERB_DEFAULT_REFLECTIONS_GAIN;
	ReflectionsDelay	= AL_REVERB_DEFAULT_REFLECTIONS_DELAY;
	Reverb				= AL_REVERB_DEFAULT_LATE_REVERB_GAIN;
	ReverbDelay			= AL_REVERB_DEFAULT_LATE_REVERB_DELAY;
	EnvironmentSize		= 7.5f;
	EnvironmentDiffusion= AL_REVERB_DEFAULT_DIFFUSION;
	AirAbsorptionHF		= AL_REVERB_DEFAULT_AIR_ABSORPTION_GAINHF;
}

void CSoundRender_Environment::set_identity()
{
	set_default();
	Room = AL_REVERB_MIN_GAIN;
	clamp();
}

void CSoundRender_Environment::lerp(CSoundRender_Environment& A, CSoundRender_Environment& B, float f)
{
	float	fi = 1.f - f;

	Room = fi * A.Room + f * B.Room;
	RoomHF = fi * A.RoomHF + f * B.RoomHF;
	RoomRolloffFactor = fi * A.RoomRolloffFactor + f * B.RoomRolloffFactor;
	DecayTime = fi * A.DecayTime + f * B.DecayTime;
	DecayHFRatio = fi * A.DecayHFRatio + f * B.DecayHFRatio;
	Reflections = fi * A.Reflections + f * B.Reflections;
	ReflectionsDelay = fi * A.ReflectionsDelay + f * B.ReflectionsDelay;
	Reverb = fi * A.Reverb + f * B.Reverb;
	ReverbDelay = fi * A.ReverbDelay + f * B.ReverbDelay;
	EnvironmentSize = fi * A.EnvironmentSize + f * B.EnvironmentSize;
	EnvironmentDiffusion = fi * A.EnvironmentDiffusion + f * B.EnvironmentDiffusion;
	AirAbsorptionHF = fi * A.AirAbsorptionHF + f * B.AirAbsorptionHF;

	clamp();
}

void CSoundRender_Environment::clamp()
{
	::clamp(Room,				AL_REVERB_MIN_GAIN,					  AL_REVERB_MAX_GAIN);
	::clamp(RoomHF,				AL_REVERB_MIN_GAINHF,				  AL_REVERB_MAX_GAINHF);
	::clamp(RoomRolloffFactor,	AL_REVERB_MIN_ROOM_ROLLOFF_FACTOR,	  AL_REVERB_MAX_ROOM_ROLLOFF_FACTOR);
	::clamp(DecayTime,			AL_REVERB_MIN_DECAY_TIME,			  AL_REVERB_MAX_DECAY_TIME);
	::clamp(DecayHFRatio,		AL_REVERB_MIN_DECAY_HFRATIO,		  AL_REVERB_MAX_DECAY_HFRATIO);
	::clamp(Reflections,		AL_REVERB_MIN_REFLECTIONS_GAIN,		  AL_REVERB_MAX_REFLECTIONS_GAIN);
	::clamp(ReflectionsDelay,	AL_REVERB_MIN_REFLECTIONS_DELAY,	  AL_REVERB_MAX_REFLECTIONS_DELAY);
	::clamp(Reverb,				AL_REVERB_MIN_LATE_REVERB_GAIN,		  AL_REVERB_MAX_LATE_REVERB_GAIN);
	::clamp(ReverbDelay,		AL_REVERB_MIN_LATE_REVERB_DELAY,	  AL_REVERB_MAX_LATE_REVERB_DELAY);
	::clamp(AirAbsorptionHF,	AL_REVERB_MIN_AIR_ABSORPTION_GAINHF,  AL_REVERB_MAX_AIR_ABSORPTION_GAINHF);

	::clamp(EnvironmentDiffusion, 0.f, 1.f);
	::clamp(EnvironmentSize, 1.f, 100.f);
}

bool CSoundRender_Environment::load(IReader* fs)
{
	auto mB_to_gain= [](float mb)
	{
		return powf(10.0f, mb / 2000.0f);
	};

	version = fs->r_u32();

	if (version >= 0x0003)
	{
		fs->r_stringZ(name);

		Room = mB_to_gain(fs->r_float());
		RoomHF = mB_to_gain(fs->r_float());
		RoomRolloffFactor = fs->r_float();
		DecayTime = fs->r_float();
		DecayHFRatio = fs->r_float();
		Reflections = mB_to_gain(fs->r_float());
		ReflectionsDelay = fs->r_float();
		Reverb = mB_to_gain(fs->r_float());
		ReverbDelay = fs->r_float();
		EnvironmentSize = fs->r_float();
		EnvironmentDiffusion = fs->r_float();
		AirAbsorptionHF = mB_to_gain(fs->r_float());

		if (version > 0x0003)
			Environment = fs->r_u32();

		return true;
	}
	return false;
}

void CSoundRender_Environment::save(IWriter* fs)
{
	fs->w_u32(sdef_env_version);
	fs->w_stringZ(name);

	fs->w_float(Room);
	fs->w_float(RoomHF);
	fs->w_float(RoomRolloffFactor);
	fs->w_float(DecayTime);
	fs->w_float(DecayHFRatio);
	fs->w_float(Reflections);
	fs->w_float(ReflectionsDelay);
	fs->w_float(Reverb);
	fs->w_float(ReverbDelay);
	fs->w_float(EnvironmentSize);
	fs->w_float(EnvironmentDiffusion);
	fs->w_float(AirAbsorptionHF);

	fs->w_u32(Environment);
}

//////////////////////////////////////////////////////////////////////////
void SoundEnvironment_LIB::Load(const char* name)
{
	R_ASSERT(library.empty());
	IReader* F = FS.r_open(name);
	IReader* C;
	library.reserve(256);
	for (u32 chunk = 0; 0 != (C = F->open_chunk(chunk)); ++chunk)
	{
		CSoundRender_Environment*	E = new CSoundRender_Environment();
		if (E->load(C))	library.push_back(E);
		C->close();
	}
	FS.r_close(F);
}

bool SoundEnvironment_LIB::Save(const char* name)
{
	IWriter* F = FS.w_open(name);
	if (F)
	{
		for (u32 chunk = 0; chunk < library.size(); ++chunk)
		{
			F->open_chunk(chunk);
			library[chunk]->save(F);
			F->close_chunk();
		}
		FS.w_close(F);
		return true;
	}

	return false;
}

void	SoundEnvironment_LIB::Unload()
{
	for (u32 chunk = 0; chunk < library.size(); ++chunk)
		xr_delete(library[chunk]);

	library.clear();
}

int		SoundEnvironment_LIB::GetID(const char* name)
{
	for (auto it = library.begin(); it != library.end(); ++it)
		if (!_stricmp(name, (*it)->name.c_str()))
			return int(it - library.begin());

	return -1;
}

CSoundRender_Environment*	SoundEnvironment_LIB::Get(const char* name)
{
	for (CSoundRender_Environment* it : library)
		if (!_stricmp(name, it->name.c_str()))
			return it;

	return nullptr;
}

CSoundRender_Environment*	SoundEnvironment_LIB::Get(int id)
{
	return library[id];
}

CSoundRender_Environment*	SoundEnvironment_LIB::Append(CSoundRender_Environment* parent)
{
	library.push_back(parent ? new CSoundRender_Environment(*parent) : new CSoundRender_Environment());
	return library.back();
}

void SoundEnvironment_LIB::Remove(const char* name)
{
	for (auto it = library.begin(); it != library.end(); ++it)
		if (!_stricmp(name, *(*it)->name))
		{
			xr_delete(*it);
			library.erase(it);
			break;
		}
}

void SoundEnvironment_LIB::Remove(int id)
{
	xr_delete(library[id]);
	library.erase(library.begin() + id);
}

SoundEnvironment_LIB::SE_VEC& SoundEnvironment_LIB::Library()
{
	return library;
}