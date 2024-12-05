/* FILE: scriptuzeeman.h         -*-Mode: c++-*-
 *
 * Uniform Zeeman (applied field) energy, derived from Oxs_Energy class,
 * specified from a Tcl proc.
 *
 */

#ifndef _OXS_SCRIPTUZEEMAN
#define _OXS_SCRIPTUZEEMAN

#include <vector>
#include "nb.h"
#include "threevector.h"
#include "output.h"
#include "chunkenergy.h"
#include "energy.h"

OC_USE_STD_NAMESPACE;

/* End includes */

class Oxs_ScriptUZeeman:public Oxs_ChunkEnergy {
private:
  vector<Nb_TclCommandLineOption> command_options;
  Nb_TclCommand cmd;

  OC_REAL8m hscale;
  OC_UINT4m number_of_stages;

  void GetAppliedField(const Oxs_SimState& state,
		       ThreeVector& H,ThreeVector& dHdt) const;

  // H_work and dHdt_work are set inside ComputeEnergyChunkInitialize
  // for use in immediately succeeding ComputeEnergyChunk (for same
  // state).
  mutable ThreeVector H_work;
  mutable ThreeVector dHdt_work;

  // Supplied outputs, in addition to those provided by Oxs_Energy.
  Oxs_ScalarOutput<Oxs_ScriptUZeeman> Bapp_output;
  Oxs_ScalarOutput<Oxs_ScriptUZeeman> Bappx_output;
  Oxs_ScalarOutput<Oxs_ScriptUZeeman> Bappy_output;
  Oxs_ScalarOutput<Oxs_ScriptUZeeman> Bappz_output;
  void Fill__Bapp_output(const Oxs_SimState& state);

protected:
  virtual void GetEnergy(const Oxs_SimState& state,
			 Oxs_EnergyData& oed) const {
    GetEnergyAlt(state,oed);
  }

  virtual void ComputeEnergy(const Oxs_SimState& state,
                             Oxs_ComputeEnergyData& oced) const {
    ComputeEnergyAlt(state,oced);
  }

  virtual void ComputeEnergyChunkInitialize
  (const Oxs_SimState& state,
   Oxs_ComputeEnergyDataThreaded& ocedt,
   Oc_AlignedVector<Oxs_ComputeEnergyDataThreadedAux>& thread_ocedtaux,
   int number_of_threads) const;

  virtual void ComputeEnergyChunk(const Oxs_SimState& state,
                                  Oxs_ComputeEnergyDataThreaded& ocedt,
                                  Oxs_ComputeEnergyDataThreadedAux& ocedtaux,
                                  OC_INDEX node_start,OC_INDEX node_stop,
                                  int threadnumber) const;

public:
  virtual const char* ClassName() const; // ClassName() is
  /// automatically generated by the OXS_EXT_REGISTER macro.
  Oxs_ScriptUZeeman(const char* name,     // Child instance id
		     Oxs_Director* newdtr, // App director
		     const char* argstr);  // MIF input block parameters
  virtual ~Oxs_ScriptUZeeman();
  virtual OC_BOOL Init();
  virtual void StageRequestCount(unsigned int& min,
				 unsigned int& max) const;
};

#endif // _OXS_SCRIPTUZEEMAN