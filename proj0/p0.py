import os
for t in [ 1, 4 ]:
	print ("NUMT = %d" % t)
	for s in [ 2, 4,8,16,32,64]:
		#print "NUMS = %d" % s
		cmd = "g++ -DNUMS=%d -DNUMT=%d p0.cpp -o p0 -lm -fopenmp" % ( s, t )
		os.system( cmd )
		cmd = "./p0"
		os.system( cmd )
os.remove("p0")
