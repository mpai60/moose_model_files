# moose_model_files
Custom function files and input file for initial graphite turnaround model in INL MOOSE.

This is a very preliminary version of my model. My goal is to eventually develop my own MOOSE application that will eventually be uploaded here.

For this initial work, the best way to run it is to install MOOSE and then insert the custom function files in the correct locations.

1. Install MOOSE: https://mooseframework.inl.gov/getting_started/installation/
2. Once that is running, these are the custom functions. For each directory, the header goes in the /include and the source file in /src.
3. Under /moose/modules/solid_mechanics/[src or header]/materials  
   a. ComputeRotatedEigenstrain  
   b. GraphiteIrradiationCreepEigestrain

4. Under /moose/modules/solid_mechanics/[src or header]/materials/crystal_plasticity, add ComputeVariableOrthotropicElasticityTensorCP.
5. Under /moose/framework/[src or include]/postprocessors, add InterfaceFailedFractionPP.

***

Once everything is installed, these are the steps to run it. I have included a mesh file and an Euler angle file, so if you're running with these, you can start at step 5. The first 4 steps are for running with new geometries and Euler angles configurations.

1. Generate the periodic tessellation in Neper.
2. Run neper mosaic to make the periodic tessellation rectilinear.
3. Mesh in gmsh.
4. Run MOOSE with --mesh-only on the gmsh file to generate an exodus input file.
5. Run pair_boundaries.py with the meshonly_in.e file. This will require the python library exodusii: https://github.com/sandialabs/exodusii
6. The output for pair_boundaries.py needs to be used as an input for the MOOSE run. It can be written into a variable in bash or something similar.
7. Similarly, generate a bash variable that lists all the block IDs in the model. This can be done with exodusii the same as pair_boundaries.
8. Run the final MOOSE simulation, with the meshed input file, Euler angle file, pair_boundaries, and block IDs as inputs. This will generate an exodus file for visualization and a .csv file with postprocessor outputs.

***

For reading the csv data:
1. The porosity output is czm_gap_volume.
2. global_strain_0 and global_strain_1 are the dimensional changes in the x and y directions respectively, you can use these to calculate the total area change.
3. Because the global strain is computing all the macroscopic strain, the area of the periodic unit (block_area + gap_area) is always 1. The area change should only come from the global strain.
