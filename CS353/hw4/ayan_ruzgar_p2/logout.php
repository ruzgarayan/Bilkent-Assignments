<?php
session_start();

unset($_SESSION['sid']);
echo "<div> Logged out. Going back to the index page in 3 seconds.</div>";
header( "refresh:3;url=index.php" );
die();
?>
