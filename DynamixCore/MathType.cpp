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

    auto constexpr flags = SymbolFlags::Static | SymbolFlags::Native;
    BEGIN_METHODS(MathType)
        METHOD_EX(Sin, 1, flags, return std::sin(args[0].ToReal());),
        METHOD_EX(Cos, 1, flags, return std::cos(args[0].ToReal());),
        METHOD_EX(Tan, 1, flags, return std::tan(args[0].ToReal());),
        METHOD_EX(Sinh, 1, flags, return std::sinh(args[0].ToReal());),
        METHOD_EX(Cosh, 1, flags, return std::cosh(args[0].ToReal());),
        METHOD_EX(Tanh, 1, flags, return std::tanh(args[0].ToReal());),
        METHOD_EX(ASin, 1, flags, return std::asin(args[0].ToReal());),
        METHOD_EX(ACos, 1, flags, return std::acos(args[0].ToReal());),
        METHOD_EX(ATan, 1, flags, return std::atan(args[0].ToReal());),
        METHOD_EX(ATan2, 2, flags, return std::atan2(args[0].ToReal(), args[1].ToReal());),
        METHOD_EX(Abs, 1, flags, return args[0].IsInteger() ? std::abs(args[0].AsInteger()) : std::abs(args[0].ToReal());),
        METHOD_EX(Exp, 1, flags, return std::exp(args[0].ToReal());),
        METHOD_EX(Log, 1, flags, return std::log10(args[0].ToReal());),
        METHOD_EX(Ln, 1, flags, return std::log(args[0].ToReal());),
        METHOD_EX(Floor, 1, flags, return std::floor(args[0].ToReal());),
        METHOD_EX(Trunc, 1, flags, return std::floor(args[0].ToReal());),
        METHOD_EX(Rounf, 1, flags, return std::round(args[0].ToReal());),
        METHOD_EX(ASinh, 1, flags, return std::asinh(args[0].ToReal());),
        METHOD_EX(ACosh, 1, flags, return std::acosh(args[0].ToReal());),
        METHOD_EX(ATanh, 1, flags, return std::atanh(args[0].ToReal());),
        METHOD_EX(Power, 2, flags, return std::pow(args[0].ToReal(), args[1].ToReal());),
        METHOD_EX(Sqrt, 1, flags, return std::sqrt(args[0].ToReal());),
        METHOD_EX(Beta, 2, flags, return std::beta(args[0].ToReal(), args[1].ToReal());),
        METHOD_EX(Gamma, 1, flags, return std::tgamma(args[0].ToReal());),
        END_METHODS()
}
