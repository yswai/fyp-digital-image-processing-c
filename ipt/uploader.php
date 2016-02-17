<?php
session_start();

$target_path = "/usr/share/images/";
$image_path = "/usr/share/images/";

$target_path = $target_path.basename( $_FILES['uploadedfile']['name']); 

if(move_uploaded_file($_FILES['uploadedfile']['tmp_name'], $target_path))
{
	echo "The file ".basename( $_FILES['uploadedfile']['name'])." has been uploaded to directory: ".$target_path; 
	echo ' ';
	$_SESSION['imgf'] = basename($_FILES['uploadedfile']['name']);
	chmod($target_path,0755);
	$copy_command = 'cp '.$target_path.' '.$image_path.'out_'.basename($target_path);
	echo `$copy_command`;	
	echo '';
} else{
	echo "There was an error uploading the file, please try again!";
}
	echo "<br/>";
	echo '<h2><a href=/ipt>Continue</a></h2>';
?>
