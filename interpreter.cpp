/*
 * interpreter.cpp
 *
 *  Created on: 2010-4-28
 *      Author: Argon
 */

#include "lua_obj.h"
#include "duel.h"
#include "group.h"
#include "card.h"
#include "effect.h"
#include "scriptlib.h"
#include "interpreter.h"
#include <cmath>

template<typename T>
struct objref {
	static int32 get_lua_ref(lua_State* L) {
		lua_pushinteger(L, lua_get<T*>(L, 1)->ref_handle);
		return 1;
	}
	static int32 from_lua_ref(lua_State* L) {
		auto ref = lua_get<int32>(L, 1);
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		auto obj = lua_get<T*>(L, -1);
		if(!obj) {
			if(std::is_same<T,card>::value)
				luaL_error(L, "Parameter 1 should be a lua reference to a Card.");
			else if(std::is_same<T, group>::value)
				luaL_error(L, "Parameter 1 should be a lua reference to a Group.");
			else if(std::is_same<T, effect>::value)
				luaL_error(L, "Parameter 1 should be a lua reference to an Effect.");
		}
		return 1;
	}
};

constexpr luaL_Reg cardlib[] = {
	{ "GetCode", scriptlib::card_get_code },
	{ "GetOriginalCode", scriptlib::card_get_origin_code },
	{ "GetOriginalCodeRule", scriptlib::card_get_origin_code_rule },
	{ "GetSetCard", scriptlib::card_get_set_card },
	{ "GetOriginalSetCard", scriptlib::card_get_origin_set_card },
	{ "GetPreviousSetCard", scriptlib::card_get_pre_set_card },
	{ "GetType", scriptlib::card_get_type },
	{ "GetOriginalType", scriptlib::card_get_origin_type },
	{ "GetLevel", scriptlib::card_get_level },
	{ "GetRank", scriptlib::card_get_rank },
	{ "GetLink", scriptlib::card_get_link },
	{ "GetSynchroLevel", scriptlib::card_get_synchro_level },
	{ "GetRitualLevel", scriptlib::card_get_ritual_level },
	{ "GetOriginalLevel", scriptlib::card_get_origin_level },
	{ "GetOriginalRank", scriptlib::card_get_origin_rank },
	{ "IsXyzLevel", scriptlib::card_is_xyz_level },
	{ "GetLeftScale", scriptlib::card_get_lscale },
	{ "GetOriginalLeftScale", scriptlib::card_get_origin_lscale },
	{ "GetRightScale", scriptlib::card_get_rscale },
	{ "GetOriginalRightScale", scriptlib::card_get_origin_rscale },
	{ "GetLinkMarker", scriptlib::card_get_link_marker },
	{ "IsLinkMarker", scriptlib::card_is_link_marker },
	{ "GetLinkedGroup", scriptlib::card_get_linked_group },
	{ "GetLinkedGroupCount", scriptlib::card_get_linked_group_count },
	{ "GetLinkedZone", scriptlib::card_get_linked_zone },
	{ "GetFreeLinkedZone", scriptlib::card_get_free_linked_zone },
	{ "GetMutualLinkedGroup", scriptlib::card_get_mutual_linked_group },
	{ "GetMutualLinkedGroupCount", scriptlib::card_get_mutual_linked_group_count },
	{ "GetMutualLinkedZone", scriptlib::card_get_mutual_linked_zone },
	{ "IsLinked", scriptlib::card_is_linked },
	{ "IsExtraLinked", scriptlib::card_is_extra_linked },
	{ "GetColumnGroup", scriptlib::card_get_column_group },
	{ "GetColumnGroupCount", scriptlib::card_get_column_group_count },
	{ "GetColumnZone", scriptlib::card_get_column_zone },
	{ "IsAllColumn", scriptlib::card_is_all_column },
	{ "GetAttribute", scriptlib::card_get_attribute },
	{ "GetOriginalAttribute", scriptlib::card_get_origin_attribute },
	{ "GetRace", scriptlib::card_get_race },
	{ "GetOriginalRace", scriptlib::card_get_origin_race },
	{ "GetAttack", scriptlib::card_get_attack },
	{ "GetBaseAttack", scriptlib::card_get_origin_attack },
	{ "GetTextAttack", scriptlib::card_get_text_attack },
	{ "GetDefense",	scriptlib::card_get_defense },
	{ "GetBaseDefense", scriptlib::card_get_origin_defense },
	{ "GetTextDefense", scriptlib::card_get_text_defense },
	{ "GetPreviousCodeOnField", scriptlib::card_get_previous_code_onfield },
	{ "GetPreviousTypeOnField", scriptlib::card_get_previous_type_onfield },
	{ "GetPreviousLevelOnField", scriptlib::card_get_previous_level_onfield },
	{ "GetPreviousRankOnField", scriptlib::card_get_previous_rank_onfield },
	{ "GetPreviousAttributeOnField", scriptlib::card_get_previous_attribute_onfield },
	{ "GetPreviousRaceOnField", scriptlib::card_get_previous_race_onfield },
	{ "GetPreviousAttackOnField", scriptlib::card_get_previous_attack_onfield },
	{ "GetPreviousDefenseOnField", scriptlib::card_get_previous_defense_onfield },
	{ "GetOwner", scriptlib::card_get_owner },
	{ "GetControler", scriptlib::card_get_controler },
	{ "GetPreviousControler", scriptlib::card_get_previous_controler },
	{ "GetReason", scriptlib::card_get_reason },
	{ "GetReasonCard", scriptlib::card_get_reason_card },
	{ "GetReasonPlayer", scriptlib::card_get_reason_player },
	{ "GetReasonEffect", scriptlib::card_get_reason_effect },
	{ "SetReason", scriptlib::card_set_reason },
	{ "SetReasonCard", scriptlib::card_set_reason_card },
	{ "SetReasonPlayer", scriptlib::card_set_reason_player },
	{ "SetReasonEffect", scriptlib::card_set_reason_effect },
	{ "GetPosition", scriptlib::card_get_position },
	{ "GetPreviousPosition", scriptlib::card_get_previous_position },
	{ "GetBattlePosition", scriptlib::card_get_battle_position },
	{ "GetLocation", scriptlib::card_get_location },
	{ "GetPreviousLocation", scriptlib::card_get_previous_location },
	{ "GetSequence", scriptlib::card_get_sequence },
	{ "GetPreviousSequence", scriptlib::card_get_previous_sequence },
	{ "GetSummonType", scriptlib::card_get_summon_type },
	{ "GetSummonLocation", scriptlib::card_get_summon_location },
	{ "GetSummonPlayer", scriptlib::card_get_summon_player },
	{ "GetDestination", scriptlib::card_get_destination },
	{ "GetLeaveFieldDest", scriptlib::card_get_leave_field_dest },
	{ "GetTurnID", scriptlib::card_get_turnid },
	{ "GetFieldID", scriptlib::card_get_fieldid },
	{ "GetRealFieldID", scriptlib::card_get_fieldidr },
	{ "GetCardID", scriptlib::card_get_cardid },
	{ "IsOriginalCodeRule", scriptlib::card_is_origin_code_rule },
	{ "IsCode", scriptlib::card_is_code },
	{ "IsSummonCode", scriptlib::card_is_summon_code },
	{ "IsSetCard", scriptlib::card_is_set_card },
	{ "IsOriginalSetCard", scriptlib::card_is_origin_set_card },
	{ "IsPreviousSetCard", scriptlib::card_is_pre_set_card },
	{ "IsType", scriptlib::card_is_type },
	{ "IsLevel", scriptlib::card_is_level },
	{ "IsRank", scriptlib::card_is_rank },
	{ "IsLink", scriptlib::card_is_link },
	{ "IsAttack", scriptlib::card_is_attack },
	{ "IsDefense", scriptlib::card_is_defense },
	{ "IsRace", scriptlib::card_is_race },
	{ "IsAttribute", scriptlib::card_is_attribute },
	{ "IsReason", scriptlib::card_is_reason },
	{ "IsSummonType", scriptlib::card_is_summon_type },
	{ "IsStatus", scriptlib::card_is_status },
	{ "IsNotTuner", scriptlib::card_is_not_tuner },
	{ "SetStatus", scriptlib::card_set_status },
	{ "IsGeminiState", scriptlib::card_is_gemini_state },
	{ "EnableGeminiState", scriptlib::card_enable_gemini_state },
	{ "SetTurnCounter",	scriptlib::card_set_turn_counter },
	{ "GetTurnCounter", scriptlib::card_get_turn_counter },
	{ "SetMaterial", scriptlib::card_set_material },
	{ "GetMaterial", scriptlib::card_get_material },
	{ "GetMaterialCount", scriptlib::card_get_material_count },
	{ "GetEquipGroup", scriptlib::card_get_equip_group },
	{ "GetEquipCount", scriptlib::card_get_equip_count },
	{ "GetEquipTarget", scriptlib::card_get_equip_target },
	{ "GetPreviousEquipTarget", scriptlib::card_get_pre_equip_target },
	{ "CheckEquipTarget", scriptlib::card_check_equip_target },
	{ "CheckUnionTarget", scriptlib::card_check_union_target },
	{ "GetUnionCount", scriptlib::card_get_union_count },
	{ "GetOverlayGroup", scriptlib::card_get_overlay_group },
	{ "GetOverlayCount", scriptlib::card_get_overlay_count },
	{ "GetOverlayTarget", scriptlib::card_get_overlay_target },
	{ "CheckRemoveOverlayCard", scriptlib::card_check_remove_overlay_card },
	{ "RemoveOverlayCard", scriptlib::card_remove_overlay_card },
	{ "GetAttackedGroup", scriptlib::card_get_attacked_group },
	{ "GetAttackedGroupCount", scriptlib::card_get_attacked_group_count },
	{ "GetAttackedCount", scriptlib::card_get_attacked_count },
	{ "GetBattledGroup", scriptlib::card_get_battled_group },
	{ "GetBattledGroupCount", scriptlib::card_get_battled_group_count },
	{ "GetAttackAnnouncedCount", scriptlib::card_get_attack_announced_count },
	{ "IsDirectAttacked", scriptlib::card_is_direct_attacked },
	{ "SetCardTarget", scriptlib::card_set_card_target },
	{ "GetCardTarget", scriptlib::card_get_card_target },
	{ "GetFirstCardTarget", scriptlib::card_get_first_card_target },
	{ "GetCardTargetCount", scriptlib::card_get_card_target_count },
	{ "IsHasCardTarget", scriptlib::card_is_has_card_target },
	{ "CancelCardTarget", scriptlib::card_cancel_card_target },
	{ "GetOwnerTarget", scriptlib::card_get_owner_target },
	{ "GetOwnerTargetCount", scriptlib::card_get_owner_target_count },
	{ "GetActivateEffect", scriptlib::card_get_activate_effect },
	{ "CheckActivateEffect", scriptlib::card_check_activate_effect },
	{ "RegisterEffect", scriptlib::card_register_effect },
	{ "IsHasEffect", scriptlib::card_is_has_effect },
	{ "GetCardEffect", scriptlib::card_get_card_effect },
	{ "ResetEffect", scriptlib::card_reset_effect },
	{ "GetEffectCount", scriptlib::card_get_effect_count },
	{ "RegisterFlagEffect", scriptlib::card_register_flag_effect },
	{ "GetFlagEffect", scriptlib::card_get_flag_effect },
	{ "ResetFlagEffect", scriptlib::card_reset_flag_effect },
	{ "SetFlagEffectLabel", scriptlib::card_set_flag_effect_label },
	{ "GetFlagEffectLabel", scriptlib::card_get_flag_effect_label },
	{ "CreateRelation", scriptlib::card_create_relation },
	{ "ReleaseRelation", scriptlib::card_release_relation },
	{ "CreateEffectRelation", scriptlib::card_create_effect_relation },
	{ "ReleaseEffectRelation", scriptlib::card_release_effect_relation },
	{ "ClearEffectRelation", scriptlib::card_clear_effect_relation },
	{ "IsRelateToEffect", scriptlib::card_is_relate_to_effect },
	{ "IsRelateToChain", scriptlib::card_is_relate_to_chain },
	{ "IsRelateToCard", scriptlib::card_is_relate_to_card },
	{ "IsRelateToBattle", scriptlib::card_is_relate_to_battle },
	{ "CopyEffect", scriptlib::card_copy_effect },
	{ "ReplaceEffect", scriptlib::card_replace_effect },
	{ "EnableUnsummonable", scriptlib::card_enable_unsummonable },
	{ "EnableReviveLimit", scriptlib::card_enable_revive_limit },
	{ "CompleteProcedure", scriptlib::card_complete_procedure },
	{ "IsDisabled", scriptlib::card_is_disabled },
	{ "IsDestructable", scriptlib::card_is_destructable },
	{ "IsSummonableCard", scriptlib::card_is_summonable },
	{ "IsFusionSummonableCard", scriptlib::card_is_fusion_summonable_card },
	{ "IsSpecialSummonable", scriptlib::card_is_special_summonable },
	{ "IsSynchroSummonable", scriptlib::card_is_synchro_summonable },
	{ "IsXyzSummonable", scriptlib::card_is_xyz_summonable },
	{ "IsLinkSummonable", scriptlib::card_is_link_summonable },
	{ "IsProcedureSummonable", scriptlib::card_is_procedure_summonable },
	{ "IsSummonable", scriptlib::card_is_can_be_summoned },
	{ "IsMSetable", scriptlib::card_is_msetable },
	{ "IsSSetable", scriptlib::card_is_ssetable },
	{ "IsCanBeSpecialSummoned", scriptlib::card_is_can_be_special_summoned },
	{ "IsAbleToHand", scriptlib::card_is_able_to_hand },
	{ "IsAbleToDeck", scriptlib::card_is_able_to_deck },
	{ "IsAbleToExtra", scriptlib::card_is_able_to_extra },
	{ "IsAbleToGrave", scriptlib::card_is_able_to_grave },
	{ "IsAbleToRemove", scriptlib::card_is_able_to_remove },
	{ "IsAbleToHandAsCost", scriptlib::card_is_able_to_hand_as_cost },
	{ "IsAbleToDeckAsCost", scriptlib::card_is_able_to_deck_as_cost },
	{ "IsAbleToExtraAsCost", scriptlib::card_is_able_to_extra_as_cost },
	{ "IsAbleToDeckOrExtraAsCost", scriptlib::card_is_able_to_deck_or_extra_as_cost },
	{ "IsAbleToGraveAsCost", scriptlib::card_is_able_to_grave_as_cost },
	{ "IsAbleToRemoveAsCost", scriptlib::card_is_able_to_remove_as_cost },
	{ "IsReleasable", scriptlib::card_is_releasable },
	{ "IsReleasableByEffect", scriptlib::card_is_releasable_by_effect },
	{ "IsDiscardable", scriptlib::card_is_discardable },
	{ "CanAttack", scriptlib::card_can_attack },
	{ "CanChainAttack", scriptlib::card_can_chain_attack },
	{ "IsFaceup", scriptlib::card_is_faceup },
	{ "IsAttackPos", scriptlib::card_is_attack_pos },
	{ "IsFacedown", scriptlib::card_is_facedown },
	{ "IsDefensePos", scriptlib::card_is_defense_pos },
	{ "IsPosition", scriptlib::card_is_position },
	{ "IsPreviousPosition", scriptlib::card_is_pre_position },
	{ "IsControler", scriptlib::card_is_controler },
	{ "IsOnField", scriptlib::card_is_onfield },
	{ "IsLocation", scriptlib::card_is_location },
	{ "IsPreviousLocation", scriptlib::card_is_pre_location },
	{ "IsLevelBelow", scriptlib::card_is_level_below },
	{ "IsLevelAbove", scriptlib::card_is_level_above },
	{ "IsRankBelow", scriptlib::card_is_rank_below },
	{ "IsRankAbove", scriptlib::card_is_rank_above },
	{ "IsLinkBelow", scriptlib::card_is_link_below },
	{ "IsLinkAbove", scriptlib::card_is_link_above },
	{ "IsAttackBelow", scriptlib::card_is_attack_below },
	{ "IsAttackAbove", scriptlib::card_is_attack_above },
	{ "IsDefenseBelow", scriptlib::card_is_defense_below },
	{ "IsDefenseAbove", scriptlib::card_is_defense_above },
	{ "IsPublic", scriptlib::card_is_public },
	{ "IsForbidden", scriptlib::card_is_forbidden },
	{ "IsAbleToChangeControler", scriptlib::card_is_able_to_change_controler },
	{ "IsControlerCanBeChanged", scriptlib::card_is_controler_can_be_changed },
	{ "AddCounter", scriptlib::card_add_counter },
	{ "RemoveCounter", scriptlib::card_remove_counter },
	{ "GetCounter", scriptlib::card_get_counter },
	{ "EnableCounterPermit", scriptlib::card_enable_counter_permit },
	{ "SetCounterLimit", scriptlib::card_set_counter_limit },
	{ "IsCanChangePosition", scriptlib::card_is_can_change_position },
	{ "IsCanTurnSet", scriptlib::card_is_can_turn_set },
	{ "IsCanAddCounter", scriptlib::card_is_can_add_counter },
	{ "IsCanRemoveCounter", scriptlib::card_is_can_remove_counter },
	{ "IsCanOverlay", scriptlib::card_is_can_overlay },
	{ "IsCanBeFusionMaterial", scriptlib::card_is_can_be_fusion_material },
	{ "IsCanBeSynchroMaterial", scriptlib::card_is_can_be_synchro_material },
	{ "IsCanBeRitualMaterial", scriptlib::card_is_can_be_ritual_material },
	{ "IsCanBeXyzMaterial", scriptlib::card_is_can_be_xyz_material },
	{ "IsCanBeLinkMaterial", scriptlib::card_is_can_be_link_material },
	{ "IsCanBeMaterial", scriptlib::card_is_can_be_material },
	{ "CheckFusionMaterial", scriptlib::card_check_fusion_material },
	{ "CheckFusionSubstitute", scriptlib::card_check_fusion_substitute },
	{ "IsImmuneToEffect", scriptlib::card_is_immune_to_effect },
	{ "IsCanBeEffectTarget", scriptlib::card_is_can_be_effect_target },
	{ "IsCanBeBattleTarget", scriptlib::card_is_can_be_battle_target },
	{ "AddMonsterAttribute", scriptlib::card_add_monster_attribute },
	{ "AddMonsterAttributeComplete", scriptlib::card_add_monster_attribute_complete },
	{ "CancelToGrave", scriptlib::card_cancel_to_grave },
	{ "GetTributeRequirement", scriptlib::card_get_tribute_requirement },
	{ "GetBattleTarget", scriptlib::card_get_battle_target },
	{ "GetAttackableTarget", scriptlib::card_get_attackable_target },
	{ "SetHint", scriptlib::card_set_hint },
	{ "ReverseInDeck", scriptlib::card_reverse_in_deck },
	{ "SetUniqueOnField", scriptlib::card_set_unique_onfield },
	{ "CheckUniqueOnField", scriptlib::card_check_unique_onfield },
	{ "ResetNegateEffect", scriptlib::card_reset_negate_effect },
	{ "AssumeProperty", scriptlib::card_assume_prop },
	{ "SetSPSummonOnce", scriptlib::card_set_spsummon_once },
	{ "Code", scriptlib::card_code },
	{ "Alias", scriptlib::card_alias },
	{ "Setcode", scriptlib::card_setcode },
	{ "Type", scriptlib::card_type },
	{ "Level", scriptlib::card_level },
	{ "Attribute", scriptlib::card_attribute },
	{ "Race", scriptlib::card_race },
	{ "Attack", scriptlib::card_attack },
	{ "Defense", scriptlib::card_defense },
	{ "Rscale", scriptlib::card_rscale },
	{ "Lscale", scriptlib::card_lscale },
	{ "LinkMarker", scriptlib::card_link_marker },
	{ "Recreate", scriptlib::card_recreate },
	{ "Cover", scriptlib::card_cover },
	{ "GetLuaRef", objref<card>::get_lua_ref },
	{ "FromLuaRef", objref<card>::from_lua_ref },
	{ NULL, NULL }
};

