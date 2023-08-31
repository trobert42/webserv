<?php

$userFirstName = "";
$userLastName = "";

if (isset($_GET["firstName"]) && isset($_GET["lastName"]))
{
	$userFirstName = htmlspecialchars($_GET["firstName"]);
	$userLastName = htmlspecialchars($_GET["lastName"]);
}

if (!empty($userFirstName) && !empty($userLastName))
{
	$html = '<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta http-equiv="X-UA-Compatible" content="IE=edge">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<link rel="stylesheet" href="./assets/css/style.css">
		<title>Bonjour ' . $userFirstName . ' ' . $userLastName . '</title>
	</head>
	<body>
		<header>
			<div class="big_msg">
				<h1><strong>Bonjour '. $userFirstName . ' ' . $userLastName . '!!</strong></h1>
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
		<title>Not Hello Anonymous :/</title>
	</head>
	<body>
		<header>
			<div class="big_msg">
				<h1><strong>Not saying hello to you. You, who did not give me a name!!</strong></h1>
			</div>
		</header>
	</body>
	</html>';
}

echo $html;
?>