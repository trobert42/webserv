import sys
import unittest

class ColoredTextTestResult(unittest.TextTestResult):
	COLORS = {
		'HEADER': '\033[95m',
		'OKGREEN': '\033[92m',
		'FAIL': '\033[91m',
		'ENDC': '\033[0m'
	}

	def getDescription(self, test):
		return test.shortDescription() or str(test)

	def addSuccess(self, test):
		super().addSuccess(test)
		sys.stdout.write(f"{self.COLORS['OKGREEN']}✓{self.COLORS['ENDC']} {self.getDescription(test)}\n")

	def addFailure(self, test, err):
		super().addFailure(test, err)
		sys.stdout.write(f"{self.COLORS['FAIL']}✗{self.COLORS['ENDC']} {self.getDescription(test)}\n")

	def addError(self, test, err):
		super().addError(test, err)
		sys.stdout.write(f"{self.COLORS['FAIL']}✗{self.COLORS['ENDC']} {self.getDescription(test)}\n")

class ColoredTextTestRunner(unittest.TextTestRunner):
	resultclass = ColoredTextTestResult

def extract_status_line(response):
	lines = response.split('\r\n')
	status_line = lines[0]

	return status_line

def extract_header(response):
	header_end = response.find('\r\n\r\n')
	header = response[:header_end]

	return header

def extract_body(response):
	header_end = response.find('\r\n\r\n')
	body = response[header_end + 4:]

	return body

# create a file with write permission
# returns 0 if created successfuly
# returns 1 if creation failed
def create_file(file_path):
	try:
		with open(file_path, 'w') as file:
			pass
		return 0
	except IOError as e:
		print(f"An error occurred while creating the file: {e}")
		return 1

def test_ResponseHeader(test, header, header_name, expected_value):
	# Extract the value of the specified header from the response
	header_value = None
	for line in header.splitlines():
		if line.startswith(header_name):
			if header_value is not None:
				test.fail(f"Multiple occurrences of {header_name} header in the response")
			header_value = line[len(header_name):].strip()

	# Compare the extracted header value to the expected value
	test.assertEqual(header_value, str(expected_value))