constexpr luaL_Reg effectlib[] = {
	{ "CreateEffect", scriptlib::effect_new },
	{ "GlobalEffect", scriptlib::effect_newex },
	{ "Clone", scriptlib::effect_clone },
	{ "Reset", scriptlib::effect_reset },
	{ "GetFieldID", scriptlib::effect_get_field_id },
	{ "SetDescription", scriptlib::effect_set_description },
	{ "SetCode", scriptlib::effect_set_code },
	{ "SetRange", scriptlib::effect_set_range },
	{ "SetTargetRange", scriptlib::effect_set_target_range },
	{ "SetAbsoluteRange", scriptlib::effect_set_absolute_range },
	{ "SetCountLimit", scriptlib::effect_set_count_limit },
	{ "SetReset", scriptlib::effect_set_reset },
	{ "SetType", scriptlib::effect_set_type },
	{ "SetProperty", scriptlib::effect_set_property },
	{ "SetLabel", scriptlib::effect_set_label },
	{ "SetLabelObject", scriptlib::effect_set_label_object },
	{ "SetCategory", scriptlib::effect_set_category },
	{ "SetHintTiming", scriptlib::effect_set_hint_timing },
	{ "SetCondition", scriptlib::effect_set_condition },
	{ "SetTarget", scriptlib::effect_set_target },
	{ "SetCost", scriptlib::effect_set_cost },
	{ "SetValue", scriptlib::effect_set_value },
	{ "SetOperation", scriptlib::effect_set_operation },
	{ "SetOwnerPlayer", scriptlib::effect_set_owner_player },
	{ "GetDescription", scriptlib::effect_get_description },
	{ "GetCode", scriptlib::effect_get_code },
	{ "GetCountLimit", scriptlib::effect_get_count_limit },
	{ "GetReset", scriptlib::effect_get_reset },
	{ "GetType", scriptlib::effect_get_type },
	{ "GetProperty", scriptlib::effect_get_property },
	{ "GetLabel", scriptlib::effect_get_label },
	{ "GetLabelObject", scriptlib::effect_get_label_object },
	{ "GetCategory", scriptlib::effect_get_category },
	{ "GetOwner", scriptlib::effect_get_owner },
	{ "GetHandler", scriptlib::effect_get_handler },
	{ "GetCondition", scriptlib::effect_get_condition },
	{ "GetTarget", scriptlib::effect_get_target },
	{ "GetCost", scriptlib::effect_get_cost },
	{ "GetValue", scriptlib::effect_get_value },
	{ "GetOperation", scriptlib::effect_get_operation },
	{ "GetActiveType", scriptlib::effect_get_active_type },
	{ "IsActiveType", scriptlib::effect_is_active_type },
	{ "GetOwnerPlayer", scriptlib::effect_get_owner_player },
	{ "GetHandlerPlayer", scriptlib::effect_get_handler_player },
	{ "IsHasProperty", scriptlib::effect_is_has_property },
	{ "IsHasCategory", scriptlib::effect_is_has_category },
	{ "IsHasType", scriptlib::effect_is_has_type },
	{ "IsActivatable", scriptlib::effect_is_activatable },
	{ "IsActivated", scriptlib::effect_is_activated },
	{ "GetActivateLocation", scriptlib::effect_get_activate_location },
	{ "GetActivateSequence", scriptlib::effect_get_activate_sequence },
	{ "CheckCountLimit", scriptlib::effect_check_count_limit },
	{ "UseCountLimit", scriptlib::effect_use_count_limit },
	{ "GetLuaRef", objref<effect>::get_lua_ref },
	{ "FromLuaRef", objref<effect>::from_lua_ref },
	{ NULL, NULL }
};

