import os
for t in [ 1, 2, 4, 8 ]:
	print "NUMT = %d" % t
	for s in [100,10000,100000,1000000]:
		print "NUMS = %d" % s
		cmd = "g++ -DNUMS=%d -DNUMT=%d prog.cpp -o prog -lm -fopenmp" % ( s, t )
		os.system( cmd )
		cmd = "./prog"
		os.system( cmd )