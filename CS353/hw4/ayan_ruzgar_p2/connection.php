<?php 
$serverName = "localhost";
$username = "ruzgar.ayan";
$password = "mQqnw780";
$dbName = "ruzgar_ayan";
$connection = mysqli_connect($serverName, $username, $password, $dbName);


if (!$connection)
{
	echo "<div> Connection to database failed </div>";
	header( "refresh:3;url=index.php" );
	die();
}
?>