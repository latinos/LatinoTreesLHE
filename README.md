# LatinoTreesLHE
LHE level analysis


to compile:

    c++ -o ntupleMaker.exe `root-config --cflags` ntupleMaker.cpp `root-config --libs`

to use:

    ./ntupleMaker.exe fileLHE outputFileRoot
    ./ntupleMaker.exe /data2/amassiro/Generation/MG5/MG5_aMC_v2_0_1/MY_W_LO/Events/run_01/unweighted_events.lhe   W13TeV.root
 
    ./ntupleMaker.exe cmsgrid_final.lhe   superGGH.root
 
 
credits:

    http://home.thep.lu.se/~leif/LHEF/
    
    
where:

    cmsneu
    /home/amassiro/Latinos/LatinoTreesLHE/

    xps
    /home/amassiro/Cern/Code/Latino/LatinoTreesLHE/
    
    


