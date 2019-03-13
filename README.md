# SHMS_optics 

Contains updated matrix elements for SHMS and variables with hallc_replay
# SHMS_optics

I run my codes on ifarm:
```
 cd source
 mkdir build
 cd build
 cmake ..
 make
```
This makes the executables and puts everything into the build directory. This is how I run the optimization procedure:
```
 cd build
 ./shms_optics setup_optics_example.txt -o outputFile.root -a
```
Configuration File Specfication
-------------------------------

The code parses the configuration file for a list of "runs", each of which is associated
with a given set of conditions such as beam position, SHMS central angle, number of target 
foils, z position of the foils, cuts, etc. 

The code reads the file until the "endlist" keyword is encountered. The lines are expected 
to be in the form "keyword value". Possible keywords are: 

`newrun XXXXX`: This starts a new "run" definition. The "run number" XXXXX, which might or might not correspond with a CODA run number, must be unique or the properties will be overwritten. Everything read between this line and the next line starting with "newrun" will be assigned to this run. Whenever this keyword is read, all possible properties of the run are initialized with sensible defaults. 

`filelist file1.root file2.root file*.root`: This line contains a list of ROOT files containing hms ntuples associated with this run. Wildcards are accepted. This command can be used multiple times per run, subsequent invocations add to the file list. 

`beampos x y dx/dz dy/dz`: Expects four numbers after the "beampos" keyword. Defines the beam position on target and beam angles. Currently, the beam angles are not used and assumed to be zero.

`thetaSHMS thetadeg`: "thetaSHMS" keyword expects to be followed by a single parameter, the SHMS central angle in degrees. SHMS angles are positive. Note that this is not the overall angle used for all the runs (see Theta) 

`nfoil N`: "nfoil" keyword defines number of target foils in the run. Expects one integer argument.

`zfoil z1 z2 ... zN`: "zfoil" keyword defines z position of target foils in the run. Expects nfoil arguments, assumed to be given in cm. If nfoil is not yet defined for the run, the command has no effect. 

`sieveslit 1`: "sieveslit" flag defines whether the sieve slit was in or out for this run. Expects an integer argument (0, 1, or 2). Sieve slit 0 is no sieve slit. Sieve slit 1 is the centered sieve and sieve slit 2 is the shifted sieve. If you have thin-foil data without sieve slit, the code will attempt to include the data in the ytarget fit, but generally you will only want to use data with sieve slit to fit ytarget, xptar and yptar, so usually you want to use "sieveslit 1". You can omit this command and it will default to 1. 

`cut cut1 cut2 ... cutN`: Defines event selection cuts, allows multiple cut definitions separated by spaces. Any expression that defines a valid TCut can be used here. 

`use2017Corr 0`: This can be used to rotate yTar vs zVertex when there are large correlations that mess up cutting on yTar and zVertex. 0 Excludes this correction, 1 includes the correction. This has no effect on the optimization, but it rotates yTar for the selection of events with each foil. 

`Theta theta1 theta2 ... thetaN`: This should have the same number of arguments as files in the filelist. This was created to combine similar runs that have slightly different angles due to experimental conditions. The angles listed here are used in the reconstruction of events and should be checked against the camera angles for accuracy. 

In the case of keywords beampos, thetaSHMS, nfoil, zfoil and sieveslit, if the keyword appears more than once, the last invocation supersedes any previous ones. In the case of filelist and cut, subsequent invocations add files, TCut objects to the list of files and cuts for the run in question.  

After the "endlist" keyword is encountered, there are a few subsequent arguments expected. 

`oldcoeff.dat`: name of file containing "old" (starting) reconstruction matrix elements. This will search for two files of the pattern `oldcoeff__dep.dat` and `oldcoeff__indep.dat`. These are separated since we optimize over the indep variables, but we include the dep variable for reconstructing the events.

`newcoeff.dat`: name of file containing "new" reconstruction matrix elements (file to which fit results will be written). This will produce two output files with the dep and indep matrix elements separately in the pattern of `newcoeff__dep.dat` and `newcoeff__indep.dat`. 

`htheta_offset_old`: offset applied to yptar (in-plane) angle. If using newly optimized matrix elements, this comes from the 0000 elements. 

`hphi_offset_old`: offset applied to xptar (out-of-plane) angle. If using newly optimized matrix elements, this comes from the 0000 elements. 

`fitorder`: usually 5 or 6, this is the order of the fit to perform.

`maxnperhole`: integer, max number of events per sieve hole per target foil to include in the fit (generally a small number like 100-1000 events is good here, to ensure roughly equal weighting of sieve holes in the fit). 

`maxnperfoil`: max number of events per target foil to include in the fit (only applies to the "sieveslit 0" case, so generally has no effect). 

`zoffset_foil`: Global offset applied to the z position of all target foils. 

`fit_xtar_coeffs_flag`: 0 or 1, flag to choose whether to include xtarget-dependent coefficients in the fit (generally better to use 0 as these are computed from COSY, since in real data the system is underdetermined). 

