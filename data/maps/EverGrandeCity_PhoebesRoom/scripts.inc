EverGrandeCity_PhoebesRoom_MapScripts:: @ 8228174
	map_script MAP_SCRIPT_ON_LOAD, EverGrandeCity_PhoebesRoom_OnLoad
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, EverGrandeCity_PhoebesRoom_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, EverGrandeCity_PhoebesRoom_OnFrame
	.byte 0

EverGrandeCity_PhoebesRoom_OnWarp: @ 8228184
	map_script_2 VAR_TEMP_1, 0, EverGrandeCity_PhoebesRoom_EventScript_PlayerTurnNorth
	.2byte 0

EverGrandeCity_PhoebesRoom_EventScript_PlayerTurnNorth:: @ 822818E
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end

EverGrandeCity_PhoebesRoom_OnFrame: @ 8228193
	map_script_2 VAR_ELITE_4_STATE, 1, EverGrandeCity_PhoebesRoom_EventScript_WalkInCloseDoor
	.2byte 0

EverGrandeCity_PhoebesRoom_EventScript_WalkInCloseDoor:: @ 822819D
	lockall
	call PokemonLeague_EliteFour_EventScript_WalkInCloseDoor
	setvar VAR_ELITE_4_STATE, 2
	releaseall
	end

EverGrandeCity_PhoebesRoom_OnLoad: @ 82281AA
	random 2
	copyvar VAR_0x800A, VAR_RESULT
	switch VAR_RESULT
	case 0, EverGrandeCity_PhoebeAppears
	case 1, EverGrandeCity_LucyAppears
	call_if_set FLAG_DEFEATED_ELITE_4_PHOEBE, EverGrandeCity_PhoebesRoom_EventScript_ResetAdvanceToNextRoom
	compare VAR_ELITE_4_STATE, 2
	call_if_eq EverGrandeCity_PhoebesRoom_EventScript_CloseDoor
	end

EverGrandeCity_PhoebeAppears:
    setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_PHOEBE
    return

EverGrandeCity_LucyAppears:
    setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_LUCY
    setflag FLAG_TEMP_5
    return

EverGrandeCity_PhoebesRoom_EventScript_ResetAdvanceToNextRoom:: @ 82281BF
	call PokemonLeague_EliteFour_EventScript_ResetAdvanceToNextRoom
	return

EverGrandeCity_PhoebesRoom_EventScript_CloseDoor:: @ 82281C5
	call PokemonLeague_EliteFour_EventScript_CloseDoor
	return

EverGrandeCity_PhoebesRoom_EventScript_Phoebe:: @ 82281CB
	setvar VAR_FORCEANIM, 1
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_ELITE_4_PHOEBE, EverGrandeCity_PhoebesRoom_EventScript_PostBattleSpeech
	playbgm MUS_ENCOUNTER_ELITE_FOUR, 0
	msgbox EverGrandeCity_PhoebesRoom_Text_IntroSpeech, MSGBOX_DEFAULT
	goto_if_set FLAG_TEMP_5, EverGrandeCity_PhoebesRoom_EventScript_Lucy
	trainerbattle_no_intro TRAINER_PHOEBE, EverGrandeCity_PhoebesRoom_Text_Defeat
	goto EverGrandeCity_PhoebesRoom_EventScript_Defeated
	end

EverGrandeCity_PhoebesRoom_EventScript_Lucy::
	trainerbattle_no_intro TRAINER_LUCY, EverGrandeCity_PhoebesRoom_Text_Defeat
	goto EverGrandeCity_PhoebesRoom_EventScript_Defeated
	end

EverGrandeCity_PhoebesRoom_EventScript_PostBattleSpeech:: @ 82281F2
	msgbox EverGrandeCity_PhoebesRoom_Text_PostBattleSpeech, MSGBOX_DEFAULT
	release
	end

EverGrandeCity_PhoebesRoom_EventScript_Defeated:: @ 82281FC
	setvar VAR_FORCEANIM, 0
	setflag FLAG_DEFEATED_ELITE_4_PHOEBE
	call PokemonLeague_EliteFour_SetAdvanceToNextRoomMetatiles
	msgbox EverGrandeCity_PhoebesRoom_Text_PostBattleSpeech, MSGBOX_DEFAULT
	clearflag FLAG_TEMP_5
	release
	end

EverGrandeCity_PhoebesRoom_Text_IntroSpeech: @ 822820E
	.string "Ahahaha!\n"
	.string "I did my training on MT. PYRE.\p"
	.string "Yes, the bond I developed with POKéMON\n"
	.string "is extremely tight.\p"
	.string "So, come on, just try and see if you can\n"
	.string "even pose a threat!$"

EverGrandeCity_PhoebesRoom_Text_Defeat: @ 8228325
	.string "Oh, darn.\n"
	.string "I've gone and lost.$"

EverGrandeCity_PhoebesRoom_Text_PostBattleSpeech: @ 8228343
	.string "There's a definite bond between you\n"
	.string "and your POKéMON, too.\p"
	.string "I didn't recognize it, so it's only\n"
	.string "natural that I lost.\p"
	.string "Yup, I'd like to see how far your bond\n"
	.string "will carry you.\p"
	.string "Go ahead, move on to the next room.$"

