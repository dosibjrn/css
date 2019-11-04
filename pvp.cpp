#include "pvp.h"

#include <iostream>

#include "dps.h"
#include "spells_priest_disc.h"
#include "stats.h"

namespace css
{

int PvpStats(int argc, char** argv)
{
  // PriestCharacter c = PaistiLvl60PvPShadow();
  // PriestCharacter c = PaistiLvl60PvPShadowIter2();
  PriestCharacter c = PaistiLvl60PvPShadowRaid1();
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

  return 0;
}

}  // namespace css
