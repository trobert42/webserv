import unittest
import argparse

import test_helpers
import test_response


if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('-m', '--module', help='Run specific test module')
	parser.add_argument('-t', '--test', help='Run specific test case or test method')
	args = parser.parse_args()

	if args.module:
		# Run specific test module
		if args.test:
			suite = unittest.TestLoader().loadTestsFromName('test_response.' + args.module + '.' + args.test)
		else:
			suite = unittest.TestLoader().loadTestsFromName('test_response.' + args.module)
	else:
		# Run all test modules
		suite = unittest.TestSuite()
		suite.addTest(unittest.makeSuite(test_response.ResponseTest))

	runner = test_helpers.ColoredTextTestRunner()
	runner.run(suite)