constexpr luaL_Reg grouplib[] = {
	{ "__band", scriptlib::group_band },
	{ "__add", scriptlib::group_add },
	{ "__sub", scriptlib::group_sub_const },
	{ "__len", scriptlib::group_len },
	{ "__eq", scriptlib::group_equal_size },
	{ "__lt", scriptlib::group_less_than },
	{ "__le", scriptlib::group_less_equal_than },
	{ "CreateGroup", scriptlib::group_new },
	{ "KeepAlive", scriptlib::group_keep_alive },
	{ "DeleteGroup", scriptlib::group_delete },
	{ "Clone", scriptlib::group_clone },
	{ "FromCards", scriptlib::group_from_cards },
	{ "Clear", scriptlib::group_clear },
	{ "AddCard", scriptlib::group_add_card },
	{ "RemoveCard", scriptlib::group_remove_card },
	{ "GetNext", scriptlib::group_get_next },
	{ "GetFirst", scriptlib::group_get_first },
	{ "TakeatPos", scriptlib::group_take_at_pos },
	{ "GetCount", scriptlib::group_get_count },
	{ "ForEach", scriptlib::group_for_each },
	{ "Filter", scriptlib::group_filter },
	{ "FilterCount", scriptlib::group_filter_count },
	{ "FilterSelect", scriptlib::group_filter_select },
	{ "Select", scriptlib::group_select },
	{ "SelectUnselect", scriptlib::group_select_unselect },
	{ "RandomSelect", scriptlib::group_random_select },
	{ "IsExists", scriptlib::group_is_exists },
	{ "CheckWithSumEqual", scriptlib::group_check_with_sum_equal },
	{ "SelectWithSumEqual", scriptlib::group_select_with_sum_equal },
	{ "CheckWithSumGreater", scriptlib::group_check_with_sum_greater },
	{ "SelectWithSumGreater", scriptlib::group_select_with_sum_greater },
	{ "GetMinGroup", scriptlib::group_get_min_group },
	{ "GetMaxGroup", scriptlib::group_get_max_group },
	{ "GetSum", scriptlib::group_get_sum },
	{ "GetClass", scriptlib::group_get_class },
	{ "GetClassCount", scriptlib::group_get_class_count },
	{ "Remove", scriptlib::group_remove },
	{ "Merge", scriptlib::group_merge },
	{ "Sub", scriptlib::group_sub },
	{ "Equal", scriptlib::group_equal },
	{ "IsContains", scriptlib::group_is_contains },
	{ "SearchCard", scriptlib::group_search_card },
	{ "Split", scriptlib::group_split },
	{ "Includes", scriptlib::group_includes },
	{ "GetLuaRef", objref<group>::get_lua_ref },
	{ "FromLuaRef", objref<group>::from_lua_ref },
	{ NULL, NULL }
};

