#pragma once

#include "TMatrix22.h"
#include "TMatrix44.h"
#include "TQuaternion.h"
#include "TRectangle2D.h"
#include "TVector2D.h"
#include "TVector3D.h"

namespace sf
{

/*
 * Implementation of some vector math types using templates.
 */
typedef TVector2D<double> Vector2D;

typedef TRectangle2D<double> Rectangle2D;

typedef TMatrix22<double> Matrix22;

typedef TVector3D<double> Vector3D;

typedef TMatrix44<double> Matrix44;

typedef TQuaternion<double> Quaternion;

}// namespace sf