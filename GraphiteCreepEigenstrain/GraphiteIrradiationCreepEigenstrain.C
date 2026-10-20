#include "GraphiteIrradiationCreepEigenstrain.h"

registerMooseObject(
    "SolidMechanicsApp",
    GraphiteIrradiationCreepEigenstrain);

InputParameters
GraphiteIrradiationCreepEigenstrain::validParams()
{
  InputParameters params = ComputeEigenstrainBase::validParams();

  params.addParam<Real>(
    "stress_reset_time",
    -1.0,
    "Time at which a one-time stress reset is applied");

params.addParam<Real>(
    "stress_reset_tol",
    1e-12,
    "Tolerance for triggering stress reset");

  params.addClassDescription(
      "Irradiation creep eigenstrain for graphite");


  params.addRequiredCoupledVar(
      "fluence",
      "Irradiation fluence");

  params.addRequiredParam<Real>(
      "U0",
      "Reference fluence");

  params.addParam<Real>(
      "C55",
      4.5e9,
      "Crystal shear modulus C55");

  params.addParam<Real>(
      "C66",
      4.5e9,
      "Crystal shear modulus C66");

  params.addRequiredParam<UserObjectName>(
        "prop_read_user_object",
        "PropertyReadFile user object containing Euler angles");

  return params;
}

GraphiteIrradiationCreepEigenstrain::
GraphiteIrradiationCreepEigenstrain(
    const InputParameters & parameters)
  : ComputeEigenstrainBase(parameters),

    _stress_old(
        getMaterialPropertyOld<RankTwoTensor>("stress")),

    _prop_reader(
        getUserObject<PropertyReadFile>(
            "prop_read_user_object")),

    _fluence(
        coupledValue("fluence")),

    _fluence_old(
        coupledValueOld("fluence")),

    _creep_eigenstrain(
        declareProperty<RankTwoTensor>(
            _base_name + "creep_eigenstrain")),

    _creep_eigenstrain_old(
        getMaterialPropertyOld<RankTwoTensor>(
            _base_name + "creep_eigenstrain")),

    _U0(
        getParam<Real>("U0")),

    _C55(
        getParam<Real>("C55")),

    _C66(
        getParam<Real>("C66")),

    _stress_reset_time(
        getParam<Real>("stress_reset_time")),

    _stress_reset_tol(
        getParam<Real>("stress_reset_tol")),

    _stress_reset_done(false)
{
}

void
GraphiteIrradiationCreepEigenstrain::
initQpStatefulProperties()
{
  _creep_eigenstrain[_qp].zero();
}

void
GraphiteIrradiationCreepEigenstrain::
computeQpEigenstrain()
{
  const Real dU =
      _fluence[_qp] - _fluence_old[_qp];

  const libMesh::Elem * elem = _current_elem;

  Real phi1 = _prop_reader.getData(elem, 0);
  Real Phi  = _prop_reader.getData(elem, 1);
  Real phi2 = _prop_reader.getData(elem, 2);

  RealVectorValue euler_angles(phi1, Phi, phi2);

    RotationTensor rotation_tensor(euler_angles);

    //
    // Diagnostic stress reset
    //
    if (std::abs(_t - _stress_reset_time) < _stress_reset_tol)
    {
        mooseWarning("STRESS RESET TRIGGERED");
    }

    if (!_stress_reset_done &&
        _stress_reset_time > 0.0 &&
        std::abs(_t - _stress_reset_time) < _stress_reset_tol)
    {
    RankTwoTensor stress_reset_strain;
    stress_reset_strain.zero();

    RankTwoTensor sigma_crystal = _stress_old[_qp];
    sigma_crystal.rotate(rotation_tensor);

    // normal components
    stress_reset_strain(0,0) = sigma_crystal(0,0) / _C55;
    stress_reset_strain(1,1) = sigma_crystal(1,1) / _C55;
    stress_reset_strain(2,2) = sigma_crystal(2,2) / _C55;

    // shear components
    stress_reset_strain(0,1) = sigma_crystal(0,1) / (2.0 * _C55);
    stress_reset_strain(1,0) = stress_reset_strain(0,1);

    stress_reset_strain(0,2) = sigma_crystal(0,2) / (2.0 * _C66);
    stress_reset_strain(2,0) = stress_reset_strain(0,2);

    stress_reset_strain(1,2) = sigma_crystal(1,2) / (2.0 * _C66);
    stress_reset_strain(2,1) = stress_reset_strain(1,2);

    RankTwoTensor stress_reset_global = stress_reset_strain;
    stress_reset_global.rotate(rotation_tensor.transpose());

    _creep_eigenstrain[_qp] =
        _creep_eigenstrain_old[_qp] +
        stress_reset_global;

    _eigenstrain[_qp] =
        _creep_eigenstrain[_qp];

    if (_qp == 0)
        _stress_reset_done = true;

    return;
    }

  if (std::abs(dU) < std::numeric_limits<Real>::epsilon())
  {
    _creep_eigenstrain[_qp] =
        _creep_eigenstrain_old[_qp];

    _eigenstrain[_qp] =
        _creep_eigenstrain[_qp];

    return;
  }

  RankTwoTensor sigma_crystal = _stress_old[_qp];
    sigma_crystal.rotate(rotation_tensor);

  const Real sigma12 =
      sigma_crystal(0,1);

  const Real sigma13 =
      sigma_crystal(0,2);

  const Real gamma12 =
      dU / _U0 *
      sigma12 / _C55;

  const Real gamma13 =
      dU / _U0 *
      sigma13 / _C66;

  RankTwoTensor creep_increment_crystal;
  creep_increment_crystal.zero();

  /*
   * engineering shear strain
   * gamma = 2 epsilon
   */

  creep_increment_crystal(0,1) =
      0.5 * gamma12;

  creep_increment_crystal(1,0) =
      0.5 * gamma12;

  creep_increment_crystal(0,2) =
      0.5 * gamma13;

  creep_increment_crystal(2,0) =
      0.5 * gamma13;

  RankTwoTensor creep_increment_global = creep_increment_crystal;
  creep_increment_global.rotate(rotation_tensor.transpose());

  _creep_eigenstrain[_qp] =
      _creep_eigenstrain_old[_qp]
      + creep_increment_global;

  _eigenstrain[_qp] =
      _creep_eigenstrain[_qp];
}