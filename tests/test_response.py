import socket
import unittest
import subprocess
import time

import test_helpers

class ResponseTest(unittest.TestCase):
	@classmethod
	def setUpClass(cls):
		# Launch the webserv program as a subprocess
		cls.server_process = subprocess.Popen(["./webserv", "config_files/default.conf"])
		time.sleep(1)

	@classmethod
	def tearDownClass(cls):
		# Terminate the server process
		cls.server_process.terminate()
		cls.server_process.wait()

	def setUp(self):
		self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.client_socket.connect(('localhost', 8080))

	def tearDown(self):
		self.client_socket.close()

	# TESTS :

	def test_index_request(self):
		# define the request that will be sent
		request = "GET / HTTP/1.1\r\n\r\n"

		# define the expected body
		with open('html/index.html', 'r') as file:
			expected_body = file.read()

		self.client_socket.send(request.encode())
		response = self.client_socket.recv(1024).decode()

		status_line = test_helpers.extract_status_line(response)
		header = test_helpers.extract_header(response)
		body = test_helpers.extract_body(response)

		# compare the received response and the expected response
		self.assertEqual(status_line, "HTTP/1.1 200 OK")
		test_helpers.test_ResponseHeader(self, header, "Content-Length:", len(expected_body))
		test_helpers.test_ResponseHeader(self, header, "Content-Type:", "text/html")
		self.assertEqual(body, expected_body)

	def test_bad_request(self):
		# define the request that will be sent
		request = "GIT / HTTP/1.1\r\n\r\n"

		self.client_socket.send(request.encode())
		response = self.client_socket.recv(1024).decode()

		status_line = test_helpers.extract_status_line(response)

		# compare the received response and the expected response
		self.assertEqual(status_line, "HTTP/1.1 400 Bad Request")

	def test_bad_filepath(self):
		# define the request that will be sent
		request = "GET /nfief HTTP/1.1\r\n\r\n"

		self.client_socket.send(request.encode())
		response = self.client_socket.recv(1024).decode()

		status_line = test_helpers.extract_status_line(response)

		# compare the received response and the expected response
		self.assertEqual(status_line, "HTTP/1.1 404 Not Found")

	def test_DELETE_method(self):
		filepath = "html/todelete.html"
		if test_helpers.create_file(filepath) == 1:
			self.fail(f"Test could not execute properly, failed to create test file.")

		# define the request that will be sent
		request = f"DELETE {filepath} HTTP/1.1\r\n\r\n"

		self.client_socket.send(request.encode())
		response = self.client_socket.recv(1024).decode()

		status_line = test_helpers.extract_status_line(response)

		# compare the received response and the expected response
		self.assertEqual(status_line, "HTTP/1.1 204 No Content")
