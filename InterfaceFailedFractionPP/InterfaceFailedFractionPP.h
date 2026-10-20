#pragma once

#include "SidePostprocessor.h"

class InterfaceFailedFractionPP : public SidePostprocessor
{
public:
  static InputParameters validParams();

  InterfaceFailedFractionPP(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;
  virtual void finalize() override;
  virtual void threadJoin(const UserObject & y) override;
  virtual Real getValue() const override;

protected:
  /// normal_jump auxiliary variable
  const VariableValue & _normal_jump;

  /// opening criterion
  const Real _tolerance;

  /// number of fully-open interfaces
  Real _open_faces;

  /// total number of interfaces
  Real _total_faces;

  /// final fraction
  Real _fraction;
};