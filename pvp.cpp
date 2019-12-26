#include "pvp.h"

#include <iostream>
#include <fstream>

#include "dps.h"
#include "spells_priest_disc.h"
#include "stats.h"
#include "item_picking_main.h"

namespace css
{

void PvpStats(const PriestCharacter &c)
{
  PriestCharacter c_plus_100sp = c;
  c_plus_100sp.sp += 100.0f;

  float dps_a = ShadowDps(c);
  float dps_b = ShadowDps(c_plus_100sp);
  float rel_imp_per_100_sp = (dps_b/dps_a - 1.0f);

  std::cout << "100 sp, dps from: " << dps_a << " to : " << dps_b << " which was a " << rel_imp_per_100_sp*100
      << "\% improvement." << std::endl;


  rel_imp_per_100_sp = (1.0f + rel_imp_per_100_sp)*(1.0f + rel_imp_per_100_sp) - 1.0f;
  std::cout << "Due to the square nature of (ehp * dps)*(dps * eff mana pool), let's square that to: " << rel_imp_per_100_sp*100 << std::endl;

  Spell shield = Shield(c, 10); 
  float ehp_start = c.base_hp + (c.stamina + 70)*10.0f + 2.0f*shield.shield;
  std::cout << "Ehp: " << ehp_start << std::endl;

  float stam_to_match = (ehp_start*rel_imp_per_100_sp)/10.0f;
  std::cout << "same rel improvement for ehp requires: " << stam_to_match << " stam." << std::endl;


  float combat_dur = 100.0f;
  float under_fsr = 2.0f/3.0f;

  int no_fsr_ticks = (combat_dur * (1.0f - under_fsr))/2.0f;
  int fsr_ticks = combat_dur/2.0f - no_fsr_ticks;
  Stats s(c);
  float effective_mana = s.getMaxMana() + no_fsr_ticks*s.getManaRegenTickOutOfFsr()
      + fsr_ticks*s.getManaRegenTickUnderFsr();
  std::cout << "Effective mana with duration: " << combat_dur << ", under_fsr: " << under_fsr << ": " << effective_mana << std::endl;

  float matching_int = (effective_mana*rel_imp_per_100_sp)/15.0f; 
  std::cout << "Same rel improvement for effective mana requires: " << matching_int << " int." << std::endl;

  float matching_mp5 = (effective_mana*rel_imp_per_100_sp)/(combat_dur/5.0f);
  std::cout << "Or " << matching_mp5 << " mp5" << std::endl;

  float mana_goal = effective_mana*(1.0 + rel_imp_per_100_sp); 
  std::cout << "For spirit, we wanted to get effective_mana of: " << mana_goal << std::endl;
  
  float spirit_attempt = 100.0f/35.0f*matching_int;
  std::cout << "If we remember scores spirit: 40 to int: 100, let's try with : " << spirit_attempt  << " spirit. " << std::endl;

  PriestCharacter spirit_boosted = c;
  spirit_boosted.spirit += spirit_attempt;

  Stats s_spirit(spirit_boosted);
  float effective_mana_spirit = s_spirit.getMaxMana() + no_fsr_ticks*s_spirit.getManaRegenTickOutOfFsr()
      + fsr_ticks*s_spirit.getManaRegenTickUnderFsr();
  std::cout << "We got: " << effective_mana_spirit << " out of it, good enough." << std::endl;

  std::cout << "So, 100 sp equals to: " << std::endl;
  std::cout << matching_int << " int" << std::endl;
  std::cout << spirit_attempt << " spirit" << std::endl;
  std::cout << matching_mp5 << " mp5" << std::endl;
  std::cout << stam_to_match << " stamina" << std::endl;
  std::cout << std::endl;
  std::cout << "Which gives us relative values of: " << std::endl;
  std::cout << "Sp:      " << 100.0f << std::endl;
  std::cout << "Int:     " << 100.0f/matching_int*100.0f << std::endl;
  std::cout << "Spirit:  " << 100.0f/spirit_attempt*100.0f << std::endl;
  std::cout << "Mp5:     " << 100.0f/matching_mp5*100.0f << std::endl;
  std::cout << "Stamina: " << 100.0f/stam_to_match*100.0f << std::endl;
 
  std::cout << std::endl;
  std::cout << "Your stats btw: " << std::endl;

  std::cout << "Dps: " << dps_a << std::endl;
  std::cout << "Effective mana for " << combat_dur << " s fight with " << under_fsr*100 << "\% in fsr: " << effective_mana << std::endl;
  std::cout << "Effective hp: " << ehp_start << std::endl;
  std::cout << "dps*dps*emana*ehp/1e12: " << (dps_a*dps_a*ehp_start*effective_mana)/1e12 << std::endl;
}

int PvpStats(int argc, char** argv)
{
  // PriestCharacter c = PaistiLvl60PvPShadow();
  PriestCharacter c = PaistiLvl60PvPShadowIter2();
  PvpStats(c);
  return 0;
}

int PvpStatsVsBase(int argc, char** argv)
{
  PriestCharacter a = PaistiLvl60PvPShadowIter2();

  PriestCharacter b = a;
  PriestCharacter base = BaseLvl60UdShadow();
  b.sp = base.sp;
  b.sp_shadow = base.sp;

  float dps_a = ShadowDps(a);
  float dps_b = ShadowDps(b);
  float sp_rel_imp_sq = (((dps_a*dps_a)/(dps_b*dps_b)) - 1.0f)/(a.sp + a.sp_shadow - b.sp - b.sp_shadow);
  std::cout << "Squared relative improvement of dps per point of sp: " << sp_rel_imp_sq*100 << "\%" << std::endl;

  b = a;
  b.stamina = base.stamina;
  
  Spell shield = Shield(a, 10); 
  float ehp_a = a.base_hp + (a.stamina + 70)*10.0f + 2.0f*shield.shield;
  float ehp_b = b.base_hp + (b.stamina + 70)*10.0f + 2.0f*shield.shield;
  float ehp_rel_imp = (ehp_a/ehp_b - 1.0f)/(a.stamina - b.stamina);
  std::cout << "Relative improvement of ehp per point of stamina: " << ehp_rel_imp << std::endl;

  float stam_to_match_sp = sp_rel_imp_sq/ehp_rel_imp;
  std::cout << "To match 1 sp, we need: " << stam_to_match_sp << " stam." << std::endl;

  float combat_dur = 100.0f;
  float under_fsr = 2.0f/3.0f;

  int no_fsr_ticks = (combat_dur * (1.0f - under_fsr))/2.0f;
  int fsr_ticks = combat_dur/2.0f - no_fsr_ticks;

  float int_to_match_sp = 0.0f;
  {
    b = base;
    b.intelligence = a.intelligence;
    Stats s_a(b);
    Stats s_b(base);
    float effective_mana_a = s_a.getMaxMana() + no_fsr_ticks*s_a.getManaRegenTickOutOfFsr()
        + fsr_ticks*s_a.getManaRegenTickUnderFsr();
    float effective_mana_b = s_b.getMaxMana() + no_fsr_ticks*s_b.getManaRegenTickOutOfFsr()
        + fsr_ticks*s_b.getManaRegenTickUnderFsr();
    float rel_imp_per_point = (effective_mana_a/effective_mana_b - 1.0f)/(b.intelligence - base.intelligence);
    int_to_match_sp = sp_rel_imp_sq/rel_imp_per_point;
  }

  float spi_to_match_sp = 0.0f;
  {
    b = base;
    b.spirit = a.spirit;
    Stats s_a(b);
    Stats s_b(base);
    float effective_mana_a = s_a.getMaxMana() + no_fsr_ticks*s_a.getManaRegenTickOutOfFsr()
        + fsr_ticks*s_a.getManaRegenTickUnderFsr();
    float effective_mana_b = s_b.getMaxMana() + no_fsr_ticks*s_b.getManaRegenTickOutOfFsr()
        + fsr_ticks*s_b.getManaRegenTickUnderFsr();
    float rel_imp_per_point = (effective_mana_a/effective_mana_b - 1.0f)/(b.spirit - base.spirit);
    spi_to_match_sp = sp_rel_imp_sq/rel_imp_per_point;
  }

  float mp5_to_match_sp = 0.0f;
  {
    b = base;
    b.mp5 = std::max<float>(1, a.mp5);
    Stats s_a(b);
    Stats s_b(base);
    float effective_mana_a = s_a.getMaxMana() + no_fsr_ticks*s_a.getManaRegenTickOutOfFsr()
        + fsr_ticks*s_a.getManaRegenTickUnderFsr();
    float effective_mana_b = s_b.getMaxMana() + no_fsr_ticks*s_b.getManaRegenTickOutOfFsr()
        + fsr_ticks*s_b.getManaRegenTickUnderFsr();
    float rel_imp_per_point = (effective_mana_a/effective_mana_b - 1.0f)/(b.mp5 - base.mp5);
    mp5_to_match_sp = sp_rel_imp_sq/rel_imp_per_point;
  }
  std::cout << "Int: " << int_to_match_sp << " to match." << std::endl;
  std::cout << "Spirit: " << spi_to_match_sp << " to match." << std::endl;
  std::cout << "Mp5: " << mp5_to_match_sp << " to match." << std::endl;
   

  std::cout << std::endl;
  std::cout << "Which gives us relative values of: " << std::endl;
  std::cout << "Sp:      " << 100.0f << std::endl;
  std::cout << "Int:     " << 1.0f/int_to_match_sp*100.0f << std::endl;
  std::cout << "Spirit:  " << 1.0f/spi_to_match_sp*100.0f << std::endl;
  std::cout << "Mp5:     " << 1.0f/mp5_to_match_sp*100.0f << std::endl;
  std::cout << "Stamina: " << 1.0f/stam_to_match_sp*100.0f << std::endl;
 
  std::cout << std::endl;
  std::cout << "Your stats btw: " << std::endl;

  Stats s_a(a);
  float effective_mana = s_a.getMaxMana() + no_fsr_ticks*s_a.getManaRegenTickOutOfFsr()
      + fsr_ticks*s_a.getManaRegenTickUnderFsr();
  std::cout << "Dps: " << dps_a << std::endl;
  std::cout << "Effective mana for " << combat_dur << " s fight with " << under_fsr*100 << "\% in fsr: " << effective_mana << std::endl;
  std::cout << "Effective hp: " << ehp_a << std::endl;
  std::cout << "dps*dps*emana*ehp/1e12: " << (dps_a*dps_a*ehp_a*effective_mana)/1e12 << std::endl;

  return 0;
}


int PvpItemPicking(int argc, char** argv)
{
  // PriestCharacter c = BaseLvl60UdShadow();
  PriestCharacter c = BaseLvl60UdShadowEnchanted();
  auto value_choice = ItemPicker::ValueChoice::pvp_shadow;
  return ItemPickingMain(argc, argv, value_choice, c);
}

int PvpHealingItemPicking(int argc, char** argv)
{
  auto c = BaseLvl60DiscHolyPvpHealing();
  // auto c = BaseLvl60HolyDiscHealing();
  auto value_choice = ItemPicker::ValueChoice::pvp_healing;
  return ItemPickingMain(argc, argv, value_choice, c);
}

// TODO: one hand, main hand, off hand, second ring, second trinket, wrong base stats


}  // namespace css
