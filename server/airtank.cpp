/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"

class CAirtank : public CGrenade
{
	DECLARE_CLASS( CAirtank, CGrenade );

	void Spawn( void );
	void Precache( void );
	void TankThink( void );
	void TankTouch( CBaseEntity *pOther );
	int BloodColor( void ) { return DONT_BLEED; };
	void Killed( entvars_t *pevAttacker, int iGib );

	DECLARE_DATADESC();

	int	 m_state;
};

LINK_ENTITY_TO_CLASS( item_airtank, CAirtank );


BEGIN_DATADESC( CAirtank )
	DEFINE_FIELD( m_state, FIELD_INTEGER ),
	DEFINE_FUNCTION( TankThink ),
	DEFINE_FUNCTION( TankTouch ),
END_DATADESC()

void CAirtank :: Spawn( void )
{
	Precache( );

	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL( edict(), "models/w_oxygen.mdl" );
	UTIL_SetSize( pev, Vector( -16, -16, 0 ), Vector( 16, 16, 36 ));
	UTIL_SetOrigin( this, GetLocalOrigin( ));

	SetTouch( TankTouch );
	SetThink( TankThink );

	pev->flags |= FL_MONSTER;
	pev->takedamage = DAMAGE_YES;
	pev->health = 20;
	pev->dmg = 50;
	m_state = 1;
}

void CAirtank::Precache( void )
{
	PRECACHE_MODEL("models/w_oxygen.mdl");
	PRECACHE_SOUND("doors/aliendoor3.wav");
}


void CAirtank :: Killed( entvars_t *pevAttacker, int iGib )
{
	pev->owner = ENT( pevAttacker );

	// UNDONE: this should make a big bubble cloud, not an explosion

	Explode( GetAbsOrigin(), Vector( 0, 0, -1 ) );
}


void CAirtank::TankThink( void )
{
	// Fire trigger
	m_state = 1;
	SUB_UseTargets( this, USE_TOGGLE, 0 );
}


void CAirtank::TankTouch( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
		return;

	if (!m_state)
	{
		// "no oxygen" sound
		EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_swim2.wav", 1.0, ATTN_NORM );
		return;
	}
		
	// give player 12 more seconds of air
	pOther->pev->air_finished = gpGlobals->time + 12;

	// suit recharge sound
	EMIT_SOUND( ENT(pev), CHAN_VOICE, "doors/aliendoor3.wav", 1.0, ATTN_NORM );

	// recharge airtank in 30 seconds
	pev->nextthink = gpGlobals->time + 30;
	m_state = 0;
	SUB_UseTargets( this, USE_TOGGLE, 1 );
}
