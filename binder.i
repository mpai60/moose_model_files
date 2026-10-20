[Mesh]

  construct_side_list_from_node_list=true

  [tess]
    type = FileMeshGenerator
    file = 'uniform_recti_original.msh'
  []

  [right_boundary]
    type = BoundingBoxNodeSetGenerator
    input = tess
    new_boundary = 1
    bottom_left = '0.9999 -0.2 0'
    top_right = '1.2 1.2 0'
  []

  [left_boundary]
    type = BoundingBoxNodeSetGenerator
    input = right_boundary
    new_boundary = 2
    bottom_left = '-0.2 -0.2 0'
    top_right = '0.0001 1.2 0'
  []


  [top_boundary]
    type = BoundingBoxNodeSetGenerator
    input = left_boundary
    new_boundary = 3
    bottom_left = '-0.2 0.999999 0'
    top_right = '1.2 1.2 0'
  []

  [bottom_boundary]
    type = BoundingBoxNodeSetGenerator
    input = top_boundary
    new_boundary = 4
    bottom_left = '-0.2 -0.2 0'
    top_right = '1.2 0.00015 0'
  []



  [bottom_right]
    type = BoundingBoxNodeSetGenerator
    input = bottom_boundary
    new_boundary = 5
    bottom_left = '0.999999 -0.2 0'
    top_right = '1.2 0.0000001 0'
  []

  [top_left]
    type = BoundingBoxNodeSetGenerator
    input = bottom_right
    new_boundary = 6
    bottom_left = '-0.2 0.999999 0'
    top_right = '0.000001 1.2 0'
  []
  
  [anchor_point]
    type = ExtraNodesetGenerator
    input = top_left
    new_boundary = 7
    nodes = 47
  []
  [breakmesh]
    type = BreakMeshByBlockGenerator
    input = anchor_point
    split_interface = true
  []


[]

[UserObjects]
  [euler_angles]
    type = PropertyReadFile
    nprop = 3
    prop_file_name = 'euler_ang_file.txt'
    read_type = 'block'
    nblock = 100
    use_zero_based_block_indexing = false
  []

[]


##################################################################################
[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Variables]
  [disp_x]
    order = FIRST
    family = LAGRANGE
  []
  [disp_y]
    order = FIRST
    family = LAGRANGE
  []
  [global_strain]
    order = THIRD
    family = SCALAR
  []
[]

[AuxVariables]
  [czm_damage]
    order = CONSTANT
    family = MONOMIAL
  []
  [eta0]
    order = CONSTANT
    family = MONOMIAL
  []
  [eta1]
    order = CONSTANT
    family = MONOMIAL
  []
  [eta2]
    order = CONSTANT
    family = MONOMIAL
  []
  [temp_1]
    order = CONSTANT
    family = MONOMIAL
  []
  [temp_2]
    order = CONSTANT
    family = MONOMIAL
  []
  [irr]
    order = CONSTANT
    family = MONOMIAL
  []
  [time]
    order = CONSTANT
    family = MONOMIAL
  []
  [m_ec]
    order = CONSTANT
    family = MONOMIAL
  []
  [m_ea]
    order = CONSTANT
    family = MONOMIAL
  []
  [m_vc]
    order = CONSTANT
    family = MONOMIAL
  []
  [CTE_0]
    order = CONSTANT
    family = MONOMIAL
  []
  [saved_x]
  []
  [saved_y]
  []

[]

###################################################################################
[Functions]

  [temp_1_def]
    type = ParsedFunction
    # expression = 'if(t < 1, -2000*t, if(t < 2, -2000 + 550*(t-1), -1450))' 
    expression = 'if(t < 1, -2000*t, -2000)' 
  []
  [temp_2_def]
    type = ConstantFunction
    value = 298
  []
  [irr_def]
    type = ParsedFunction
    expression = 'if(t > 1.0, 20*(t-1),0)'
  []
  [dt_def]
    type = ParsedFunction
    expression = 'if(t < 1, 0.1, 0.01)'
    #expression = 'if(t >= 1, if(t <= 1.5, 0.01, 0.1), 0.1)'
  []
  [time_def]
    type = ParsedFunction
    expression = '1*t'
  []
  [m_ec_def]
    type = ConstantFunction
    value = -0.01
  []
  [m_ea_def]
    type = ConstantFunction
    value = 0.05
  []
  [m_vc_def]
    type = ConstantFunction
    value = -0.01
  []
  [CTE_0_def]
    type = ConstantFunction
    value = 1.3e-5 #1.3e-5
  []
