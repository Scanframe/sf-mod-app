#pragma once

#include "TMatrix2D.h"
#include "TRectangle2D.h"
#include "TVector2D.h"
/*
#include "TMatrix3D.h"
#include "TVector3D.h"
#include "TVector4D.h"
#include "Quaterion.h"
*/

namespace sf
{

/*
 * Implementation of some vector math types using templates.
 */
typedef TVector2D<double> Vector2D;
typedef TRectangle2D<double> Rectangle2D;
typedef TMatrix2D<double> Matrix2D;

/*
typedef TVector3D<double, double> Vector3;
typedef TVector4D<double, double> Vector4;
typedef TMatrix3D<double, double> Matrix3D;
typedef TQuaterion<double, double> Quaterion;
*/

}// namespace sf