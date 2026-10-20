#pragma once

#include "ComputeEigenstrainBase.h"

class GraphiteIrradiationCreepEigenstrain : public ComputeEigenstrainBase
{
public:
  static InputParameters validParams();

  GraphiteIrradiationCreepEigenstrain(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpEigenstrain() override;

  const MaterialProperty<RankTwoTensor> & _stress_old;

  const PropertyReadFile & _prop_reader;

  const VariableValue & _fluence;
  const VariableValue & _fluence_old;

  MaterialProperty<RankTwoTensor> & _creep_eigenstrain;
  const MaterialProperty<RankTwoTensor> & _creep_eigenstrain_old;

  const Real _U0;
  const Real _C55;
  const Real _C66;

  /// diagnostic stress-reset time
  const Real _stress_reset_time;

  /// tolerance for triggering reset
  const Real _stress_reset_tol;

  /// true after reset is applied
  bool _stress_reset_done;
};