constexpr luaL_Reg duellib[] = {
	{ "EnableGlobalFlag", scriptlib::duel_enable_global_flag },
	{ "GetLP", scriptlib::duel_get_lp },
	{ "SetLP", scriptlib::duel_set_lp },
	{ "GetTurnPlayer", scriptlib::duel_get_turn_player },
	{ "GetTurnCount", scriptlib::duel_get_turn_count },
	{ "GetDrawCount", scriptlib::duel_get_draw_count },
	{ "RegisterEffect", scriptlib::duel_register_effect },
	{ "RegisterFlagEffect", scriptlib::duel_register_flag_effect },
	{ "GetFlagEffect", scriptlib::duel_get_flag_effect },
	{ "ResetFlagEffect", scriptlib::duel_reset_flag_effect },
	{ "SetFlagEffectLabel", scriptlib::duel_set_flag_effect_label },
	{ "GetFlagEffectLabel", scriptlib::duel_get_flag_effect_label },
	{ "Destroy", scriptlib::duel_destroy },
	{ "Remove", scriptlib::duel_remove },
	{ "SendtoGrave", scriptlib::duel_sendto_grave },
	{ "SendtoHand", scriptlib::duel_sendto_hand },
	{ "SendtoDeck", scriptlib::duel_sendto_deck },
	{ "SendtoExtraP", scriptlib::duel_sendto_extra },
	{ "Sendto", scriptlib::duel_sendto },
	{ "RemoveCards", scriptlib::duel_remove_cards },
	{ "GetOperatedGroup", scriptlib::duel_get_operated_group },
	{ "Summon", scriptlib::duel_summon },
	{ "SpecialSummonRule", scriptlib::duel_special_summon_rule },
	{ "SynchroSummon", scriptlib::duel_synchro_summon },
	{ "XyzSummon", scriptlib::duel_xyz_summon },
	{ "LinkSummon", scriptlib::duel_link_summon },
	{ "ProcedureSummon", scriptlib::duel_procedure_summon },
	{ "PendulumSummon", scriptlib::duel_pendulum_summon },
	{ "ProcedureSummonGroup", scriptlib::duel_procedure_summon_group },
	{ "MSet", scriptlib::duel_setm },
	{ "SSet", scriptlib::duel_sets },
	{ "CreateToken", scriptlib::duel_create_token },
	{ "SpecialSummon", scriptlib::duel_special_summon },
	{ "SpecialSummonStep", scriptlib::duel_special_summon_step },
	{ "SpecialSummonComplete", scriptlib::duel_special_summon_complete },
	{ "IsCanAddCounter", scriptlib::duel_is_can_add_counter },
	{ "RemoveCounter", scriptlib::duel_remove_counter },
	{ "IsCanRemoveCounter", scriptlib::duel_is_can_remove_counter },
	{ "GetCounter", scriptlib::duel_get_counter },
	{ "ChangePosition", scriptlib::duel_change_form },
	{ "Release", scriptlib::duel_release },
	{ "MoveToField", scriptlib::duel_move_to_field },
	{ "ReturnToField", scriptlib::duel_return_to_field },
	{ "MoveSequence", scriptlib::duel_move_sequence },
	{ "SwapSequence", scriptlib::duel_swap_sequence },
	{ "Activate", scriptlib::duel_activate_effect },
	{ "SetChainLimit", scriptlib::duel_set_chain_limit },
	{ "SetChainLimitTillChainEnd", scriptlib::duel_set_chain_limit_p },
	{ "GetChainMaterial", scriptlib::duel_get_chain_material },
	{ "ConfirmDecktop", scriptlib::duel_confirm_decktop },
	{ "ConfirmExtratop", scriptlib::duel_confirm_extratop },
	{ "ConfirmCards", scriptlib::duel_confirm_cards },
	{ "SortDecktop", scriptlib::duel_sort_decktop },
	{ "SortDeckbottom", scriptlib::duel_sort_deckbottom },
	{ "CheckEvent", scriptlib::duel_check_event },
	{ "RaiseEvent", scriptlib::duel_raise_event },
	{ "RaiseSingleEvent", scriptlib::duel_raise_single_event },
	{ "CheckTiming", scriptlib::duel_check_timing },
	{ "GetEnvironment", scriptlib::duel_get_environment },
	{ "IsEnvironment", scriptlib::duel_is_environment },
	{ "Win", scriptlib::duel_win },
	{ "Draw", scriptlib::duel_draw },
	{ "Damage", scriptlib::duel_damage },
	{ "Recover", scriptlib::duel_recover },
	{ "RDComplete", scriptlib::duel_rd_complete },
	{ "Equip", scriptlib::duel_equip },
	{ "EquipComplete", scriptlib::duel_equip_complete },
	{ "GetControl", scriptlib::duel_get_control },
	{ "SwapControl", scriptlib::duel_swap_control },
	{ "CheckLPCost", scriptlib::duel_check_lp_cost },
	{ "PayLPCost", scriptlib::duel_pay_lp_cost },
	{ "DiscardDeck", scriptlib::duel_discard_deck },
	{ "DiscardHand", scriptlib::duel_discard_hand },
	{ "DisableShuffleCheck", scriptlib::duel_disable_shuffle_check },
	{ "DisableSelfDestroyCheck", scriptlib::duel_disable_self_destroy_check },
	{ "ShuffleDeck", scriptlib::duel_shuffle_deck },
	{ "ShuffleExtra", scriptlib::duel_shuffle_extra },
	{ "ShuffleHand", scriptlib::duel_shuffle_hand },
	{ "ShuffleSetCard", scriptlib::duel_shuffle_setcard },
	{ "ChangeAttacker", scriptlib::duel_change_attacker },
	{ "ChangeAttackTarget", scriptlib::duel_change_attack_target },
	{ "AttackCostPaid", scriptlib::duel_attack_cost_paid },
	{ "ForceAttack", scriptlib::duel_force_attack },
	{ "CalculateDamage", scriptlib::duel_calculate_damage },
	{ "GetBattleDamage", scriptlib::duel_get_battle_damage },
	{ "ChangeBattleDamage", scriptlib::duel_change_battle_damage },
	{ "ChangeTargetCard", scriptlib::duel_change_target },
	{ "ChangeTargetPlayer", scriptlib::duel_change_target_player },
	{ "ChangeTargetParam", scriptlib::duel_change_target_param },
	{ "BreakEffect", scriptlib::duel_break_effect },
	{ "ChangeChainOperation", scriptlib::duel_change_effect },
	{ "NegateActivation", scriptlib::duel_negate_activate },
	{ "NegateEffect", scriptlib::duel_negate_effect },
	{ "NegateRelatedChain", scriptlib::duel_negate_related_chain },
	{ "NegateSummon", scriptlib::duel_disable_summon },
	{ "IncreaseSummonedCount", scriptlib::duel_increase_summon_count },
	{ "CheckSummonedCount", scriptlib::duel_check_summon_count },
	{ "GetLocationCount", scriptlib::duel_get_location_count },
	{ "GetMZoneCount", scriptlib::duel_get_mzone_count },
	{ "GetLocationCountFromEx", scriptlib::duel_get_location_count_fromex },
	{ "GetUsableMZoneCount", scriptlib::duel_get_usable_mzone_count },
	{ "GetLinkedGroup", scriptlib::duel_get_linked_group },
	{ "GetLinkedGroupCount", scriptlib::duel_get_linked_group_count },
	{ "GetLinkedZone", scriptlib::duel_get_linked_zone },
	{ "GetFreeLinkedZone", scriptlib::duel_get_free_linked_zone },
	{ "GetFieldCard", scriptlib::duel_get_field_card },
	{ "CheckLocation", scriptlib::duel_check_location },
	{ "GetCurrentChain", scriptlib::duel_get_current_chain },
	{ "GetChainInfo", scriptlib::duel_get_chain_info },
	{ "GetChainEvent", scriptlib::duel_get_chain_event },
	{ "GetFirstTarget", scriptlib::duel_get_first_target },
	{ "GetCurrentPhase", scriptlib::duel_get_current_phase },
	{ "SkipPhase", scriptlib::duel_skip_phase },
	{ "IsAttackCostPaid", scriptlib::duel_is_attack_cost_paid },
	{ "IsDamageCalculated", scriptlib::duel_is_damage_calculated },
	{ "GetAttacker", scriptlib::duel_get_attacker },
	{ "GetAttackTarget", scriptlib::duel_get_attack_target },
	{ "GetBattleMonster", scriptlib::duel_get_battle_monster },
	{ "NegateAttack", scriptlib::duel_disable_attack },
	{ "ChainAttack", scriptlib::duel_chain_attack },
	{ "Readjust", scriptlib::duel_readjust },
	{ "AdjustInstantly", scriptlib::duel_adjust_instantly },
	{ "GetFieldGroup", scriptlib::duel_get_field_group },
	{ "GetFieldGroupCount", scriptlib::duel_get_field_group_count },
	{ "GetDecktopGroup", scriptlib::duel_get_decktop_group },
	{ "GetExtraTopGroup", scriptlib::duel_get_extratop_group },
	{ "GetMatchingGroup", scriptlib::duel_get_matching_group },
	{ "GetMatchingGroupCount", scriptlib::duel_get_matching_count },
	{ "GetFirstMatchingCard", scriptlib::duel_get_first_matching_card },
	{ "IsExistingMatchingCard", scriptlib::duel_is_existing_matching_card },
	{ "SelectMatchingCard", scriptlib::duel_select_matching_cards },
	{ "SelectCardsFromCodes", scriptlib::duel_select_cards_code },
	{ "GetReleaseGroup", scriptlib::duel_get_release_group },
	{ "GetReleaseGroupCount", scriptlib::duel_get_release_group_count },
	{ "CheckReleaseGroup", scriptlib::duel_check_release_group },
	{ "SelectReleaseGroup", scriptlib::duel_select_release_group },
	{ "CheckReleaseGroupEx", scriptlib::duel_check_release_group_ex },
	{ "SelectReleaseGroupEx", scriptlib::duel_select_release_group_ex },
	{ "GetTributeGroup", scriptlib::duel_get_tribute_group },
	{ "GetTributeCount", scriptlib::duel_get_tribute_count },
	{ "CheckTribute", scriptlib::duel_check_tribute },
	{ "SelectTribute", scriptlib::duel_select_tribute },
	{ "GetTargetCount", scriptlib::duel_get_target_count },
	{ "IsExistingTarget", scriptlib::duel_is_existing_target },
	{ "SelectTarget", scriptlib::duel_select_target },
	{ "SelectFusionMaterial", scriptlib::duel_select_fusion_material },
	{ "SetFusionMaterial", scriptlib::duel_set_fusion_material },
	{ "GetRitualMaterial", scriptlib::duel_get_ritual_material },
	{ "ReleaseRitualMaterial", scriptlib::duel_release_ritual_material },
	{ "GetFusionMaterial", scriptlib::duel_get_fusion_material },
	{ "IsSummonCancelable", scriptlib::duel_is_summon_cancelable },
	{ "SetSelectedCard", scriptlib::duel_set_must_select_cards },
	{ "GrabSelectedCard", scriptlib::duel_grab_must_select_cards },
	{ "SetTargetCard", scriptlib::duel_set_target_card },
	{ "ClearTargetCard", scriptlib::duel_clear_target_card },
	{ "SetTargetPlayer", scriptlib::duel_set_target_player },
	{ "SetTargetParam", scriptlib::duel_set_target_param },
	{ "SetOperationInfo", scriptlib::duel_set_operation_info },
	{ "GetOperationInfo", scriptlib::duel_get_operation_info },
	{ "GetOperationCount", scriptlib::duel_get_operation_count },
	{ "ClearOperationInfo", scriptlib::duel_clear_operation_info },
	{ "Overlay", scriptlib::duel_overlay },
	{ "GetOverlayGroup", scriptlib::duel_get_overlay_group },
	{ "GetOverlayCount", scriptlib::duel_get_overlay_count },
	{ "CheckRemoveOverlayCard", scriptlib::duel_check_remove_overlay_card },
	{ "RemoveOverlayCard", scriptlib::duel_remove_overlay_card },
	{ "Hint", scriptlib::duel_hint },
	{ "HintSelection", scriptlib::duel_hint_selection },
	{ "SelectEffectYesNo", scriptlib::duel_select_effect_yesno },
	{ "SelectYesNo", scriptlib::duel_select_yesno },
	{ "SelectOption", scriptlib::duel_select_option },
	{ "SelectPosition", scriptlib::duel_select_position },
	{ "SelectDisableField", scriptlib::duel_select_disable_field },
	{ "SelectFieldZone", scriptlib::duel_select_field_zone },
	{ "AnnounceRace", scriptlib::duel_announce_race },
	{ "AnnounceAttribute", scriptlib::duel_announce_attribute },
	{ "AnnounceLevel", scriptlib::duel_announce_level },
	{ "AnnounceCard", scriptlib::duel_announce_card },
	{ "AnnounceType", scriptlib::duel_announce_type },
	{ "AnnounceNumber", scriptlib::duel_announce_number },
	{ "AnnounceCoin", scriptlib::duel_announce_coin },
	{ "TossCoin", scriptlib::duel_toss_coin },
	{ "TossDice", scriptlib::duel_toss_dice },
	{ "RockPaperScissors", scriptlib::duel_rock_paper_scissors },
	{ "GetCoinResult", scriptlib::duel_get_coin_result },
	{ "GetDiceResult", scriptlib::duel_get_dice_result },
	{ "SetCoinResult", scriptlib::duel_set_coin_result },
	{ "SetDiceResult", scriptlib::duel_set_dice_result },
	{ "IsDuelType", scriptlib::duel_is_duel_type },
	{ "IsPlayerAffectedByEffect", scriptlib::duel_is_player_affected_by_effect },
	{ "GetPlayerEffect", scriptlib::duel_get_player_effect },
	{ "IsPlayerCanDraw", scriptlib::duel_is_player_can_draw },
	{ "IsPlayerCanDiscardDeck", scriptlib::duel_is_player_can_discard_deck },
	{ "IsPlayerCanDiscardDeckAsCost", scriptlib::duel_is_player_can_discard_deck_as_cost },
	{ "IsPlayerCanSummon", scriptlib::duel_is_player_can_summon },
	{ "CanPlayerSetMonster", scriptlib::duel_can_player_set_monster },
	{ "CanPlayerSetSpellTrap", scriptlib::duel_can_player_set_spell_trap },
	{ "IsPlayerCanSpecialSummon", scriptlib::duel_is_player_can_spsummon },
	{ "IsPlayerCanFlipSummon", scriptlib::duel_is_player_can_flipsummon },
	{ "IsPlayerCanSpecialSummonMonster", scriptlib::duel_is_player_can_spsummon_monster },
	{ "IsPlayerCanSpecialSummonCount", scriptlib::duel_is_player_can_spsummon_count },
	{ "IsPlayerCanRelease", scriptlib::duel_is_player_can_release },
	{ "IsPlayerCanRemove", scriptlib::duel_is_player_can_remove },
	{ "IsPlayerCanSendtoHand", scriptlib::duel_is_player_can_send_to_hand },
	{ "IsPlayerCanSendtoGrave", scriptlib::duel_is_player_can_send_to_grave },
	{ "IsPlayerCanSendtoDeck", scriptlib::duel_is_player_can_send_to_deck },
	{ "IsPlayerCanAdditionalSummon", scriptlib::duel_is_player_can_additional_summon },
	{ "IsPlayerCanPendulumSummon", scriptlib::duel_is_player_can_pendulum_summon },
	{ "IsPlayerCanProcedureSummonGroup", scriptlib::duel_is_player_can_procedure_summon_group },
	{ "IsChainNegatable", scriptlib::duel_is_chain_negatable },
	{ "IsChainDisablable", scriptlib::duel_is_chain_disablable },
	{ "CheckChainTarget", scriptlib::duel_check_chain_target },
	{ "CheckChainUniqueness", scriptlib::duel_check_chain_uniqueness },
	{ "GetActivityCount", scriptlib::duel_get_activity_count },
	{ "CheckPhaseActivity", scriptlib::duel_check_phase_activity },
	{ "AddCustomActivityCounter", scriptlib::duel_add_custom_activity_counter },
	{ "GetCustomActivityCount", scriptlib::duel_get_custom_activity_count },
	{ "GetBattledCount", scriptlib::duel_get_battled_count },
	{ "IsAbleToEnterBP", scriptlib::duel_is_able_to_enter_bp },
	{ "TagSwap", scriptlib::duel_tag_swap },
	{ "GetPlayersCount", scriptlib::duel_get_player_count },
	{ "SwapDeckAndGrave", scriptlib::duel_swap_deck_and_grave },
	{ "MajesticCopy", scriptlib::duel_majestic_copy },
	{ "GetRandomNumber", scriptlib::duel_get_random_number },
	{ "AssumeReset", scriptlib::duel_assume_reset },
	{ "GetCardFromCardID", scriptlib::duel_get_card_from_cardid },
	{ "LoadScript", scriptlib::duel_load_script },
	{ "GetStartingHand", scriptlib::duel_get_starting_hand },
	{ NULL, NULL }
};