In general, the code will then loop over all sieve holes for each foil for each run, and attempt to automatically determine a cut to select events going through a given sieve hole, but will require input from the user to validate each hole (you can tweak the range of the fit to get a better result, or you can reject a hole if a good fit cannot be achieved). Once cuts have been defined for all sieve holes for all foils for all runs, the program sets up and solves the equations for the new coefficients, and writes the file. The ROOT tree in the output root file contains diagnostic information, basically it has the focal plane track parameters, the "true" target track parameters determined from the sieve hole positions and the target foil/beam positions and the reconstructed target track parameters using the old (initial) reconstruction matrix elements. 

Coordinate System Issues and Notes
---------------------------------

`x` is the dispersive coordinate, always pointing vertically down, `y` is the non-dispersive 
coordinate, pointing to beam left (SHMS is on beam left) in the plane perpendicular to the 
SHMS optical axis containing the origin, and `z` is the SHMS optical axis, with +z in the 
direction of particle motion. This transport coordinate system is rotated by the SHMS 
central angle to beam left with respect to the Hall C coordinate system. 

On the other hand, the beam position is in a different coordinate system. The BPMs in Hall C 
have +X to beam right, +Y vertically UP. It should be possible 
to infer from context in the code in what coordinate system the beam position is expected 
to be given by the user. 

Notes on handling of xtar
-------------------------

Right near the end of the code, after the second event loop that fills the SVD matrices, there is a short loop that has a comment at the top that says:

`//Now, do some post-processing of these matrices depending on xtar behavior`

In that loop, any terms where the xtar exponent is non-zero (i.e. xtar-dependent terms) are set to zero for the ytarget/yptarget/xptarget column fitting matrices, and to diagonal=1/off-diagonal=0 for the square “lambda_fp” fitting matrix.

The question is:  why do it this way?

The motivation here was that typically either one lacks sufficient information to determine 
the xtar-dependent coefficients from the data, and/or including the xtar-dependent coefficients 
in the fit introduces significant correlations with the xtar-independent parameters and leads to 
a worse quality of the fit overall (We have tried in the past to fit these coefficients, 
but concluded that it was better to use the pre-existing ones). 

Recall that the problem of reconstruction is actually under-determined; in any given event, we have four equations (xfp, yfp, xpfp, ypfp) in five unknowns (xtar, ytar, xptar, yptar, delta) ). "xtar" is never actually directly measured, but it is reconstructed using our knowledge of the beam position and the reconstructed values of xptar, yptar and ytar. 

For a given z position of the interaction vertex, xtar and xptar are essentially 100% correlated. The raster size in the vertical direction is typically small compared to the range of xtar that is populated by the combined zvertex-xptar phase space populated by an extended target. If you plot (reconstructed) xtar vs zvertex, you will typically get an "hourglass" shape with a waist at zvertex = 0. The width of this "waist" equals the vertical raster size. If you select a narrow range of xptar (zvertex) and plot xtar vs zvertex (xptar) you will get a linear correlation, the slope of which depends on xptar (zvertex). xtar, as you can find in the code, is defined by the equation:

`xtar = -ybeam - xptar * ( cos(thetaHMS) * zvertex + sin(thetaHMS) * xbeam )` 

where (xbeam, ybeam) is the beam position at the interaction vertex measured in a coordinate system with +xbeam horizontal, pointing to beam right and +ybeam vertically up. Because xtar and xptar are essentially 100% correlated, there isn't really enough independent information to determine the xtar-dependent coefficients, which are significantly non-zero only for the reconstruction of xptar and delta. Note that attempting to fit the xtar-dependent coefficients for xptar also affects the momentum reconstruction, since the value of xtar used as input to the momentum reconstruction is affected by the quality of the reconstruction of xptar. If one really wants to optimize these coefficients, one needs to obtain a set of optics calibration data for several different (and widely varying) vertical beam positions and/or with a large raster size.

Therefore, instead of including the xtar-dependent coefficients as free parameters in the fit, we take these as given (from previous COSY calculations) and effectively subtract out the effects of xtar-dependence in fitting the xtar-independent coefficients. The way we do this is by including the xtar-dependent terms in the sums used to do the reconstruction, but then setting all the off-diagonal matrix elements on the left-hand side of the equation and all the xtar-dependent components of the column vector on the right-hand side of the equation to zero before doing the matrix inversion, and then when we write out the file with the new coefficients, all the xtar-dependent coefficients assume the same values that were in the "old" file. 


Notes on Cuts Applied to both ytarget and zbeam
-----------------------------------------------

In the initial stages of the code, cuts are applied to both ytarget and zbeam to select good events from the appropriate target foil.  The question is:  why do we put cuts on both of these variables, as they are presumably 100% correlated with one another.

While in principle this is true, in practice, applying both cuts leads to a cleaner sample 
of events for the fit, since, for example, an event with good initial ytarget reconstruction 
but pathological yptarget could give a bad zvertex, OR an event with good yptarget reconstruction 
but bad ytarget reconstruction could also give a bad zvertex. On the other hand, an event with 
bad reconstruction of both ytarget and yptarget could still give a zvertex that passes the cut. 
In any case, applying both cuts more nearly insures that the events included in the fit have 
a reasonable initial reconstruction of both ytarget AND yptarget. The important thing is 
that to the maximum possible extent, events identified as coming from a given foil in fact 
should have actually scattered from that foil.  