[]



###################################################################################
[Physics]

  [SolidMechanics]

    [QuasiStatic]
      [all]
        eigenstrain_names = 'thermal_strain_1 irr_strain creep_strain' #
        add_variables = true
        global_strain = global_strain
        generate_output = 'stress_xx stress_yy stress_xy stress_xz' 
        save_in = 'saved_x saved_y'
        strain = SMALL
      []
    []
    
    [CohesiveZone]
      [all]
        boundary = ${pair_boundaries} 
        strain = SMALL
        generate_output = 'normal_jump'
      []
    []  

    [GlobalStrain]
      [global_strain]
        scalar_global_strain = global_strain
        displacements = 'disp_x disp_y'
        auxiliary_displacements = 'gs_ux gs_uy'
        global_displacements = 'ug_x ug_y'
        applied_stress_tensor = '0 0 0 0 0 0 0 0 0'
      []
    []
  []
[]

###################################################################################



[AuxKernels]
  [temp1funcaux]
    type = FunctionAux
    variable = temp_1
    function = temp_1_def
    use_displaced_mesh = false
  []
  [temp2funcaux]
    type = FunctionAux
    variable = temp_2
    function = temp_2_def
    use_displaced_mesh = false
  []
  [timefuncaux]
    type = FunctionAux
    variable = time
    function = time_def
    use_displaced_mesh = false
  []
  [irrfuncaux]
    type = FunctionAux
    variable = irr
    function = irr_def
    use_displaced_mesh = false
  []
  [cte0funcaux]
    type = FunctionAux
    variable = CTE_0
    function = CTE_0_def
    use_displaced_mesh = false
  []
  [mvcfuncaux]
    type = FunctionAux
    variable = m_vc
    function = m_vc_def
    use_displaced_mesh = false
  []
  [mecfuncaux]
    type = FunctionAux
    variable = m_ec
    function = m_ec_def
    use_displaced_mesh = false
  []
  [meafuncaux]
    type = FunctionAux
    variable = m_ea
    function = m_ea_def
    use_displaced_mesh = false
  []
  [czm_damage_aux]
    type = MaterialRealAux
    variable = czm_damage
    property = damage
    boundary = '200' #${pair_boundaries}
    execute_on = 'timestep_end'
  []

[]

###################################################################################
[BCs]

  [Periodic]
    [all]
      variable = 'disp_x disp_y'
      auto_direction = 'x y'
    []
  []
  [anchor_point_y1]
    type = DirichletBC
    boundary = 7
    variable = disp_y
    value = 0.
  []  
  [anchor_point_x1]
    type = DirichletBC
    boundary = 7
    variable = disp_x
    value = 0.
  []  

[]



