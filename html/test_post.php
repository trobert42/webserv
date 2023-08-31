<?php

$uploaded_folder = $_SERVER['UPLOAD_DIR'];
$uploaded_file = $_SERVER['FILENAME'];

$path_file = getcwd() . $uploaded_folder . $uploaded_file;

if (empty($uploaded_file))
{
	$html = '<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta http-equiv="X-UA-Compatible" content="IE=edge">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<link rel="stylesheet" href="./assets/css/style.css">
		<title>Hello</title>
	</head>
	<body>
		<header>
			<div class="big_msg">
				<h1><strong>Please select a file :)</strong></h1> <br> <br> <br> <br>
				<div class="button2">
					<a href="test_cgi.php" class="btn">go back</a>
				</div>
			</div>
		</header>
	</body>
	</html>';
}
else if (!file_exists($path_file))
{
	$html = '<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta http-equiv="X-UA-Compatible" content="IE=edge">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<link rel="stylesheet" href="./assets/css/style.css">
		<title>Hello</title>
	</head>
	<body>
		<header>
			<div class="big_msg">
				<h1><strong>Failed to upload '. $uploaded_file . ' into ' . $uploaded_folder . ' </strong></h1> <br> <br> <br> <br>
				<div class="button2">
					<a href="test_cgi.php" class="btn">go back</a>
				</div>
		</header>
	</body>
	</html>';
}
else
{
	$html = '<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta http-equiv="X-UA-Compatible" content="IE=edge">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<link rel="stylesheet" href="./assets/css/style.css">
		<title>Hello</title>
	</head>
	<body>
		<header>
			<div class="big_msg">
				<h1><strong>Success: uploaded '. $uploaded_file . ' into ' . $uploaded_folder . '</strong></h1> <br> <br> <br> <br>
				<div class="button2">
					<a href="test_cgi.php" class="btn">go back</a>
				</div>
			</div>
		</header>
	</body>
	</html>';
}

echo $html;

?>