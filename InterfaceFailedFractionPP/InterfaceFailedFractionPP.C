#include "InterfaceFailedFractionPP.h"

registerMooseObject("SolidMechanicsApp", InterfaceFailedFractionPP);

InputParameters
InterfaceFailedFractionPP::validParams()
{
  InputParameters params = SidePostprocessor::validParams();

  params.addClassDescription(
      "Computes the fraction of interface faces for which every quadrature "
      "point has normal_jump greater than a specified tolerance.");

  params.addRequiredCoupledVar(
      "normal_jump",
      "The generated normal_jump auxiliary variable.");

  params.addParam<Real>(
      "tolerance",
      1e-4,
      "Minimum opening required at every quadrature point on a face.");

  return params;
}

InterfaceFailedFractionPP::InterfaceFailedFractionPP(
    const InputParameters & parameters)
  : SidePostprocessor(parameters),
    _normal_jump(coupledValue("normal_jump")),
    _tolerance(getParam<Real>("tolerance")),
    _open_faces(0.0),
    _total_faces(0.0),
    _fraction(0.0)
{
}

void
InterfaceFailedFractionPP::initialize()
{
  _open_faces = 0.0;
  _total_faces = 0.0;
}

void
InterfaceFailedFractionPP::execute()
{
  Real weighted_jump = 0.0;
  Real total_weight = 0.0;

  for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
  {
    const Real w = _JxW[qp];

    weighted_jump += _normal_jump[qp] * w;
    total_weight += w;
  }

  const Real avg_jump =
      total_weight > 0.0 ? weighted_jump / total_weight : 0.0;

  _total_faces += 1.0;

  if (avg_jump > _tolerance)
    _open_faces += 1.0;
}

void
InterfaceFailedFractionPP::finalize()
{
  gatherSum(_open_faces);
  gatherSum(_total_faces);

  _fraction = (_total_faces > 0.0)
                  ? _open_faces / _total_faces
                  : 0.0;
}

void
InterfaceFailedFractionPP::threadJoin(const UserObject & y)
{
  const auto & pp =
      static_cast<const InterfaceFailedFractionPP &>(y);

  _open_faces += pp._open_faces;
  _total_faces += pp._total_faces;
}

Real
InterfaceFailedFractionPP::getValue() const
{
  return _fraction;
}