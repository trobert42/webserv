<?php

$entityBody = file_get_contents('php://input');
parse_str($entityBody, $BODY);

$fav_animal = $BODY['fav_animal'];
$fav_drink = $BODY['fav_drink'];
$fav_movie = $BODY['fav_movie'];
$fav_sport = $BODY['fav_sport'];
?>

<!DOCTYPE html>
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
			<h3> Favorite animal : <?php echo $fav_animal ?>, yeah i guess that's not bad, but dragon's better <br> <br>
				 Favorite drink : <?php echo $fav_drink ?> --what ?!! fresh orange juice, S top-tier, u can't change my mind!<br> <br>
				 Favorite movie : <?php echo $fav_movie ?> okok, but i don't think Forrest Gump will be surpassed <br> <br>
				 Favorite sport : <?php echo $fav_sport ?>, cool ! but you may check on the greatest sport ever: beer pong <br> <br></h3>
			<br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br> <br>
			<div class="button2">
				<a href="test_cgi.php" class="btn">go back</a>
			</div>
		</div>
	</header>
</body>
</html>
