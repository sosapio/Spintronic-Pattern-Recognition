/* FILE: ellipsoidatlas.h          -*-Mode: c++-*-
 *
 * Atlas class derived from Oxs_Atlas class for ellipsoidal regions.
 */

#ifndef _OXS_ELLIPSOIDATLAS
#define _OXS_ELLIPSOIDATLAS

#include <string>
#include <vector>

#include "threevector.h"
#include "util.h"
#include "atlas.h"

OC_USE_STRING;

/* End includes */

class Oxs_EllipsoidAtlas:public Oxs_Atlas {
private:
  vector<String> region_name_list;
  OC_INDEX interior_id;
  OC_INDEX exterior_id;
  Oxs_Box ellipsoid;
  Oxs_Box world;
  ThreeVector centerpt;
  ThreeVector invaxes;
public:
  virtual const char* ClassName() const; // ClassName() is
  /// automatically generated by the OXS_EXT_REGISTER macro.

  Oxs_EllipsoidAtlas(const char* name,
		     Oxs_Director* newdtr,
		     const char* argstr);   // MIF block argument string

  ~Oxs_EllipsoidAtlas() {}

  void GetWorldExtents(Oxs_Box &mybox) const { mybox = world; }
  /// Fills mybox with bounding box for the atlas.

  OC_BOOL GetRegionExtents(OC_INDEX id,Oxs_Box &mybox) const;
  /// If id is 0 or 1, sets mybox to world and returns 1.
  /// If id > 1, leaves mybox untouched and returns 0.

  OC_INDEX GetRegionId(const ThreeVector& point) const;
  /// Returns the id number for the region containing point.
  /// The return value is 0 if the point is not contained in
  /// the atlas, i.e., belongs to the "universe" region.

  OC_INDEX GetRegionId(const String& name) const;
  /// Given a region id string (name), returns
  /// the corresponding region id index.  If
  /// "name" is not included in the atlas, then
  /// -1 is returned.  Note: If name == "universe",
  /// then the return value will be 0.

  OC_BOOL GetRegionName(OC_INDEX id,String& name) const;
  /// Given an id number, fills in "name" with
  /// the corresponding region id string.  Returns
  /// 1 on success, 0 if id is invalid.  If id is 0,
  /// then name is set to "universe", and the return
  /// value is 1.

  OC_INDEX GetRegionCount() const {
    return static_cast<OC_INDEX>(region_name_list.size());
    // In principle, there are three regions, the interior of ellipsoid,
    // the exterior of the ellipsoid, and the "universe". However, if
    // either (or both) of the regions are associated with "universe"
    // then the region count is reduced accordingly.
  }
};

#endif // _OXS_ELLIPSOIDATLAS
