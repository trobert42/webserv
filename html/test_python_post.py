#!/usr/bin/env python3

import cgi

form = cgi.FieldStorage()
fav_meal = form.getvalue('fav_meal', '')

print("Content-type: text/html; charset=UTF-8\r\n\r\n")

print('''<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="./assets/css/style.css">
	<title>test post form</title>
</head>
<body>
	<header>
		<div class="big_msg">
			<h1><strong>I know everything about you now</strong></h1> <br> <br> <br>
			<h3> Favorite meal : {fav_meal}, love that!<br> <br> <br></h3>
			<br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br>
			<div class="button2">
				<a href="test_cgi.php" class="btn">go back</a>
			</div>
		</div>
	</header>
</body>
</html>
'''.format(fav_meal=fav_meal))