###################################################################################
[Materials]

  [E11]
    type = DerivativeParsedMaterial
    property_name = 'E11'
    coupled_variables = 'm_ea irr'
    constant_names = 'ea_0'
    constant_expressions = '1.095e12'
    expression = '(1+(m_ea*irr))*ea_0'
  []
  [E22]
    type = DerivativeParsedMaterial
    property_name = 'E22'
    coupled_variables = 'm_ec irr'
    constant_names = 'ec_0'
    constant_expressions = '3.65e10' 
    expression = '(1+(m_ec*irr))*ec_0'
  []
  [G12]
    type = DerivativeParsedMaterial
    property_name = 'G12'  
    constant_names = 'm'
    constant_expressions = '15e9'
    expression = 'm'
  []
  [G23]
    type = DerivativeParsedMaterial
    property_name = 'G23' 
    constant_names = 'm'
    constant_expressions = '180e9'
    expression = 'm'
  []
  [G31]
    type = DerivativeParsedMaterial
    property_name = 'G31'
    constant_names = 'm'
    constant_expressions = '15e9'
    expression = 'm'
  []
  [nu12]
    type = DerivativeParsedMaterial
    property_name = 'nu12'
    coupled_variables = 'm_vc m_ea m_ec irr'
    constant_names = 'vc_0 ea_0 ec_0'
    constant_expressions = '0.3 1.095e12 3.65e10'
    expression = '((1+(m_ec*irr))*ec_0)*((1+(m_vc*irr))*vc_0)/((1+(m_ea*irr))*ea_0)'
  []
  [nu23]
    type = DerivativeParsedMaterial
    property_name = 'nu23'
    coupled_variables = 'm_vc irr'
    constant_names = 'vc_0'
    constant_expressions = '0.3'
    expression = '(1+(m_vc*irr))*vc_0'
  []
  [thermal_strain_1]
    type = ComputeRotatedEigenstrain
    prop_read_user_object = euler_angles
    eigenstrain_x = 0 
    eigenstrain_y = 3e-5 
    eigenstrain_z = 0.0
    temperature = temp_1
    eigenstrain_name = thermal_strain_1
  []
  [irr_strain]
    type = ComputeRotatedEigenstrain
    prop_read_user_object = euler_angles
    eigenstrain_x =  -0.002 
    eigenstrain_y =  0.004
    eigenstrain_z = 0.0
    temperature = irr
    eigenstrain_name = irr_strain
  []
  [elasticity_tensor]
    type = ComputeVariableOrthotropicElasticityTensorCP
    E11 = 'E11'
    E22 = 'E22'
    E33 = 'E11'
    G12 = 'G12'
    G23 = 'G23'
    G31 = 'G31'
    nu12 = 'nu12'
    nu23 = 'nu23'
    nu31 = 'nu12'
    C_ijkl = '0 0 0 0 0 0 0 0 0 0 0 0'
    read_prop_user_object = euler_angles
  []
  [czm]
    type = BiLinearMixedModeTraction
    boundary = ${pair_boundaries} 
    penalty_stiffness = 1e13
    GI_c = 250
    GII_c = 250
    eta = 2.2
    normal_strength = 250e6
    shear_strength = 250e6
    displacements = 'disp_x disp_y'
    viscosity = 1e-6 
    lag_mode_mixity = false
    lag_displacement_jump = false
  []
  [irradiation_creep]
    type = GraphiteIrradiationCreepEigenstrain
    fluence = irr
    U0 = 8.5
    C55 = 4.5e9
    C66 = 4.5e9
    eigenstrain_name = creep_strain
    prop_read_user_object = euler_angles
    # stress_reset_time = 2.025
    # stress_reset_tol = 1e-6
  []
  [stress]
    type = ComputeLinearElasticStress
  []
[]


###################################################################################

[Preconditioning]
  [prec1]
    type = SMP
    full = true
  []
[]



[Executioner]
  [Predictor]
    type = SimplePredictor
    scale = 1
  []
  type = Transient
  solve_type = 'NEWTON'
  nl_max_its = 100
  nl_abs_tol = 5e-5 
  line_search = bt
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  automatic_scaling = true
  [TimeStepper]
    type = FunctionDT
    function = dt_def
  []
  # dt = 0.1
  end_time = 2
[]

[Postprocessors]
  [max_czm_damage]
    type = SideExtremeValue
    variable = czm_damage
    boundary = 200
    value_type = max
  []
  [czm_gap_volume]
    type = SideIntegralVariablePostprocessor
    boundary = ${pair_boundaries}
    variable = normal_jump
    use_displaced_mesh = true
    execute_on = 'initial timestep_end'
  []
  [block_area]
    type = VolumePostprocessor
    block = ${blocks}
    use_displaced_mesh = true
    execute_on = 'initial timestep_end'
  []
  [temp_1]
    type = ElementalVariableValue
    variable = temp_1
    elementid = 736
  []
  [irr]
    type = ElementalVariableValue
    variable = irr
    elementid = 736
  []
  [avg_normal_xx]
    type = ElementAverageMaterialProperty
    mat_prop = stress_xx
  []
  [avg_normal_yy]
    type = ElementAverageMaterialProperty
    mat_prop = stress_yy
  []
    
  [avg_shear_xy]
    type = ElementAverageMaterialProperty
    mat_prop = stress_xy
  []
  [max_shear_xy]
    type = ElementExtremeMaterialProperty
    mat_prop = stress_xy
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [max_shear_xz]
    type = ElementExtremeMaterialProperty
    mat_prop = stress_xz
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [czm_ratio]
    type = InterfaceFailedFractionPP
    boundary = ${pair_boundaries}
    normal_jump = normal_jump
    tolerance = 1e-3
    # jump_property = 'normal_jump'
    # execute_on = 'timestep_end'
  []
[]

[Outputs]
  exodus = true
  csv = true
  print_linear_residuals = true
  [out_log]
    type = Console
    output_file = true
  []
[]


 





