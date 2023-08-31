<?php

$dir = $_SERVER['LOCATION'];
$dir_location = substr($dir, 1);
$files = scandir($dir);
sort($files);

$html = '<!DOCTYPE html>
<html lang="en">
<head>

<title>Index of ' . $dir_location . '</title>
<style>
	table {
		width: 100%;
		border-collapse: collapse;
	}
	th, td {
		padding: 5px;
		text-align: left;
		border-bottom: 1px solid #ddd;
	}
	tbody {
		text-align: left;
	}
</style>
</head>
<body>
	<header>
		<div class="listing">
			<h1><strong>Index of ' . $dir_location . '</strong></h1>
			<hr>
			<table class="listing">
				<thead>
					<tr>
						<th>File Name</th>
						<th>Last Modification</th>
						<th>Size</th>
					</tr>
				</thead>
				<tbody>';
				foreach ($files as $file) {

					if (is_dir($dir . $file)) {
						$file = $file . '/';
					}

					$html .='
					<tr>
					<td><a href="' . $file . '">' . $file . '</a></td>';
					$lastmodified = date("F d Y H:i:s", filemtime($dir . $file));
					$html .= '
					<td>' . $lastmodified . '</td>';
					if (!is_dir($file))
					{
						$file_name = $dir . $file;
						$filesize = filesize($file_name);
					}
					else
					{
						$filesize = filesize($file);
					}
					$html .= '
						<td>' . $filesize . '</td>
						</tr>';
					}
				$html .= '
				</tbody>
				</table>
			<hr>
		</div>
	</header>
</body>
</html>';

echo $html;
?>