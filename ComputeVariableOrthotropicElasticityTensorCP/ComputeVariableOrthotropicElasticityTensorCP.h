//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ComputeElasticityTensor.h"
#include "PropertyReadFile.h"
#include "RankTwoTensor.h"
#include "RotationTensor.h"

/**
 * ComputeElasticityTensorCP defines an elasticity tensor material
 * object for crystal plasticity models. This class accepts either Bunge Euler
 * angles or a 3x3 rotation matrix, from which the 'passive' rotation matrix
 * is generated. This rotation matrix is used by the crystal plasticity models
 * to rotate the crystal slip system direction and plane normals into the
 * user-specified orientation.
 */
class ComputeVariableOrthotropicElasticityTensorCP : public ComputeElasticityTensor
{
public:
  static InputParameters validParams();

  ComputeVariableOrthotropicElasticityTensorCP(const InputParameters & parameters);

protected:
  /**
   * Defines the constant rotation matrix from the user specified
   * Bunge Euler Angles or user-supplied rotation matrix.
   */
  virtual void computeQpElasticityTensor() override;
  //virtual void initQpStatefulProperties() override;
  virtual void assignEulerAngles();

  const MaterialProperty<Real> & _E11;
  const MaterialProperty<Real> & _E22;
  const MaterialProperty<Real> & _E33;
  
  const MaterialProperty<Real> & _G12;
  const MaterialProperty<Real> & _G23;
  const MaterialProperty<Real> & _G31;

  const MaterialProperty<Real> & _nu12;
  const MaterialProperty<Real> & _nu23;
  const MaterialProperty<Real> & _nu31;

  ///Element property read user object used to read in Euler angles
  const PropertyReadFile * const _read_prop_user_object;

  /// Material property that stores the values of the Euler Angles for postprocessing
  MaterialProperty<RealVectorValue> & _Euler_angles_mat_prop;

  /// Crystal Rotation Matrix used to rotate the slip system direction and normal
  MaterialProperty<RankTwoTensor> & _crysrot;

  /// vector to store the stiffness values
  std::vector<Real> _ortho_stiffness_components ; 
  ///
  RankFourTensor _ortho_Cijkl ;

  /// Rotation matrix
  RotationTensor _R;

  /// flag for user-defined rotation matrix, supplied in input file
  bool _user_provided_rotation_matrix;

  // The coupled Euler angles component variables
  unsigned int _n_euler_angle_vars;
  const std::vector<const VariableValue *> _euler_angle_vars;
};
