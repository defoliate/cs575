import os


outputDirectory = os.path.join("Output")
outputFileName = "results.csv"
outputPath = os.path.join(outputDirectory, outputFileName)
trials = [ 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576 ]
threads = [ 1, 2, 4, 8, 16 ]
runs = 1

if(not os.path.exists(outputPath) or input("Path exists. ReRun? (Y)") == "Y"):
    if(os.path.exists(outputPath)):
        os.remove(outputPath)
    else:
        if not os.path.exists(outputDirectory):
            os.makedirs(outputDirectory)

    for trial in trials:
        for thread in threads:
            print("Number of trials: %d Number of threads: %d" % (trial, thread))
            cmd = "g++ -DNUMTRIALS=%d -DNUMT=%d p1.cpp -fopenmp" % ( trial, thread )
            os.system( cmd )
            for run in range(runs):
                cmd = "./a.out >> " + outputPath # outputPath = Output/P1/results.csv
                os.system( cmd )
os.remove("a.out")

