#pragma once

#include "TMatrix2D.h"
#include "TRect2D.h"
#include "TVector2D.h"
/*
#include "TMatrix44.h"
#include "TVector3.h"
#include "TVector4.h"
#include "Quat.h"
*/

namespace sf
{

/*
 * Implementation of some vector math types using templates.
 */
typedef TVector2D<double> Vector2D;
typedef TRect2D<double> Rect2D;
typedef TMatrix2D<double> Matrix2D;

/*
typedef TVector3D<double, double> Vector3;
typedef TVector4D<double, double> Vector4;
typedef TMatrix44D<double, double> Matrix44;
typedef TQuat<double, double> Quat;
*/

}// namespace sf