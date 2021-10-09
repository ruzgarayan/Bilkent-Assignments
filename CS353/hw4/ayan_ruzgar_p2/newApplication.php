<!DOCTYPE html>
<html>

<head>
    <title>Student Welcome Page</title>
    <style>
      table,
      th,
      td {
        padding: 10px;
        border: 1px solid black;
        border-collapse: collapse;
		margin: auto;
		text-align: center;
      }
    </style>
</head>

<body>
	<div>
		<?php
		include 'connection.php';
		session_start();
		
		if(empty($_SESSION))
		{
			header("refresh:0;url=index.php");
		}

		$sid = $_SESSION['sid'];

		$sqlQuery = "SELECT * FROM apply WHERE sid =\"" . $sid . "\";";
		$queryResult = mysqli_query($connection, $sqlQuery);
		$numRows = mysqli_num_rows($queryResult);

		if ($numRows >= 3)
		{
			echo "<div>You already have 3 applications, you cannot apply to more companies. Click the <a href=\"student.php\">link</a> to go back. </div>";
		}
		else
		{
			echo "<h1> Applicable Companies </h1>";
			
			$sqlQuery = "SELECT cid, cname FROM company WHERE NOT EXISTS (SELECT * FROM apply WHERE sid='$sid' AND cid=company.cid) AND quota > (SELECT count(*) FROM apply where cid=company.cid);";
			$queryResult = mysqli_query($connection, $sqlQuery);
			$numRows = mysqli_num_rows($queryResult);
			
			if ($numRows > 0)
			{
				echo "
				<table>
					<thead>
						<tr>
							<th>cid</th>
							<th>cname</th>
						</tr>
					</thead>
				";
						
				for ($i = 0; $i < mysqli_num_rows($queryResult); $i++)
				{
					$currTuple = mysqli_fetch_assoc($queryResult);
					echo "
					<tbody>
						<td>" . $currTuple["cid"] . "</td>
						<td>" . $currTuple["cname"] . "</td>
					</tbody>
					";
				}
						
				echo "</table>";
				
				echo "
					</br>
					<div>
						<form action=\"apply.php\" method=\"post\" style='margin: auto; text-align: center'>
							<div>
								<label>Company Id:</label>
								<input type=\"text\" name=\"cid\" placeholder=\"Cxxx\">
							</div>
							<br/>
							<div>
								<button type=\"submit\" name=\"submit\">Submit</button>
							</div>
						</form>
					</div>
				";
			}
			else
			{
				echo "<div> There are no applicable companies. </div>";
			}
		}

		mysqli_close($connection);
		?>
	</div>
	</br>
	<div>
		<a href="student.php">Back to the Welcome Page</a>.
	</div>
	</br>
	<div>
		<a href="logout.php">Logout</a>.
	</div>
</body>

</html>