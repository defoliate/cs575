for t in 1 10
do
    g++ -DNUMT=$t AutoCorrOpenMP.cc -o OpenMPproj -lm -fopenmp
    ./OpenMpproj
done