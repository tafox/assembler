from subprocess import Popen, PIPE

with open("test-suite") as file:
	tests = file.readlines()

counter = 1
num_of_tests = len(tests)
for test in tests:
	test = test.strip()
	print "Running Test {0}/{1}: {2}".format(counter, num_of_tests, test) 	
	with open(test) as file:
		input = file.read()
		process = Popen(['./asm'], stdout=PIPE, stdin=PIPE)
		process.stdin.write(input)
		process.communicate()
		process.stdin.close()
	print "Finishing Test {0}/{1}: {2}".format(counter, num_of_tests, test) 	
	counter += 1
	
	
	
