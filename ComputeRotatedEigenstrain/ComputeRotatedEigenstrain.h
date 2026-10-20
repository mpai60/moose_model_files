#pragma once

#include "ComputeEigenstrainBase.h"
#include "PropertyReadFile.h"
#include "RankTwoTensor.h"
#include "RotationTensor.h"


class ComputeRotatedEigenstrain : public ComputeEigenstrainBase
{
public:
  static InputParameters validParams();
  ComputeRotatedEigenstrain(const InputParameters & parameters);

protected:
  virtual void computeQpEigenstrain() override;

  /// Pointer to the PropertyReadFile UserObject
  const PropertyReadFile & _prop_reader;

  /// Temperature coupled variable
  const VariableValue & _temperature;

  /// Local crystal eigenstrain components (e.g., thermal expansion or phase change)
  Real _eigenstrain_x;
  Real _eigenstrain_y;
  Real _eigenstrain_z;
};
