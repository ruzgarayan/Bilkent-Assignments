<?php
include 'connection.php';
session_start();

$name = $_POST["username"];
$password = $_POST["password"];

$sqlQuery = "SELECT * FROM student WHERE UPPER(sname) LIKE UPPER('$name') AND sid='$password';";
$queryResult =  mysqli_query($connection, $sqlQuery);

if (mysqli_num_rows($queryResult) > 0)
{
	echo "<div> Login is successful. Going to the student page. </div>";
	$_SESSION['sid'] = $_POST['password'];
	header( "refresh:3;url=student.php" );
	die();
}
else
{
	echo "<div> Login is unsuccessful. Going back to the index page in 3 seconds. </div>";
	header( "refresh:3;url=index.php" );
	die();
}

mysqli_close($connection);
?>
