#include "ComputeRotatedEigenstrain.h"
#include "RotationTensor.h"

registerMooseObject("SolidMechanicsApp", ComputeRotatedEigenstrain);

InputParameters
ComputeRotatedEigenstrain::validParams()
{
  InputParameters params = ComputeEigenstrainBase::validParams();
  params.addClassDescription("Computes a crystal eigenstrain rotated into the global simulation "
                             "frame using Euler angles read from PropertyReadFile.");
  
  // Require the user to pass the PropertyReadFile UserObject name
  params.addRequiredParam<UserObjectName>("prop_read_user_object", 
                                          "The PropertyReadFile UserObject reading the data.");
  
  // Coupled temperature variable
  params.addRequiredCoupledVar("temperature", "Coupled temperature variable.");
  
  // Local anisotropic eigenstrain inputs
  params.addParam<Real>("eigenstrain_x", 0.0, "Eigenstrain component along local x-axis.");
  params.addParam<Real>("eigenstrain_y", 0.0, "Eigenstrain component along local y-axis.");
  params.addParam<Real>("eigenstrain_z", 0.0, "Eigenstrain component along local z-axis.");
  
  return params;
}

ComputeRotatedEigenstrain::ComputeRotatedEigenstrain(const InputParameters & parameters)
  : ComputeEigenstrainBase(parameters),
    _prop_reader(getUserObject<PropertyReadFile>("prop_read_user_object")),
    _temperature(coupledValue("temperature")),
    _eigenstrain_x(getParam<Real>("eigenstrain_x")),
    _eigenstrain_y(getParam<Real>("eigenstrain_y")),
    _eigenstrain_z(getParam<Real>("eigenstrain_z"))
{
}

void
ComputeRotatedEigenstrain::computeQpEigenstrain()
{
  // 1. Get the current element pointer
  const libMesh::Elem * elem = _current_elem;

  // 2. Extract the 3 Euler angles (phi1, Phi, phi2) for this specific element.
  // Assumes your input data file lists phi1 as property 0, Phi as property 1, phi2 as property 2.
  Real phi1 = _prop_reader.getData(elem, 0);
  Real Phi  = _prop_reader.getData(elem, 1);
  Real phi2 = _prop_reader.getData(elem, 2);

  // 3. Create Euler angles vector and RotationTensor.
  // Note: MOOSE's RotationTensor typically expects angles in DEGREES.
  RealVectorValue euler_angles(phi1, Phi, phi2);
  RotationTensor rotation_tensor(euler_angles);

  // 4. Get the temperature at this quadrature point
  Real temp = _temperature[_qp];

  // 5. Construct local crystal frame eigenstrain tensor (assumed diagonal/orthotropic)
  //    Multiply eigenstrain components by temperature
  RankTwoTensor local_eigenstrain;
  local_eigenstrain(0, 0) = _eigenstrain_x * temp;
  local_eigenstrain(1, 1) = _eigenstrain_y * temp;
  local_eigenstrain(2, 2) = _eigenstrain_z * temp;

  // 6. Rotate tensor into global system frame: epsilon_global = R * epsilon_local * R^T
  _eigenstrain[_qp] = local_eigenstrain;
  _eigenstrain[_qp].rotate(rotation_tensor.transpose());
}