constexpr luaL_Reg debuglib[] = {
	{ "Message", scriptlib::debug_message },
	{ "AddCard", scriptlib::debug_add_card },
	{ "SetPlayerInfo", scriptlib::debug_set_player_info },
	{ "PreSummon", scriptlib::debug_pre_summon },
	{ "PreEquip", scriptlib::debug_pre_equip },
	{ "PreSetTarget", scriptlib::debug_pre_set_target },
	{ "PreAddCounter", scriptlib::debug_pre_add_counter },
	{ "ReloadFieldBegin", scriptlib::debug_reload_field_begin },
	{ "ReloadFieldEnd", scriptlib::debug_reload_field_end },
	{ "SetAIName", scriptlib::debug_set_ai_name },
	{ "ShowHint", scriptlib::debug_show_hint },
	{ "PrintStacktrace", scriptlib::debug_print_stacktrace },
	{ NULL, NULL }
};

interpreter::interpreter(duel* pd): coroutines(256), deleted(pd) {
	lua_state = luaL_newstate();
	current_state = lua_state;
	pduel = pd;
	no_action = 0;
	call_depth = 0;
	memcpy(lua_getextraspace(lua_state), &pd, LUA_EXTRASPACE);
	//Initial
	luaL_openlibs(lua_state);
	lua_pushnil(lua_state);
	lua_setglobal(lua_state, "file");
	lua_pushnil(lua_state);
	lua_setglobal(lua_state, "io");
	lua_pushnil(lua_state);
	lua_setglobal(lua_state, "os");
	//open all libs
	luaL_newlib(lua_state, cardlib);
	lua_pushstring(lua_state, "__index");
	lua_pushvalue(lua_state, -2);
	lua_rawset(lua_state, -3);
	lua_setglobal(lua_state, "Card");
	luaL_newlib(lua_state, effectlib);
	lua_pushstring(lua_state, "__index");
	lua_pushvalue(lua_state, -2);
	lua_rawset(lua_state, -3);
	lua_setglobal(lua_state, "Effect");
	luaL_newlib(lua_state, grouplib);
	lua_pushstring(lua_state, "__index");
	lua_pushvalue(lua_state, -2);
	lua_rawset(lua_state, -3);
	lua_setglobal(lua_state, "Group");
	luaL_newlib(lua_state, duellib);
	lua_setglobal(lua_state, "Duel");
	luaL_newlib(lua_state, debuglib);
	lua_setglobal(lua_state, "Debug");
}
interpreter::~interpreter() {
	lua_close(lua_state);
}
//creates a pointer to a lua_obj in the lua stack
static inline lua_obj** create_object(lua_State* L) {
	return static_cast<lua_obj**>(lua_newuserdata(L, sizeof(lua_obj*)));
}
int32 interpreter::register_card(card* pcard) {
	//create a card in by userdata
	lua_obj** ppcard = create_object(lua_state);
	*ppcard = pcard;
	pcard->ref_handle = luaL_ref(lua_state, LUA_REGISTRYINDEX);
	//creates the pointer in the main state, then after taking a reference
	//pushes it in the stack of the current_state, as load_card_script push the metatable there
	lua_rawgeti(current_state, LUA_REGISTRYINDEX, pcard->ref_handle);
	//load script
	if(pcard->data.alias && (pcard->data.alias < pcard->data.code + 10) && (pcard->data.code < pcard->data.alias + 10))
		load_card_script(pcard->data.alias);
	else
		load_card_script(pcard->data.code);
	//set metatable of pointer to base script
	lua_setmetatable(current_state, -2);
	lua_pop(current_state, 1);
	//Initial
	if(pcard->data.code) {
		const bool forced = !(pcard->data.type & TYPE_NORMAL) || (pcard->data.type & TYPE_PENDULUM);
		pcard->set_status(STATUS_INITIALIZING, TRUE);
		add_param(pcard, PARAM_TYPE_CARD);
		call_card_function(pcard, "initial_effect", 1, 0, forced);
		pcard->set_status(STATUS_INITIALIZING, FALSE);
	}
	pcard->cardid = pduel->game_field->infos.card_id++;
	return OPERATION_SUCCESS;
}
static inline void remove_object(lua_State* L, lua_obj* obj, lua_obj* replacement) {
	if(!obj)
		return;
	lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_handle);
	lua_obj** lobj = static_cast<lua_obj**>(lua_touserdata(L, -1));
	*lobj = replacement;
	lua_pop(L, 1);
	luaL_unref(L, LUA_REGISTRYINDEX, obj->ref_handle);
	obj->ref_handle = 0;
}
void interpreter::register_effect(effect* peffect) {
	register_obj(peffect, "Effect");
}
void interpreter::unregister_effect(effect* peffect) {
	if (!peffect)
		return;
	if(peffect->condition)
		luaL_unref(lua_state, LUA_REGISTRYINDEX, peffect->condition);
	if(peffect->cost)
		luaL_unref(lua_state, LUA_REGISTRYINDEX, peffect->cost);
	if(peffect->target)
		luaL_unref(lua_state, LUA_REGISTRYINDEX, peffect->target);
	if(peffect->operation)
		luaL_unref(lua_state, LUA_REGISTRYINDEX, peffect->operation);
	if(peffect->value && peffect->is_flag(EFFECT_FLAG_FUNC_VALUE))
		luaL_unref(lua_state, LUA_REGISTRYINDEX, peffect->value);
	if(peffect->label_object) {
		lua_rawgeti(lua_state, LUA_REGISTRYINDEX, peffect->label_object);
		if(lua_istable(lua_state, -1))
			luaL_unref(lua_state, LUA_REGISTRYINDEX, peffect->label_object);
		lua_pop(lua_state, 1);
	}
	remove_object(lua_state, peffect, &deleted);
}
void interpreter::register_group(group* pgroup) {
	register_obj(pgroup, "Group");
}
void interpreter::unregister_group(group* pgroup) {
	remove_object(lua_state, pgroup, &deleted);
}
void interpreter::register_obj(lua_obj* obj, const char* tablename) {
	if(!obj)
		return;
	lua_obj** pobj = create_object(lua_state);
	*pobj = obj;
	//set metatable current lua object
	lua_getglobal(lua_state, tablename);
	lua_setmetatable(lua_state, -2);
	//pops the lua object from the stack and takes a reference of it
	obj->ref_handle = luaL_ref(lua_state, LUA_REGISTRYINDEX);
}
int32 interpreter::load_script(const char* buffer, int len, const char* script_name) {
	if(!buffer)
		return OPERATION_FAIL;
	no_action++;
	int32 error = luaL_loadbuffer(current_state, buffer, len, script_name) || lua_pcall(current_state, 0, 0, 0);
	if(error) {
		pduel->handle_message(lua_tostring_or_empty(current_state, -1), OCG_LOG_TYPE_ERROR);
		lua_pop(current_state, 1);
		no_action--;
		return OPERATION_FAIL;
	}
	no_action--;
	return OPERATION_SUCCESS;
}
int32 interpreter::load_card_script(uint32 code) {
	const char* class_name = format("c%u", code);
	lua_getglobal(current_state, class_name);
	//if script is not loaded, create and load it
	if (lua_isnoneornil(current_state, -1)) {
		lua_pop(current_state, 1);
		lua_pushinteger(current_state, code);
		lua_setglobal(current_state, "self_code");
		//create a table & set metatable
		lua_createtable(current_state, 0, 0);
		lua_setglobal(current_state, class_name);
		lua_getglobal(current_state, class_name);
		lua_getglobal(current_state, "Card");
		lua_setmetatable(current_state, -2);
		lua_pushstring(current_state, "__index");
		lua_pushvalue(current_state, -2);
		lua_rawset(current_state, -3);
		lua_getglobal(current_state, class_name);
		lua_setglobal(current_state, "self_table");
		if(!pduel->read_script(format("c%u.lua", code))) {
			lua_pushnil(current_state);
			lua_setglobal(current_state, "self_table");
			lua_pushnil(current_state);
			lua_setglobal(current_state, "self_code");
			return OPERATION_FAIL;
		}
		lua_pushnil(current_state);
		lua_setglobal(current_state, "self_table");
		lua_pushnil(current_state);
		lua_setglobal(current_state, "self_code");
	}
	return OPERATION_SUCCESS;
}
void interpreter::add_param(void* param, int32 type, bool front) {
	add_param(reinterpret_cast<lua_Integer>(param), type, front);
}
void interpreter::add_param(lua_Integer param, int32 type, bool front) {
	if(front)
		params.emplace_front(param, type);
	else
		params.emplace_back(param, type);
}
void interpreter::push_param(lua_State* L, bool is_coroutine) {
	int32 pushed = 0;
	for (const auto& it : params) {
		switch(it.second) {
		case PARAM_TYPE_INT:
			lua_pushinteger(L, it.first);
			break;
		case PARAM_TYPE_STRING:
			lua_pushstring(L, reinterpret_cast<const char*>(it.first));
			break;
		case PARAM_TYPE_BOOLEAN:
			lua_pushboolean(L, static_cast<bool>(it.first));
			break;
		case PARAM_TYPE_CARD:
		case PARAM_TYPE_EFFECT:
		case PARAM_TYPE_GROUP:
			pushobject(L, reinterpret_cast<lua_obj*>(it.first));
			break;
		case PARAM_TYPE_FUNCTION:
			pushobject(L, static_cast<int32>(it.first));
			break;
		case PARAM_TYPE_INDEX:
			auto index = static_cast<int32>(it.first);
			if(index > 0)
				lua_pushvalue(L, index);
			else if(is_coroutine) {
				//copy value from current_state to new stack
				lua_pushvalue(current_state, index);
				int32 ref = luaL_ref(current_state, LUA_REGISTRYINDEX);
				lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
				luaL_unref(current_state, LUA_REGISTRYINDEX, ref);
			} else {
				//the calling function is pushed before the params, so the actual index is: index - pushed -1
				lua_pushvalue(L, index - pushed - 1);
			}
			break;
		}
		pushed++;
	}
	params.clear();
}
//increase the no_action and call_depth counter by 1
inline void interpreter::deepen() {
	no_action++;
	call_depth++;
}
//decrease the no_action and call_depth counter by 1, and calls scripts/assumes cleanup if needed
inline void interpreter::flatten() {
	no_action--;
	call_depth--;
	if(call_depth == 0) {
		pduel->release_script_group();
		pduel->restore_assumes();
	}
}
inline int32 interpreter::ret_fail(const char* message) {
	interpreter::print_stacktrace(current_state);
	pduel->handle_message(message, OCG_LOG_TYPE_ERROR);
	params.clear();
	return OPERATION_FAIL;
}
inline int32 interpreter::ret_fail(const char* message, bool error) {
	if(error) {
		interpreter::print_stacktrace(current_state);
		pduel->handle_message(message, OCG_LOG_TYPE_ERROR);
	}
	params.clear();
	return OPERATION_FAIL;
}
int32 interpreter::call_function(int param_count, int ret_count) {
	deepen();
	push_param(current_state);
	auto ret = OPERATION_SUCCESS;
	if(lua_pcall(current_state, param_count, ret_count, 0) != LUA_OK) {
		print_stacktrace(current_state);
		pduel->handle_message(lua_tostring_or_empty(current_state, -1), OCG_LOG_TYPE_ERROR);
		lua_pop(current_state, 1);
		ret = OPERATION_FAIL;
	}
	flatten();
	return ret;
}
int32 interpreter::call_function(int32 f, uint32 param_count, int32 ret_count) {
	if (!f)
		return ret_fail(R"("CallFunction": attempt to call a null function.)");
	if (param_count != params.size())
		return ret_fail(format(R"("CallFunction": incorrect parameter count (%u expected, %zu pushed))", param_count, params.size()));
	pushobject(current_state, f);
	if (!lua_isfunction(current_state, -1))
		return ret_fail(R"("CallFunction": attempt to call an error function)");
	return call_function(param_count, ret_count);
}
int32 interpreter::call_card_function(card* pcard, const char* f, uint32 param_count, int32 ret_count, bool forced) {
	if (param_count != params.size())
		return ret_fail(format(R"("CallCardFunction"(c%u.%s): incorrect parameter count)", pcard->data.code, f));
	pushobject(current_state, pcard);
	lua_getfield(current_state, -1, f);
	if (!lua_isfunction(current_state, -1)) {
		lua_pop(current_state, 2);
		return ret_fail(format(R"("CallCardFunction"(c%u.%s): attempt to call an error function)", pcard->data.code, f), forced);
	}
	lua_remove(current_state, -2);
	return call_function(param_count, ret_count);
}
int32 interpreter::call_code_function(uint32 code, const char* f, uint32 param_count, int32 ret_count) {
	if (param_count != params.size())
		return ret_fail(R"("CallCodeFunction": incorrect parameter count)");
	load_card_script(code);
	lua_getfield(current_state, -1, f);
	if (!lua_isfunction(current_state, -1)) {
		lua_pop(current_state, 2);
		return ret_fail(R"("CallCodeFunction": attempt to call an error function)");
	}
	lua_remove(current_state, -2);
	return call_function(param_count, ret_count);
}
int32 interpreter::check_condition(int32 f, uint32 param_count) {
	if(!f) {
		params.clear();
		return TRUE;
	}
	deepen();
	int32 result = OPERATION_FAIL;
	if (call_function(f, param_count, 1) != OPERATION_FAIL) {
		result = lua_toboolean(current_state, -1);
		lua_pop(current_state, 1);
	}
	flatten();
	return result;
}
int32 interpreter::check_matching(card* pcard, int32 findex, int32 extraargs) {
	if(!findex || lua_isnoneornil(current_state, findex))
		return TRUE;
	deepen();
	lua_pushvalue(current_state, findex);
	pushobject(current_state, pcard);
	for(int32 i = 0; i < extraargs; ++i)
		lua_pushvalue(current_state, (int32)(-extraargs - 2));
	int32 result = OPERATION_FAIL;
	if(lua_pcall(current_state, 1 + extraargs, 1, 0) != LUA_OK) {
		print_stacktrace(current_state);
		pduel->handle_message(lua_tostring_or_empty(current_state, -1), OCG_LOG_TYPE_ERROR);
	} else
		result = lua_toboolean(current_state, -1);
	lua_pop(current_state, 1);
	flatten();
	return result;
}
int32 interpreter::check_matching_table(card* pcard, int32 findex, int32 table_index) {
	if(!findex || !lua_istable(current_state, table_index))
		return TRUE;
	deepen();
	lua_pushvalue(current_state, findex);
	pushobject(current_state, pcard);
	int extraargs = pushExpandedTable(current_state, table_index);
	int32 result = OPERATION_FAIL;
	if(lua_pcall(current_state, 1 + extraargs, 1, 0) != LUA_OK) {
		print_stacktrace(current_state);
		pduel->handle_message(lua_tostring_or_empty(current_state, -1), OCG_LOG_TYPE_ERROR);
	} else
		result = lua_toboolean(current_state, -1);
	lua_pop(current_state, 1);
	flatten();
	return result;
}
int32 interpreter::get_operation_value(card* pcard, int32 findex, int32 extraargs) {
	if(!findex || lua_isnoneornil(current_state, findex))
		return 0;
	deepen();
	lua_pushvalue(current_state, findex);
	pushobject(current_state, pcard);
	for(int32 i = 0; i < extraargs; ++i)
		lua_pushvalue(current_state, (int32)(-extraargs - 2));
	int32 result = OPERATION_FAIL;
	if(lua_pcall(current_state, 1 + extraargs, 1, 0) != LUA_OK) {
		print_stacktrace(current_state);
		pduel->handle_message(lua_tostring_or_empty(current_state, -1), OCG_LOG_TYPE_ERROR);
	} else
		result = lua_get<int32>(current_state, -1);
	lua_pop(current_state, 1);
	flatten();
	return result;
}
int32 interpreter::get_operation_value(card* pcard, int32 findex, int32 extraargs, std::vector<int32>* result) {
	if(!findex || lua_isnoneornil(current_state, findex))
		return 0;
	deepen();
	lua_pushvalue(current_state, findex);
	int32 stack_top = lua_gettop(current_state);
	lua_rawgeti(current_state, LUA_REGISTRYINDEX, pcard->ref_handle);
	for(int32 i = 0; i < extraargs; ++i)
		lua_pushvalue(current_state, (int32)(-extraargs - 2));
	no_action++;
	call_depth++;
	auto ret = OPERATION_FAIL;
	if(lua_pcall(current_state, extraargs, LUA_MULTRET, 0) != LUA_OK) {
		print_stacktrace(current_state);
		pduel->handle_message(lua_tostring_or_empty(current_state, -1), OCG_LOG_TYPE_ERROR);
		lua_pop(current_state, 1);
	} else {
		int32 stack_newtop = lua_gettop(current_state);
		for(int32 index = stack_top + 1; index <= stack_newtop; ++index) {
			int32 return_value = 0;
			if(lua_isboolean(current_state, index))
				return_value = lua_get<bool>(current_state, index);
			else
				return_value = lua_get<int32, 0>(current_state, index);
			result->push_back(return_value);
		}
		lua_settop(current_state, stack_top);
		ret = OPERATION_SUCCESS;
	}
	flatten();
	return ret;
}
int32 interpreter::get_function_value(int32 f, uint32 param_count) {
	if(!f) {
		params.clear();
		return OPERATION_FAIL;
	}
	deepen();
	int32 result = OPERATION_FAIL;
	if (call_function(f, param_count, 1) == OPERATION_SUCCESS) {
		if(lua_isboolean(current_state, -1))
			result = lua_get<bool>(current_state, -1);
		else
			result = lua_get<int32, 0>(current_state, -1);
		lua_pop(current_state, 1);
	}
	flatten();
	return result;
}
int32 interpreter::get_function_value(int32 f, uint32 param_count, std::vector<int32>* result) {
	if(!f) {
		params.clear();
		return OPERATION_FAIL;
	}
	deepen();
	int32 res = OPERATION_FAIL;
	const int32 stack_top = lua_gettop(current_state);
	if (call_function(f, param_count, LUA_MULTRET) == OPERATION_SUCCESS) {
		const int32 stack_newtop = lua_gettop(current_state);
		for (int32 index = stack_top + 1; index <= stack_newtop; ++index) {
			int32 return_value = 0;
			if(lua_isboolean(current_state, index))
				return_value = lua_get<bool>(current_state, index);
			else
				return_value = lua_get<int32, 0>(current_state, index);
			result->push_back(return_value);
		}
		//pops all the results from the stack (lua_pop(current_state, stack_newtop - stack_top))
		lua_settop(current_state, stack_top);
		res = OPERATION_SUCCESS;
	}
	flatten();
	return res;
}
int32 interpreter::call_coroutine(int32 f, uint32 param_count, uint32* yield_value, uint16 step) {
	if(yield_value)
		*yield_value = 0;
	if (!f)
		return ret_fail(R"("CallCoroutine": attempt to call a null function)");
	if (param_count != params.size())
		return ret_fail(R"("CallCoroutine": incorrect parameter count)");
	auto it = coroutines.find(f);
	lua_State* rthread;
	if (it == coroutines.end()) {
		rthread = lua_newthread(lua_state);
		const auto threadref = luaL_ref(lua_state, LUA_REGISTRYINDEX);
		pushobject(rthread, f);
		if(!lua_isfunction(rthread, -1))
			return ret_fail(R"("CallCoroutine": attempt to call an error function)");
		call_depth++;
		auto ret = coroutines.emplace(f, std::make_pair(rthread, threadref));
		it = ret.first;
	} else {
		rthread = it->second.first;
		if(step == 0) {
			call_depth--;
			if(call_depth == 0) {
				pduel->release_script_group();
				pduel->restore_assumes();
			}
			return ret_fail("recursive event trigger detected.");
		}
	}
	push_param(rthread, true);
	auto prev_state = current_state;
	current_state = rthread;
	int32 result = lua_resumec(rthread, prev_state, param_count, &result);
	if (result != LUA_YIELD) {
		if(result != LUA_OK) {
			print_stacktrace(current_state);
			pduel->handle_message(lua_tostring_or_empty(rthread, -1), OCG_LOG_TYPE_ERROR);
		} else if(yield_value) {
			if(lua_isboolean(rthread, -1))
				*yield_value = lua_toboolean(rthread, -1);
			else 
				*yield_value = (uint32)lua_tointeger(rthread, -1);
		}
		auto ref = it->second.second;
		coroutines.erase(it);
		current_state = lua_state;
		luaL_unref(lua_state, LUA_REGISTRYINDEX, ref);
		call_depth--;
		if(call_depth == 0) {
			pduel->release_script_group();
			pduel->restore_assumes();
		}
		return (result == LUA_OK) ? COROUTINE_FINISH : COROUTINE_ERROR;
	}
	return COROUTINE_YIELD;
}
int32 interpreter::clone_function_ref(int32 func_ref) {
	lua_rawgeti(current_state, LUA_REGISTRYINDEX, func_ref);
	int32 ref = luaL_ref(current_state, LUA_REGISTRYINDEX);
	return ref;
}
void* interpreter::get_ref_object(int32 ref_handler) {
	if(ref_handler == 0)
		return nullptr;
	lua_rawgeti(current_state, LUA_REGISTRYINDEX, ref_handler);
	auto obj = reinterpret_cast<lua_obj**>(lua_touserdata(current_state, -1));
	void* p = obj ? *obj : nullptr;
	lua_pop(current_state, 1);
	return p;
}
//Convert a pointer to a lua value, +1 -0
void interpreter::pushobject(lua_State* L, lua_obj* obj) {
	if(!obj || obj->ref_handle == 0)
		lua_pushnil(L);
	else
		lua_rawgeti(L, LUA_REGISTRYINDEX, obj->ref_handle);
}
void interpreter::pushobject(lua_State* L, int32 lua_ptr) {
	if(!lua_ptr)
		lua_pushnil(L);
	else
		lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ptr);
}
//Push all the elements of the table to the stack, +len(table) -0
int interpreter::pushExpandedTable(lua_State* L, int32 table_index) {
	int extraargs = 0;
	lua_table_iterate(L, table_index, [&extraargs, &L] {
		lua_pushvalue(L, -1);
		lua_insert(L, -3);
		extraargs++;
	});
	return extraargs;
}
int32 interpreter::get_function_handle(lua_State* L, int32 index) {
	lua_pushvalue(L, index);
	int32 ref = luaL_ref(L, LUA_REGISTRYINDEX);
	return ref;
}

void interpreter::print_stacktrace(lua_State* L) {
	const auto pduel = lua_get<duel*>(L);
	luaL_traceback(L, L, NULL, 1);
	auto len = lua_rawlen(L, -1);
	/*checks for an empty stack*/
	if(len > sizeof("stack traceback:"))
		pduel->handle_message(lua_tostring_or_empty(L, -1), OCG_LOG_TYPE_FOR_DEBUG);
	lua_pop(L, 1);
}
