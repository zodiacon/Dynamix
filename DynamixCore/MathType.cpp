#include <cmath>
#include <numbers>
#include "MathType.h"
#include "TypeHelper.h"

using namespace Dynamix;

MathType* MathType::Get() {
    static MathType type;
    return &type;
}

MathType::MathType() : StaticObjectType("Math") {
    BEGIN_FIELDS
        FIELD(PI, std::numbers::pi),
        FIELD(E, std::numbers::e),
        FIELD(OneOverPI, std::numbers::inv_pi),
        FIELD(Sqr2, std::numbers::sqrt2),
        FIELD(Sqr3, std::numbers::sqrt3),
        FIELD(Ln2, std::numbers::ln2),
        FIELD(Phi, std::numbers::phi),
        FIELD(Ln10, std::numbers::ln10),
        END_FIELDS

    BEGIN_METHODS(MathType)
        METHOD_STATIC(Sin, 1, return std::sin(args[0].ToReal());),
        METHOD_STATIC(Cos, 1, return std::cos(args[0].ToReal());),
        METHOD_STATIC(Tan, 1, return std::tan(args[0].ToReal());),
        METHOD_STATIC(Sinh, 1, return std::sinh(args[0].ToReal());),
        METHOD_STATIC(Cosh, 1, return std::cosh(args[0].ToReal());),
        METHOD_STATIC(Tanh, 1, return std::tanh(args[0].ToReal());),
        METHOD_STATIC(ASin, 1, return std::asin(args[0].ToReal());),
        METHOD_STATIC(ACos, 1, return std::acos(args[0].ToReal());),
        METHOD_STATIC(ATan, 1, return std::atan(args[0].ToReal());),
        METHOD_STATIC(ATan2, 2, return std::atan2(args[0].ToReal(), args[1].ToReal());),
        METHOD_STATIC(Abs, 1, return args[0].IsInteger() ? std::abs(args[0].AsInteger()) : std::abs(args[0].ToReal());),
        METHOD_STATIC(Exp, 1, return std::exp(args[0].ToReal());),
        METHOD_STATIC(Log, 1, return std::log10(args[0].ToReal());),
        METHOD_STATIC(Ln, 1, return std::log(args[0].ToReal());),
        METHOD_STATIC(Floor, 1, return std::floor(args[0].ToReal());),
        METHOD_STATIC(Trunc, 1, return std::trunc(args[0].ToReal());),
        METHOD_STATIC(Round, 1, return std::round(args[0].ToReal());),
        METHOD_STATIC(ASinh, 1, return std::asinh(args[0].ToReal());),
        METHOD_STATIC(ACosh, 1, return std::acosh(args[0].ToReal());),
        METHOD_STATIC(ATanh, 1, return std::atanh(args[0].ToReal());),
        METHOD_STATIC(Power, 2, return std::pow(args[0].ToReal(), args[1].ToReal());),
        METHOD_STATIC(Sqrt, 1, return std::sqrt(args[0].ToReal());),
        METHOD_STATIC(Beta, 2, return std::beta(args[0].ToReal(), args[1].ToReal());),
        METHOD_STATIC(Gamma, 1, return std::tgamma(args[0].ToReal());),
        METHOD_STATIC(Deg, 1, return args[0].ToReal() * 180 / std::numbers::pi;),
        METHOD_STATIC(Rad, 1, return args[0].ToReal() * std::numbers::pi / 180;),
        END_METHODS()
}
