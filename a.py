import os
for t in [ 1, 4 ]:
	print "NUMT = %d" % t
	for s in [ 2, 4,8,16,32,64, 128, 256,512]:
		#print "NUMS = %d" % s
		cmd = "g++ -DNUMS=%d -DNUMT=%d a.cpp -o prog -lm -fopenmp" % ( s, t )
		os.system( cmd )
		cmd = "./prog"
		os.system( cmd )
