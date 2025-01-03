/* FILE: uniaxialanisotropy.h            -*-Mode: c++-*-
 *
 * Uniaxial Anisotropy, derived from Oxs_Energy class.
 *
 */

#ifndef _OXS_UNIAXIALANISOTROPY
#define _OXS_UNIAXIALANISOTROPY

#include "nb.h"
#include "threevector.h"
#include "util.h"
#include "chunkenergy.h"
#include "energy.h"
#include "key.h"
#include "simstate.h"
#include "mesh.h"
#include "meshvalue.h"
#include "oxsthread.h"
#include "scalarfield.h"
#include "vectorfield.h"

/* End includes */

class Oxs_UniaxialAnisotropy
  : public Oxs_ChunkEnergy, public Oxs_EnergyPreconditionerSupport {
private:
  enum AnisotropyCoefType {
    ANIS_UNKNOWN, K1_TYPE, Ha_TYPE
  } aniscoeftype;

  Oxs_OwnedPointer<Oxs_ScalarField> K1_init;
  Oxs_OwnedPointer<Oxs_ScalarField> Ha_init;
  Oxs_OwnedPointer<Oxs_VectorField> axis_init;
  mutable OC_UINT4m mesh_id;
  mutable Oxs_MeshValue<OC_REAL8m> K1;
  mutable Oxs_MeshValue<OC_REAL8m> Ha;
  mutable Oxs_MeshValue<ThreeVector> axis;
  /// K1, Ha, and axis are cached values filled by corresponding
  /// *_init members when a change in mesh is detected.

  mutable OC_REAL8m max_K1;  // Max K1 magnitude. Used for energy
                             // density error estimate.

  // It is not uncommon for the anisotropy to be specified by uniform
  // fields.  In this case, memory traffic can be significantly
  // reduced, which may be helpful in parallelized code.  The
  // variables uniform_K1/Ha/axis_value are valid iff the corresponding
  // boolean is true.
  OC_BOOL K1_is_uniform;
  OC_BOOL Ha_is_uniform;
  OC_BOOL axis_is_uniform;
  OC_REAL8m uniform_K1_value;
  OC_REAL8m uniform_Ha_value;
  ThreeVector uniform_axis_value;

  enum IntegrationMethod {
    UNKNOWN_INTEG, RECT_INTEG, QUAD_INTEG
  } integration_method;
  /// Integration formulation to use.  "unknown" is invalid; it
  /// is defined for error detection.

  // RectIntegEnergy is a helper function for ComputeEnergyChunk;
  // it computes using "RECT_INTEG" method.
  void RectIntegEnergy(const Oxs_SimState& state,
                       Oxs_ComputeEnergyDataThreaded& ocedt,
                       Oxs_ComputeEnergyDataThreadedAux& ocedtaux,
                       OC_INDEX node_start,OC_INDEX node_stop) const;


  OC_BOOL has_multscript;
  vector<Nb_TclCommandLineOption> command_options;
  Nb_TclCommand cmd;
  OC_UINT4m number_of_stages;

  // Variables to track and store multiplier value for each simulation
  // state.  This data is computed once per state by the main thread,
  // and shared with all the children.
  mutable OC_UINT4m mult_state_id;
  mutable OC_REAL8m mult;
  mutable OC_REAL8m dmult; // Partial derivative of multiplier wrt t

  void GetMultiplier(const Oxs_SimState& state,
                     OC_REAL8m& mult,
                     OC_REAL8m& dmult) const;


  // C++11 provides std::fma() that computes fused-multiply-add with
  // correct (i.e., single) rounding. This is very useful for building
  // numerically finicky code like double-double multiplication
  // (cf. oommf/pkg/xp/doubledouble.cc), but can be painfully slow if
  // the compiler target doesn't have hardware support for fma. If
  // precision is not an issue, then you'll likely get faster binaries
  // by simply expanding out fma(a,b,c) as a*b+c and letting the
  // compiler decide when and where to insert fma's in the executable.
  //   The following inline function is a transparent wrapper around an
  // expanded fma. It is provided and used in the class to make it easy
  // to compare the speed vs. precission trade-offs of using the
  // expanded form vs. std::fma; just change the "a*b+c" to
  // std::fma(a,b,c) to test.
  inline static OC_REAL8m FMA_Block(OC_REAL8m a,OC_REAL8m b,OC_REAL8m c) {
    return a*b+c;
  }

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
  Oxs_UniaxialAnisotropy(const char* name,  // Child instance id
			 Oxs_Director* newdtr, // App director
			 const char* argstr);  // MIF input block parameters
  virtual ~Oxs_UniaxialAnisotropy() {}
  virtual OC_BOOL Init();
  virtual void StageRequestCount(unsigned int& min,
				 unsigned int& max) const;

  // Optional interface for conjugate-gradient evolver.
  virtual int IncrementPreconditioner(PreconditionerData& pcd);
};

#endif // _OXS_UNIAXIALANISOTROPY
