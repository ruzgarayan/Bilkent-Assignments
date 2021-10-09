<?php
include 'connection.php';
session_start();

$cid = $_POST["cid"];
$sid = $_SESSION['sid'];

$sqlQuery = "DELETE FROM apply WHERE sid =\"" . $sid . "\" AND cid =\"" . $cid . "\";";
$queryResult = mysqli_query($connection, $sqlQuery);

if ($queryResult === TRUE)
{
	echo "<div>Successfully deleted the application. Click the <a href=\"student.php\">link</a> to go back. </div>";
}
else
{
	echo "<div>A problem occurred when deleting the application, try again later. Click the <a href=\"student.php\">link</a> to go back. </div>";
}

mysqli_close($connection);
?>