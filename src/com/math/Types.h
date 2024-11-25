#pragma once
#include <math/TMatrix22.h>
#include <math/TMatrix44.h>
#include <math/TQuaternion.h>
#include <math/TRectangle2D.h>
#include <math/TVector2D.h>
#include <math/TVector3D.h>

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
