<?php
include 'connection.php';
session_start();

$cid = $_POST['cid'];
$sid = $_SESSION['sid'];

$sqlQuery = "SELECT * from company WHERE cid='$cid';";
$queryResult =  mysqli_query($connection, $sqlQuery);

if (mysqli_num_rows($queryResult) <= 0)
{
	echo "<div> The company ID you have entered is invalid. Returning to the previous page in 5 seconds.</div>";
	header( "refresh:5;url=newApplication.php");
	die();
}
$quota = mysqli_fetch_assoc($queryResult)["quota"];

$sqlQuery = "SELECT * from apply WHERE cid='$cid';";
$queryResult =  mysqli_query($connection, $sqlQuery);
$numApplications = mysqli_num_rows($queryResult);

if ($numApplications >= $quota)
{
	echo "<div>The quota for this company is full, your application could not be done. Returning to the previous page in 5 seconds.</div>";
	header( "refresh:5;url=newApplication.php");
	die();
}

$sqlQuery = "SELECT * from apply WHERE cid='$cid' AND sid='$sid';";
$queryResult =  mysqli_query($connection, $sqlQuery);
if (mysqli_num_rows($queryResult) > 0)
{
	echo "<div> You alread have an application for the given company. Returning to the previous page in 5 seconds.</div>";
	header( "refresh:5;url=newApplication.php");
	die();
}

$sqlQuery = "INSERT INTO apply VALUES('$sid', '$cid');";
$queryResult =  mysqli_query($connection, $sqlQuery);

if ($queryResult === TRUE)
{
	echo "<div> Application is successful. Returning to the previous page in 5 seconds.</div>";
	header( "refresh:5;url=newApplication.php");
	die();
}
else
{
	echo "<div>A problem occurred when applying to the company, try again later. Returning to the previous page in 5 seconds.</div>";
	header( "refresh:5;url=newApplication.php");
	die();
}

mysqli_close($connection);
?>
