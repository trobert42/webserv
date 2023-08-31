<?php
	$cwd = getcwd();
	$upload_dir = 'assets/uploaded_files/';
	$fileList = glob($upload_dir . '*');

	$wdir = $cwd . '/' . $upload_dir;
?>

<!DOCTYPE html>

<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="./assets/css/style.css">
	<title>CGI testooo</title>
</head>
<body>
	<header>
		<div class="principal">
			<div class="logo">
				<a href="./assets/images/creeper.jpg">Antoine <span>G</span></a>
			</div>
			<div class="menu">
				<ul>
					<li> <a href="index.html">Home</a> </li>
					<li> <a href="test_cgi.php">TEST CGI</a> </li>
					<li> <a href="about_us.html">About us</a> </li>
					<li> <a href="./error_pages/404.html">Inexisting page</a> </li>
				</ul>
			</div>
		</div>
	</header>
	<div class="flexbox-container">
		<div class="flexbox-item flexbox-item-1">
			<h1>GET METHOD</h1>
			<h2>What's your name ?</h2> <br>
			<form enctype="multipart/form-data" action ="test_get.php" method="GET">
				<h3>First Name : <input id="nameForm" type="text" name="firstName"> <br>
					Last Name : <input id="nameForm" type="text" name="lastName">
				</h3>
				<br>
				<input type="submit" value="Say my name, say my name~~!">
			</form>
			<br> <br> <br> <br>
			<div>
				<h2> Uploaded files </h2>
				<?php
					foreach($fileList as $filePath)
					{
						$filename = basename($filePath);
						echo '<h2>';
						echo '<a style="text-decoration:none;" href="' . $filePath . '"' . $filename . ' download="' . $filename . '"> - Download ' . $filename . '</a>';
						echo '<br>';
						echo '</h2>';
					}
				?>
			</div>
		</div>


		<div class="flexbox-item flexbox-item-2">
			<h1>POST METHOD</h1>
			<form enctype="multipart/form-data" action="test_post.php" method="POST" class="upload">
				<h2>File to upload: <input type="file" name="fileToUpload" id="fileToUpload"></h2> <br>
				<input type="submit" name="submit" value="Send file to server!">
			</form>
			<br> <br> <br> <br>
			<h3>PHP form</h3> <br>
			<form method="POST" action="form_post.php">
				<input type="text" name="fav_animal" placeholder="Favorite animal"><br /><br />
				<input type="text" name="fav_drink" placeholder="Favorite drink"><br /><br />
				<input type="text" name="fav_movie" placeholder="Favorite movie"><br /><br />
				<input type="text" name="fav_sport" placeholder="Favorite sport"><br /><br />
				<button type="submit">Submit</button>
			</form>

			<br> <br> <br> <br>
			<h3>Python form</h3> <br>
			<form method="POST" action="test_python_post.py">
				<input type="text" name="fav_meal" placeholder="Favorite meal"><br /><br />
				<button type="submit">Submit</button>
			</form>
		</div>


		<div class="flexbox-item flexbox-item-3">
			<h1>DELETE METHOD</h1>
			<form enctype="multipart/form-data" id="deleteForm" method="POST">
					<label for="fileInput"><p class="Select"><h3>Select a file to delete:</h3></p></label>
					<select name="fileInput" id="fileInput">
						<option value="">Select a file</option>
						<?php
						$dir = scandir($wdir);
						foreach ($dir as $doc)
						{
							if ($doc != '.' && $doc != '..')
							{
								$op = fopen($wdir . $doc, 'r');
								echo '<option value="' . $doc . '">'. $doc .'</h1>';
							}
						}
						?>
					</select>
					<br><br>
					<input type="submit" value="Delete the file!">
				</form>
			</div>
			<script>
				document.getElementById('deleteForm').addEventListener('submit', function(event)
				{
					event.preventDefault();
					var fileInput = document.getElementById('fileInput');
					var file = fileInput.value;
					if (file)
					{
						var formData = new FormData(this);
						var xhr = new XMLHttpRequest();
						var file_path = '<?php echo $upload_dir; ?>' + file;
						xhr.open('DELETE', file_path, true);
						xhr.onreadystatechange = function()
						{
							if (xhr.readyState === XMLHttpRequest.DONE)
							{
								if (xhr.status === 200 || xhr.status === 204) 
								{
									console.log('File deleted.');
									window.location.href = 'delete_sucess.html';
								}
								else
								{
									console.log('Error deleting file. Status code: ' + xhr.status);
									window.location.href = 'delete_fail.html';
								}
							}
						};
						xhr.send(formData);
					}
					else
					{
						console.log('Please select a file to delete.');
					}
				});
			</script>
		</div>
	</div>
</body>
</html>